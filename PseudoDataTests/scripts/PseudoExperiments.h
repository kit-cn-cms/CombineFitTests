#ifndef PSEUDO_EXPERIMENTS_H
#define PSEUDO_EXPERIMENTS_H

#include <exception>
#include <iostream>
#include <map>
#include <set>
#include <vector>

#include "TSystemFile.h"
#include "TSystemDirectory.h"
#include "TIterator.h"
#include "TList.h"

#include "TFile.h"
#include "TH1.h"
#include "TTree.h"
#include "TBranch.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TString.h"
#include "TUUID.h"
#include "TStopwatch.h"

#include "RooArgSet.h"
#include "RooFitResult.h"
#include "RooRealVar.h"

#include "ShapeContainer.h"


class PseudoExperiments {
public:
  PseudoExperiments(const TString& label, const double injectedMu);
  ~PseudoExperiments();

  TString operator()() const { return label_; }

  void addExperiment(const TString& mlfit);
  void addExperiments(TString& sourceDir, const TString& mlfitFile = "mlfit.root");
  void setColor(const int c) {
    color_ = c;
  }

  TH1* mu() const {
    return getClone(muValues_);
  }
  double muMean() const {
    return muValues_->GetMean();
  }
  double muRMS() const {
    return muValues_->GetRMS();
  }
  double muError() const{
    return muErrors_->GetMean();
  }
  double muInjected() const {
    return injectedMu_;
  }

  size_t npsN() const { return nps_.size(); }
  const std::vector<TString>& nps() const { return nps_; }
  std::vector<TString>::const_iterator npsBegin() const { return nps_.begin(); }
  std::vector<TString>::const_iterator npsEnd() const { return nps_.end(); }
  TH1* npPrefit(const TString& np) const {
    return getClone(getHist(npValuesPrefit_,np));
  }
  double npPrefitMean(const TString& np) const {
    double returnVal = -9999;
    TH1* tempPointer = getHist(npValuesPrefit_,np);
    if(tempPointer != NULL) returnVal = tempPointer->GetMean();
    return returnVal;
  }
  double npPrefitRMS(const TString& np) const {
    return getHist(npValuesPrefit_,np)->GetRMS();
  }
  TH1* npPostfitB(const TString& np) const {
    return getClone(getHist(npValuesPostfitB_,np));
  }
  TH1* npPostfitBerrorDist(const TString& np) const {
    return getClone(getHist(npValuesPostfitBerrors_,np));
  }
  TH1* npPostfitBerrorHiDist(const TString& np) const {
    return getClone(getHist(npValuesPostfitBerrorHi_,np));
  }
  TH1* npPostfitBerrorLoDist(const TString& np) const {
    return getClone(getHist(npValuesPostfitBerrorLo_,np));
  }
  double npPostfitBMean(const TString& np) const {
    return getHist(npValuesPostfitB_,np)->GetMean();
  }
  double npPostfitBRMS(const TString& np) const {
    return getHist(npValuesPostfitB_,np)->GetRMS();
  }
  double npPostfitBerrorHi(const TString& np) const{
      return getHist(npValuesPostfitBerrorHi_,np)->GetMean();
  }
  double npPostfitBerrorLo(const TString& np) const{
      return getHist(npValuesPostfitBerrorLo_,np)->GetMean();
  }
  double npPostfitBerror(const TString& np) const{
      return getHist(npValuesPostfitBerrors_,np)->GetMean();
  }



  TH1* npPostfitS(const TString& np) const {
    return getClone(getHist(npValuesPostfitS_,np));
  }
  TH1* npPostfitSerrorDist(const TString& np) const {
    return getClone(getHist(npValuesPostfitSerrors_,np));
  }
  TH1* npPostfitSerrorHiDist(const TString& np) const {
    return getClone(getHist(npValuesPostfitSerrorHi_,np));
  }
  TH1* npPostfitSerrorLoDist(const TString& np) const {
    return getClone(getHist(npValuesPostfitSerrorLo_,np));
  }
  double npPostfitSMean(const TString& np) const {
    return getHist(npValuesPostfitS_,np)->GetMean();
  }
  double npPostfitSRMS(const TString& np) const {
    return getHist(npValuesPostfitS_,np)->GetRMS();
  }
  double npPostfitSerrorHi(const TString& np) const{
      return getHist(npValuesPostfitSerrorHi_,np)->GetMean();
  }
  double npPostfitSerrorLo(const TString& np) const{
      return getHist(npValuesPostfitSerrorLo_,np)->GetMean();
  }
  double npPostfitSerror(const TString& np) const{
      return getHist(npValuesPostfitSerrors_,np)->GetMean();
  }

