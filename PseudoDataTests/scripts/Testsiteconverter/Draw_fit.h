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
			Ttemp->SetBranchAddress("High Error",&hie);
			Ttemp->SetBranchAddress("Low Error",&loe);
			// Creating Histogramms
			TH1D HistVal = TH1D((TString("Value").Append(Ttemp->GetName())).Data(),"Values",300,0,3);
			TH1D HistErr = TH1D((TString("Error").Append(Ttemp->GetName())).Data(),"Errors",1000,0,20);
			TH1D HistHiE = TH1D((TString("Hi_Er").Append(Ttemp->GetName())).Data(),"High Errors",1000,0,20);
			TH1D HistLoE = TH1D((TString("Lo_Er").Append(Ttemp->GetName())).Data(),"Low Errors",1000,0,20);

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
			/*
			const int bins = 200;
			int mode[200] = {-999};
			double hist[bins] = {0};
			for(int i =0; i < Ttemp->GetEntries(); ++i)
			{
				for(int j=0; j < bins; ++j)
				{
					if((j*1./((float) bins) <= maxv[i])&&(maxv[i] < (j+1)*1./((float) bins))) {++hist[j];}
				}
			}
			int j = 0;
			for(int i = 0; i < bins; ++i)
			{
				if((hist[i] > hist[mode[0]])&&(hist[i]!=0))
				{
					for(int k =0; k < 10; ++k){mode[k]=0; j=0;}
					mode[0]=i;
				}
				else if((hist[i] == hist[mode[0]])&&(hist[i]!=0))
				{
					++j;
					mode[j] = i;
				}
			}
			double res[j];
			std::cout << Ttemp->GetName() << " : " << j << std::endl;
			for(int i =0; i < j ; ++i)
			{
				res[i] = mode[i]/((float) bins);
				std::cout << res[i] <<" | " << hist[mode[i]] << std::endl;
			}
			*/		
			// Creating TTree and Branches to save means of Hists
			TTree* Tsave = new TTree(Ttemp->GetName(), Ttemp->GetTitle());
			Tsave->Branch("Value", &sval, "value[1]/D");
			Tsave->Branch("Error", &serr, "error[1]/D");
			Tsave->Branch("High_Error", &shie, "high_error[1]/D");
			Tsave->Branch("Low_Error", &sloe, "low_error[1]/D");
			Tsave->Branch("Mean_Error", &mene, "mean_error[1]/D");
			/*
			if(j!=0)
			{
				Tsave->Branch("j", &j, "j/I");
				Tsave->Branch("Mode_Value", res, "res[j]/D");
			}
			*/
			HistVal.SetBins(300,0.,3.);

			// Assigning Means
			sval = HistVal.GetMean();
			serr = HistErr.GetMean();
			shie = HistHiE.GetMean();
			sloe = HistLoE.GetMean();
			mene = HistVal.GetMeanError();
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
