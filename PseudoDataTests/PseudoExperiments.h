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
#include "TString.h"
#include "TUUID.h"

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
  const std::set<TString>& nps() const { return nps_; }
  std::set<TString>::const_iterator npsBegin() const { return nps_.begin(); }
  std::set<TString>::const_iterator npsEnd() const { return nps_.end(); }
  TH1* npPrefit(const TString& np) const {
    return getClone(getHist(npValuesPrefit_,np));
  }
  double npPrefitMean(const TString& np) const {
    return getHist(npValuesPrefit_,np)->GetMean();
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

private:
  bool debug_;
  bool fitBmustConverge_;
  bool fitSBmustConverge_;
  bool accurateCovariance_;

  TString label_;
  double injectedMu_;
  int color_;

  double nBins_;
  double min_;
  double max_;

  TH1* muValues_;
  TH1* muErrors_;
  std::set<TString> nps_;
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


  void initContainers(TFile& file);
  void createHistograms(std::map<TString,TH1*>& hists, const std::set<TString>& nps, const TString& name) const;
  TH1* createHistogram(const TString& par, const TString& name) const;
  void storeRooArgSetResults(std::map<TString,TH1*>& hists, TFile& file, const TString& name) const;
  void storeRooFitResults(std::map<TString,TH1*>& hists, std::map<TString,TH1*>& hErrors, std::map<TString,TH1*>& hErrorsHi, std::map<TString, TH1*>& hErrorsLo, TFile& file, const TString& name) const;
  TH1* getHist(const std::map<TString,TH1*>& hists, const TString& key) const;
  TH1* getClone(const TH1* h) const;
  bool checkFitStatus(TFile& file);
  bool checkCovarianceMatrix(TFile& file);
  void storeShapes(std::vector<ShapeContainer*>& shapes, TFile& file, const TString& name) const;
};


