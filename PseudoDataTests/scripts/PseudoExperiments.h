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
#include "RooAbsCollection.h"

#include "ShapeContainer.h"

Double_t checkValues(Double_t x){
    if(std::isnan(x) || std::isinf(x)) return 0;
    else return x;
}

class PseudoExperiments {
public:
  PseudoExperiments(const TString& label, const Double_t injectedMu);
  ~PseudoExperiments();

  TString operator()() const { return label_; }

  void addExperiment(const TString& mlfit);
  void addExperiments(TString& sourceDir, const TString& mlfitFile = "mlfit.root");
  void setColor(const int c) {
    color_ = c;
  }

  TH1* mu() const {
    std::cout << "original mu histo: " << muValues_ << std::endl;
    return getClone(muValues_);
  }
  Double_t muMean() const {
    if(muValues_ != NULL) return muValues_->GetMean();
    else return -9999;
  }
  Double_t muMeanError() const{
    if(muValues_ != NULL) return muValues_->GetMeanError();
    else return -9999;
  }
  Double_t muRMS() const {
    if(muValues_ != NULL) return muValues_->GetRMS();
    else return -9999;
  }
  Double_t muError() const{
    if(muValues_ != NULL) return muErrors_->GetMean();
    else return -9999;
  }
  Double_t muInjected() const {
    return injectedMu_;
  }

