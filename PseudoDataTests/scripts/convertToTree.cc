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


void change(TFile* file, int mod = 0, TFolder* Fpostb = NULL, TFolder* Fcorr = NULL)
{
	// Initilazing fitb and setting the names of the folders
	RooFitResult* fitb;
	if(mod == 0){		file->GetObject("nuisances_prefit_res",fitb);
				Fpostb= new TFolder("Prefit","Prefit Nuisances");
				Fcorr = new TFolder("Correlation_pre","Correlation_pre");}
	else if(mod == 1){	file->GetObject("fit_b",fitb);
				Fpostb= new TFolder("background","background Nuisances");
				Fcorr = new TFolder("Correlation_bac","Correaltion_bac");}
	else if(mod == 2){	file->GetObject("fit_s",fitb);
				Fpostb= new TFolder("signal","signal Nuisances");
				Fcorr = new TFolder("Correlation_sig","Correlation_sig");}
	
	// Creating Iterator for fitb
	TIter Ifitb = fitb->floatParsFinal().createIterator();
	
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
	Fpostb->Write();
	Fcorr->Write();
}



int TConvert(TString filename = "mlfit")
{
	TString* readfile = new TString(filename);
	readfile->Append(".root");
	TFile* read = new TFile(readfile->Data());

	filename.Append("_test.root");
	TFile* output = new TFile(filename.Data(),"RECREATE");
	TFolder* Ffitpre;
	TFolder* Ffitbac;
	TFolder* Ffitsig;
	TFolder* Fcorpre;
	TFolder* Fcorbac;
	TFolder* Fcorsig;
	TFolder* Fshapepre = (TFolder*) shapes(read,0);
	TFolder* Fshapebac = (TFolder*) shapes(read,1);
	TFolder* Fshapesig = (TFolder*) shapes(read,2);
	change(read,0,Ffitpre,Fcorpre);
	change(read,1,Ffitbac,Fcorbac);
	change(read,2,Ffitsig,Fcorsig);

	Fshapepre->Write();
	Fshapebac->Write();
	Fshapesig->Write();
	
	output->Close();
	return 0;
}
