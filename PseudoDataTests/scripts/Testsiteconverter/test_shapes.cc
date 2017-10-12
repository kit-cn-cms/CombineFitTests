#include "TF1.h"
#include "TH1D.h"
#include "TFile.h"
#include "iostream"
#include "iomanip"
#include "RooFitResult.h"
#include "RooArgSet.h"
#include "RooRealVar.h"
#include "TTree.h"
#include "TFolder.h"
#include "TString.h"
#include "TList.h"
#include "TGraphAsymmErrors.h"
#include "TDirectoryFile.h"
#include "TDirectory.h"
#include "TKey.h"
#include "TROOT.h"

using namespace std;

void test22(TString filename = "mlfit.root", int mod=0)
{
    // Reading of data
    TFile* file = new TFile(filename.Data());
    std::cout << "opened file\n";
    // Checking whether file exists and has data saved in it
    if (file->IsOpen() && !file->IsZombie() && !file->TestBit(TFile::kRecovered) )
    {
        // Creating and filling directory, according to input
        TDirectoryFile * Ddirect;
        if(mod == 0){file->GetObject("shapes_prefit",Ddirect);}
        else if(mod == 1){file->GetObject("shapes_fit_b",Ddirect);}
        else if(mod == 2){file->GetObject("shapes_fit_s",Ddirect);}
        
        // Creating Iterator for directory
        TIter Idir = Ddirect->GetListOfKeys()->MakeIterator();
        
        // Creating directory for temprary saving
        TDirectory* Dsubdir;
        
        // Opening new File to write data
        TFile* results = new TFile("test_shapes.root","RECREATE");
        
        // Creating Folder to store trees
        TDirectoryFile* folder = new TDirectoryFile("shapes","shapes");
        folder->Write();
        cout << "created all stuff before first while loop\n";
        
        //Creating trees to temp. store values
        TTree* Tree;
        TKey* key;
        TKey* subkey;
        while((key = (TKey*) Idir.Next()))
        {
            cout << "Managed it into fist while loop\n";
            // Creating Iterator for subdirectories
            if(key->IsFolder()){
                Ddirect->cd(key->GetName());
                Dsubdir = gDirectory; 
                Dsubdir->ls();
                
                TList* Lsubdir = (TList*) Dsubdir->GetListOfKeys();
                TIter Isubdir = Lsubdir->MakeIterator();
                
                Tree = new TTree(Dsubdir->GetName(), Dsubdir->GetName());
                Tree->SetDirectory(folder->GetDirectory(folder->GetPath()) );
                // Creating  for temporary saving
                TH1D* Htemp;
               
		int count=0; 
		double_t inte[Dsubdir->GetListOfKeys()->GetSize()];
                while((subkey = (TKey*) Isubdir.Next()))
                {
                    if(!subkey->IsFolder() && TString(subkey->GetClassName()).Contains("TH1") ){
                        cout << "current key: " << subkey->GetName() << endl;
                        Htemp = (TH1D*)Dsubdir->Get(subkey->GetName());
                        inte[count] = Htemp->Integral();
                        Tree->Branch(Htemp->GetName(),&inte[count], "Integral/D");
			++count;
                    }
                }
                Tree->Fill();

                results->cd(folder->GetName());
                Tree->Write();
                delete Tree;
                cout << "added trees\n";
            }
        }
        cout << "Closed loops.\n";
        results->Close();
        cout << "Closed file.\n";
    }
}                                         