  int color() const { return color_; }

  void print() const {
    for(auto& str: nps_) {
      std::cout << str << std::endl;
      std::cout << npPostfitSMean(str) << std::endl;
    }
    std::cout << muMean() << std::endl;
  }
  std::vector<ShapeContainer*> getPrefitShapes() const{
    return prefitShapes_;
  }
  std::vector<ShapeContainer*> getPostfitBshapes()const{
    return postfitBshapes_;
  }
  std::vector<ShapeContainer*> getPostfitSshapes()const{
    return postfitSshapes_;
  }

  TH1* correlationPostfitBDist(TString np1, TString np2)const {
    std::map<TString, std::map<TString,TH1*> >::const_iterator it = correlationsPostfitB_.find(np1);
    if(it != correlationsPostfitB_.end()){
      return getClone(getHist(it->second, np2));
    }
    else{
      std::cerr << "Could not find distribution for correlations of nuisance parameter" << np1 << std::endl;
      return NULL;
    }
  }

  TH1* correlationPostfitSDist(TString np1, TString np2)const {
    std::map<TString, std::map<TString,TH1*> >::const_iterator it = correlationsPostfitS_.find(np1);
    if(it != correlationsPostfitB_.end()){
      return getClone(getHist(it->second, np2));
    }
    else{
      std::cerr << "Could not find distribution for correlations of nuisance parameter" << np1 << std::endl;
      return NULL;
    }
  }

  TH2D* getCorrelationPlotPostfitB() const{
    return getCorrelationPlot(correlationsPostfitB_);
  }
  TH2D* getCorrelationPlotPostfitS() const{
    return getCorrelationPlot(correlationsPostfitS_);
  }

private:
  bool debug_;
  bool fitBmustConverge_;
  bool fitSBmustConverge_;
  bool accurateCovariance_;

  TString label_;
  double injectedMu_;
  int color_;

  static constexpr int nBins_ = 400;
  static constexpr double min_ = -10;
  static constexpr double max_ = 10;

  TH1* muValues_;
  TH1* muErrors_;
  std::vector<TString> nps_;
  std::map<TString,TH1*> npValuesPrefit_;

  std::map<TString,TH1*> npValuesPostfitB_;
  std::map<TString,TH1*> npValuesPostfitBerrors_;
  std::map<TString,TH1*> npValuesPostfitBerrorHi_;
  std::map<TString,TH1*> npValuesPostfitBerrorLo_;


  std::map<TString,TH1*> npValuesPostfitS_;
  std::map<TString,TH1*> npValuesPostfitSerrors_;
  std::map<TString,TH1*> npValuesPostfitSerrorHi_;
  std::map<TString,TH1*> npValuesPostfitSerrorLo_;

  std::vector<ShapeContainer*> prefitShapes_;
  std::vector<ShapeContainer*> postfitBshapes_;
  std::vector<ShapeContainer*> postfitSshapes_;

  std::map<TString, std::map<TString,TH1*> > correlationsPostfitB_;
  std::map<TString, std::map<TString,TH1*> > correlationsPostfitS_;


  void initContainers(TFile* file, RooFitResult* result, int nBins = nBins_, double min = min_, double max = max_);
  void createHistograms(std::map<TString,TH1*>& hists, const std::vector<TString>& nps, const TString& name, int nBins = nBins_, double min = min_, double max = max_) const;
  TH1* createHistogram(const TString& par, const TString& name, int nBins = nBins_, double min = min_, double max = max_) const;
  void storeRooArgSetResults(std::map<TString,TH1*>& hists, TFile* file, const TString& name) const;
  void storeRooFitResults(std::map<TString,TH1*>& hists, std::map<TString,TH1*>& hErrors, std::map<TString,TH1*>& hErrorsHi, std::map<TString, TH1*>& hErrorsLo, TFile* file, const TString& name, std::map<TString, std::map<TString, TH1*> >& correlations);
  TH1* getHist(const std::map<TString,TH1*>& hists, const TString& key) const;
  TH1* getClone(const TH1* h) const;
  bool checkFitStatus(TFile* file);
  bool checkCovarianceMatrix(TFile* file);
  void storeShapes(std::vector<ShapeContainer*>& shapes, TFile* file, const TString& name) const;
  TH2D* getCorrelationPlot(const std::map<TString, std::map<TString,TH1*> >& correlations) const;
  void collectCorrelations(std::map<TString, std::map<TString,TH1*> >& correlations, RooFitResult* result) const;
};

