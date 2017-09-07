#inlcude "TROOT.h"
#include "TChain.h"
#include "TH1F.h"
#include "TF1.h"
#include "TFile.h"
#include "iostream"
#include "iomanip"

using namespace std;

TFolder TConvert(char **filename)
{
	// Veryfying filename
	if(double(filename) == 0)
	{
	cout << "There is no file to read, nothing will be done!" << endl;
	}

	//Readng in file
	char pathnam = ""; //<-FIX ME!!
	pathname += filename[1];
	TFile * file = new TFile(pathname);

	// Seting output variables
	TFolder * background("background", ttHb);
	TFolder * signal("signal",ttHsb);

	// Seting folders for correlation matrixes
	TFolder * fb_cor("fb_cor");
	TFolder * fs_cor("fs_cor");

	// Seting folders for shapes
	TFolder * fb_shape("fb_shape");
	TFolder * fs_shape("fs_shape");

	// Seting Trees for fit values
	TTree * tpreb("tpreb");
	TTree * tpres("tpres";

	TTree * tpostb("tpostb");
	TTree * tposts("tposts");

	//Iterators
	/// Iterator prefit
	TIter  itpre = nuisances_prefit_res->floatParsInit().CreateIterator(); 

	/// Iterators postfit
	TIter  itpostb = fit_b->floatParsInit().CreateIterator();
	TIter  itposts = fit_s->floatParsInit().CreateIterator();

	/// Second set of Iterators for correlation matrix
	TIter  itpostb_cor = fit_b->floatParsInit().createIterator();
	TIter  itposts_cor = fit_s->floatParsInit().createIterator();

	/// Iterator for shapes
	TIter * itshapesb = shapes_fit_b->GetListOfKeys();
	TIter * itshapess = shapes_fit_s->GetListOfKeys();


	// Variables for nuisances
	RooRealVar varpre; 
	RooRealVar varb, varb_cor;
	RooRealVar vars, vars_cor;

	// Keys for shapes
	TKey * lshapeb;
	TKey * lshapes;

	// Iterate for prefit values
	
	while(varpre =(RooRealVar*) itpre.Next())
	{
		RooRealVar * Ttempb(varpre.GetName(),varpre.GetName(),varpre.GetVal(),varpre.GetLow(),varpre.GetHigh()); // NOT DONE!!!
		RooRealVar * Rtemps(varpre.GetName(),varpre.GetName(),varpre.GetVal(),varpre.GetLow(),varpre.GetHigh()); // NOT DONE!!!
		tpreb->SetBranch(varpre.GetName(),varb); //<-NOT DONE!!!
		tpres->SetBranch(varpre.GetName(),vars); //<-NOT DONE!!!
		tpreb->Fill();
		tpres->Fill();
	}

	// Iterate for postfit values

	while(varb =(RooRealVar*) itpostb.Next())
	{
		// Filling TTree for new nuisances values
		RooRealVar * var(varb.GetName(), varb.GetName(), varb.GetVal(), varb.GetLow(), varb.GetHigh()) // NOT DONE!!!
		tpostb->SetBranch(varb.GetName(),var); //<-NOT DONE!!!
		tpostb->Fill();
		// Generating TTrees for correlation Matrix
		TTree * ttemp  = new TTree(varb.GetName(),varb.GetName()); //<- NOT DONE!!!
	
	// Filling TTrees of correlation Matrix
		while(varb_cor = itpostb_cor.Next())
		{
			Double_t val = correlation(varb,varb_cor); //<- NOT DONE!!!
			ttemp->SetBranch(varb.GetName(),val); // NOT DONE!!!
			ttemp->Fill();
		}
		// Adding TTrees to TFolders
		fb_cor->Add(temp); // <-NOT DONE!!!
	}

	while(vars = itposts.Next())
	{
		while(vars_cor = itposts_cor.Next())
		{
		}
	}

	// Integrate over shapes and save
	
	while(lshapeb =  itshapesb.Next()) // NOT DONE!!!
	{
		TTree * ttemp(lshapeb.GetName(),lshapeb.GetName());
		TIter ittemp = lshapeb.floatInitPars().createIterator();
		RooRealVar vartempb;
		while(vartempb = ittemp.Next())
		{
			ttemp->SetBranch(vartempb.GetName(),vartempb.Integrate());
		}
		fb_shape -> Add(ttemp);
	}

	while(lshapes = itshapess.Next())
	{
		
	}


	// Combining all Objects into the two TFolders
	
	background -> Add(tpreb);
	background -> Add(tpostb);
	background -> Add(fb_cor);
	background -> Add(fb_shape);

	signal -> Add(tpres);
	signal -> Add(tposts);
	signal -> Add(fs_cor);
	signal -> Add(fs_shape);

	// returning TFolders

	return background, signal;
}
