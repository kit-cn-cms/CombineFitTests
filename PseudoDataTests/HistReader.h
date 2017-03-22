#ifndef HIST_READER_H
#define HIST_READER_H

#include <exception>
#include <iostream>
#include <map>

#include "TFile.h"
#include "TH1.h"
#include "TString.h"


class HistReader {
public:
  static void closeAllFiles();

  ~HistReader();

  TH1* operator()(const TString& fileName, const TString& histName);
  
  
private:
  static std::map<TString,TFile*> openFiles_;

  TFile* getFile(const TString& fileName);
};

std::map<TString,TFile*> HistReader::openFiles_ = std::map<TString,TFile*>();


void HistReader::closeAllFiles() {
  for(auto& it: openFiles_) {
    it.second->Close();
    delete it.second;
  }
  openFiles_.clear();
}


HistReader::~HistReader() {
  closeAllFiles();
}


TH1* HistReader::operator()(const TString& fileName, const TString& histName) {
  TH1* h = 0;
  TFile* file = getFile(fileName);
  file->GetObject(histName,h);
  if( h == 0 ) {
    std::cerr << "\n\nERROR: no TH1 '" << histName << "' in file '" << fileName << "'" << std::endl;
    throw std::exception();
  }
  h->SetDirectory(0);

  return h;
}


TFile* HistReader::getFile(const TString& fileName) {
  std::map<TString,TFile*>::iterator fit = openFiles_.find(fileName);
  if( fit != openFiles_.end() ) {
    return fit->second;
  } else {
    TFile* file = new TFile(fileName,"READ");
    openFiles_[fileName] = file;
    return file;
  }
}

#endif
