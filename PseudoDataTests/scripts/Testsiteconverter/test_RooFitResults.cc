#include "TROOT.h"
#include "TChain.h"
#include "TH1F.h"
#include "TF1.h"
#include "TFile.h"
#include "iostream"
#include "iomanip"
#include "RooFitResult.h"
#include "RooRealVar.h"
#include "TTree.h"

using namespace std;

int main()
{
	// Reading of data
	TFile* file = new TFile("mlfit.root");

	// Initalizing fit_b
	RooFitResult* fit_b = (RooFitResult*) file->Get("fit_b");

	// Trees for fit values
	TTree* tpostb;

	// Iterator for fit values
	TIter* itpostb = (TIter*) fit_b->floatParsInit().createIterator();


	// Variables for nuisances
	RooRealVar* varpostb;

	// Tree to save values from Fit
	TTree* Tpostb = new TTree("Tpostb","Nuisances postfit,only background");

	// Test for Iterator
	while((varpostb = (RooRealVar*) itpostb->Next()))
	{
		double_t cat[3];
		Tpostb->Branch(varpostb->GetName(),cat,"cat[3]/D");
		cat[0] = varpostb->GetVal();
		cat[1] = varpostb->GetError();
		cat[2] = varpostb->GetHigh();
		cat[3] = varpostb->GetLow();
		Tpostb->Fill();
	}

	Tpost.Print();
	return 0;
}
