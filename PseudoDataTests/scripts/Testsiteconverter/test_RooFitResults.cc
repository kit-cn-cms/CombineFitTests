#include "TROOT.h"
#include "TH1D.h"
#include "TF1.h"
#include "TFile.h"
#include "iostream"
#include "iomanip"
#include "RooFitResult.h"
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
	if (file->IsOpen() && !file->IsZombie() && !file->TestBit(TFile::kRecovered) )
	{
	// Initalizing fit_b
	RooFitResult* fit_b = (RooFitResult*) file->Get("fit_b");
	std::cout << "loaded fit_b\n";
	// Iterator for fit values
	TIter Itpostb = fit_b->floatParsInit().createIterator();
	std::cout << "created iterator\n";

	// Variables for nuisances
	RooRealVar* Varpostb;
	TFile* output = new TFile("test.root", "RECREATE");
	// Tree to save values from Fit
	TFolder* Fpostb = new TFolder("Tpostb","Nuisances postfit,only background");
	std::cout << "created folder " << Fpostb->GetName() << std::endl;
	// Test for Iterator

	while((Varpostb = (RooRealVar*) Itpostb.Next()))
	{
                double_t val, err, hie, loe;
		std::cout << "loading variable " << Varpostb->GetName() << std::endl;
		TTree* Ttemp = new TTree(Varpostb->GetName(),Varpostb->GetName());
		Ttemp->Branch("Value",&val,"value/D");
		Ttemp->Branch("Error",&err,"error/D");
		Ttemp->Branch("HighError",&hie,"higherror/D");
		Ttemp->Branch("LowError",&loe,"lowerrorii/D");
		val = Varpostb->getVal();
                err = Varpostb->getError();
                hie = Varpostb->getErrorHi();
                loe = Varpostb->getErrorLo();
		Ttemp->Fill();
		Fpostb->Add(Ttemp);
	}

	Fpostb->Write();
	output->Close();
	}
}
