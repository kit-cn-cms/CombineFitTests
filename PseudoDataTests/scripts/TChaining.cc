#include "TChain.h"
#include "TString.h"
#include "iostream"
#include "TROOT.h"
#include "TTree.h"
#include "TFile.h"


int chaining(TString Filename = "mlfit.root")
{
	TString* firstfile = new TString(Filename);
	while(firstfile->MaybeWildcard())
	{
		int aces = firstfile->Index("*",1);
		firstfile->Replace(aces,1,"0");
	}
	// Loading first file as a sample, to dynamically create the right amount of TChains
	TFile* Samplefile = new TFile(firstfile->Data());

	// Creating samplelist
	TList* Lsamples = (TList*) Samplefile->GetListOfKeys();

	// Creating File, to store all the collected Trees
	TFile* output = new TFile("newfile.root","RECREATE"); // <- FIX ME!!! It'd be nice to have some sort of dynamic naming of the files, maybe something with the allready generated "firstfile"
ues
	TDirectoryFile* Dlayer1;

	while(Lsamples->GetSize()!=0)
	{
		// Temporalily save subdirectory
		Dlayer1= (TDirectoryFile*) Samplefile->Get(Lsamples->Last()->GetName());
		TDirectoryFile* Ftemp = new TDirectoryFile(Dlayer1->GetName(),Dlayer1->GetTitle());
		TList* Lsub = Dlayer1->GetListOfKeys();
		TTree* Ttemp;

		while((Lsub->GetSize()!=0)&&(!TString(Dlayer1->Get(Lsub->Last()->GetName())->ClassName()).Contains("TDirectoryFile")))
		{
			// Generating location name for chain
			TString* Spath = new TString(Lsamples->Last()->GetName());
			Spath->Append("/");
			Spath->Append(Lsub->Last()->GetName());
			TChain* temp = new TChain(Spath->Data());
			temp->Add(Filename.Data());
			Ttemp =(TTree*) temp->CloneTree(-1);
			Ftemp->Add(Ttemp);
			Lsub->RemoveLast();
		}
		Ftemp->Write();
		Lsamples->RemoveLast();
	}
	output->Close();
	return 0;
}
