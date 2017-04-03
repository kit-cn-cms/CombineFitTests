#ifndef PSEUDO_DATA_GENERATOR_H
#define PSEUDO_DATA_GENERATOR_H

#include <map>
#include <vector>

#include "TH1.h"
#include "TRandom3.h"
#include "TString.h"

#include "Category.h"
#include "Process.h"


class PseudoDataGenerator {
public:
  PseudoDataGenerator()
    : outputDir_("."),
      outputDirPerExpSuffix_("PseudoData"),
      outputFileSuffix_("Data_Obs"),
      usePoissonStatistics_(true) {}
  
  void setOutputDirectory(const TString& dir) {
    outputDir_ = dir;
  }
  void setOutputDirectoryPerExperimentSuffix(const TString& dir) {
    outputDirPerExpSuffix_ = dir;
  }
  void setOutputFileSuffix(const TString& str) {
    outputFileSuffix_ = str;
  }

  void usePoissonStatistics(const bool smear) {
    usePoissonStatistics_ = smear;
  }
  
  std::vector<TString> run(const std::vector<Category::Type>& categories,
			   const std::vector<Process>& processes,
			   const unsigned int nExperiments) const;


private:
  TString outputDir_;
  TString outputDirPerExpSuffix_;
  TString outputFileSuffix_;
  bool usePoissonStatistics_;
  mutable TRandom3 rand_;
};


std::vector<TString> PseudoDataGenerator::run(const std::vector<Category::Type>& categories,
					      const std::vector<Process>& processes,
					      const unsigned int nExperiments) const {

  // check for consistent binning
  for(auto& category: categories) {
    const unsigned int nBins = processes.front().nBins(category);
    for(auto& process: processes) {
      if( process.nBins(category) != nBins ) {
	std::cerr << "ERROR: template for '" << Process::toString(process) << "' has different binning than others in '" << Category::toString(category) << "'" << std::endl;
	throw std::exception();
      }
    }
  }

  // create output dir
  if( gSystem->mkdir(outputDir_) != 0 ) {
    std::cerr << "ERROR creating working directory '" << outputDir_ << "'" << std::endl;
    throw std::exception();
  }

  // compute mean of background (+signal) per bin
  std::map< Category::Type, std::vector<double> > meansPerCategory;
  for(auto& category: categories) {
    const unsigned int nBins = processes.front().nBins(category);
    std::vector<double> means(nBins,0.);
    for(auto& process: processes) {
      for(unsigned int iBin = 0; iBin < nBins; ++iBin) {
	means.at(iBin) += process.binContent(category,iBin+1);
      }
    }
    meansPerCategory[category] = means;
  }

  // create pseudo data and store output
  std::vector<TString> resultDirs;
  for(unsigned int iExp = 0; iExp < nExperiments; ++iExp) {
    std::vector<TH1*> pseudoDataHists;

    for(auto& category: categories) {
      TString histName = Process::histName(Process::data,category);
      const unsigned int nBins = meansPerCategory[category].size();
      TH1* pseudoDataHist = new TH1D(histName,"",nBins,0,nBins);
      for(unsigned int iBin = 0; iBin < nBins; ++iBin) {
	const double mean = meansPerCategory[category].at(iBin);
	// convert into int
	const int meanInt = rand_.Uniform()>0.5 ? ceil(mean) : floor(mean); //trying for int casts
	const double val = usePoissonStatistics_ ? rand_.Poisson(meanInt) : mean;
	pseudoDataHist->SetBinContent(iBin+1,val);
      }
      pseudoDataHists.push_back(pseudoDataHist);
    }

    // create output dir
    char idx[10];
    sprintf(idx,"%03d",iExp+1);
    const TString outputDirPerExp = outputDirPerExpSuffix_+idx;
    const TString resultDir = outputDir_+"/"+outputDirPerExp;
    if( gSystem->mkdir(resultDir) != 0 ) {
      std::cerr << "ERROR creating working directory '" << resultDir << "'" << std::endl;
      throw std::exception();
    }
    resultDirs.push_back(resultDir);

    TFile outFile(resultDir+"/"+outputFileSuffix_+".root","RECREATE");
    for(auto& h: pseudoDataHists) {
      outFile.WriteTObject(h);
    }
    outFile.Close();
    for(auto& h: pseudoDataHists) {
      delete h;
    }
    pseudoDataHists.clear();
  }

  return resultDirs;
}





#endif
