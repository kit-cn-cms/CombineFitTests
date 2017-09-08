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

	// Iterator for fit values
	TIter* Itpostb = (TIter*) fit_b->floatParsInit().createIterator();


	// Variables for nuisances
	RooRealVar* Varpostb;

	// Tree to save values from Fit
	TTree* Tpostb = new TTree("Tpostb","Nuisances postfit,only background");

	// Test for Iterator
	while((Varpostb = (RooRealVar*) Itpostb->Next()))
	{
		double_t cat[4];
		Tpostb->Branch(Varpostb->GetName(),cat,"cat[3]/D");
		cat[0] = Varpostb->getVal();
		cat[1] = Varpostb->getError();
		cat[2] = Varpostb->getErrorHi();
		cat[3] = Varpostb->getErrorLo();
		Tpostb->Fill();
	}

	Tpostb->Scan();
	return 0;
}
