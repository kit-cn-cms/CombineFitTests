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

using namespace std;

void test(TString filename = "mlfit.root")
{
	// Reading of data
	TFile* file = new TFile(filename.Data());
	std::cout << "opened file\n";
	// Checking whether file exists and has data saved in it
	if (file->IsOpen() && !file->IsZombie() && !file->TestBit(TFile::kRecovered) )
	{
		// Initalizing fitb
		RooFitResult* fitb;
		file->GetObject("fit_s",fitb);
		std::cout << "loaded fit_b\n";

		// Iterator for fit values
		std::cout << "created iterator\n";
		TIter Ipostb = fitb->floatParsFinal().createIterator();

		// Creating Folder for Trees
		TFolder* Fpostb = new TFolder("test","Nuisances postfit,only background");

		// Creating Varpostb to temporarily save data

		RooRealVar* Varpostb;

		// Opening new TFile to save data

		TFile * output= new TFile("test.root","RECREATE");

		while((Varpostb = (RooRealVar*) Ipostb.Next()))
		{
			// Creating variables for Branches
			double val = -999;
			double err = -999;
			double hie = -999;
			double loe = -999;
			std::cout << "loading variable " << Varpostb->GetName() << std::endl;
			// Creating Tree for prozess to save values in Branches
			TTree* Ttemp = new TTree(Varpostb->GetName(),Varpostb->GetName());
			// Creating Branches for Tree with specific data
			Ttemp->Branch("Value",&val,"value/D");
			Ttemp->Branch("Error",&err,"error/D");
			Ttemp->Branch("HighError",&hie,"higherror/D");
			Ttemp->Branch("LowError",&loe,"lowerrorii/D");
			val = Varpostb->getVal();
                	err = Varpostb->getError();
                	hie = Varpostb->getErrorHi();
                	loe = Varpostb->getErrorLo();
                
			Ttemp->Fill();
			// Adding Trees to folder
			Fpostb->Add(Ttemp);
		}
		// Writing data in new File
		Fpostb->Write();
		output->Close();
	}
}
