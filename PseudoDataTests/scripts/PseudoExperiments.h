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
#include "helperFuncs.h"

class PseudoExperiments {
public:
  PseudoExperiments(const TString& label, const Double_t injectedMu, const bool noBinByBin = true);
  ~PseudoExperiments();

  TString operator()() const { return label_; }

  void addExperimentFromRoofit(const TString& mlfit);
  bool addExperimentFromTree(const TString& mlfit);
  void addExperiments(TString& sourceDir, const TString& mlfitFile = "mlfit.root");
  void setColor(const int c) {
    color_ = c;
  }

  size_t npsN() const { return nps_.size(); }
  const std::vector<TString>& nps() const { return nps_; }
  std::vector<TString>::const_iterator npsBegin() const { return nps_.begin(); }
  std::vector<TString>::const_iterator npsEnd() const { return nps_.end(); }
  
  size_t poisN() const { return pois_.size(); }
  const std::vector<TString>& pois() const { return pois_; }
  std::vector<TString>::const_iterator poisBegin() const { return pois_.begin(); }
  std::vector<TString>::const_iterator poisEnd() const { return pois_.end(); }
  
  TH1* prefit(const TString& np) const {
    return getClone(getHist(valuesPrefit_,np));
  }
  TH1* prefitErrorDist(const TString& np) const {
    return getClone(getHist(valuesPostfitBerrors_,np));
  }
  TH1* prefitErrorHiDist(const TString& np) const {
    return getClone(getHist(valuesPostfitBerrorHi_,np));
  }
  TH1* prefitErrorLoDist(const TString& np) const {
    return getClone(getHist(valuesPostfitBerrorLo_,np));
  }
  
  Double_t prefitMean(const TString& np) const {
    Double_t returnVal = -9999;
    TH1* tempPointer = getHist(valuesPrefit_,np);
    if(tempPointer != NULL) returnVal = tempPointer->GetMean();
    return returnVal;
  }
  Double_t prefitMeanError(const TString& np) const{
    Double_t returnVal = -9999;
    TH1* tempPointer = getHist(valuesPrefit_,np);
    if(tempPointer != NULL) returnVal = tempPointer->GetMeanError();
    return returnVal;
  }
  Double_t prefitRMS(const TString& np) const {
    return getHist(valuesPrefit_,np)->GetRMS();
  }
  Double_t prefitErrorHi(const TString& np) const{
      return getHist(valuesPostfitBerrorHi_,np)->GetMean();
  }
  Double_t prefitErrorLo(const TString& np) const{
      return getHist(valuesPostfitBerrorLo_,np)->GetMean();
  }
  Double_t prefitError(const TString& np) const{
      return getHist(valuesPostfitBerrors_,np)->GetMean();
  }
  
  
  TH1* postfitB(const TString& np) const {
    return getClone(getHist(valuesPostfitB_,np));
  }
  TH1* postfitBerrorDist(const TString& np) const {
    return getClone(getHist(valuesPostfitBerrors_,np));
  }
  TH1* postfitBerrorHiDist(const TString& np) const {
    return getClone(getHist(valuesPostfitBerrorHi_,np));
  }
  TH1* postfitBerrorLoDist(const TString& np) const {
    return getClone(getHist(valuesPostfitBerrorLo_,np));
  }
  Double_t postfitBMean(const TString& np) const {
    return getHist(valuesPostfitB_,np)->GetMean();
  }
  Double_t postfitBMeanError(const TString& np) const {
    return getHist(valuesPostfitB_,np)->GetMeanError();
  }
  Double_t postfitBRMS(const TString& np) const {
    return getHist(valuesPostfitB_,np)->GetRMS();
  }
  Double_t postfitBerrorHi(const TString& np) const{
      return getHist(valuesPostfitBerrorHi_,np)->GetMean();
  }
  Double_t postfitBerrorLo(const TString& np) const{
      return getHist(valuesPostfitBerrorLo_,np)->GetMean();
  }
  Double_t postfitBerror(const TString& np) const{
      return getHist(valuesPostfitBerrors_,np)->GetMean();
  }