void printTime(TStopwatch& watch, TString text){
  std::cout << text.Data() << "\n\tReal Time: " << watch.RealTime() << "\tCPU Time: " << watch.CpuTime() << std::endl;

}

PseudoExperiments::PseudoExperiments(const TString& label, const double injectedMu)
: debug_(false),
fitBmustConverge_(true),
fitSBmustConverge_(true),
accurateCovariance_(true),
label_(label), injectedMu_(injectedMu), color_(kGray),
muValues_(0) {
  if( debug_ ) std::cout << "DEBUG " << this << ": constructor" << std::endl;
}

PseudoExperiments::~PseudoExperiments() {
  if( debug_ ) std::cout << "DEBUG " << this << ": destructor" << std::endl;
  // for(std::map<TString,TH1*>::const_iterator it = npValuesPrefit_.begin(); it!= npValuesPrefit_.end(); it++) delete it->second;
  // for(std::map<TString,TH1*>::const_iterator it = npValuesPostfitB_.begin(); it!= npValuesPostfitB_.end(); it++) delete it->second;
  // for(std::map<TString,TH1*>::const_iterator it = npValuesPostfitS_.begin(); it!= npValuesPostfitS_.end(); it++) delete it->second;
  //
  // // for(auto& it: npValuesPrefit_) {
  // //   delete it.second;
  // // }
  // // for(auto& it: npValuesPostfitB_) {
  // //   delete it.second;
  // // }
  // // for(auto& it: npValuesPostfitS_) {
  // //   delete it.second;
  // // }
  // if( muValues_ != 0 ) delete muValues_;
  // for(int i=0; i<int(prefitShapes_.size());i++) delete prefitShapes_[i];
  // for(int i=0; i<int(postfitBshapes_.size());i++) delete postfitBshapes_[i];
  // for(int i=0; i<int(postfitSshapes_.size());i++) delete postfitSshapes_[i];

}


