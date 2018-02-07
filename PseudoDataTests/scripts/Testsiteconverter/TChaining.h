#include "TChain.h"
#include "TString.h"
#include "iostream"
#include "TROOT.h"
#include "TTree.h"
#include "TFile.h"

int chaining(TString Filename = "fitDiagnosics1.root", TString Output = "combined_fits.root")
{
	TString firstfile = Filename;
	while(firstfile.MaybeWildcard())
	{
		int aces = firstfile.Index("*",1);
		firstfile.Replace(aces,1,"1");
	}
	// Loading first file as a sample, to dynamically create the right amount of TChains
	TFile* Samplefile = new TFile(firstfile.Data());

	// Creating File, to store all the collected Trees
	TFile* output = new TFile(Output.Data(),"RECREATE");

	// Creating sample list
	TList* Lsamples = (TList*) Samplefile->GetListOfKeys();

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
			TString Spath = TString(Lsamples->Last()->GetName());
			Spath.Append("/");
			Spath.Append(Lsub->Last()->GetName());
			// Calling Chain
			TChain* temp = new TChain(Spath.Data());
			temp->Add(Filename.Data());
			// retrieving TTree
			Ttemp =(TTree*) temp->CloneTree(-1);
			Ftemp->Add(Ttemp);
			Lsub->RemoveLast();
		}
		Ftemp->Write();
		Lsamples->RemoveLast();
		delete Ftemp;
	}
	output->Close();
	return 0;
}