  TH1* postfitS(const TString& np) const {
    return getClone(getHist(valuesPostfitS_,np));
  }
  TH1* postfitSerrorDist(const TString& np) const {
    return getClone(getHist(valuesPostfitSerrors_,np));
  }
  TH1* postfitSerrorHiDist(const TString& np) const {
    return getClone(getHist(valuesPostfitSerrorHi_,np));
  }
  TH1* postfitSerrorLoDist(const TString& np) const {
    return getClone(getHist(valuesPostfitSerrorLo_,np));
  }
  Double_t postfitSMean(const TString& np) const {
    return getHist(valuesPostfitS_,np)->GetMean();
  }
  Double_t postfitSMeanError(const TString& np) const {
    return getHist(valuesPostfitS_,np)->GetMeanError();
  }
  Double_t postfitSRMS(const TString& np) const {
    return getHist(valuesPostfitS_,np)->GetRMS();
  }
  Double_t postfitSerrorHi(const TString& np) const{
      return getHist(valuesPostfitSerrorHi_,np)->GetMean();
  }
  Double_t postfitSerrorLo(const TString& np) const{
      return getHist(valuesPostfitSerrorLo_,np)->GetMean();
  }
  Double_t postfitSerror(const TString& np) const{
      return getHist(valuesPostfitSerrors_,np)->GetMean();
  }

  int color() const { return color_; }

