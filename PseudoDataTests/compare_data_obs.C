#include "TSystem.h"
#include "TROOT.h"
#include "TSystemFile.h"
#include "TSystemDirectory.h"
#include "TIterator.h"
#include "TKey.h"
#include "TList.h"
#include "TString.h"
#include "TH1.h"
#include "TH1D.h"
#include "TFile.h"
#include "TClass.h"
#include "TCanvas.h"

#include <iostream>
#include <vector>
#include <map>
//#include <exception>

void changeDirectory(const TString outDir)
{
  if(!gSystem->cd(outDir.Data()))
  {
    std::cout << "creating new directory " << outDir.Data() << std::endl;
    if( gSystem->mkdir(outDir.Data()) != 0 ) {
      std::cerr << "ERROR creating working directory '" << outDir << "'" << std::endl;
      throw std::exception();
    }
    gSystem->cd(outDir.Data());
  }
}

int getHistos(TKey* key, std::map<TString, std::vector<TH1D*> >& histoCatergories)
{
  TH1D* binHistoTemplate = new TH1D("binHistoTemplate", "Bin Entry; Frequency; ", 100, 0, 100);
  TClass *cl = gROOT->GetClass(key->GetClassName());
  if (!cl->InheritsFrom("TH1")) return 0;
  TH1 *h = (TH1*)key->ReadObj();
  TString helper;

  if((h != NULL))
  {
    bool initNewCategory = false;
    int nBins = h->GetNbinsX();
    TString hname = h->GetName();
    std::map<TString, std::vector<TH1*> >::iterator it = histoCatergories.find(hname);
    if(it == histoCatergories.end()) initNewCategory = true;
    for(int currentBin=1; currentBin<= nBins; currentBin++){
      if(initNewCategory)
      {
        helper.Form("%s_bin%u", hname.Data(), currentBin);
        histoCatergories[hname].push_back((TH1*)binHistoTemplate->Clone(helper.Data()));
      }
      histoCatergories[hname][currentBin-1]->Fill(h->GetBinContent(currentBin));
    }
    delete h;
  }
  delete binHistoTemplate;
  delete cl;

  return 0;
}

void safeHistos(std::map<TString, std::vector<TH1D*> >& histoCategories)
{
  TCanvas can;
  TString outDir;
  for(std::map<TString, std::vector<TH1D*> >::iterator it = histoCategories.begin(); it != histoCategories.end(); it++)
  {
    outDir = sourceDir + "/" + it->first;
    changeDirectory(outDir);

    for(int histos=0; histos<it->second.size(); histos++)
    {
      it->second[histos]->Draw();
      can.SaveAs(TString(it->second[histos]->GetName()+".pdf").Data());
    }
    gSystem->cd("../");
  }
}

void extractBinInfo(TList* folders, const TString sourceDir, const TString targetRootFile){
  std::map<TString, std::vector<TH1D*> > histoCatergories;

  TFile* file = NULL;
  TString filename;

  TSystemFile *pseudoExperimentFolder;
  TString folderName;
  TIter next(folders);
  while ((pseudoExperimentFolder=(TSystemFile*)next())) {
   folderName = pseudoExperimentFolder->GetName();
   if (pseudoExperimentFolder->IsDirectory() && !folderName.EndsWith(".")) {
      filename = sourceDir + "/" + folderName + "/" + targetRootFile;
      file = TFile::Open(filename.Data(), "READ");
      TIter nextFileObject(file->GetListOfKeys());
      TKey* key;
      while((key = (TKey*)nextFileObject()))
      {
        getHistos(key, histoCatergories);
      }
    }
  }
  if(!histoCatergories.empty()) safeHistos(histoCatergories);
  else std::cout << "Could not get any info on bin content\n";
  delete pseudoExperimentFolder;
  delete folders;
  delete file;
}

void compare_data_obs(TString sourceDir, TString targetRootFile = "Data_Obs.root"){

  TSystemDirectory dir(sourceDir.Data(), sourceDir.Data());
  if(sourceDir.EndsWith("/")) sourceDir.Chop();
  TList *folders = dir.GetListOfFiles();
  //if folders are found, go through each one an look for the targetRootFile
  if (folders) {
    extractBinInfo(folders, sourceDir, targetRootFile);
  }
}