void PseudoExperiments::addExperiment(const TString& mlfit) {
  if( debug_ ) std::cout << "DEBUG: addExperiment: " << mlfit << std::endl;
  TFile* file = new TFile(mlfit,"READ");
  if( !file->IsOpen() || file->IsZombie()) {
    std::cerr << "ERROR opening file '" << mlfit << "'" << std::endl;
    //throw std::exception();
  }
  else {
    //check if the s+b and the b fit converged
    if(!file->TestBit(TFile::kRecovered))
    {
      bool storeExperiment = true;
      TStopwatch overallTime;
      overallTime.Start();
      if(fitSBmustConverge_ || fitBmustConverge_) storeExperiment = checkFitStatus(file);
      if(storeExperiment && accurateCovariance_) storeExperiment = checkCovarianceMatrix(file);
      // store POI value
      if(storeExperiment)
      {
        TStopwatch watch;

        if( muValues_ == 0 ) {
          watch.Start();
          muValues_ = createHistogram("mu","postfitS");
          muErrors_ = createHistogram("muErrors", "postfitS");
          watch.Stop();
          if(debug_) printTime(watch, "Time to create muValues_ and muErrors_");
        }
        if( debug_ ) std::cout << "  DEBUG: getting mu" << std::endl;
        RooFitResult* result = NULL;//(RooFitResult*)file->Get("fit_s");
        TTree* result_tree = (TTree*)file->Get("tree_fit_sb");
        watch.Start();
        //file->GetObject("fit_s",result);
        watch.Stop();
        if(debug_) printTime(watch, "Time to load RooFitResult Object");

        if( result == 0) {
          //std::cerr << "ERROR getting 'fit_s' from file '" << file->GetName() << "'" << std::endl;
          //throw std::exception();
        }
        else{
          watch.Start();
          const RooRealVar* var = static_cast<RooRealVar*>( result->floatParsFinal().find("r") );
          if(var != 0)
          {

            if( debug_ ) std::cout << "    DEBUG: found mu = " << var->getVal() << std::endl;
            muValues_->Fill( var->getVal() );
            muErrors_->Fill( var->getError() );
            watch.Stop();
            if(debug_) printTime(watch, "Time to get mu and fill Histos");

            // if called the first time, get list of NPs
            if( nps_.empty() ) {
              if( debug_ ) std::cout << "  DEBUG: initialize NPs" << std::endl;
              //initContainers(file);
            }
            // store nuisance parameter values
            // if( debug_ ) std::cout << "  DEBUG: store NPs" << std::endl;
            // if( debug_ ) std::cout << "    DEBUG: prefit NPs" << std::endl;
            // storeRooArgSetResults(npValuesPrefit_,file,"nuisances_prefit");
            // if( debug_ ) std::cout << "    DEBUG: postfit B NPs" << std::endl;
            // storeRooFitResults(npValuesPostfitB_, npValuesPostfitBerrors_, npValuesPostfitBerrorHi_, npValuesPostfitBerrorLo_, file,"fit_b");
            // if( debug_ ) std::cout << "    DEBUG: postfit S NPs" << std::endl;
            // storeRooFitResults(npValuesPostfitS_,npValuesPostfitSerrors_, npValuesPostfitSerrorHi_, npValuesPostfitSerrorLo_, file,"fit_s");
            // if( debug_ ) std::cout << "  DEBUG: done storing NPs" << std::endl;
            // if(debug_) std::cout << "  DEBUG: store shapes\n";
            // if(debug_) std::cout << "\tDEBUG: prefit shapes\n";
            // storeShapes(prefitShapes_, file, "shapes_prefit");
            // if(debug_) std::cout << "\tDEBUG: postfit B shapes\n";
            // storeShapes(postfitBshapes_, file, "shapes_fit_b");
            // if(debug_) std::cout << "\tDEBUG: postfit S shapes\n";
            // storeShapes(postfitSshapes_, file, "shapes_fit_s");
            // if( debug_ ) std::cout << "  DEBUG: done storing shapes" << std::endl;
            delete var;
          }

          //result->Delete();
        }
        if(result_tree != 0){
          double muVal = 0;
          double muError = 0;
          if(result_tree->SetBranchAddress("mu", &muVal)>=0 && result_tree->SetBranchAddress("muErr", &muError) >= 0){
            for(int i=0; i<result_tree->GetEntries(); i++){
              result_tree->GetEntry(i);
              muValues_->Fill(muVal);
              muErrors_->Fill(muError);
            }
          }

          // store nuisance parameter values
          if( debug_ ) std::cout << "  DEBUG: store NPs" << std::endl;
          if( debug_ ) std::cout << "    DEBUG: postfit B NPs" << std::endl;
          storeRooFitResults(npValuesPostfitB_, npValuesPostfitBerrors_, npValuesPostfitBerrorHi_, npValuesPostfitBerrorLo_, file,"fit_b", correlationsPostfitB_);
          if( debug_ ) std::cout << "    DEBUG: prefit NPs" << std::endl;
          storeRooArgSetResults(npValuesPrefit_,file,"tree_fit_b");
          if( debug_ ) std::cout << "    DEBUG: postfit S NPs" << std::endl;
          storeRooFitResults(npValuesPostfitS_,npValuesPostfitSerrors_, npValuesPostfitSerrorHi_, npValuesPostfitSerrorLo_, file,"fit_s", correlationsPostfitS_);
          if( debug_ ) std::cout << "  DEBUG: done storing NPs" << std::endl;

          if(debug_) std::cout << "  DEBUG: store shapes\n";
          if(debug_) std::cout << "\tDEBUG: prefit shapes\n";
          storeShapes(prefitShapes_, file, "shapes_prefit");
          if(debug_) std::cout << "\tDEBUG: postfit B shapes\n";
          storeShapes(postfitBshapes_, file, "shapes_fit_b");
          if(debug_) std::cout << "\tDEBUG: postfit S shapes\n";
          storeShapes(postfitSshapes_, file, "shapes_fit_s");
          if( debug_ ) std::cout << "  DEBUG: done storing shapes" << std::endl;
          delete result_tree;
        }
      }
      else std::cout << "skipping experiment in '" << mlfit.Data() << "'\n";
      overallTime.Stop();
      if(debug_) printTime(overallTime, "Time it took to process " + mlfit);
    }
    else std::cerr << "  ERROR while opening file " << mlfit.Data() << ", skipping...\n";
  }
  file->Close();
  delete file;
}