  void print() const {
    for(auto& str: nps_) {
      std::cout << str << std::endl;
      std::cout << postfitSMean(str) << std::endl;
    }
    for(auto& poi : pois_)
    {
        std::cout << poi << std::endl;
        std::cout << postfitSMean(poi) << std::endl;
    }
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
  void printCorrelations(const std::map<TString, std::map<TString,TH1*> >& correlations, const TString& outpath) const;
  void printPostfitScorrelations(const TString& outpath) const{
    printCorrelations(correlationsPostfitS_, outpath);
  }
  void printPostfitBcorrelations(const TString& outpath) const{
    printCorrelations(correlationsPostfitB_, outpath);
  }
  
private:
  bool debug_;
  bool fitBmustConverge_;
  bool fitSBmustConverge_;
  bool accurateCovariance_;
  bool noBinByBin_;

  TString label_;
  Double_t injectedMu_;
  int color_;

  static constexpr int nBins_ = 400;
  static constexpr Double_t min_ = -10;
  static constexpr Double_t max_ = 10;

  std::vector<TString> nps_;
  std::vector<TString> pois_;
  std::map<TString,TH1*> valuesPrefit_;
  std::map<TString,TH1*> valuesPrefiterrors_;
  std::map<TString,TH1*> valuesPrefiterrorHi_;
  std::map<TString,TH1*> valuesPrefiterrorLo_;

  std::map<TString,TH1*> valuesPostfitB_;
  std::map<TString,TH1*> valuesPostfitBerrors_;
  std::map<TString,TH1*> valuesPostfitBerrorHi_;
  std::map<TString,TH1*> valuesPostfitBerrorLo_;


  std::map<TString,TH1*> valuesPostfitS_;
  std::map<TString,TH1*> valuesPostfitSerrors_;
  std::map<TString,TH1*> valuesPostfitSerrorHi_;
  std::map<TString,TH1*> valuesPostfitSerrorLo_;

  std::vector<ShapeContainer*> prefitShapes_;
  std::vector<ShapeContainer*> postfitBshapes_;
  std::vector<ShapeContainer*> postfitSshapes_;

  std::map<TString, std::map<TString,TH1*> > correlationsPostfitB_;
  std::map<TString, std::map<TString,TH1*> > correlationsPostfitS_;


  void initContainers(TFile* file,  const RooFitResult* result, int nBins = nBins_, Double_t min = min_, Double_t max = max_) ;
  void createHistograms(std::map<TString,TH1*>& hists, const std::vector<TString>& nps, const TString& name, int nBins = nBins_, Double_t min = min_, Double_t max = max_) const;
  TH1* createHistogram(const TString& par, const TString& name, int nBins = nBins_, Double_t min = min_, Double_t max = max_) const;
  void storePrefitValues(std::map<TString,TH1*>& hists, std::map<TString,TH1*>& hErrors, std::map<TString,TH1*>& hErrorsHi, std::map<TString, TH1*>& hErrorsLo, TFile* file) const;
  void storePrefitValues(std::map<TString,std::vector<Double_t> >& histVecs, std::map<TString,std::vector<Double_t> >& vecErrors, std::map<TString,std::vector<Double_t> >& vecErrorsHi, std::map<TString,std::vector<Double_t> >& vecErrorsLo, TFile* file) const;
  void storeRooFitResults(std::map<TString,TH1*>& hists, std::map<TString,TH1*>& hErrors, std::map<TString,TH1*>& hErrorsHi, std::map<TString, TH1*>& hErrorsLo, TFile* file, const RooFitResult* result, std::map<TString, std::map<TString, TH1*> >& correlations);
  void storeRooFitResults(std::map<TString,std::vector<Double_t> >& histVecs, std::map<TString,std::vector<Double_t> >& hErrorVecs, std::map<TString,std::vector<Double_t> >& hErrorsHiVecs, std::map<TString, std::vector<Double_t> >& hErrorsLoVecs, TFile* file, const RooFitResult* result, std::map<TString, std::map<TString, std::vector<Double_t> > >& correlationVecs);
  void readRooRealVar(std::map<TString,TH1*>& hists, std::map<TString,TH1*>& hErrors, std::map<TString,TH1*>& hErrorsHi, std::map<TString, TH1*>& hErrorsLo, TIter it) const;
  void readRooRealVar(std::map<TString,std::vector<Double_t> >& histVecs, std::map<TString,std::vector<Double_t> >& hErrorVecs, std::map<TString,std::vector<Double_t> >& hErrorsHiVecs, std::map<TString, std::vector<Double_t> >& hErrorsLoVecs, TIter it) const;
  void readRooRealVar(TH1* hist, const RooFitResult* result, const TString& currentVarName) const;
  void readRooRealVar(std::vector<Double_t>& histVec, const RooFitResult* result, const TString& currentVarName) const;
  bool readCorrFolder(TFile* infile, const TString& folderName, std::map<TString, std::map<TString,TH1*> >& correlations);
  void readCorrTree(TTree* tree, std::map<TString, std::map<TString,TH1*> >& correlations);
  bool readParamFolder(TFile* infile, const TString& folderName, std::map<TString,TH1*>& hists, std::map<TString, TH1*>& hErrors, std::map<TString,TH1*>& hErrorsHi, std::map<TString,TH1*>& hErrorsLo);
  void readParamTree(TTree* tree, std::map<TString,TH1*>& hists, std::map<TString, TH1*>& hErrors, std::map<TString,TH1*>& hErrorsHi, std::map<TString,TH1*>& hErrorsLo);
  bool readShapeFolder(TFile* infile, const TString& folderName, std::vector<ShapeContainer*>& shapeVec);
  void readShapeTree(TTree* tree, std::vector<ShapeContainer*>& shapeVec);
  
  
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

PseudoExperiments::PseudoExperiments(const TString& label, const Double_t injectedMu, const bool noBinByBin)
: debug_(false),
fitBmustConverge_(true),
fitSBmustConverge_(true),
accurateCovariance_(true),
noBinByBin_(noBinByBin),
label_(label), injectedMu_(injectedMu), color_(kGray){
  if( debug_ ) std::cout << "DEBUG " << this << ": constructor" << std::endl;
}

PseudoExperiments::~PseudoExperiments() {
  if( debug_ ) std::cout << "DEBUG " << this << ": destructor" << std::endl;

}

void PseudoExperiments::readRooRealVar(std::map<TString,TH1*>& hists, std::map<TString,TH1*>& hErrors, std::map<TString,TH1*>& hErrorsHi, std::map<TString, TH1*>& hErrorsLo, TIter it) const{
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
      
      std::map<TString, TH1*>::const_iterator iter_errorHi = hErrorsHi.find( iter->first);

      iter_errorHi->second->Fill(param->getErrorHi());
    
      std::map<TString, TH1*>::const_iterator iter_errorLo = hErrorsLo.find( iter->first);
    
      iter_errorLo->second->Fill(param->getErrorLo());
    
      std::map<TString, TH1*>::const_iterator iter_errors = hErrors.find( iter->first);
    
      iter_errors->second->Fill(param->getError());
    }
  }
}

