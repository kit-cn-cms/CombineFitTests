#ifndef PROCESS_H
#define PROCESS_H

#include <exception>
#include <iostream>
#include <map>

#include "TH1.h"
#include "TUUID.h"

#include "Category.h"
#include "HistReader.h"


class Process {
public:
  enum Type { data,
	      ttHbb, ttHcc, ttHgg, ttHgluglu, ttHtt, ttHww, ttHzg, ttHzz,
	      ttbb, ttb, tt2b, ttcc, ttlf };
  static TString toString(const Type type);
  static TString toString(const Process& process) {
    return toString(process());
  }
  static TString histName(const Type type, const Category::Type category);

  Process(const Type type, const TString& fileName, const double scale=1.);
  ~Process();

  void addCategory(const Category::Type category);

  Type operator()() const {
    return type_;
  }
  TString toTString() const {
    return toString(type_);
  }
  unsigned int nBins(const Category::Type category) const {
    return getTemplate(category)->GetNbinsX();
  }
  double binContent(const Category::Type category, const unsigned int bin) const {
    return getTemplate(category)->GetBinContent(bin);
  }
  

private:
  Type type_;
  TString fileName_;
  double scale_;
  std::map<Category::Type,TH1*> templates_;

  HistReader histReader_;

  TH1* getTemplate(const Category::Type category) const;
};


Process::Process(const Type type, const TString& fileName, const double scale)
  : type_(type), fileName_(fileName), scale_(scale) {}


Process::~Process() {
  for(auto& it: templates_) {
    delete it.second;
  }
  templates_.clear();
}


void Process::addCategory(const Category::Type category) {
  //  std::cout << toString(type_) << ": adding category " << Category::toString(category) << std::endl;
  std::map<Category::Type,TH1*>::const_iterator it = templates_.find(category);
  if( it != templates_.end() ) {
    std::cerr << "WARNING: template for '" << Category::toString(category) << "' already exists in '" << toString(type_) << "' --> skipping" << std::endl;
  } else {
    TH1* h = histReader_(fileName_,histName(type_,category));
    TUUID id;
    TString name = h->GetName();
    h->SetName(name+id.AsString());
    h->Scale(scale_);
    templates_[category] = h;
  }
}


TH1* Process::getTemplate(const Category::Type category) const {
  std::map<Category::Type,TH1*>::const_iterator it = templates_.find(category);
  if( it == templates_.end() ) {
    std::cerr << "ERROR: no template for '" << Category::toString(category) << "' in '" << toString(type_) << "'" << std::endl;
    throw std::exception();
  }
  return it->second;
}


TString Process::histName(const Type type, const Category::Type category) {
  return Process::toString(type)+"_finaldiscr_"+Category::toString(category);
}


TString Process::toString(const Process::Type type) {
  if( type == data  ) return "data_obs";

  if( type == ttHbb ) return "ttH_hbb";
  if( type == ttHcc ) return "ttH_hcc";
  if( type == ttHgg ) return "ttH_hgg";
  if( type == ttHgluglu ) return "ttH_hgluglu";
  if( type == ttHtt ) return "ttH_htt";
  if( type == ttHww ) return "ttH_hww";
  if( type == ttHzg ) return "ttH_hzg";
  if( type == ttHzz ) return "ttH_hzz";
  
  if( type == ttbb  ) return "ttbarPlusBBbar";
  if( type == ttb   ) return "ttbarPlusB";
  if( type == tt2b  ) return "ttbarPlus2B";
  if( type == ttcc  ) return "ttbarPlusCCbar";
  if( type == ttlf  ) return "ttbarOther";
  
  return "UNDEFINED";
};
#endif
