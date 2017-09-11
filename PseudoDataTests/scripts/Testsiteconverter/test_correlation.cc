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

		// Initilaising Folder to save values
		TFolder * Fpostb = new TFolder("Correlation", "Correlation");

		// Creating iterators
		TIter Ipostb_one = fitb->floatParsFinal().createIterator();
		TIter Ipostb_two = fitb->floatParsFinal().createIterator();
		std::cout << "created iterators\n";

		// Initialising Vars to temporarily save data
		RooRealVar* Varpostb_one;
		RooRealVar* Varpostb_two;

		// Creating Folder
		TFolder* Fpostb = new TFolder("test","correlation");

		while(Varpostb_one = (RooRealVar*) Ipostb_one.Next())
		{
			TTree* Tpostb = new TFolder(Varpostb_one->GetName(),Varpostb_one->GetName());
			while(Varpostb_two=(RooRealVar*) Ipostb_two.Next())
			{
				value = -999;
				Tpostb->Branch(Varpostb_two->GetName(),&value, "correlation\D");
				value = correlation(Varpostb_one,Varpostb_two);
				Tpost->Fill();
			}
		Fpostb->Add(Tpostb);
		}
	TFile* output = new TFile("testcor.root","RECREATE");
	Fpostb->Write();
	output->close();
}
