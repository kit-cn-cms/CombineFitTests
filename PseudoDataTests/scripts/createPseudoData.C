#include <fstream>
#include <vector>

#include "TSystem.h"
#include "TROOT.h"

#include "Category.h"
#include "DataCardModifier.h"
#include "Process.h"
#include "PseudoDataGenerator.h"
#include "StringOperations.h"

const TString workdir = "/nfs/dust/cms/user/pkeicher/tth_analysis_study/Spring17_v22/newkit_ljets_v22/";
const TString datacard = workdir+"limits_All_v22_datacard_ljets_jge6_t3_hdecay_majorProcs_noBinByBin_theoryNP.txt";
const TString templatesNominal = workdir+"common/ttH_hbb_13TeV_sl.root";
// const TString templatesTTBB = workdir+"datacards/limits_Spring17v2p2_ttbarincl/limits_Spring17v2p2_ttbarincl_limitInput.root";
const TString templatesTTBB = templatesNominal;
const TString CMSSW_BASE = "/nfs/dust/cms/user/pkeicher/CMSSW_7_4_7";
const TString combineCmd = "combine -M MaxLikelihoodFit -m 125 --minimizerStrategy 0 --minimizerTolerance 0.00001 --saveNormalizations --saveShapes --minos all";


void generatePseudoData(const TString& outdir,
		      const int nExperiments,
		      const double expectSignal=1.,
		      const double scanIntervallSize = 5.)
{

  std::vector<Category::Type> categories = { 
               Category::SL_44,
					     Category::SL_54,
					     Category::SL_63,
					     Category::SL_64
					    };

  std::vector<Process> processes;

  if( expectSignal > 0 ) {
    processes.push_back( Process(Process::ttHbb,templatesNominal,expectSignal) );
    //processes.push_back( Process(Process::ttHcc,templatesNominal,expectSignal) );
    //processes.push_back( Process(Process::ttHgg,templatesNominal,expectSignal) );
    //processes.push_back( Process(Process::ttHgluglu,templatesNominal,expectSignal) );
    //processes.push_back( Process(Process::ttHtt,templatesNominal,expectSignal) );
    //processes.push_back( Process(Process::ttHww,templatesNominal,expectSignal) );
    //processes.push_back( Process(Process::ttHzg,templatesNominal,expectSignal) );
    //processes.push_back( Process(Process::ttHzz,templatesNominal,expectSignal) );
  }

  processes.push_back( Process(Process::ttlf,templatesNominal) );
  processes.push_back( Process(Process::ttcc,templatesNominal) );

  processes.push_back( Process(Process::ttbb,templatesTTBB) );
  processes.push_back( Process(Process::ttb,templatesTTBB) );
  processes.push_back( Process(Process::tt2b,templatesTTBB) );

  std::cout << "Reading processes" << std::endl;
  for(auto& category: categories) {
    for(auto& process: processes) {
      process.addCategory(category);
    }
  }


  // create pseudo data
  std::cout << "Creating pseudo data" << std::endl;
  PseudoDataGenerator generator;
  //  generator.usePoissonStatistics(false); // for debugging, do not smear

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
  TString finalCombineCmd;
  finalCombineCmd.Form(" --rMin=%.2f --rMax=%.2f", expectSignal-scanIntervallSize, expectSignal+scanIntervallSize);
  finalCombineCmd.Prepend(combineCmd);

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
      out << "cd -\n\n";
      out << "cd " << experimentDir << "\n";
      out << "echo '" << finalCombineCmd << " " << datacardName << "'\n";
      out << finalCombineCmd << " " << datacardName << "\n";
      out << "rm higgsCombineTest.MaxLikelihoodFit*.root\n";
      out << "cd -\n";

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

void createPseudoData(TString outdir,
		      const int nExperiments,
		      const double scanIntervallSize = 5.)
{
	if(!gSystem->cd(outdir.Data())){
		gSystem->mkdir(outdir.Data());
		gSystem->cd(outdir.Data());
	}
	double signalStrengths[2] = {0.0, 1.0};
	TString outputDir;
	if(outdir.EndsWith("/")) outdir.Chop();
	for(int i=0; i<2;i++){
		outputDir.Form("/sig%0.1f", signalStrengths[i]);
		outputDir.Prepend(outdir);
		generatePseudoData(outputDir, nExperiments, signalStrengths[i], scanIntervallSize);
	}
}
