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

class ShapeContainer{
public:
  ShapeContainer(const TString& category);
  ~ShapeContainer();
  //void setHistogram(const TH1D* histo, const TString& keyName) const;
  void loadShape(const TH1D* histo, const TString& signalStrength);
  void loadShapes(TFile& file, const TString& folderName, const TString& signalStrength);
  void checkShapeContainer() const;
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
  TString categoryName_;

};
// void ShapeContainer::setHistogram(const TH1D* histo, const TString& keyName) const{
//   TH1D* tempHisto = (TH1D*)histo->Clone();
//   TString tempKey = keyName;
//   this->shapeHistos_[tempKey] = tempHisto;
// }
ShapeContainer::ShapeContainer(const TString& category):debug_(false), categoryName_(category)
{
  if(debug_) std::cout << "DEBUG " << this << ":  constructing new shape container for category " << categoryName_.Data() << std::endl;
}

ShapeContainer::~ShapeContainer()
{
  if(debug_) std::cout << "DEBUG "<< this << ":   destructing shape container for category " << categoryName_.Data() << std::endl;
  for(std::map<TString, TH1D*>::const_iterator it = shapeHistos_.begin(); it!= shapeHistos_.end(); it++) delete it->second;
}

void ShapeContainer::loadShape(const TH1D* histo, const TString& signalStrength){
  if(debug_) std::cout << "DEBUG    reading " << histo->GetName() << std::endl;
  std::map<TString, TH1D*>::const_iterator it = shapeHistos_.find(histo->GetName());
  double integral = histo->Integral();
  if(debug_) std::cout << "\t integral value: " << integral << std::endl;
  if(debug_) checkShapeContainer();
  if(it == shapeHistos_.end())
  {
    if(debug_) std::cout << "DEBUG    creating new histogram\n";
    TString tempHistoName;
    tempHistoName.Form("%s_%s_%s_intDist", signalStrength.Data(), categoryName_.Data(), histo->GetName());
    double lowerBound = 0;
    double upperBound = 10;
    if(integral <= 0)
    {
      lowerBound = -100.;
      upperBound = 100.;
    }
    else
    {
      lowerBound = 0.05*integral;
      upperBound = 2.5*integral;
    }
    TH1D* hTemp = new TH1D(tempHistoName, "; Integral; Frequency", 400, lowerBound, upperBound);
    hTemp->SetDirectory(0);
    if(debug_)
    {
      std::cout << "DEBUG    new histogram parameters:\n";
      std::cout << "\tname: " << hTemp->GetName() << std::endl;
      std::cout << "\tnBins: " << hTemp->GetNbinsX() << " (input: 400)\n";
      std::cout << "\tlower Bound: " << hTemp->GetBinLowEdge(1) << " (input: " << lowerBound <<")\n";
      std::cout << "\tupper Bound: " << hTemp->GetBinCenter(hTemp->GetNbinsX()) + hTemp->GetBinWidth(hTemp->GetNbinsX())/2 << " (input: " << upperBound <<")\n";

    }
    hTemp->Fill(integral);
    shapeHistos_[histo->GetName()] = hTemp;
  }
  else{
    if(debug_) std::cout << "DEBUG    found matching histogram! Filling...\n";
    it->second->Fill(integral);
  }
}

void ShapeContainer::loadShapes(TFile& file, const TString& folderName, const TString& signalStrength){
  TString helper;
  if(folderName.EndsWith("/")) helper = folderName + categoryName_;
  else helper.Form("%s/%s", folderName.Data(), categoryName_.Data());
  file.cd(helper.Data());
  TDirectory* categoryDir = gDirectory;
  TIter nextHistoObject(categoryDir->GetListOfKeys());
  TKey* key;
  while ((key = (TKey*)nextHistoObject()) && (!key->IsFolder()) ) {
    TClass *cl = gROOT->GetClass(key->GetClassName());
    if(cl->InheritsFrom("TH1") && !cl->InheritsFrom("TH2")) loadShape((TH1D*)key->ReadObj(), signalStrength);
    else{
      if(debug_) std::cout << "DEBUG    object " << key->GetName() << " does not inherit from TH1, skipping\n";
    }

  }
  if(debug_) std::cout << "DEBUG    done saving normalisations for category " << categoryName_.Data() << std::endl;
  if(debug_) checkShapeContainer();
}