void PseudoExperiments::addExperiments(TString& sourceDir, const TString& mlfitFile){
  /*
  Input:
  sourceDir: directory which contains PseudoExperiment folders
  mlfitFile: .root file which contains the fit results from the combine fit
  */
  //load PseudoExperiment folders
  TSystemDirectory dir(sourceDir.Data(), sourceDir.Data());
  TList *folders = dir.GetListOfFiles();
  int counter = 1;
  //if folders are found, go through each one an look for the mlfitFile
  if (folders) {
    TSystemFile *pseudoExperimentFolder;
    TString folderName;
    TIter next(folders);
    TStopwatch watch;
    while ((pseudoExperimentFolder=(TSystemFile*)next())) {
      watch.Start();
      folderName = pseudoExperimentFolder->GetName();
      if (pseudoExperimentFolder->IsFolder() && !folderName.EndsWith(".") && !folderName.Contains("asimov")) {
        if(sourceDir.EndsWith("/")) sourceDir.Chop();
        if(debug_) std::cout << "DEBUG    ";
        if(debug_ || counter%10==0) std::cout << "Adding PseudoExperiment #" << counter << std::endl;
        addExperiment(sourceDir+"/"+folderName+"/"+mlfitFile);
        counter++;
      }
      watch.Stop();
      if(debug_) printTime(watch, "Time for last loop");
    }
    delete pseudoExperimentFolder;
    delete folders;
  }
}

bool PseudoExperiments::checkFitStatus(TFile* file){
  bool storeExperiment = true;
  int fit_status=7;
  TString fit_trees[2] = {"tree_fit_sb", "tree_fit_b"};
  bool fit_flags[2] = {fitSBmustConverge_, fitBmustConverge_};
  for(int nTrees=0; nTrees<2; nTrees++)
  {
    TTree* tree = (TTree*)file->Get(fit_trees[nTrees].Data());
    if(tree != NULL)
    {
      if(tree->SetBranchAddress("fit_status",&fit_status)>= 0)
      {
        tree->GetEntry(0);
        if((fit_status != 0)){
          std::cout << "WARNING fit_status in " << fit_trees[nTrees].Data() << " did not converge!\n";
          if(fit_flags[nTrees]) storeExperiment = false;
        }
      }
      delete tree;
      fit_status=7;
    }
    else{
      std::cerr << "ERROR   could not load tree " << fit_trees[nTrees].Data() << " from file " << file->GetName() << std::endl;
      storeExperiment = false;
    }
  }
  return storeExperiment;
}

bool PseudoExperiments::checkCovarianceMatrix(TFile* file){
  bool accurateCovariance = false;
  TString rooFitObjects[2] = {"fit_b", "fit_s"};
  int quality = -1;
  bool qualities[2] = {false, false};
  for(int currentObject=0; currentObject < 2; currentObject++){
    RooFitResult* fitObject = 0;
    file->GetObject(rooFitObjects[currentObject].Data(),fitObject);
    if( fitObject == 0 ) {
      std::cerr << "ERROR getting '" << rooFitObjects[currentObject].Data() << "' from file '" << file->GetName() << "'" << std::endl;
      //throw std::exception();
    }
    else{
      quality=-1;
      quality=fitObject->covQual();
      if(debug_)
      {
        std::cout << "    DEBUG: quality of covariance matrix in " << rooFitObjects[currentObject].Data() << " is " << quality;
        if(quality==-1) std::cout << rooFitObjects[currentObject].Data() << ": Unknown, matrix was externally provided\n";
        if(quality==0) std::cout << rooFitObjects[currentObject].Data() << ": Not calculated at all\n";
        if(quality==1) std::cout << rooFitObjects[currentObject].Data() << ": Approximation only, not accurate\n";
        if(quality==2) std::cout << rooFitObjects[currentObject].Data() << ": Full matrix, but forced positive-definite\n";
      }
      if(quality==3) {
        if(debug_) std::cout << rooFitObjects[currentObject].Data() << ": Full, accurate covariance matrix\n";
        qualities[currentObject] = true;
      }
    }
    delete fitObject;
  }
  if(!fitSBmustConverge_ && !fitBmustConverge_) return true;
  if(fitBmustConverge_){
    if(qualities[0]) accurateCovariance = true;
  }
  if(fitSBmustConverge_){
    if(qualities[1]) accurateCovariance = true;
    else accurateCovariance = false;
  }
  return accurateCovariance;
}

