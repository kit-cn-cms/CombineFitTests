#include "TROOT.h"
#include "TH1D.h"
#include "TF1.h"
#include "TFile.h"
#include "iostream"
#include "iomanip"
#include "RooFitResult.h"
#include "RooArgSet.h"
#include "RooRealVar.h"
#include "TTree.h"
#include "TFolder.h"
#include "TString.h"


void test1(TString filename = "mlfit.root", bool mod =0)
{
	// Reading in data
	TFile* file = new TFile(filename.Data());
	std::cout << "Opened data\n";
        if (file->IsOpen() && !file->IsZombie() && !file->TestBit(TFile::kRecovered) )
        {
		// Retrieving right data
		RooFitResult* fitb;
		if(mod==0){file->GetObject("fit_b",fitb);}
		else file->GetObject("fit_s",fitb);
		
		std::cout << "loaded data\n";
	        TFile* output = new TFile("testcor.root","RECREATE");

		// Initilaising Folder to save values
		TFolder * Fpostb = new TFolder("Correlation", "Correlation");

		// Creating iterators
		TIter Ipostb_one = fitb->floatParsFinal().createIterator();
		std::cout << "created iterators\n";

		// Initialising Vars to temporarily save data
		RooRealVar* Varpostb_one;
		RooRealVar* Varpostb_two;
		TTree* Tpostb = NULL;
		int count;
		while((Varpostb_one = (RooRealVar*) Ipostb_one.Next()))
		{
			count = 0;
			Double_t* values = new Double_t[fitb->floatParsFinal().getSize()];
			std::cout << "before creating new TTree: Tpostb = " << Tpostb <<std::endl;
			Tpostb = new TTree(Varpostb_one->GetName(),Varpostb_one->GetName());
			std::cout << "after creating new TTree: Tpostb = " << Tpostb <<std::endl;
			TIter Ipostb_two = fitb->floatParsFinal().createIterator();	
			while((Varpostb_two=(RooRealVar*) Ipostb_two.Next()))
			{
				values[count] = fitb->correlation(Varpostb_one->GetName(), Varpostb_two->GetName());
				Tpostb->Branch(Varpostb_two->GetName(), &values[count], "correlation/D");
				std::cout << "filling for parameter " << Varpostb_two->GetName()  << " value " << values[count] << " into tree " << Tpostb->GetName() << std::endl;
				count++;
			}
			for(int i=0; i<count; i++) std::cout << "\t" << values[i] << std::endl;
			Tpostb->Fill();
			Fpostb->Add(Tpostb);
			delete[] values;
		}
	Fpostb->Write();
	output->Close();
	}
}
