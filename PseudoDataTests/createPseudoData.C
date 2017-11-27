#include <fstream>
#include <vector>

#include "TSystem.h"

#include "Category.h"
#include "DataCardModifier.h"
#include "Process.h"
#include "PseudoDataGenerator.h"
#include "StringOperations.h"


//const TString datacard = "limits_All_v24_datacard_ljets/datacard_jge6_tge4_high_hdecay.txt";
const TString templatesNominal = "limits_All_v24_datacard_ljets/ttH_hbb_13TeV_sl.root";
const TString CMSSW_BASE = "/afs/desy.de/user/m/matsch/CMSSW_8_1_0";
const TString PWD = "/afs/desy.de/user/m/matsch/ttH/CombineFitTests/PseudoDataTests";

const TString combineCmd = "combine -M FitDiagnostics --rMin -10 --rMax 10 --cminDefaultMinimizerStrategy 0 --cminDefaultMinimizerTolerance 0.001 --saveNormalizations --saveShapes";


void createPseudoData(const TString& datacard,
		      const TString& outdir,
		      const int nExperiments,
		      const double expectSignal=1.) {

  std::vector<Category::Type> categories = {
    Category::SL_43,
    Category::SL_44l,
    Category::SL_44h,
    Category::SL_53,
    Category::SL_54l,
    Category::SL_54h,
    Category::SL_63l,
    Category::SL_63h,
    Category::SL_64l,
    Category::SL_64h
  };

  std::vector<Process> processes;

  if( expectSignal > 0 ) {
    processes.push_back( Process(Process::ttHbb,templatesNominal,expectSignal) );
    // processes.push_back( Process(Process::ttHcc,templatesNominal,expectSignal) );
    // processes.push_back( Process(Process::ttHgg,templatesNominal,expectSignal) );
    // processes.push_back( Process(Process::ttHgluglu,templatesNominal,expectSignal) );
    // processes.push_back( Process(Process::ttHtt,templatesNominal,expectSignal) );
    // processes.push_back( Process(Process::ttHww,templatesNominal,expectSignal) );
    // processes.push_back( Process(Process::ttHzg,templatesNominal,expectSignal) );
    // processes.push_back( Process(Process::ttHzz,templatesNominal,expectSignal) );
  }
    
  processes.push_back( Process(Process::ttlf,templatesNominal) );
  processes.push_back( Process(Process::ttcc,templatesNominal) );

  processes.push_back( Process(Process::ttbb,templatesNominal) );
  processes.push_back( Process(Process::ttb,templatesNominal) );
  processes.push_back( Process(Process::tt2b,templatesNominal) );

  std::cout << "Reading processes" << std::endl;
  for(auto& category: categories) {
    for(auto& process: processes) {
      process.addCategory(category);
    }
  }


  // create pseudo data
  std::cout << "Creating pseudo data" << std::endl;
  PseudoDataGenerator generator;
  //generator.usePoissonStatistics(false); // for debugging, do not smear

  generator.setOutputDirectory(outdir);
  generator.setOutputDirectoryPerExperimentSuffix("PseudoExperiment");
  generator.setOutputFileSuffix("Data_Obs");

  std::vector<TString> experimentDirs = generator.run(categories,processes,nExperiments);


  // prepare datacards
  std::cout << "Creating datacards" << std::endl;
    
  // cp input root files to working dir
  if( gSystem->CopyFile(templatesNominal,outdir+"/"+str::fileName(templatesNominal)) != 0 ) {
    std::cerr << "ERROR copying inputs to working directory '" << outdir << "'" << std::endl;
    throw std::exception();
  }

  // create datacards
  const TString datacardName = "datacard.txt";
  DataCardModifier card(datacard);
  card.setInputsPath("..");
  for(auto& experimentDir: experimentDirs) {
    card.setObservation(experimentDir+"/Data_Obs.root","Data_Obs.root");
    card.write(experimentDir+"/"+datacardName);
  }

  
  // create batch scripts
  std::cout << "Creating submission scripts" << std::endl;
  
  const TString nameRunScript = "run_fit.sh";
  for(auto& experimentDir: experimentDirs) {
    std::ofstream out(experimentDir+"/"+nameRunScript);
    if( out.is_open() ) {
      
      out << "#!/bin/bash\n\n";

      out << "export VO_CMS_SW_DIR=/cvmfs/cms.cern.ch\n";
      out << "source $VO_CMS_SW_DIR/cmsset_default.sh\n";
      out << "cd " << CMSSW_BASE << "/src\n";
      out << "eval `scram runtime -sh`\n";
      out << "\ncd " << PWD << "/" << experimentDir << "\n";
      out << "echo '" << combineCmd << " " << datacardName << "'\n";
      out << combineCmd << " " << datacardName << "\n";
      out << "rm higgsCombineTest.FitDiagnostics.mH*.root\n";
      
      out.close();
      
    } else {
      std::cerr << "ERROR: unable to open '" << nameRunScript << "' for writing" << std::endl;
      throw std::exception();
    }
  }

  // create submission script
  const TString nameSubScript = outdir+"_sub.sh";
  std::ofstream out(nameSubScript);
  if( out.is_open() ) {
      
    out << "#!/bin/bash\n\n";
    out << "for i in";
    for(auto& experimentDir: experimentDirs) {
      out << " " << experimentDir;
    }
    out << "; do\n";
    out << "    cd ${i}\n";
    out << "    qsub -q default.q -l h=bird* -hard -l os=sld6 -l h_vmem=2000M -l s_vmem=2000M -cwd -S /bin/bash -V -o log.out -e log.err " << nameRunScript << "\n";
    out << "    cd -\n";
    out << "done\n";

    out.close();
      
  } else {
    std::cerr << "ERROR: unable to open '" << nameRunScript << "' for writing" << std::endl;
    throw std::exception();
  }
  gSystem->Exec("chmod u+x "+nameSubScript);

  
  std::cout << "Done" << std::endl;
  std::cout << "  project in " << outdir << std::endl;
  std::cout << "  submit fits with './" << nameSubScript << "'" << std::endl;
}
