#include "TChain.h"
#include "TString.h"
#include "iostream"
#include "TROOT.h"
#include "TTree.h"
#include "TFile.h"
#include "TDirectory.h"

int chaining(TString Filename = "fitDiagnosics1.root", TString Output = "combined_fits.root")
{
	bool proper = false;
	int i = 1;
	TFile* output;
	TFile* Samplefile;
	TList* Lsamples;
	while(!proper)
	{
	// Converting int to char
	int templ =0;
	int tempi = i;
	do{++templ;
	tempi/=10;} while(tempi);
	char tempc[templ];
	sprintf(tempc,"%d",i);
	TString firstfile = Filename;
	while(firstfile.MaybeWildcard())
	{	int aces = firstfile.Index("*",1);
		firstfile.Replace(aces,1,tempc);}

	if(TFile(firstfile.Data()).GetListOfKeys()->Contains("signal"))
	{
		output = TFile::Open(Output.Data(),"RECREATE");
		Samplefile = new TFile(firstfile.Data());
	        Lsamples = (TList*) Samplefile->GetListOfKeys();
		proper = true;
	}
	else {++i;}
	}
	while(Lsamples->GetSize()!=0)
	{
		// Temporalily save subdirectory
		TDirectoryFile* Dir = (TDirectoryFile*) Samplefile->Get(Lsamples->Last()->GetName());
		// Creating subdirectories to save the TTrees from TChains
		output->mkdir(Lsamples->Last()->GetName(),Lsamples->Last()->GetTitle())->cd();
		// Finding all the TTrees within the current Directory
		TList* Lsub = (TList*) Dir->GetListOfKeys();

		while(	(Lsub->GetSize()!=0)&&(!TString(Lsub->Last()->GetTitle()).Contains("TDirectory")))
		{
			// Generating location name for chain
			TString Spath = TString(Lsamples->Last()->GetName());
			Spath += TString("/") + TString(Lsub->Last()->GetName());
			// Calling Chain
			TChain* temp= new TChain(Spath.Data());
			temp->Add(Filename.Data());
			// saving TTree
			temp->Merge(output,1000,"keep, fast");
			Lsub->RemoveLast();
			temp->~TChain();
		}
		Lsamples->RemoveLast();
	}
	output->Close();
	return 0;
}