void PseudoExperiments::initContainers(TFile* file, RooFitResult* result, int nBins, double min, double max) {
  // RooFitResult* fit_b = 0;
  // file->GetObject("fit_b",fit_b);
  // if( fit_b == 0 ) {
  //   std::cerr << "ERROR getting 'fit_b' from file '" << file->GetName() << "'" << std::endl;
  //   //throw std::exception();
  // }
  // else{
    TIter it = result->floatParsInit().createIterator();
    RooRealVar* var = NULL;
    TString varName;
    while( (var =  static_cast<RooRealVar*>(it.Next())) ) {
      varName = var->GetName();
      nps_.push_back( varName );
      //var = static_cast<RooRealVar*>(it.Next());
      delete var;
    }

    createHistograms(npValuesPrefit_,nps_,"prefit");
    createHistograms(npValuesPostfitB_,nps_,"postfitB");
    createHistograms(npValuesPostfitBerrors_, nps_, "postfitB_errors");
    createHistograms(npValuesPostfitBerrorHi_, nps_, "postfitB_errorHi");
    createHistograms(npValuesPostfitBerrorLo_, nps_, "postfitB_errorLo");
    createHistograms(npValuesPostfitS_,nps_,"postfitS");
    createHistograms(npValuesPostfitSerrors_,nps_,"postfitS_errors");
    createHistograms(npValuesPostfitSerrorHi_, nps_, "postfitS_errorHi");
    createHistograms(npValuesPostfitSerrorLo_, nps_, "postfitS_errorLo");
  }
//}


void PseudoExperiments::createHistograms(std::map<TString,TH1*>& hists, const std::vector<TString>& nps, const TString& name, int nBins, double min, double max) const {
  for(auto& np: nps) {
    hists[np] = createHistogram(np,name);
  }
}


TH1* PseudoExperiments::createHistogram(const TString& par, const TString& name, int nBins, double min, double max) const {
  if( debug_ ) std::cout << "DEBUG: createHistogram: " << par << ", " << name << std::endl;
  const TUUID id;
  TH1* h = new TH1D(label_+":"+par+":"+name+":"+id.AsString(),name+";"+par+";N(experiments)",nBins_,min_,max_);
  h->SetDirectory(0);
  return h;
}


void PseudoExperiments::storeRooArgSetResults(std::map<TString,TH1*>& hists, TFile* file, const TString& name) const {
  // RooArgSet* result = (RooArgSet*)file->Get(name.Data());
  //
  // //file->GetObject(name,result);
  // if( result == 0 ) {
  //   std::cerr << "ERROR getting '" << name << "' from file '" << file->GetName() << "'" << std::endl;
  //   //throw std::exception();
  // }
  // else{
  //   for(auto& it: hists) {
  //     //std::cout << "filling " << it.first << " with value " << result->getRealValue(it.first) << std::endl;
  //     it.second->Fill( result->getRealValue(it.first) );
  //   }
  // }
  TTree* results_tree = (TTree*)file->Get(name.Data());
  if(results_tree != NULL){
    double vals = 0;
    TString currentValue;
    for(auto& it: hists){
      currentValue = it.first;
      currentValue.Append("_In");
      if(results_tree->SetBranchAddress(currentValue.Data(), &vals)>=0){
        for(int i=0; i<results_tree->GetEntries(); i++){
          results_tree->GetEntry(i);
          it.second->Fill(vals);
        }
        results_tree->ResetBranchAddresses();
      }
      else{
        if(debug_) std::cerr << "\tError      could not find branch " << currentValue.Data() << " in TTree " << name.Data() << std::endl;
      }
    }

    delete results_tree;
  }
}


