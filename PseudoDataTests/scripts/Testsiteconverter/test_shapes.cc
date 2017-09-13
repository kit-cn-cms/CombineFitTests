#include "TF1.h"
#include "TFile.h"
#include "iostream"
#include "iomanip"
#include "RooFitResult.h"
#include "RooArgSet.h"
#include "RooRealVar.h"
#include "TTree.h"
#include "TFolder.h"
#include "TString.h"

using namespace std;

void test(TString filename = "mlfit.root", int mod=0)
{
        // Reading of data
	TFile* file = new TFile(filename.Data());
	std::cout << "opened file\n";
	// Checking whether file exists and has data saved in it
	if (file->IsOpen() && !file->IsZombie() && !file->TestBit(TFile::kRecovered) )
	{
		// Creating and filling directory, according to input
		TDirectory * Dprefit;
		if(mod == 0){file->GetObject("shapes_prefit",Dprefit);}
		else if(mod ==1){file->GetObject("shapes_fit_b",Dprefit);}
		else if(mod ==2){file->GetObject("shapes_fit_s",Dprefit):}
		
		// Creating Iterator for directory
		TIter Idir = Dprefit->GetListOfKeys()->MakeIterator();

		// Creating directory for temprary saving
		TDirectory* Dsubdir;

		// Opening new File to write data
		TFile* results = new TFile("test_shapes.root","RECREATE");

		// Creating Folder to store trees
		TFolder folder = new TFolder("shapes","shapes");

		while((Dsubdir = (TDirectory*) Idir.Next()))
		{
			// Creating Iterator for subdirectories
			TIter Isubdir = Dsubdir->GetListOfKeys()->MakeIterator();
			
			// Creating Trees for storing values
			TTree* Tree = new TTree(Dsubdir->GetName(), Dsubdir->GetName());

			// Creating  for temporary saving
			//
			while()
			{
				
			}
		}
	}
}                                         
