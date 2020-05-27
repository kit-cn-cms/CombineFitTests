#ifndef PSEUDO_EXPERIMENTS_H
#define PSEUDO_EXPERIMENTS_H

#include <exception>
#include <iostream>
#include <map>
#include <set>
#include <vector>
#include <fstream>

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
  double get_injectedMu() const{
    return injectedMu_;
  };

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

  static constexpr int nBins_ = 180;
  static constexpr Double_t min_ = -10;
  static constexpr Double_t max_ = 10;

  std::vector<TString> nps_;
  std::vector<TString> pois_;
  std::map<TString,TH1*> valuesPrefit_;
  std::map<TString,TH1*> valuesPrefiterrors_;
  std::map<TString,TH1*> valuesPrefiterrorHi_;
  std::map<TString,TH1*> valuesPrefiterrorLo_;
  std::map<TString,std::vector<Double_t> > valuesPrefiterrorVecs_;
  std::map<TString,std::vector<Double_t> > valuesPrefiterrorHiVecs_;
  std::map<TString,std::vector<Double_t> > valuesPrefiterrorLoVecs_;

  std::map<TString,TH1*> valuesPostfitB_;
  std::map<TString,TH1*> valuesPostfitBerrors_;
  std::map<TString,TH1*> valuesPostfitBerrorHi_;
  std::map<TString,TH1*> valuesPostfitBerrorLo_;
  std::map<TString,std::vector<Double_t> > valuesPostfitBerrorVecs_;
  std::map<TString,std::vector<Double_t> > valuesPostfitBerrorHiVecs_;
  std::map<TString,std::vector<Double_t> > valuesPostfitBerrorLoVecs_;

  std::map<TString,TH1*> valuesPostfitS_;
  std::map<TString,TH1*> valuesPostfitSerrors_;
  std::map<TString,TH1*> valuesPostfitSerrorHi_;
  std::map<TString,TH1*> valuesPostfitSerrorLo_;
  std::map<TString,TH1*> biasPostfitS_;
  std::map<TString,std::vector<Double_t> > valuesPostfitSerrorVecs_;
  std::map<TString,std::vector<Double_t> > valuesPostfitSerrorHiVecs_;
  std::map<TString,std::vector<Double_t> > valuesPostfitSerrorLoVecs_;

  std::vector<ShapeContainer*> prefitShapes_;
  std::vector<ShapeContainer*> postfitBshapes_;
  std::vector<ShapeContainer*> postfitSshapes_;

  std::map<TString, std::map<TString,TH1*> > correlationsPostfitB_;
  std::map<TString, std::map<TString,TH1*> > correlationsPostfitS_;
  std::map<TString, std::map<TString,std::vector<double> > > correlationvalsPostfitB_;
  std::map<TString, std::map<TString,std::vector<double> > > correlationvalsPostfitS_;


  void initContainers(TFile* file,  const RooFitResult* result, int nBins = nBins_, Double_t min = min_, Double_t max = max_) ;
  void createHistograms(std::map<TString,TH1*>& hists, const std::vector<TString>& nps, const TString& name, int nBins = nBins_, Double_t min = min_, Double_t max = max_) const;
  TH1* createHistogram(const TString& par, const TString& name, int nBins = nBins_, Double_t min = min_, Double_t max = max_) const;
  void storePrefitValues(std::map<TString,TH1*>& hists, std::map<TString,TH1*>& hErrors, std::map<TString,TH1*>& hErrorsHi, std::map<TString, TH1*>& hErrorsLo, TFile* file) const;
  void storePrefitValues(std::map<TString,std::vector<Double_t> >& histVecs, std::map<TString,std::vector<Double_t> >& vecErrors, std::map<TString,std::vector<Double_t> >& vecErrorsHi, std::map<TString,std::vector<Double_t> >& vecErrorsLo, TFile* file) const;
  
  void storeRooFitResults(std::map<TString,TH1*>& hists, std::map<TString,TH1*>& hErrors, std::map<TString,TH1*>& hErrorsHi, std::map<TString, TH1*>& hErrorsLo, TFile* file, const RooFitResult* result, std::map<TString, std::map<TString, TH1*> >& correlations);
  void storeRooFitResults(std::map<TString,TH1*>& hists, std::map<TString, TH1*>& hErrors, std::map<TString,TH1*>& hErrorsHi, std::map<TString,TH1*>& hErrorsLo, TFile* file, const RooFitResult* result, std::map<TString, std::map<TString, std::vector<double> > >& correlations);
  void storeRooFitResults(std::map<TString,std::vector<Double_t> >& histVecs, std::map<TString,std::vector<Double_t> >& hErrorVecs, std::map<TString,std::vector<Double_t> >& hErrorsHiVecs, std::map<TString, std::vector<Double_t> >& hErrorsLoVecs, TFile* file, const RooFitResult* result, std::map<TString, std::map<TString, std::vector<Double_t> > >& correlationVecs);

  void readRooRealVar(std::map<TString,TH1*>& hists, std::map<TString,TH1*>& hErrors, std::map<TString,TH1*>& hErrorsHi, std::map<TString, TH1*>& hErrorsLo, TIter it) const;  
  void readRooRealVar(std::map<TString,TH1*>& hists, std::map<TString,TH1*>& hErrors, std::map<TString,TH1*>& hErrorsHi, std::map<TString, TH1*>& hErrorsLo, TIter it, std::map<TString, TH1*>& biasMap) const;
  void readRooRealVar(std::map<TString,std::vector<Double_t> >& histVecs, std::map<TString,std::vector<Double_t> >& hErrorVecs, std::map<TString,std::vector<Double_t> >& hErrorsHiVecs, std::map<TString, std::vector<Double_t> >& hErrorsLoVecs, TIter it) const;
  void readRooRealVar(TH1* hist, const RooFitResult* result, const TString& currentVarName) const;
  void readRooRealVar(std::vector<Double_t>& histVec, const RooFitResult* result, const TString& currentVarName) const;
  bool readCorrFolder(TFile* infile, const TString& folderName, std::map<TString, std::map<TString,TH1*> >& correlations);
  void readCorrTree(TTree* tree, std::map<TString, std::map<TString,TH1*> >& correlations);
  bool readParamFolder(TFile* infile, const TString& folderName, std::map<TString,TH1*>& hists, std::map<TString, TH1*>& hErrors, std::map<TString,TH1*>& hErrorsHi, std::map<TString,TH1*>& hErrorsLo);
  void readParamTree(TTree* tree, std::map<TString,TH1*>& hists, std::map<TString, TH1*>& hErrors, std::map<TString,TH1*>& hErrorsHi, std::map<TString,TH1*>& hErrorsLo);
  bool readShapeFolder(TFile* infile, const TString& folderName, std::vector<ShapeContainer*>& shapeVec);
  void readShapeTree(TTree* tree, std::vector<ShapeContainer*>& shapeVec);
  
  void convertToHisto(std::map<TString, std::map<TString,std::vector<double> > >& vecs, std::map<TString, std::map<TString,TH1*> >& histos, const TString title) const;
  
  TH1* getHist(const std::map<TString,TH1*>& hists, const TString& key) const;
  TH1* getClone(const TH1* h) const;
  bool checkFitStatus(TFile* file);
  bool checkCovarianceMatrix(TFile* file);
  void storeShapes(std::vector<ShapeContainer*>& shapes, TFile* file, const TString& name) const;
  TH2D* getCorrelationPlot(const std::map<TString, std::map<TString,TH1*> >& correlations) const;
  void printCorrelations(const std::map<TString, std::map<TString,TH1*> >& correlations, const TString& outpath) const;
  void collectCorrelations(std::map<TString, std::map<TString,TH1*> >& correlations, const RooFitResult* result) ;
  void collectCorrelations(std::map<TString, std::map<TString,std::vector<double> > >& correlations, const RooFitResult* result);
};

#endif