PseudoExperiments::PseudoExperiments(const TString& label, const double injectedMu)
: debug_(false),
fitBmustConverge_(true),
fitSBmustConverge_(true),
accurateCovariance_(true),
label_(label), injectedMu_(injectedMu), color_(kGray),
nBins_(400), min_(-10), max_(10),
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
  TFile file(mlfit,"READ");
  if( !file.IsOpen() || file.IsZombie()) {
    std::cerr << "ERROR opening file '" << mlfit << "'" << std::endl;
    //throw std::exception();
  }
  else {
    //check if the s+b and the b fit converged
    if(!file.TestBit(TFile::kRecovered))
    {
      bool storeExperiment = true;

      if(fitSBmustConverge_ || fitBmustConverge_) storeExperiment = checkFitStatus(file);
      if(storeExperiment && accurateCovariance_) storeExperiment = checkCovarianceMatrix(file);
      // store POI value
      if(storeExperiment)
      {
        if( muValues_ == 0 ) {
          muValues_ = createHistogram("mu","postfitS");
          muErrors_ = createHistogram("muErrors", "postfitS");
        }
        if( debug_ ) std::cout << "  DEBUG: getting mu" << std::endl;
        RooFitResult* result = 0;
        file.GetObject("fit_s",result);
        if( result == 0 ) {
          std::cerr << "ERROR getting 'fit_s' from file '" << file.GetName() << "'" << std::endl;
          //throw std::exception();
        }
        const RooRealVar* var = static_cast<RooRealVar*>( result->floatParsFinal().find("r") );
        if( debug_ ) std::cout << "    DEBUG: found mu = " << var->getVal() << std::endl;
        muValues_->Fill( var->getVal() );
        muErrors_->Fill( var->getError() );

        // if called the first time, get list of NPs
        if( nps_.empty() ) {
          if( debug_ ) std::cout << "  DEBUG: initialize NPs" << std::endl;
          initContainers(file);
        }
        // store nuisance parameter values
        if( debug_ ) std::cout << "  DEBUG: store NPs" << std::endl;
        if( debug_ ) std::cout << "    DEBUG: prefit NPs" << std::endl;
        storeRooArgSetResults(npValuesPrefit_,file,"nuisances_prefit");
        if( debug_ ) std::cout << "    DEBUG: postfit B NPs" << std::endl;
        storeRooFitResults(npValuesPostfitB_, npValuesPostfitBerrors_, npValuesPostfitBerrorHi_, npValuesPostfitBerrorLo_, file,"fit_b");
        if( debug_ ) std::cout << "    DEBUG: postfit S NPs" << std::endl;
        storeRooFitResults(npValuesPostfitS_,npValuesPostfitSerrors_, npValuesPostfitSerrorHi_, npValuesPostfitSerrorLo_, file,"fit_s");
        if( debug_ ) std::cout << "  DEBUG: done storing NPs" << std::endl;
        if(debug_) std::cout << "  DEBUG: store shapes\n";
        if(debug_) std::cout << "\tDEBUG: prefit shapes\n";
        storeShapes(prefitShapes_, file, "shapes_prefit");
        if(debug_) std::cout << "\tDEBUG: postfit B shapes\n";
        storeShapes(postfitBshapes_, file, "shapes_fit_b");
        if(debug_) std::cout << "\tDEBUG: postfit S shapes\n";
        storeShapes(postfitSshapes_, file, "shapes_fit_s");
        if( debug_ ) std::cout << "  DEBUG: done storing shapes" << std::endl;
      }

      else std::cout << "skipping experiment in '" << mlfit.Data() << "'\n";
    }
    else std::cerr << "  ERROR while opening file " << mlfit.Data() << ", skipping...\n";
  }
  file.Close();
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
    while ((pseudoExperimentFolder=(TSystemFile*)next())) {
      folderName = pseudoExperimentFolder->GetName();
      if (pseudoExperimentFolder->IsFolder() && !folderName.EndsWith(".")) {
        if(sourceDir.EndsWith("/")) sourceDir.Chop();
        if(debug_) std::cout << "DEBUG    ";
        if(debug_ || counter%10==0) std::cout << "Adding PseudoExperiment #" << counter << std::endl;
        addExperiment(sourceDir+"/"+folderName+"/"+mlfitFile);
        counter++;
      }
    }
    delete pseudoExperimentFolder;
    delete folders;
  }
}

bool PseudoExperiments::checkFitStatus(TFile& file){
  bool storeExperiment = true;
  int fit_status=7;
  TString fit_trees[2] = {"tree_fit_sb", "tree_fit_b"};
  bool fit_flags[2] = {fitSBmustConverge_, fitBmustConverge_};
  for(int nTrees=0; nTrees<2; nTrees++)
  {
    TTree* tree = (TTree*)file.Get(fit_trees[nTrees].Data());
    tree->SetBranchAddress("fit_status",&fit_status);
    tree->GetEntry(0);
    if((fit_status != 0)){
      std::cout << "WARNING fit_status in " << fit_trees[nTrees].Data() << " did not converge!\n";
      if(fit_flags[nTrees]) storeExperiment = false;
    }
    delete tree;
    fit_status=7;
  }
  return storeExperiment;
}

