#ifndef PSEUDO_EXPERIMENTS_H
#define PSEUDO_EXPERIMENTS_H

#include <exception>
#include <iostream>
#include <map>
#include <set>
#include <vector>

#include "TFile.h"
#include "TH1.h"
#include "TH1D.h"
#include "TString.h"
#include "TUUID.h"

#include "RooArgSet.h"
#include "RooFitResult.h"
#include "RooRealVar.h"



class PseudoExperiments {
public:
  PseudoExperiments(const TString& label, const double injectedMu);
  ~PseudoExperiments();

  TString operator()() const { return label_; }

  void addExperiment(const TString& mlfit);
  void addExperiments(const TString& mlfitTemplateName, const unsigned int nExps);
  void setColor(const int c) {
    color_ = c;
  }

  TH1* mu() const {
    return getClone(muValues_);
  }
  double muMean() const {
    return muValues_->GetMean();
  }
  double muMeanError() const {
    return muValues_->GetMeanError();
  }
  double muRMS() const {
    return muValues_->GetRMS();
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
  double npPostfitBMean(const TString& np) const {
    return getHist(npValuesPostfitB_,np)->GetMean();
  }
  double npPostfitBRMS(const TString& np) const {
    return getHist(npValuesPostfitB_,np)->GetRMS();
  }
  TH1* npPostfitS(const TString& np) const {
    return getClone(getHist(npValuesPostfitS_,np));
  }
  double npPostfitSMean(const TString& np) const {
    return getHist(npValuesPostfitS_,np)->GetMean();
  }
  double npPostfitSRMS(const TString& np) const {
    return getHist(npValuesPostfitS_,np)->GetRMS();
  }

  int color() const { return color_; }
  
  void print() const {
    for(auto& str: nps_) {
      std::cout << str << std::endl;
    }
    std::cout << npPostfitSMean("CMS_ttH_QCDscale_ttbarPlusBBbar") << std::endl;
    std::cout << muMean() << std::endl;
  }
  
  
private:
  bool debug_;
  
  TString label_;
  double injectedMu_;
  int color_;

  double nBins_;
  double min_;
  double max_;
  
  TH1* muValues_;
  std::set<TString> nps_;
  std::map<TString,TH1*> npValuesPrefit_;
  std::map<TString,TH1*> npValuesPostfitB_;
  std::map<TString,TH1*> npValuesPostfitS_;

  void initContainers(TFile& file);
  void createHistograms(std::map<TString,TH1*>& hists, const std::set<TString>& nps, const TString& name) const;
  TH1* createHistogram(const TString& par, const TString& name) const;
  void storeRooArgSetResults(std::map<TString,TH1*>& hists, TFile& file, const TString& name) const;
  void storeRooFitResults(std::map<TString,TH1*>& hists, TFile& file, const TString& name) const;
  TH1* getHist(const std::map<TString,TH1*>& hists, const TString& key) const;
  TH1* getClone(const TH1* h) const;
};


PseudoExperiments::PseudoExperiments(const TString& label, const double injectedMu)
  : debug_(false),
    label_(label), injectedMu_(injectedMu), color_(kGray),
    nBins_(4000), min_(-20), max_(20),
    muValues_(0) {
  if( debug_ ) std::cout << "DEBUG " << this << ": constructor" << std::endl;
}

PseudoExperiments::~PseudoExperiments() {
  // if( debug_ ) std::cout << "DEBUG " << this << ": destructor" << std::endl;
  // for(auto& it: npValuesPrefit_) {
  //   delete it.second;
  // }
  // for(auto& it: npValuesPostfitB_) {
  //   delete it.second;
  // }
  // for(auto& it: npValuesPostfitS_) {
  //   delete it.second;
  // }
  // if( muValues_ != 0 ) delete muValues_;
}


void PseudoExperiments::addExperiment(const TString& mlfit) {
  if( debug_ ) std::cout << "DEBUG: addExperiment: " << mlfit << std::endl;
  TFile file(mlfit,"READ");
  if( !file.IsOpen() ) {
    std::cerr << "ERROR opening file '" << mlfit << "' ... skipping" << std::endl;
    //throw std::exception();
  } else {

    // store POI value
    if( muValues_ == 0 ) {
      muValues_ = createHistogram("mu","postfitS");
    }
    if( debug_ ) std::cout << "  DEBUG: getting mu" << std::endl;
    RooFitResult* result = 0;
    file.GetObject("fit_s",result);
    if( result == 0 ) {
      std::cerr << "ERROR getting 'fit_s' from file '" << file.GetName() << "' ... skipping" << std::endl;
      // throw std::exception();
    } else {
      const RooRealVar* var = static_cast<RooRealVar*>( result->floatParsFinal().find("r") );
      if( debug_ ) std::cout << "    DEBUG: found mu = " << var->getVal() << std::endl;
      muValues_->Fill( var->getVal() );
      
      // if called the first time, get list of NPs
      if( nps_.empty() ) {
	if( debug_ ) std::cout << "  DEBUG: initialize NPs" << std::endl;
	initContainers(file);
      }
      // store nuisance parameter values
      if( debug_ ) std::cout << "  DEBUG: store NPs" << std::endl;
      if( debug_ ) std::cout << "    DEBUG: prefit NPs" << std::endl;
      try {
	storeRooArgSetResults(npValuesPrefit_,file,"nuisances_prefit");
      } catch (...) {
	std::cerr << "WARNING: no 'nuisances_prefit' object in '" << mlfit << "'" << std::endl;
      }
      if( debug_ ) std::cout << "    DEBUG: postfit B NPs" << std::endl;
      try {
	storeRooFitResults(npValuesPostfitB_,file,"fit_b");
      } catch (...) {
	std::cerr << "WARNING: no 'fit_b' object in '" << mlfit << "'" << std::endl;
    }
      if( debug_ ) std::cout << "    DEBUG: postfit S NPs" << std::endl;
      try {
	storeRooFitResults(npValuesPostfitS_,file,"fit_s");
      } catch (...) {
	std::cerr << "WARNING: no 'fit_s' object in '" << mlfit << "'" << std::endl;
      }
      if( debug_ ) std::cout << "  DEBUG: done storing NPs" << std::endl;
    }
    
    file.Close();
  }
}


void PseudoExperiments::addExperiments(const TString& mlfitTemplateName, const unsigned int nExps) {
  for(unsigned int iE = 1; iE <= nExps; ++iE) {
    char idx[10];
    sprintf(idx,"%03d",iE);
    TString name = mlfitTemplateName;
    name.ReplaceAll("*",TString(idx));
    addExperiment(name);
  }
}


void PseudoExperiments::initContainers(TFile& file) {
  RooArgSet* nuisances_prefit = 0;
  file.GetObject("nuisances_prefit",nuisances_prefit);
  if( nuisances_prefit == 0 ) {
    std::cerr << "ERROR getting 'nuisances_prefit' from file '" << file.GetName() << "'" << std::endl;
    throw std::exception();
  }
  TIter it = nuisances_prefit->createIterator();
  RooRealVar* var = static_cast<RooRealVar*>(it.Next());
  while( var != 0 ) {
    nps_.insert( var->GetName() );
    var = static_cast<RooRealVar*>(it.Next());
  }

  createHistograms(npValuesPrefit_,nps_,"prefit");
  createHistograms(npValuesPostfitB_,nps_,"postfitB");
  createHistograms(npValuesPostfitS_,nps_,"postfitS");
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
    throw std::exception();
  } else {
    for(auto& it: hists) {
      it.second->Fill( result->getRealValue(it.first) );
    }
  }
}


void PseudoExperiments::storeRooFitResults(std::map<TString,TH1*>& hists, TFile& file, const TString& name) const {
  RooFitResult* result = 0;
  file.GetObject(name,result);
  if( result == 0 ) {
    std::cerr << "ERROR getting '" << name << "' from file '" << file.GetName() << "'" << std::endl;
    throw std::exception();
  } else {
    for(auto& it: hists) {
      const RooRealVar* var = static_cast<RooRealVar*>( result->floatParsFinal().find( it.first ) );
      it.second->Fill(var->getVal());
    }
  }
}


TH1* PseudoExperiments::getHist(const std::map<TString,TH1*>& hists, const TString& key) const {
  std::map<TString,TH1*>::const_iterator it = hists.find(key);
  if( it == hists.end() ) { 
    std::cerr << "ERROR trying to access histogram for '" << key << "' in '" << label_ << "'" << std::endl;
    throw std::exception();
  }
  return it->second;
}


TH1* PseudoExperiments::getClone(const TH1* h)const {
  const TUUID id;
  const TString name = h->GetName();
  return static_cast<TH1*>(h->Clone(name+":"+id.AsString()));
}

#endif