  size_t npsN() const { return nps_.size(); }
  const std::vector<TString>& nps() const { return nps_; }
  std::vector<TString>::const_iterator npsBegin() const { return nps_.begin(); }
  std::vector<TString>::const_iterator npsEnd() const { return nps_.end(); }
  TH1* npPrefit(const TString& np) const {
    return getClone(getHist(npValuesPrefit_,np));
  }
  Double_t npPrefitMean(const TString& np) const {
    Double_t returnVal = -9999;
    TH1* tempPointer = getHist(npValuesPrefit_,np);
    if(tempPointer != NULL) returnVal = tempPointer->GetMean();
    return returnVal;
  }
  Double_t npPrefitRMS(const TString& np) const {
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
  Double_t npPostfitBMean(const TString& np) const {
    return getHist(npValuesPostfitB_,np)->GetMean();
  }
  Double_t npPostfitBRMS(const TString& np) const {
    return getHist(npValuesPostfitB_,np)->GetRMS();
  }
  Double_t npPostfitBerrorHi(const TString& np) const{
      return getHist(npValuesPostfitBerrorHi_,np)->GetMean();
  }
  Double_t npPostfitBerrorLo(const TString& np) const{
      return getHist(npValuesPostfitBerrorLo_,np)->GetMean();
  }
  Double_t npPostfitBerror(const TString& np) const{
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
  Double_t npPostfitSMean(const TString& np) const {
    return getHist(npValuesPostfitS_,np)->GetMean();
  }
  Double_t npPostfitSRMS(const TString& np) const {
    return getHist(npValuesPostfitS_,np)->GetRMS();
  }
  Double_t npPostfitSerrorHi(const TString& np) const{
      return getHist(npValuesPostfitSerrorHi_,np)->GetMean();
  }
  Double_t npPostfitSerrorLo(const TString& np) const{
      return getHist(npValuesPostfitSerrorLo_,np)->GetMean();
  }
  Double_t npPostfitSerror(const TString& np) const{
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
  Double_t injectedMu_;
  int color_;

  static constexpr int nBins_ = 400;
  static constexpr Double_t min_ = -10;
  static constexpr Double_t max_ = 10;

  TH1* muValues_;
  TH1* muErrors_;
  std::vector<TString> nps_;
  std::map<TString,TH1*> npValuesPrefit_;
  std::map<TString,TH1*> npValuesPrefiterrors_;
  std::map<TString,TH1*> npValuesPrefiterrorHi_;
  std::map<TString,TH1*> npValuesPrefiterrorLo_;

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


  void initContainers(TFile* file,  const RooFitResult* result, int nBins = nBins_, Double_t min = min_, Double_t max = max_) ;
  void createHistograms(std::map<TString,TH1*>& hists, const std::vector<TString>& nps, const TString& name, int nBins = nBins_, Double_t min = min_, Double_t max = max_) const;
  TH1* createHistogram(const TString& par, const TString& name, int nBins = nBins_, Double_t min = min_, Double_t max = max_) const;
  void storePrefitValues(std::map<TString,TH1*>& hists, TFile* file) const;
  void storeRooFitResults(std::map<TString,TH1*>& hists, std::map<TString,TH1*>& hErrors, std::map<TString,TH1*>& hErrorsHi, std::map<TString, TH1*>& hErrorsLo, TFile* file, const RooFitResult* result, std::map<TString, std::map<TString, TH1*> >& correlations);
  void readRooRealVar(std::map<TString,TH1*>& hists, TIter it) const;
  void readRooRealVar(TH1* hist, const RooFitResult* result, const TString& currentVarName) const;

  TH1* getHist(const std::map<TString,TH1*>& hists, const TString& key) const;
  TH1* getClone(const TH1* h) const;
  bool checkFitStatus(TFile* file);
  bool checkCovarianceMatrix(TFile* file);
  void storeShapes(std::vector<ShapeContainer*>& shapes, TFile* file, const TString& name) const;
  TH2D* getCorrelationPlot(const std::map<TString, std::map<TString,TH1*> >& correlations) const;
  void collectCorrelations(std::map<TString, std::map<TString,TH1*> >& correlations, const RooFitResult* result) ;
};

void printTime(TStopwatch& watch, TString text){
  std::cout << text.Data() << "\n\tReal Time: " << watch.RealTime() << "\tCPU Time: " << watch.CpuTime() << std::endl;

}

PseudoExperiments::PseudoExperiments(const TString& label, const Double_t injectedMu)
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

void PseudoExperiments::readRooRealVar(std::map<TString,TH1*>& hists, TIter it) const{
  RooRealVar* param = NULL;

  // if(init) TIter it = result->floatParsInit().createIterator();
  // else TIter it = result->floatParsFinal().createIterator();
  while((param = (RooRealVar*)it.Next())){
    //Double_t value = param->getVal();
    if(debug_) std::cout << "looking for parameter " << param->GetName() <<std::endl;
    std::map<TString, TH1*>::const_iterator iter = hists.find(param->GetName());
    if(iter != hists.end())
    {
      if(debug_) std::cout << "\tfound it! Filling histo for parameter " << iter->first << "\n";
      iter->second->Fill(param->getVal());

    }
  }
}

void PseudoExperiments::readRooRealVar(TH1* hist, const RooFitResult* result, const TString& currentVarName) const{

  RooRealVar* param = NULL;

  param = (RooRealVar*)result->floatParsFinal().find( currentVarName.Data() );
  if(param != NULL){
    Double_t value = param->getVal();
    hist->Fill(value);
  }
}


void PseudoExperiments::addExperiment(const TString& mlfit) {
  if( debug_ ) std::cout << "DEBUG: addExperiment: " << mlfit << std::endl;
  TFile* file = new TFile(mlfit,"READ");
  if (file != NULL)
  {
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
          //TTree* result_tree = (TTree*)file->Get("tree_fit_sb");
          if( muValues_ == 0 ) {
            watch.Start();
            muValues_ = createHistogram("mu","postfitS");
            muErrors_ = createHistogram("muErrors", "postfitS");
            watch.Stop();
            if(debug_) printTime(watch, "Time to create muValues_ and muErrors_");
          }

          //RooFitResult* result = (RooFitResult*)file->Get("fit_s");
          // if(result_tree != 0){
          //   Double_t muVal = 0;
          //   Double_t muError = 0;
          //   if(result_tree->SetBranchAddress("mu", &muVal)>=0 && result_tree->SetBranchAddress("muErr", &muError) >= 0){
          //     for(int i=0; i<result_tree->GetEntries(); i++){
          //       result_tree->GetEntry(i);
          //       //if(debug_)
          //       std::cout << "filling mu histos with " << muVal << " +- " << muError << std::endl;
          //       muValues_->Fill(muVal);
          //       muErrors_->Fill(muError);
          //     }
          //   }

          // store nuisance parameter values
          if( debug_ ) std::cout << "  DEBUG: store NPs" << std::endl;
          if( debug_ ) std::cout << "    DEBUG: postfit B NPs" << std::endl;
          const RooFitResult* result = NULL;
          result = (RooFitResult*)file->Get("fit_b");

          if( result == 0 ) {
            std::cerr << "ERROR getting 'fit_b' from file '" << file->GetName() << "'" << std::endl;
            //throw std::exception();
          }
          else{
            storeRooFitResults(npValuesPostfitB_, npValuesPostfitBerrors_, npValuesPostfitBerrorHi_, npValuesPostfitBerrorLo_, file, result, correlationsPostfitB_);
            if( debug_ ) std::cout << "    DEBUG: prefit NPs" << std::endl;
            storePrefitValues(npValuesPrefit_, file);
          }
          if(result != NULL){
            if(debug_) std::cout << "deleting RooFitResult Pointer fit_b\n";

            delete result;
            result = NULL;
          }
          result = (RooFitResult*)file->Get("fit_s");

          if( result == 0 ) {
            std::cerr << "ERROR getting 'fit_s' from file '" << file->GetName() << "'" << std::endl;
            //throw std::exception();
          }
          else{
            RooRealVar* var = NULL;
            var = (RooRealVar*)result->floatParsFinal().find("r");
            if(var != NULL){
              Double_t muVal = var->getVal();
              Double_t muError = var->getError();
              if(debug_) std::cout << "filling mu histos with " << muVal << " +- " << muError << std::endl;

              muValues_->Fill(muVal);
              muErrors_->Fill(muError);

            }

            if( debug_ ) std::cout << "    DEBUG: postfit S NPs" << std::endl;
            storeRooFitResults(npValuesPostfitS_, npValuesPostfitSerrors_, npValuesPostfitSerrorHi_, npValuesPostfitSerrorLo_, file, result, correlationsPostfitS_);
            if( debug_ ) std::cout << "  DEBUG: done storing NPs" << std::endl;
          }
          if(result != NULL) {
            if(debug_) std::cout << "deleting RooFitResult Pointer fit_s";
            delete result;
          }

          if(debug_) std::cout << "  DEBUG: store shapes\n";
          if(debug_) std::cout << "\tDEBUG: prefit shapes\n";
          storeShapes(prefitShapes_, file, "shapes_prefit");
          if(debug_) std::cout << "\tDEBUG: postfit B shapes\n";
          storeShapes(postfitBshapes_, file, "shapes_fit_b");
          if(debug_) std::cout << "\tDEBUG: postfit S shapes\n";
          storeShapes(postfitSshapes_, file, "shapes_fit_s");
          if( debug_ ) std::cout << "  DEBUG: done storing shapes" << std::endl;
          //delete result_tree;
        }
        //}
        else std::cout << "skipping experiment in '" << mlfit.Data() << "'\n";
        overallTime.Stop();
        if(debug_) printTime(overallTime, "Time it took to process " + mlfit);
      }
      else std::cerr << "  ERROR while opening file " << mlfit.Data() << ", skipping...\n";
      file->Close();

    }
    if(file != NULL) {
      delete file;
      file = NULL;
    }
  }
}


