#include "TROOT.h"
#include "TChain.h"
#include "TH1F.h"
#include "TF1.h"
#include "TFile.h"
#include "iostream"
#include "iomanip"
#include "RooFitResult.h"
#include "RooRealVar.h"
#include "TString.h"
#include "TFolder.h"
#include "TTree.h"

using namespace std;

TFolder* shapes(TFile* file, int mode =0)
{
	TDirectoryFile* Ddirect;
	TFolder* folder;
	if(mode==0){file->GetObject("shapes_prefit",Ddirect);
		folder = new TFolder("shapes_prefit","shapes_prefit");}
	else if(mode == 1){file->GetObject("shapes_fit_b",Ddirect);
		folder = new TFolder("shapes_fit_b","shapes_fit_b");}
        else if(mode == 2){file->GetObject("shapes_fit_s",Ddirect);
		folder = new TFolder("shapes_fit_s","shapes_fit_s");}

	TDirectory* Dirsub;

	TList* List = (TList*) Ddirect->GetListOfKeys();

	while(List->GetSize()!=0)
	{
		Dirsub = (TDirectoryFile*)Ddirect->Get(List->Last()->GetName());
		TList* Listsub = Dirsub->GetListOfKeys();
		TTree* Ttemp= new TTree(Dirsub->GetName(),Dirsub->GetName());
		double temp[Listsub->GetSize()];
		int count =0;

		while(Listsub->GetSize()!=0)
		{

			if(TString(Dirsub->Get(Listsub->Last()->GetName())->ClassName()).Contains("TH1F"))
			{

				temp[count] = ((TH1D*) Dirsub->Get(Listsub->Last()->GetName()))->Integral();
				Ttemp->Branch(Listsub->Last()->GetName(), &temp[count], "Integral/D");
				++count;

			}
			Listsub->RemoveLast();

		}
		Ttemp->Fill();
		folder->Add(Ttemp);
		List->RemoveLast();
	}
	return folder;
}


int TConvert(TString filename= "mlfit.root",int mod = 0)
{
	// Reading file in
	TFile* file = new TFile(filename.Data());
	std::cout << "Opened file!\n";

	// Only execute programm if there's data to read
	
if(file->IsOpen() && !file->IsZombie() && !file->TestBit(TFile::kRecovered))
{
	// Initilazing fitb
	RooFitResult* fitb;
	if(mod == 0){ file->GetObject("nuisances_prefit_res",fitb);}
	else if(mod == 1){file->GetObject("fit_b",fitb);}
	else if(mod == 2){file->GetObject("fit_s",fitb);}
	std::cout << "Filled fitb\n";
	
	// Creating Iterator for fitb
	TIter Ifitb = fitb->floatParsFinal().createIterator();
	std::cout << "Created iterator for fitb\n";

	// Outputfile
	TFile * output = new TFile("test.root","RECREATE"); // <- Vielleicht Name als TString einlesen lassen, damit von aussen kontinuierlich namen vergeben werden koennen

	// TFolder for fitb and for correlation
	TFolder* Fpostb;
	TFolder* Fcorr;
	if(mod == 0){	Fpostb= new TFolder("prefit","Nuisances prefit");
			Fcorr = new TFolder("prefit","Correaltions prefit");}
	else if(mod==1){Fpostb= new TFolder("fitb","Nuisances background");
			Fcorr = new TFolder("Correlatons","Correlations background");}
	else if(mod==2){Fpostb= new TFolder("fits","Nuisances signal");
			Fcorr = new TFolder("Correlations","Correlations signal");}

	// Initilazing temp save
	RooRealVar* Vartemp1;
	RooRealVar* Vartemp2;

	// Initilazing Tree for correlations
	TTree* Tcorr;

	while((Vartemp1 = (RooRealVar*) Ifitb.Next()))
	{
		// creating variables for branches
		double val = -999;
		double err = -999;
		double hie = -999;
		double loe = -999;
		
		// Creating Tree to save values
		TTree * Tnuisances = new TTree(Vartemp1->GetName(),Vartemp1->GetName());
		// Assigning Branches with corresponding values to tree
		Tnuisances->Branch("Value", &val, "value/D");
		Tnuisances->Branch("Error", &err, "error/D");
		Tnuisances->Branch("High Error", &hie, "high error/D");
		Tnuisances->Branch("Low Error", &loe, "low error/D");

		val = Vartemp1->getVal();
		err = Vartemp1->getError();
		hie = Vartemp1->getErrorHi();
		loe = Vartemp1->getErrorLo();

		Tnuisances->Fill();

		Fpostb->Add(Tnuisances);
		
		// creating second iterator for correlations
		TIter Ifit2 = fitb->floatParsFinal().createIterator();

		// Counter to correctly ascribe values
		int count =0;

		// Initializing array to save values
		Double_t* values = new Double_t[fitb->floatParsFinal().getSize()];

		// Creating Tree to save values
		Tcorr = new TTree(Vartemp1->GetName(),Vartemp1->GetName());

		while((Vartemp2 = (RooRealVar*) Ifit2.Next()))
		{
			values[count] = fitb->correlation(Vartemp1->GetName(),Vartemp2->GetName());
			Tcorr->Branch(Vartemp2->GetName(),&values[count],"correlation/D");
			count++;
		}
		Tcorr->Fill();
		Fcorr->Add(Tcorr);
		delete[] values;

	}
	// Folders for shapes
	TFolder* Fshapes = (TFolder*) shapes(file,mod);
	
	Fshapes->Write();
	Fpostb->Write();
	Fcorr->Write();
	output->Close();
}

}