void ShapeContainer::checkShapeContainer()const{
  std::cout << "Checking Container for category " << this->getName() << std::endl;
  std::cout << "\t number of entry (= numberOfProcesses): " << this->getNumberOfProcesses() << std::endl;
  std::cout << "\t getting listOfProcesses...\n";
  std::vector<TString> list = this->getListOfProcesses();
  std::cout << "\t obtained list with " << list.size() << " entries!\n";
  for(int i=0; i<int(list.size()); i++)
  {
    std::cout << "\t Trying to read process " << list[i] << std::endl;
    std::cout << "\t\t Entries in histogram: " << this->getDist(list[i])->GetEntries() << std::endl;
  }
}


TString ShapeContainer::getName()const{
  return categoryName_;
}

int ShapeContainer::getNumberOfProcesses()const {
  return shapeHistos_.size();
}

std::vector<TString> ShapeContainer::getListOfProcesses()const{
  std::vector<TString> listOfProcesses;
  for(std::map<TString, TH1D*>::const_iterator it = shapeHistos_.begin(); it != shapeHistos_.end(); it++) listOfProcesses.push_back(it->first);
  return listOfProcesses;
}

TH1D* ShapeContainer::getDist(const TString& processName)const
{
  TH1D* returnHisto = NULL;
  if(debug_) std::cout << "DEBUG    looking for process " << processName.Data() << std::endl;
  std::map<TString, TH1D*>::const_iterator it = shapeHistos_.find(processName.Data());
  if(it == shapeHistos_.end()){
    std::cerr << "ERROR    unable to load distribution for process " << processName.Data() << " in category " << categoryName_.Data() << ": Histogram does not exist!\n";
  }
  else
  {
    if(debug_) std::cout << "\t match found! Loading from iterator with first entry " << it->first << "\n";
    returnHisto = it->second;
    if(debug_) std::cout << "\t successfully loaded histogram with " << returnHisto->GetEntries() << " entries!\n";
  }
  return returnHisto;
}

Double_t ShapeContainer::getMean(const TString& processName)const
{
  Double_t returnVal = -1;
  std::map<TString, TH1D*>::const_iterator it = shapeHistos_.find(processName.Data());
  if(it == shapeHistos_.end()){
    std::cerr << "ERROR    unable to load mean value for process " << processName.Data() << " in category " << categoryName_.Data() << ": Histogram does not exist!\n";
  }
  else returnVal = it->second->GetMean();
  return returnVal;
}

Double_t ShapeContainer::getMeanError(const TString& processName)const
{
  Double_t returnVal = -1;
  std::map<TString, TH1D*>::const_iterator it = shapeHistos_.find(processName.Data());
  if(it == shapeHistos_.end()){
    std::cerr << "ERROR    unable to load mean error for process " << processName.Data() << " in category " << categoryName_.Data() << ": Histogram does not exist!\n";
  }
  else returnVal = it->second->GetMeanError();
  return returnVal;
}

Double_t ShapeContainer::getRMS(const TString& processName)const
{
  Double_t returnVal = -1;
  std::map<TString, TH1D*>::const_iterator it = shapeHistos_.find(processName.Data());
  if(it == shapeHistos_.end()){
    std::cerr << "ERROR    unable to load RMS for process " << processName.Data() << " in category " << categoryName_.Data() << ": Histogram does not exist!\n";
  }
  else returnVal = it->second->GetRMS();
  return returnVal;
}

#endif
