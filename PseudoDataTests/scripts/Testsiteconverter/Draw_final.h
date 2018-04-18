#include <iostream>
#include "iomanip"
#include "cmath"
#include <iomanip>
#include "TH1D.h"
#include "TDirectoryFile.h"
#include "TTree.h"
#include "TString.h"
#include "TFile.h"


int CreateFinalHist(TString  file = "newfile.root", float signal = 1.0)
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
			double m2e = -9999;
			//double mod = -9999;
			double tvmin =  9999;
			double tvmax = -9999;
			double temax = -9999;
			double thmax = -9999;
			double tlmax = -9999;
			double tmmax = -9999;
			double t2min =  9999;
			double t2max = -9999;
                        // Setting Adresses
                        Ttemp->SetBranchAddress("Value",&val);
                        Ttemp->SetBranchAddress("Error",&err);
                        Ttemp->SetBranchAddress("High_Error",&hie);
                        Ttemp->SetBranchAddress("Low_Error",&loe);
			Ttemp->SetBranchAddress("Mean_Error",&mer);
			Ttemp->SetBranchAddress("Error_of_Mean",&m2e);
			//Ttemp->SetBranchAddress("Mode",&mod);
                        // Creating Histogramms
			TH1D* HistVal = new TH1D((TString("Value_").Append(Ttemp->GetName())).Data(),"Values",1000,-10,10);
                        TH1D* HistErr = new TH1D((TString("Error_").Append(Ttemp->GetName())).Data(),"Errors",1000,-10,10);
                        TH1D* HistHiE = new TH1D((TString("Hi_Er_").Append(Ttemp->GetName())).Data(),"High_Error",1000,-10,10);
                        TH1D* HistLoE = new TH1D((TString("Lo_Er_").Append(Ttemp->GetName())).Data(),"Low_Error",1000,-10,10);
			TH1D* HistMer = new TH1D((TString("Me_Er_").Append(Ttemp->GetName())).Data(),"Mean_Error",1000,-10,10);
			TH1D* HistM2e = new TH1D((TString("Er_Me_").Append(Ttemp->GetName())).Data(),"Error_of_Mean",1000,-10,10);
                        //TH1D* HistMod = new TH1D((TString("Mod_Val_").Append(Ttemp->GetName())).Data(),"Mod_Value",1000,0.6,3);
			for(int i = 0; i < Ttemp->GetEntries(); ++i)
                        {
                                Ttemp->GetEntry(i);
                                HistVal->Fill(val);
                                HistErr->Fill(err);
                                HistHiE->Fill(hie);
                                HistLoE->Fill(loe);
				HistMer->Fill(mer);
				HistM2e->Fill(m2e);
				//HistMod->Fill(mod);
				if(val < tvmin){tvmin = val;}
				if(val > tvmax){tvmax = val;}
				if(err > temax){temax = err;}
				if(hie > thmax){thmax = hie;}
				if(loe > tlmax){tlmax = loe;}
				if(mer > tmmax){tmmax = mer;}
				if(m2e < t2min){t2min = m2e;}
				if(m2e > t2max){t2max = m2e;}
                        }


			if((TString(List->Last()->GetName()).Contains("signal"))&&(TString(tList->Last()->GetName()).Contains("r")))
			{
				double mean = HistVal->GetMean();
				double dev = HistVal->GetStdDev();
				int number = Ttemp->GetBranch("Value")->GetEntries();
				double t = (mean-signal)*sqrt(number/(2))/dev;
				std::cout << "Mean: " << mean << std::endl;
				std::cout << "std deviation: " << dev << std::endl;
				std::cout << "amount of values: " << number << std::endl;
				std::cout << "t-value: " << t << std::endl;
				std::cout << "Errorfunction: " << erf(t) << std::endl;
				std::cout << "p_Value: "<< std::setprecision(6) << 1-erf(fabs(t)) << std::endl;
			}

			HistVal->SetAxisRange(tvmin,tvmax);
			HistErr->SetAxisRange(0,temax);
			HistHiE->SetAxisRange(0,thmax);
			HistLoE->SetAxisRange(0,tlmax);
			HistMer->SetAxisRange(0,tmmax);
			HistM2e->SetAxisRange(0,t2max);
			//HistMod->SetAxisRange(20*(HistMod->GetMinimumBin())/1000,20*(HistMod->GetMaximumBin())/1000);


                        HDir->Add(HistVal);
			HDir->Add(HistErr);
			HDir->Add(HistHiE);
			HDir->Add(HistLoE);
			HDir->Add(HistMer);
			HDir->Add(HistM2e);
			//HDir->Add(HistMod);
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