void PseudoExperiments::storeRooFitResults(std::map<TString,TH1*>& hists, std::map<TString, TH1*>& hErrors, std::map<TString,TH1*>& hErrorsHi, std::map<TString,TH1*>& hErrorsLo, TFile* file, const TString& name, std::map<TString, std::map<TString, TH1*> >& correlations) {
  RooFitResult* result = (RooFitResult*)file->Get(name.Data());

  //file->GetObject(name,result);
  if( result == 0 ) {
    std::cerr << "ERROR getting '" << name << "' from file '" << file->GetName() << "'" << std::endl;
    //throw std::exception();
  }
  else{
    TStopwatch watch;
    if( nps_.empty() ) {
      if( debug_ ) std::cout << "  DEBUG: initialize NPs" << std::endl;
      initContainers(file, result);
    }
    for(auto& it: hists) {
      watch.Start();
      const RooRealVar* var = static_cast<RooRealVar*>( result->floatParsFinal().find( it.first ) );
      watch.Stop();
      if(debug_) printTime(watch, "Time to get RooRealVar Object");
      //std::cout << "filling " << it.first << " with value " << var->getVal() << std::endl;
      it.second->Fill(var->getVal());

      //watch.Start();
      std::map<TString, TH1*>::const_iterator iter_errorHi = hErrorsHi.find( it.first);
      //watch.Stop();
      //if(debug_) printTime(watch, "Time to find "+it.first+" in errorHi map");
      //watch.Start();
      iter_errorHi->second->Fill(var->getErrorHi());
      //watch.Stop();
      //if(debug_) printTime(watch, "Time to fill histo in errorHi");
      //watch.Start();
      std::map<TString, TH1*>::const_iterator iter_errorLo = hErrorsLo.find( it.first);
      //watch.Stop();
      //if(debug_) printTime(watch, "time to find "+it.first+"in errorLo");
      //watch.Start();
      iter_errorLo->second->Fill(var->getErrorLo());

      //watch.Stop();
      //if(debug_) printTime(watch, "Time to fill histo in errorLo");
      //watch.Start();
      std::map<TString, TH1*>::const_iterator iter_errors = hErrors.find( it.first);
      //watch.Stop();
      //if(debug_) printTime(watch, "time to find "+it.first+"in errors");
      //watch.Start();
      iter_errors->second->Fill(var->getError());
      //watch.Stop();
      //if(debug_) printTime(watch, "Time to fill histo in errors");
      //delete var;
    }
    collectCorrelations(correlations, result);
  }
  // TTree* result_tree = (TTree*)file->Get(name.Data());
  // if(result_tree != NULL){
  //   double vals = 0;
  //   TString currentValue;
  //   for(auto& it: hists){
  //     currentValue = it.first;
  //     if(results_tree->SetBranchAddress(currentValue.Data(), &vals)>=0){
  //       for(int i=0; i<results_tree->GetEntries(); i++){
  //         results_tree->GetEntry(i);
  //         it.second->Fill(vals);
  //       }
  //       results_tree->ResetBranchAddresses();
  //     }
  //     else{
  //       if(debug_) std::cerr << "\tError      could not find branch " << currentValue.Data() << " in TTree " << name.Data() << std::endl;
  //     }
  //   }
  //
  //   delete results_tree;
  // }
}


TH1* PseudoExperiments::getHist(const std::map<TString,TH1*>& hists, const TString& key) const {
  TH1* returnPointer = NULL;
  std::map<TString,TH1*>::const_iterator it = hists.find(key);
  if( it == hists.end() ) {
    std::cerr << "ERROR trying to access histogram for '" << key << "' in '" << label_ << "'" << std::endl;
    //throw std::exception();
  }
  else returnPointer = it->second;
  return returnPointer;
}


TH1* PseudoExperiments::getClone(const TH1* h)const {
  if(h!=NULL){
    const TUUID id;
    const TString name = h->GetName();
    return static_cast<TH1*>(h->Clone(name+":"+id.AsString()));
  }
  else std::cerr << "ERROR cloning histogram!\n";
  return NULL;
}

