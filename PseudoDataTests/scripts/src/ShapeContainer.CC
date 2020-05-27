#include "../interface/ShapeContainer.h"

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

// void ShapeContainer::loadShape(const TH1D* histo, const TString& signalStrength){
  // if(debug_) std::cout << "DEBUG    reading " << histo->GetName() << std::endl;
  // std::map<TString, TH1D*>::const_iterator it = shapeHistos_.find(histo->GetName());
  // double integral = histo->Integral();
  // if(debug_) std::cout << "\t integral value: " << integral << std::endl;
  // if(debug_) checkShapeContainer();
  // double lowerBound = 0;
  // double upperBound = 10;
  // int nBins = 0;
  // if(it == shapeHistos_.end())
  // {
    // if(debug_) std::cout << "DEBUG    creating new histogram\n";
    // TString tempHistoName;
    // tempHistoName.Form("%s_%s_%s_intDist", signalStrength.Data(), categoryName_.Data(), histo->GetName());

    // if(integral == 0)
    // {
      // lowerBound = -500.;
      // upperBound = 500.;
    // }
    // else
    // {
      // lowerBound = -10*integral;
      // upperBound = 10*integral;
    // }
    // nBins = int((upperBound - lowerBound)/0.5);

    // //if(lowerBound>-2) lowerBound = -2;
    // if(nBins <= 0) nBins = 1000;
    // TH1D* hTemp = new TH1D(tempHistoName, "; Integral; Frequency", nBins, lowerBound, upperBound);
    // hTemp->SetDirectory(0);
    // if(debug_)
    // {
      // std::cout << "DEBUG    new histogram parameters:\n";
      // std::cout << "\tname: " << hTemp->GetName() << std::endl;
      // std::cout << "\tnBins: " << hTemp->GetNbinsX() << " (input: 400)\n";
      // std::cout << "\tlower Bound: " << hTemp->GetBinLowEdge(1) << " (input: " << lowerBound <<")\n";
      // std::cout << "\tupper Bound: " << hTemp->GetBinCenter(hTemp->GetNbinsX()) + hTemp->GetBinWidth(hTemp->GetNbinsX())/2 << " (input: " << upperBound <<")\n";

    // }
    // hTemp->Fill(integral);
    // shapeHistos_[histo->GetName()] = hTemp;
  // }
  // else{
    // if(debug_) std::cout << "DEBUG    found matching histogram! Filling...\n";

    // it->second->Fill(integral);
  // }
// }

void ShapeContainer::loadShape(const TH1D* histo){
  if(debug_) std::cout << "DEBUG    reading " << histo->GetName() << std::endl;
  double integral = histo->Integral();
  if(debug_) std::cout << "\t integral value: " << integral << std::endl;
  normValues_[histo->GetName()].push_back(integral);
  
}

void ShapeContainer::createShapeHistos(const TString& signalStrength){
  TString tempHistoName;
  TString processName;
  for(std::map<TString, std::vector<Double_t> >::const_iterator it = normValues_.begin(); it != normValues_.end(); it++){
    processName = it->first.Data();
    TUUID id;
    tempHistoName.Form("%s_%s_%s_%s_intDist", signalStrength.Data(), categoryName_.Data(), processName.Data(), id.AsString());
    shapeHistos_[processName] = helperFuncs::createHistoFromVector(tempHistoName, it->second);
  }
  normValues_.clear();
}

void ShapeContainer::loadShapes(TFile& file, const TString& folderName, const TString& signalStrength){
  TString helper;
  if(folderName.EndsWith("/")) helper = folderName + categoryName_;
  else helper.Form("%s/%s", folderName.Data(), categoryName_.Data());
  if(file.cd(helper.Data()))
  {
    TDirectory* categoryDir = gDirectory;
    TIter nextHistoObject(categoryDir->GetListOfKeys());
    TKey* key;
    while ((key = (TKey*)nextHistoObject()) && (!key->IsFolder()) ) {
      TClass *cl = gROOT->GetClass(key->GetClassName());
      if(cl->InheritsFrom("TH1") && !cl->InheritsFrom("TH2")) loadShape((TH1D*)key->ReadObj());
      else{
        if(debug_) std::cout << "DEBUG    object " << key->GetName() << " does not inherit from TH1, skipping\n";
      }
    }
    delete key;
    delete categoryDir;
  }
  else std::cerr << "could not change into directory " << helper.Data() << std::endl;

  if(debug_) std::cout << "DEBUG    done saving normalisations for category " << categoryName_.Data() << std::endl;
  if(debug_) checkShapeContainer();
}

void ShapeContainer::loadShapes(TTree* tree, const TString& signalStrength){
  TString histName;
  TString processName;
  TString title;
  TObjArray* array = tree->GetListOfBranches();
  std::cout << "Normalizations for process " << categoryName_ << ":\n";
  for(int i=0; i<array->GetEntries(); i++){
    const TUUID id;
    processName = array->At(i)->GetName();
    histName.Form("%s_%s_%s_intDist", signalStrength.Data(), categoryName_.Data(), processName.Data());
    title.Form(";integral %s;Frequency", processName.Data());
    
    shapeHistos_[processName] = helperFuncs::branch2histo(tree, processName, title, histName);
    
  }
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