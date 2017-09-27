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


	// Seting TDirectory to save values
	TDirectoryFile* Dlayer1;

	while(Lsamples->GetSize()!=0)
	{
		Dlayer1= (TDirectoryFile*) Samplefile->Get(Lsamples->Last()->GetName());
		TDirectoryFile* Ftemp = new TDirectoryFile(Dlayer1->GetName(),Dlayer1->GetTitle());
		TList* Lsub = Dlayer1->GetListOfKeys();
		std::cout << "Inside first loop, created File and List\n";
		while(TString(Dlayer1->Get(Lsub->Last()->GetName())->ClassName()).Contains("TDirectoryFile"))
		{
			std::cout<< "Inside second loop\n";
			gDirectory->cd(Ftemp->GetName());
			std::cout << "Changed gDir\n";
			TChain* temp = new TChain(Dlayer1->Get(Lsub->Last()->GetName())->GetName());
			temp->Add(Filename.Data());
			std::cout << "Created chain and added stuff\n";
			temp->Merge(output,1000);
			std::cout << "Merged chain\n";
			gDirectory->cd("..");
			std::cout << "Reset gDirectory\n";
			Lsub->RemoveLast();
		}
		if(!TString(Dlayer1->Get(Lsub->Last()->GetName())->ClassName()).Contains("TDirectoryFile"))
		{
			std::cout << "Inside if clause\n";
			TChain* temp = new TChain(Lsamples->Last()->GetName());
			temp->Add(Filename.Data());
			std::cout << "Add funktioniert\n";
			temp->ls();
			temp->Merge(output,1000);
			std::cout << "Lustig, Merge auch\n";
		}
		Lsamples->RemoveLast();
	}
	std::cout << "ended all loops\n";
	output->Close();
	std::cout << "closed file\n";
	return 0;
}
