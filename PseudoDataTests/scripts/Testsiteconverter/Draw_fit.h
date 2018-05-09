#include <iostream>
#include <iomanip>
#include "TH1D.h"
#include "TDirectoryFile.h"
#include "TTree.h"
#include "TString.h"
#include "TFile.h"

int CreateHist(TString  file = "newfile.root")
{
	// Copying file
	TString delfile = file;

	// Reading in file
	TFile* read = new TFile(file.Data());
	
	// Reading in TList of TDirectories
	TList* List = (TList*) read->GetListOfKeys();
	
	// Tdir to temporarlily save the data
	TDirectoryFile* Dir;

	// Manipulating file to create new file
	int index = file.Index(".root");
	file.Replace(index,5,"_histo.root");

	// Creating output file for histogramms
	TFile* output = new TFile(file.Data(),"RECREATE");

	while(!List->IsEmpty())
	{
		// Filling temp dir with last entry
		Dir = (TDirectoryFile*) read->Get(List->Last()->GetName());

		// Reading TList of Dir
		TList* tList = (TList*) Dir->GetListOfKeys(); 

		// Only use the dirs which contain nuisances
		if(!((TString) (List->Last()->GetName())).Contains("Correlation")&&!((TString) (List->Last()->GetName())).Contains("shapes"))
		{
		// Creating Dir to save hists
		TDirectoryFile* HDir = new TDirectoryFile(Dir->GetName(),Dir->GetTitle());
	
		while(!tList->IsEmpty())
		{
			// Filling TTree
			TTree* Ttemp = (TTree*) Dir->Get(tList->Last()->GetName());
			
			// Setting data values
			double val = -9999;
			double err = -9999;
			double hie = -9999;
			double loe = -9999;
			double maxv[1000] = {-9999};
			// Setting Adresses
			Ttemp->SetBranchAddress("Value",&val);
			Ttemp->SetBranchAddress("Error",&err);
			Ttemp->SetBranchAddress("High_Error",&hie);
			Ttemp->SetBranchAddress("Low_Error",&loe);
			// Creating Histogramms
			TH1D HistVal = TH1D((TString("Value").Append(Ttemp->GetName())).Data(),"Values",300,-10,10);
			TH1D HistErr = TH1D((TString("Error").Append(Ttemp->GetName())).Data(),"Errors",1000,-20,20);
			TH1D HistHiE = TH1D((TString("Hi_Er").Append(Ttemp->GetName())).Data(),"High_Errors",1000,-20,20);
			TH1D HistLoE = TH1D((TString("Lo_Er").Append(Ttemp->GetName())).Data(),"Low_Errors",1000,-20,20);

			for(int i = 0; i < Ttemp->GetEntries(); ++i)
			{
				Ttemp->GetEntry(i);
				HistVal.Fill(val);
				HistErr.Fill(err);
				HistHiE.Fill(hie);
				HistLoE.Fill(loe);
				maxv[i] = val;
			}

			// Creating variables to save means
			double sval = -999;
			double serr = -999;
			double shie = -999;
			double sloe = -999;
			double mene = -999;
			double m2er = -999;		
			// Creating TTree and Branches to save means of Hists
			TTree* Tsave = new TTree(Ttemp->GetName(), Ttemp->GetTitle());
			Tsave->Branch("Value", &sval, "value[1]/D");
			Tsave->Branch("Error", &serr, "error[1]/D");
			Tsave->Branch("High_Error", &shie, "high_error[1]/D");
			Tsave->Branch("Low_Error", &sloe, "low_error[1]/D");
			Tsave->Branch("Mean_Error", &mene, "mean_error[1]/D");
			Tsave->Branch("Error_of_Mean",&m2er, "error_of_mean[1]/D");

			// Assigning Means
			sval = HistVal.GetMean();
			serr = HistErr.GetMean();
			shie = HistHiE.GetMean();
			sloe = HistLoE.GetMean();
			mene = HistVal.GetMeanError();
			m2er = HistVal.GetRMS();
			Tsave->Fill();

			HDir->Add(Tsave);
			// Removing last entry from tlist
			tList->RemoveLast();
		}
		HDir->Write();
		}
		// Removing last entry of list
		List->RemoveLast();
	}
	output->Close();
	return 0;
}