bool PseudoExperiments::checkCovarianceMatrix(TFile& file){
  bool accurateCovariance = false;
  TString rooFitObjects[2] = {"fit_b", "fit_s"};
  int quality = -1;
  bool qualities[2] = {false, false};
  for(int currentObject=0; currentObject < 2; currentObject++){
    RooFitResult* fitObject = 0;
    file.GetObject(rooFitObjects[currentObject].Data(),fitObject);
    if( fitObject == 0 ) {
      std::cerr << "ERROR getting '" << rooFitObjects[currentObject].Data() << "' from file '" << file.GetName() << "'" << std::endl;
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

void PseudoExperiments::initContainers(TFile& file) {
  RooArgSet* nuisances_prefit = 0;
  file.GetObject("nuisances_prefit",nuisances_prefit);
  if( nuisances_prefit == 0 ) {
    std::cerr << "ERROR getting 'nuisances_prefit' from file '" << file.GetName() << "'" << std::endl;
    //throw std::exception();
  }
  else{
    TIter it = nuisances_prefit->createIterator();
    RooRealVar* var = static_cast<RooRealVar*>(it.Next());
    while( var != 0 ) {
      nps_.insert( var->GetName() );
      var = static_cast<RooRealVar*>(it.Next());
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
}


void PseudoExperiments::createHistograms(std::map<TString,TH1*>& hists, const std::set<TString>& nps, const TString& name) const {
  for(auto& np: nps) {
    hists[np] = createHistogram(np,name);
  }
}


TH1* PseudoExperiments::createHistogram(const TString& par, const TString& name) const {
  if( debug_ ) std::cout << "DEBUG: createHistogram: " << par << ", " << name << std::endl;
  const TUUID id;
  TH1* h = new TH1D(label_+":"+par+":"+name+":"+id.AsString(),name+";"+par+";N(experiments)",nBins_,min_,max_);
  h->SetDirectory(0);
  return h;
}


void PseudoExperiments::storeRooArgSetResults(std::map<TString,TH1*>& hists, TFile& file, const TString& name) const {
  RooArgSet* result = 0;
  file.GetObject(name,result);
  if( result == 0 ) {
    std::cerr << "ERROR getting '" << name << "' from file '" << file.GetName() << "'" << std::endl;
    //throw std::exception();
  }
  else{
    for(auto& it: hists) {
      it.second->Fill( result->getRealValue(it.first) );
    }
  }
}


void PseudoExperiments::storeRooFitResults(std::map<TString,TH1*>& hists, std::map<TString, TH1*>& hErrors, std::map<TString,TH1*>& hErrorsHi, std::map<TString,TH1*>& hErrorsLo, TFile& file, const TString& name) const {
  RooFitResult* result = 0;
  file.GetObject(name,result);
  if( result == 0 ) {
    std::cerr << "ERROR getting '" << name << "' from file '" << file.GetName() << "'" << std::endl;
    //throw std::exception();
  }
  else{
    for(auto& it: hists) {
      const RooRealVar* var = static_cast<RooRealVar*>( result->floatParsFinal().find( it.first ) );
      it.second->Fill(var->getVal());
      std::map<TString, TH1*>::const_iterator iter_errorHi = hErrorsHi.find( it.first);
      iter_errorHi->second->Fill(var->getErrorHi());
      std::map<TString, TH1*>::const_iterator iter_errorLo = hErrorsLo.find( it.first);
      iter_errorLo->second->Fill(var->getErrorLo());
      std::map<TString, TH1*>::const_iterator iter_errors = hErrors.find( it.first);
      iter_errors->second->Fill(var->getError());

    }
  }
}


TH1* PseudoExperiments::getHist(const std::map<TString,TH1*>& hists, const TString& key) const {
  std::map<TString,TH1*>::const_iterator it = hists.find(key);
  if( it == hists.end() ) {
    std::cerr << "ERROR trying to access histogram for '" << key << "' in '" << label_ << "'" << std::endl;
    //throw std::exception();
  }
  return it->second;
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

void PseudoExperiments::storeShapes(std::vector<ShapeContainer*>& shapes, TFile& file, const TString& name) const
{
  if(file.cd(name))
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
          shapes[nShapeContainer]->loadShapes(file, name, signalStrength);
          createNewContainer = false;
        }
      }
      if(createNewContainer)
      {
        ShapeContainer* tempShapeContainer = new ShapeContainer(categoryName);
        tempShapeContainer->loadShapes(file, name, signalStrength);
        shapes.push_back(tempShapeContainer);
      }
    }
  }
  else std::cerr << "ERROR    Could not change into directory " << name.Data() << " in file " << file.GetName() <<std::endl;
}

#endif