void PseudoExperiments::storeShapes(std::vector<ShapeContainer*>& shapes, TFile* file, const TString& name) const
{
  if(file->cd(name))
  {
    TDirectory* shapeFolder = gDirectory;
    TIter nextFileObject(shapeFolder->GetListOfKeys());
    TKey* dirKey;
    bool createNewContainer = true;
    TString categoryName;
    TString signalStrength = label_;
    signalStrength.ReplaceAll(" ", "_");
    signalStrength.ReplaceAll("=", "");

    while ((dirKey = (TKey*)nextFileObject()) && dirKey->IsFolder()) {
      categoryName = dirKey->GetName();
      createNewContainer = true;
      for(int nShapeContainer = 0; nShapeContainer<int(shapes.size()); nShapeContainer++)
      {
        if(shapes[nShapeContainer]->getName() == categoryName)
        {
          if(debug_) std::cout << "DEBUG    found container with matching category name!\n";
          shapes[nShapeContainer]->loadShapes(*file, name, signalStrength);
          createNewContainer = false;
        }
      }
      if(createNewContainer)
      {
        ShapeContainer* tempShapeContainer = new ShapeContainer(categoryName);
        tempShapeContainer->loadShapes(*file, name, signalStrength);
        shapes.push_back(tempShapeContainer);
      }
    }
  }
  else std::cerr << "ERROR    Could not change into directory " << name.Data() << " in file " << file->GetName() <<std::endl;
}


TH2D* PseudoExperiments::getCorrelationPlot(const std::map<TString, std::map<TString,TH1*> >& correlations) const{
  int nNps = int(correlations.size());
  TH1* tempHisto = NULL;
  const TUUID id;
  TString name;
  name.Form("correlationPlot_%s", id.AsString());
  TH2D* correlationPlot = new TH2D(name.Data(),"", nNps, 0, nNps, nNps, 0, nNps);
  int i=1;
  int j=1;
  for(std::map<TString, std::map<TString,TH1*> >::const_iterator it = correlations.begin(); it != correlations.end(); it++){

    correlationPlot->GetXaxis()->SetBinLabel(i, it->first);
    correlationPlot->GetYaxis()->SetBinLabel(i, it->first);
    j=i;
    for(std::map<TString, std::map<TString,TH1*> >::const_iterator sub_it = it; sub_it != correlations.end(); sub_it++){
      if(debug_) std::cout << "getting correlations for " << it->first << ", " << sub_it->first << std::endl;
      tempHisto = getHist(it->second, sub_it->first);
      if(tempHisto){
        if (debug_) std::cout << "setting bin content: (" << i << ", " << j << ") = " << tempHisto->GetMean() << std::endl;
        correlationPlot->SetBinContent(i,j, tempHisto->GetMean());
        correlationPlot->SetBinContent(j,i, tempHisto->GetMean());
        tempHisto = NULL;
      }
      else{
        correlationPlot->SetBinContent(i,j, -2);
        correlationPlot->SetBinContent(j,i, -2);
      }
      j++;
    }
    i++;
  }

  return correlationPlot;
}

void PseudoExperiments::collectCorrelations(std::map<TString, std::map<TString,TH1*> >& correlations, RooFitResult* result) const{
  std::vector<TString> values;
  TIter it = result->floatParsFinal().createIterator();
  RooRealVar* var = NULL;
  TString varName;
  if(debug_) std::cout << result << std::endl;
  if(debug_) std::cout << "collecting variable names from RooFitResult " << result->GetName() << std::endl;
  while( (var =  static_cast<RooRealVar*>(it.Next())) ) {
    varName = var->GetName();
    values.push_back(varName);
    //delete var;
  }
  if(debug_)std::cout << "\ndone" << std::endl;
  if(debug_)std::cout << "current size of correlation container: " << correlations.size() << std::endl;
  if(correlations.empty()){
    if(debug_) std::cout << "initializing correlation histos\n";
    TString name;
    for(auto& varName : values){
      const TUUID id;
      name.Form("%s_correlations_%s", varName.Data(), id.AsString());
      createHistograms(correlations[varName], values, name, nBins_, -1.2, 1.2);
    }
  }
  for(auto& np_i : values){
    for(auto& np_j : values){
      correlations[np_i][np_j]->Fill(result->correlation(np_i, np_j));
    }
  }
}
#endif