void PseudoExperiments::readRooRealVar(std::map<TString,std::vector<Double_t> >& histVecs, std::map<TString,std::vector<Double_t> >& hErrorVecs, std::map<TString,std::vector<Double_t> >& hErrorsHiVecs, std::map<TString, std::vector<Double_t> >& hErrorsLoVecs, TIter it) const{
  RooRealVar* param = NULL;

  while((param = (RooRealVar*)it.Next())){
    if(debug_) std::cout << "looking for parameter " << param->GetName() <<std::endl;
    histVecs[param->GetName()].push_back(param->getVal());
    hErrorVecs[param->GetName()].push_back(param->getError());
    hErrorsHiVecs[param->GetName()].push_back(param->getErrorHi());
    hErrorsLoVecs[param->GetName()].push_back(param->getErrorLo());
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

void PseudoExperiments::readRooRealVar(std::vector<Double_t>& histVec, const RooFitResult* result, const TString& currentVarName) const{

  RooRealVar* param = NULL;

  param = (RooRealVar*)result->floatParsFinal().find( currentVarName.Data() );
  if(param != NULL){
    Double_t value = param->getVal();
    histVec.push_back(value);
  }
}

void PseudoExperiments::addExperimentFromRoofit(const TString& mlfit) {
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
            storeRooFitResults(valuesPostfitB_, valuesPostfitBerrors_, valuesPostfitBerrorHi_, valuesPostfitBerrorLo_, file, result, correlationsPostfitB_);
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
            if( debug_ ) std::cout << "    DEBUG: postfit S NPs" << std::endl;
            storeRooFitResults(valuesPostfitS_, valuesPostfitSerrors_, valuesPostfitSerrorHi_, valuesPostfitSerrorLo_, file, result, correlationsPostfitS_);
            if( debug_ ) std::cout << "  DEBUG: done storing NPs" << std::endl;

            
            if( debug_ ) std::cout << "    DEBUG: prefit values" << std::endl;
            storePrefitValues(valuesPrefit_, valuesPostfitBerrors_, valuesPostfitBerrorHi_, valuesPostfitBerrorLo_, file);
            if(result != NULL) {
              if(debug_) std::cout << "deleting RooFitResult Pointer fit_s";
              delete result;
            }
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

void PseudoExperiments::readCorrTree(TTree* tree, std::map<TString, std::map<TString,TH1*> >& correlations){
  if(tree){
    TString histName;
    TString branchName;
    TString treeName = tree->GetName();
    TObjArray* array = tree->GetListOfBranches();
    // std::map<TString, std::map<TString, std::map<TString,TH1*> > >::const_iterator it = correlations.find(treeName.Data());
    for(int i=0; i<array->GetEntries(); i++){
      const TUUID id;
      branchName = array->At(i)->GetName();
      histName.Form("%s_%s_correlations_%s", treeName.Data(), branchName.Data(), id.AsString());
      std::cout << "Correlations for parameter " << treeName << ":\n";
      helperFuncs::saveTreeVals(tree, branchName, correlations[treeName], histName, nBins_, -1.2, 1.2);
    }
  }
}

bool PseudoExperiments::readCorrFolder(TFile* infile, const TString& folderName, std::map<TString, std::map<TString,TH1*> >& correlations){
  if(infile->cd(folderName.Data())){
    std::cout << "loading correlations from folder " << folderName << std::endl;
    TList* listOfTrees = gDirectory->GetListOfKeys();
    if(listOfTrees){
      TIter next(listOfTrees);
      TKey* key;
      TString treeName;
      TTree* tree;
      
      while( (key = (TKey*)next()) ){
        treeName = key->GetName();
        tree = (TTree*)gDirectory->Get(treeName.Data());
        readCorrTree(tree, correlations);
      }
    }
    infile->cd();
    return true;
  }
  else{
    std::cerr << "ERROR:\tFolder " << folderName << " does not exist in source file!\n";
    return false;
  }
}

void PseudoExperiments::readParamTree(TTree* tree, std::map<TString,TH1*>& hists, std::map<TString, TH1*>& hErrors, std::map<TString,TH1*>& hErrorsHi, std::map<TString,TH1*>& hErrorsLo){
  if(tree)
  {
    TString paramName = tree->GetName();
    TString histoTitle;
    histoTitle.Form(";%s;Frequency", paramName.Data());
    hists[paramName] = helperFuncs::branch2histo(tree, "Value", histoTitle);
    histoTitle.Form(";%s error;Frequency", paramName.Data());
    hErrors[paramName] = helperFuncs::branch2histo(tree, "Error", histoTitle);
    histoTitle.Form(";%s high error;Frequency", paramName.Data());
    hErrorsHi[paramName] = helperFuncs::branch2histo(tree, "High Error", histoTitle);
    histoTitle.Form(";%s low error;Frequency", paramName.Data());
    hErrorsLo[paramName] = helperFuncs::branch2histo(tree, "Low Error", histoTitle);
  }
}

bool PseudoExperiments::readParamFolder(TFile* infile, const TString& folderName, std::map<TString,TH1*>& hists, std::map<TString, TH1*>& hErrors, std::map<TString,TH1*>& hErrorsHi, std::map<TString,TH1*>& hErrorsLo){
  if(infile->cd(folderName.Data())){
    std::cout << "saving parameter values in folder " << folderName.Data() << std::endl;
    bool saveNps = nps_.empty();
    TList* listOfTrees = gDirectory->GetListOfKeys();
    if(listOfTrees){
      TIter next(listOfTrees);
      TKey* key;
      TString treeName;
      TTree* tree;
      while( (key = (TKey*)next()) ){
        treeName = key->GetName();
        tree = (TTree*)gDirectory->Get(treeName.Data());
        
        if(saveNps) 
        {
            std::cout << "saving nuisance parameter " << treeName << std::endl;
            if(!(treeName.Contains("prop_bin") && noBinByBin_) && !(treeName.BeginsWith("r"))) nps_.push_back(treeName);
        }
        else{
            if( std::find( nps_.begin(), nps_.end(), treeName) == nps_.end() && !(treeName.Contains("prop_bin") && noBinByBin_) ){
                pois_.push_back(treeName);
            }
        }
        std::cout << "reading nuisance parameter " << treeName << std::endl;
        readParamTree(tree, hists, hErrors, hErrorsHi, hErrorsLo);
      }
    }
    return true;
  }
  else{
    std::cerr << "ERROR:\tFolder " << folderName << " does not exist in source file!\n";
    return false;
  }
}

void PseudoExperiments::readShapeTree(TTree* tree, std::vector<ShapeContainer*>& shapeVec){
  if(tree){
    TString treeName = tree->GetName();
    std::cout << "creating container for category " << treeName << std::endl;

    ShapeContainer* container = new ShapeContainer(treeName);
    container->loadShapes(tree, injectedMu_);
    shapeVec.push_back(container);
    
  }
}

bool PseudoExperiments::readShapeFolder(TFile* infile, const TString& folderName, std::vector<ShapeContainer*>& shapeVec){
  if(infile->cd(folderName.Data())){
    std::cout << "loading shapes from folder " << folderName << std::endl;
    TList* listOfTrees = gDirectory->GetListOfKeys();
    if(listOfTrees){
      TIter next(listOfTrees);
      TKey* key;
      TString treeName;
      TTree* tree;
      while( (key = (TKey*)next()) ){
        treeName = key->GetName();
        tree = (TTree*)gDirectory->Get(treeName.Data());
        readShapeTree(tree, shapeVec);
      }
    }
    return true;
  }
  else{
      std::cerr << "ERROR:\tFolder " << folderName << " does not exist in source file!\n";
      return false;
  }
}

bool PseudoExperiments::addExperimentFromTree(const TString& mlfit){
  if(mlfit.EndsWith(".root")){
    TFile* infile = TFile::Open(mlfit);
    if(infile->IsOpen() && !infile->IsZombie() && !infile->TestBit(TFile::kRecovered))
    {
      if(!(readCorrFolder(infile, "Correlation_sig", correlationsPostfitS_)) ) return false;
      if(!(readCorrFolder(infile, "Correlation_bac", correlationsPostfitB_)) ) return false;
      
      if(!(readParamFolder(infile, "signal", valuesPostfitS_, valuesPostfitSerrors_, valuesPostfitSerrorHi_, valuesPostfitSerrorLo_))) return false;
      if(!(readParamFolder(infile, "background", valuesPostfitB_, valuesPostfitBerrors_, valuesPostfitBerrorHi_, valuesPostfitBerrorLo_))) return false;
      if(!(readParamFolder(infile, "Prefit", valuesPrefit_, valuesPrefiterrors_, valuesPrefiterrorHi_, valuesPrefiterrorLo_))) return false;
      if(!(readShapeFolder(infile, "shapes_fit_s", postfitSshapes_)) ) return false;
      if(!(readShapeFolder(infile, "shapes_fit_b", postfitBshapes_)) ) return false;
      if(!(readShapeFolder(infile, "shapes_prefit", prefitShapes_))) return false;
      return true;
    }
  }
  else{
    std::cerr << "ERROR:\tinput file is not a root file!";
  }
  return false;
}

void PseudoExperiments::addExperiments(TString& sourceDir, const TString& mlfitFile){
  /*
  Input:
  sourceDir: directory which contains PseudoExperiment folders
  mlfitFile: .root file which contains the fit results from the combine fit
  */
  
  if(sourceDir.EndsWith(".root")){
    std::cout << "loading all experiment data from " << sourceDir.Data() << std::endl;
    if(!addExperimentFromTree(sourceDir)){
        std::cout << "failed to find TTree with values, will try to load directly from RooFitResult next\n";
      addExperimentFromRoofit(sourceDir);
      }
  }
  else
  {
    //load PseudoExperiment folders
    if(sourceDir.EndsWith("/")) sourceDir.Chop();
  
    TSystemFile *pseudoExperimentFolder;
    TString folderName;
    TString dirName;
    if(sourceDir.Contains("PseudoExperiment")){
      std::cout << "loading experiment from " << sourceDir << "/" << mlfitFile << std::endl;
      addExperimentFromRoofit(sourceDir+"/"+mlfitFile);
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
            addExperimentFromRoofit(sourceDir+"/"+folderName+"/"+mlfitFile);
            counter++;
          }
          watch.Stop();
          if(debug_) printTime(watch, "Time for last loop");
        }
        delete folders;
      }
    }
    for(auto& container : prefitShapes_) container->createShapeHistos(injectedMu_);
    for(auto& container : postfitBshapes_) container->createShapeHistos(injectedMu_);
    for(auto& container : postfitSshapes_) container->createShapeHistos(injectedMu_);
    if(pseudoExperimentFolder != NULL) delete pseudoExperimentFolder;
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

void PseudoExperiments::initContainers(TFile* file, const RooFitResult* result, int nBins, Double_t min, Double_t max) {
  TString varName = result->floatParsFinal().contentsString();
  if(debug_) std::cout << result << std::endl;
  if(debug_) std::cout << "collecting variable names from RooFitResult " << result->GetName() << std::endl;
  TString npName;
  if(nps_.empty())
  {
    while( varName.Contains(",") ) {
      npName = varName(varName.Last(',')+1, varName.Length() - varName.Last(','));
      
      if(!(npName.Contains("prop_bin") && noBinByBin_) && !npName.BeginsWith("r")) nps_.push_back(npName);
      varName.Remove(varName.Last(','), varName.Length()-varName.Last(','));
    }
    if(!(varName.Contains("prop_bin") && noBinByBin_) && !npName.BeginsWith("r")) nps_.push_back(varName);
  }

  createHistograms(valuesPrefit_,nps_,"prefit");
  createHistograms(valuesPostfitB_,nps_,"postfitB");
  createHistograms(valuesPostfitBerrors_, nps_, "postfitB_errors");
  createHistograms(valuesPostfitBerrorHi_, nps_, "postfitB_errorHi");
  createHistograms(valuesPostfitBerrorLo_, nps_, "postfitB_errorLo");
  createHistograms(valuesPostfitS_,nps_,"postfitS");
  createHistograms(valuesPostfitSerrors_,nps_,"postfitS_errors");
  createHistograms(valuesPostfitSerrorHi_, nps_, "postfitS_errorHi");
  createHistograms(valuesPostfitSerrorLo_, nps_, "postfitS_errorLo");
}


void PseudoExperiments::createHistograms(std::map<TString,TH1*>& hists, const std::vector<TString>& nps, const TString& name, int nBins, Double_t min, Double_t max) const {
  if (! nps.empty() )
  {
      for(auto& np: nps) {
        hists[np] = createHistogram(np, name, nBins, min, max);
      }
  }
}


TH1* PseudoExperiments::createHistogram(const TString& par, const TString& name, int nBins, Double_t min, Double_t max) const {
  if( debug_ ) std::cout << "DEBUG: createHistogram: " << par << ", " << name << std::endl;
  const TUUID id;
  TString clear_label = label_+":"+par+":"+name+":"+id.AsString();
  if(clear_label.Contains(" = ")) clear_label.ReplaceAll(" = ", "_");
  if(clear_label.Contains(" ")) clear_label.ReplaceAll(" ", "_");
  TH1* h = new TH1D(clear_label.Data(), name+";"+par+";N(experiments)", nBins, min, max);
  h->SetDirectory(0);
  return h;
}


void PseudoExperiments::storePrefitValues(std::map<TString,TH1*>& hists, std::map<TString,TH1*>& hErrors, std::map<TString,TH1*>& hErrorsHi, std::map<TString, TH1*>& hErrorsLo, TFile* file) const {
  TStopwatch watch;
  if(debug_) std::cout << "loading fit_s for prefit values\n";
  RooFitResult* test_result = (RooFitResult*) file->Get("fit_s");
  //std::vector<TString> values;
  if(test_result != NULL){
    if(debug_) std::cout << "collecting variable names from RooFitResult fit_s" << std::endl;
    readRooRealVar(hists, hErrors, hErrorsHi, hErrorsLo, test_result->floatParsInit().createIterator());

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
  else{
    if( pois_.empty()){
        if( debug_ ) std::cout << "  DEBUG: initialize POIs\n";
        TIter it = result->floatParsFinal().createIterator();
        TString name;
        RooRealVar* param;
        while(( param = (RooRealVar*)it.Next() )){
            name = param->GetName();
            if( std::find(nps_.begin(), nps_.end(), name) == nps_.end() && !(name.Contains("prop_bin") && noBinByBin_ )){
                pois_.push_back(name);
            }
        }
        createHistograms(valuesPrefit_,pois_,"prefit");
        createHistograms(valuesPostfitB_,pois_,"postfitB");
        createHistograms(valuesPostfitBerrors_, pois_, "postfitB_errors");
        createHistograms(valuesPostfitBerrorHi_, pois_, "postfitB_errorHi");
        createHistograms(valuesPostfitBerrorLo_, pois_, "postfitB_errorLo");
        createHistograms(valuesPostfitS_,pois_,"postfitS");
        createHistograms(valuesPostfitSerrors_,pois_,"postfitS_errors");
        createHistograms(valuesPostfitSerrorHi_, pois_, "postfitS_errorHi");
        createHistograms(valuesPostfitSerrorLo_, pois_, "postfitS_errorLo");
    }
  }
  readRooRealVar(hists, hErrors, hErrorsHi, hErrorsLo, result->floatParsFinal().createIterator());

  if(debug_)std::cout << std::endl;
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
        correlationPlot->SetBinError(i,j, helperFuncs::checkValues(tempHisto->GetMeanError()));
        correlationPlot->SetBinContent(j,i, tempHisto->GetMean());
        correlationPlot->SetBinError(j,i, helperFuncs::checkValues(tempHisto->GetMeanError()));
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
  correlationPlot->GetZaxis()->SetRangeUser(-1, 1);
  return correlationPlot;
}

void PseudoExperiments::collectCorrelations(std::map<TString, std::map<TString,TH1*> >& correlations, const RooFitResult* result) {
  std::vector<TString> values;
  if(debug_) std::cout << "collecting correlations\n";

  TString varName = result->floatParsFinal().contentsString();
  if(debug_) std::cout << result << std::endl;
  if(debug_) std::cout << "collecting variable names from RooFitResult " << result->GetName() << std::endl;
  TString paramName;
  while( varName.Contains(",") ) {
    paramName = varName(varName.Last(',')+1, varName.Length() - varName.Last(','));
    if(debug_) std::cout << paramName << std::endl;
    if(!(paramName.Contains("prop_bin") && noBinByBin_)) 
    {
      if(debug_)std::cout << "saving " << paramName << std::endl;
      values.push_back(paramName);
    }
    varName.Remove(varName.Last(','), varName.Length()-varName.Last(','));
  }
  if(!(varName.Contains("prop_bin") && noBinByBin_)) values.push_back(varName);
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

void PseudoExperiments::printCorrelations(const std::map<TString, std::map<TString,TH1*> >& correlations, const TString& outpath) const {
  TString currentName;
  for(std::map<TString, std::map<TString,TH1*> >::const_iterator it = correlations.begin(); it != correlations.end(); it++){
    for(std::map<TString,TH1*>::const_iterator sub_it = it->second.begin(); sub_it != it->second.end(); sub_it++){
      currentName.Form("correlation_%s_%s_%s",it->first.Data(), sub_it->first.Data(), label_.Data());
      currentName.ReplaceAll("=", "_");
      currentName.ReplaceAll(" ", "_");
      currentName.ReplaceAll(".", "p");
      currentName.Prepend(outpath);
      currentName.Append(".root");
      TFile* outfile = TFile::Open(currentName.Data(), "RECREATE");
      TCanvas canvas;
      sub_it->second->Draw();
      sub_it->second->Write();
      canvas.Write("canvas");
      currentName.ReplaceAll(".root", ".pdf");
      canvas.SaveAs(currentName.Data());
      outfile->Close();
    }
  }
}
#endif