void PseudoExperiments::addExperiments(TString& sourceDir, const TString& mlfitFile){
  /*
  Input:
  sourceDir: directory which contains PseudoExperiment folders
  mlfitFile: .root file which contains the fit results from the combine fit
  */
  //load PseudoExperiment folders
  if(sourceDir.EndsWith("/")) sourceDir.Chop();

  TSystemFile *pseudoExperimentFolder;
  TString folderName;
  TString dirName;
  if(sourceDir.Contains("PseudoExperiment")){
    std::cout << "loading experiment from " << sourceDir << "/" << mlfitFile << std::endl;
    addExperiment(sourceDir+"/"+mlfitFile);
  }
  else{
    TSystemDirectory dir(sourceDir.Data(), sourceDir.Data());
    TList *folders = dir.GetListOfFiles();
    int counter = 1;
    //if folders are found, go through each one an look for the mlfitFile
    if (folders) {
      TIter next(folders);
      TStopwatch watch;
      while ((pseudoExperimentFolder=(TSystemFile*)next())) {
        watch.Start();
        folderName = pseudoExperimentFolder->GetName();
        if (pseudoExperimentFolder->IsFolder() && !folderName.EndsWith(".") && !folderName.Contains("asimov")) {
          if(debug_) std::cout << "DEBUG    ";
          if(debug_ || counter%10==0) std::cout << "Adding PseudoExperiment #" << counter << std::endl;
          addExperiment(sourceDir+"/"+folderName+"/"+mlfitFile);
          counter++;
        }
        watch.Stop();
        if(debug_) printTime(watch, "Time for last loop");
      }
      delete folders;
    }
  }
  if(pseudoExperimentFolder != NULL) delete pseudoExperimentFolder;
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

void PseudoExperiments::initContainers(TFile* file, const RooFitResult* result, int nBins, Double_t min, Double_t max) {
  TString varName = result->floatParsFinal().contentsString();
  if(debug_) std::cout << result << std::endl;
  if(debug_) std::cout << "collecting variable names from RooFitResult " << result->GetName() << std::endl;
  while( varName.Contains(",") ) {
    nps_.push_back(varName(varName.Last(',')+1, varName.Length() - varName.Last(',')));
    varName.Remove(varName.Last(','), varName.Length()-varName.Last(','));
  }
  nps_.push_back(varName);

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


void PseudoExperiments::createHistograms(std::map<TString,TH1*>& hists, const std::vector<TString>& nps, const TString& name, int nBins, Double_t min, Double_t max) const {
  for(auto& np: nps) {
    hists[np] = createHistogram(np,name);
  }
}


TH1* PseudoExperiments::createHistogram(const TString& par, const TString& name, int nBins, Double_t min, Double_t max) const {
  if( debug_ ) std::cout << "DEBUG: createHistogram: " << par << ", " << name << std::endl;
  const TUUID id;
  TH1* h = new TH1D(label_+":"+par+":"+name+":"+id.AsString(),name+";"+par+";N(experiments)",nBins_,min_,max_);
  h->SetDirectory(0);
  return h;
}


void PseudoExperiments::storePrefitValues(std::map<TString,TH1*>& hists, TFile* file) const {
  TStopwatch watch;
  if(debug_) std::cout << "bla\n";
  RooFitResult* test_result = (RooFitResult*) file->Get("fit_b");
  //std::vector<TString> values;
  if(test_result != NULL){
    if(debug_) std::cout << "collecting variable names from RooFitResult fit_b" << std::endl;
    readRooRealVar(hists, test_result->floatParsInit().createIterator());

    if(debug_)std::cout << "\ndone" << std::endl;
    //if(debug_) std::cout << "deleting test_result in storePrefitValues\n";
    if(test_result != NULL) delete test_result;
  }
}


void PseudoExperiments::storeRooFitResults(std::map<TString,TH1*>& hists, std::map<TString, TH1*>& hErrors, std::map<TString,TH1*>& hErrorsHi, std::map<TString,TH1*>& hErrorsLo, TFile* file, const RooFitResult* result, std::map<TString, std::map<TString, TH1*> >& correlations) {

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

    std::map<TString, TH1*>::const_iterator iter_errorHi = hErrorsHi.find( it.first);

    iter_errorHi->second->Fill(var->getErrorHi());

    std::map<TString, TH1*>::const_iterator iter_errorLo = hErrorsLo.find( it.first);

    iter_errorLo->second->Fill(var->getErrorLo());

    std::map<TString, TH1*>::const_iterator iter_errors = hErrors.find( it.first);

    iter_errors->second->Fill(var->getError());

  }
  collectCorrelations(correlations, result);
}



TH1* PseudoExperiments::getHist(const std::map<TString,TH1*>& hists, const TString& key) const {
  if(debug_) std::cout << "entering 'PseudoExperiments::getHist()'\n";
  TH1* returnPointer = NULL;
  std::map<TString,TH1*>::const_iterator it = hists.find(key);
  if( it == hists.end() ) {
    std::cerr << "ERROR trying to access histogram for '" << key << "' in '" << label_ << "'" << std::endl;
    //throw std::exception();
  }
  else returnPointer = it->second;
  if(debug_) std::cout << "leaving getHist()\n";
  return returnPointer;
}


TH1* PseudoExperiments::getClone(const TH1* h)const {
  if(debug_) std::cout << "entering PseudoExperiments::getClone()\n";
  if(h!=NULL){
    if(debug_) std::cout << "h exists!\n";
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
        correlationPlot->SetBinError(i,j, checkValues(tempHisto->GetMeanError()));
        correlationPlot->SetBinContent(j,i, tempHisto->GetMean());
        correlationPlot->SetBinError(j,i, checkValues(tempHisto->GetMeanError()));
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

void PseudoExperiments::collectCorrelations(std::map<TString, std::map<TString,TH1*> >& correlations, const RooFitResult* result) {
  std::vector<TString> values;

  TString varName = result->floatParsFinal().contentsString();
  if(debug_) std::cout << result << std::endl;
  if(debug_) std::cout << "collecting variable names from RooFitResult " << result->GetName() << std::endl;
  while( varName.Contains(",") ) {
    values.push_back(varName(varName.Last(',')+1, varName.Length() - varName.Last(',')));
    varName.Remove(varName.Last(','), varName.Length()-varName.Last(','));
  }
  values.push_back(varName);
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
