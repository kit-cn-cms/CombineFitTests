#include <iostream>
#include <iomanip>
#include "TH1D.h"
#include "TDirectoryFile.h"
#include "TTree.h"
#include "TString.h"
#include "TFile.h"


int CreateFinalHist(TString  file = "newfile.root")
{
        // Copying file
        TString delfile = file;

        // Reading in file
        TFile* read = new TFile(file.Data());

        // Reading in TList of TDirectories
        TList* List = (TList*) read->GetListOfKeys();

        // Tdir to temporarlily save the data
        TDirectoryFile* Dir;

        // Manipulating file to create new file
        int index = file.Index(".root");
        file.Replace(index,5,"_histo.root");

        // Creating output file for histogramms
        TFile* output = new TFile(file.Data(),"RECREATE");

        while(!List->IsEmpty())
        {
                // Filling temp dir with last entry
                Dir = (TDirectoryFile*) read->Get(List->Last()->GetName());

                // Reading TList of Dir
                TList* tList = (TList*) Dir->GetListOfKeys();

                // Only use the dirs which contain nuisances
                if(!((TString) (List->Last()->GetName())).Contains("Correlation")&&!((TString) (List->Last()->GetName())).Contains("shapes"))
                {
                // Creating Dir to save hists
                TDirectoryFile* HDir = new TDirectoryFile(Dir->GetName(),Dir->GetTitle());

                while(!tList->IsEmpty())
                {
                        // Filling TTree
                        TTree* Ttemp = (TTree*) Dir->Get(tList->Last()->GetName());

			// Setting data values
                        double val = -9999;
                        double err = -9999;
                        double hie = -9999;
                        double loe = -9999;
			double mer = -9999;
			double mod = -9999;
                        // Setting Adresses
                        Ttemp->SetBranchAddress("Value",&val);
                        Ttemp->SetBranchAddress("Error",&err);
                        Ttemp->SetBranchAddress("High_Error",&hie);
                        Ttemp->SetBranchAddress("Low_Error",&loe);
			Ttemp->SetBranchAddress("Mean_Error",&mer);
			Ttemp->SetBranchAddress("Mode",&mod);
                        // Creating Histogramms
			TH1D* HistVal = new TH1D((TString("Value_").Append(Ttemp->GetName())).Data(),"Values",1000,0.6,3);
                        TH1D* HistErr = new TH1D((TString("Error_").Append(Ttemp->GetName())).Data(),"Errors",1000,0,20);
                        TH1D* HistHiE = new TH1D((TString("Hi_Er_").Append(Ttemp->GetName())).Data(),"High_Error",1000,0,20);
                        TH1D* HistLoE = new TH1D((TString("Lo_Er_").Append(Ttemp->GetName())).Data(),"Low_Error",1000,-20,20);
			TH1D* HistMer = new TH1D((TString("Me_Er_").Append(Ttemp->GetName())).Data(),"Mean_Error",1000,0,20);
                        TH1D* HistMod = new TH1D((TString("Mod_Val_").Append(Ttemp->GetName())).Data(),"Mod_Value",1000,0.6,3);
			for(int i = 0; i < Ttemp->GetEntries(); ++i)
                        {
                                Ttemp->GetEntry(i);
                                HistVal->Fill(val);
                                HistErr->Fill(err);
                                HistHiE->Fill(hie);
                                HistLoE->Fill(loe);
				HistMer->Fill(mer);
				HistMod->Fill(mod);
                        }

                        HDir->Add(HistVal);
			HDir->Add(HistErr);
			HDir->Add(HistHiE);
			HDir->Add(HistLoE);
			HDir->Add(HistMer);
			HDir->Add(HistMod);
                        // Removing last entry from tlist
                        tList->RemoveLast();
                }
                HDir->Write();
                }
                // Removing last entry of list
                List->RemoveLast();
        }
        output->Close();
        return 0;
}
