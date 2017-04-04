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

int getHistos(TKey* key, std::map<TString, std::vector<TH1D*> >& histoCategories)
{
  TH1D* binHistoTemplate = new TH1D("binHistoTemplate", ";Bin Entry; Frequency", 3000, 0, 3000);
  TClass *cl = gROOT->GetClass(key->GetClassName());
  if (!cl->InheritsFrom("TH1")) return 0;
  TH1D *h = (TH1D*)key->ReadObj();
  TString helper;

  if((h != NULL))
  {
    bool initNewCategory = false;
    int nBins = h->GetNbinsX();
    TString hname = h->GetName();
    //std::cout << "found histogram " << hname.Data() << " with " << nBins << " bins!\n";
    //std::cout << "searching for matching key...\n";
    std::map<TString, std::vector<TH1D*> >::iterator it = histoCategories.find(hname);
    if(it == histoCategories.end())
    {
      initNewCategory = true;
      std::cout << "Category " << hname.Data() << " flagged for initialization\n";
    }
    //std::cout << "saving bin contents\n";
    for(int currentBin=1; currentBin<= nBins; currentBin++){
      if(initNewCategory)
      {
        helper.Form("%s_bin%u", hname.Data(), currentBin);
        histoCategories[hname].push_back((TH1D*)binHistoTemplate->Clone(helper.Data()));
	histoCategories[hname][currentBin-1]->SetDirectory(0);
      }
      //std::cout << "filling histogram\n";
      histoCategories[hname][currentBin-1]->Fill(h->GetBinContent(currentBin));
      //std::cout << "content " << h->GetBinContent(currentBin) << " of bin " << currentBin << " safed in " << histoCategories[hname][currentBin-1]->GetName() << std::endl;
    }
    //delete h;
  }
  //delete binHistoTemplate;
  //delete cl;

  return 0;
}

void safeHistos(std::map<TString, std::vector<TH1D*> >& histoCategories, TString sourceDir)
{
  TCanvas can;
  TString outDir;
  TString helper;
  for(std::map<TString, std::vector<TH1D*> >::iterator it = histoCategories.begin(); it != histoCategories.end(); it++)
  {
    outDir = sourceDir + "/" + it->first;
    changeDirectory(outDir);

    for(int histos=0; histos<it->second.size(); histos++)
    {
      it->second[histos]->Draw();
      helper.Form("%s.pdf",it->second[histos]->GetName());
      can.SaveAs(helper.Data());
    }
    gSystem->cd("../");
  }
}

void extractBinInfo(TList* folders, const TString sourceDir, const TString targetRootFile){
  std::map<TString, std::vector<TH1D*> > histoCatergories;

  //TFile* file = NULL;
  TString filename;

  TSystemFile *pseudoExperimentFolder;
  TString folderName;
  TIter next(folders);
  while ((pseudoExperimentFolder=(TSystemFile*)next())) {
   folderName = pseudoExperimentFolder->GetName();
   if (pseudoExperimentFolder->IsDirectory() && !folderName.EndsWith(".")) {
      filename = sourceDir + "/" + folderName + "/" + targetRootFile;
      //std::cout << "trying to open file in " << filename.Data() << std::endl;
      TFile* file = TFile::Open(filename.Data(), "READ");
      if(file){
	TIter nextFileObject(file->GetListOfKeys());
	TKey* key;
        while((key = (TKey*)nextFileObject()))
        {
          getHistos(key, histoCatergories);
        }
 	//std::cout << "trying to delete key\n";
        delete key;
	//std::cout << "deleted key\n";
	//file->Close();
      }
      delete file;
      //std::cout << "deleted pointer file\n";
    }
  }
  if(!histoCatergories.empty()) safeHistos(histoCatergories, sourceDir);
  else std::cout << "Could not get any info on bin content\n";
  delete pseudoExperimentFolder;
  delete folders;
  //delete file;
}

void compare_data_obs(TString sourceDir, TString targetRootFile = "Data_Obs.root"){

  TSystemDirectory dir(sourceDir.Data(), sourceDir.Data());
  if(sourceDir.EndsWith("/")) sourceDir.Chop();
  //std::cout << "sourceDir: " << sourceDir.Data() << std::endl;
  TList *folders = dir.GetListOfFiles();
  //if folders are found, go through each one an look for the targetRootFile
  if (folders) {
    extractBinInfo(folders, sourceDir, targetRootFile);
  }
}
