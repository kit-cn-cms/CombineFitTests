#ifndef SHAPE_CONTAINER_H
#define SHAPE_CONTAINER_H

#include <exception>
#include <iostream>
#include <map>

#include <TH1D.h>
#include <TString.h>
#include <TFile.h>
#include <TDirectory.h>
#include <TIterator.h>
#include <TKey.h>
#include <TClass.h>
#include <TROOT.h>

#include "helperFuncs.h"

class ShapeContainer{
public:
  ShapeContainer(const TString& category);
  ~ShapeContainer();
  //void setHistogram(const TH1D* histo, const TString& keyName) const;
  void loadShape(const TH1D* histo);
  void loadShapes(TFile& file, const TString& folderName, const TString& signalStrength);
  void loadShapes(TTree* tree, const TString& signalStrength);
  void checkShapeContainer() const;
  void createShapeHistos(const TString& signalStrength);
  TString getName() const;
  int getNumberOfProcesses() const;
  std::vector<TString> getListOfProcesses() const;

  TH1D* getDist(const TString& processName) const;
  Double_t getMean(const TString& processName) const;
  Double_t getMeanError(const TString& processName) const;
  Double_t getRMS(const TString& processName) const;

private:
  bool debug_;
  std::map<TString, TH1D*> shapeHistos_;
  std::map<TString, std::vector<Double_t> > normValues_;
  TString categoryName_;

};

#endif
