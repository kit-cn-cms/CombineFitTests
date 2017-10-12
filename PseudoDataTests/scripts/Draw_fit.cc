#include <iostream>
#include <iomanip>
#include "TH1D.h"
#include "TDirectoryFile.h"
#include "TTree.h"
#include "TString.h"
#include "TFile.h"


int CreateHist(TString * file = "newfile.root")
{
	// Reading in file
	TFile* read = new TFile(file->Data());
	
	// Reading in TList of TDirectories
	TList* List = (TList*) read->GetListOfKeys();
	
	// Tdir to temporarlily save the data
	TDirectoryFile* Dir;

	// Manipulating file to create new file
	int index = file->Index(".",1);
	file->Replace(index,5,"_histo.root");

	// Creating output file for histogramms
	TFile* output = new TFile(file->Data(),"RECREATE");

	while(List->Last()->Data()!=NULL)
	{
		// Filling temp dir with last entry
		Dir = (TDirectoryFile*) read->Get(List->Last()->Data());

		// Reading TList of Dir
		TList* tList = (TList*) Dir->GetListOfKeys(); 

		// Only use the dirs which contain nuisances
		if(!List->Last()->Data().Contains("Correlation")&&!List->Last()->Data().Contains("shapes"))
		{
		// Creating Dir to save hists
		TDirectoryFile* HDir = new TDirectoryFile(Dir->GetName(),Dir->GetTitle());
		while(tList->Last()->Data()!=NULL)
		{
			// Filling TTree
			TTree* Temp = Dir->Get(tList->Last()->Data());
			// Creating subdir to save hists
			TDirectoryFile* subdir = new TDirectoryFile(Ttemp->GetName(),Ttemp->GetTitle());
			// Setting data values
			double val = -9999;
			double err = -9999;
			double hie = -9999;
			double loe = -9999;
			// Setting Adresses
			Ttemp->SetBranchAddress("Value",&val);
			Ttemp->SetBranchAddress("Error",&err);
			Ttemp->SetBranchAddress("High Erro",&hie);
			Ttemp->SetBranchAddress("Low Error",&low);
			// Creating Histogramms
			TH1D* HistVal = new TH1D("Value","Values",1000,0,500);
			TH1D* HistErr = new TH1D("Error","Errors",1000,0,20);
			TH1D* HistHiE = new TH1D("Hi_Er","High Errors",1000,0,20);
			TH1D* HistLoE = new TH1D("Lo_Er","Low Errors",1000,0,20);
			for(int i = 0; i < Ttemp->GetEntries(); ++i)
			{
				Ttemp->GetEntry(i);
				HistVal->Fill(val);
				HistErr->Fill(err);
				HistHiE->Fill(hie);
				HistLoE->Fill(loe);
			}
			subdir->Add(HistVal);
			subdir->Add(HistErr);
			subdir->Add(HistLoE);
			subdir->Add(HistHiE);
			Hdir->Add(subdir);
			// Removing last entry from tlist
			tList->RemoveLast();
		}
		}
		HDir->Write();

		// Removing last entry of list
		List->RemoveLast();
	}
	output->Close();
	return 0;
}
