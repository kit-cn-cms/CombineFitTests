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
#include "TDirectory.h"
#include "TDirectoryFile.h"

using namespace std;

bool fitBmustConverge_ = true;
bool fitSBmustConverge_ = true;


TDirectoryFile* shapes(TFile* file, int mode =0)
{
	TDirectoryFile* Ddirect;
	TDirectoryFile* folder;
	if(mode==0){file->GetObject("shapes_prefit",Ddirect);
		folder = new TDirectoryFile("shapes_prefit","shapes_prefit");}
	else if(mode == 1){file->GetObject("shapes_fit_b",Ddirect);
		folder = new TDirectoryFile("shapes_fit_b","shapes_fit_b");}
        else if(mode == 2){file->GetObject("shapes_fit_s",Ddirect);
		folder = new TDirectoryFile("shapes_fit_s","shapes_fit_s");}

	Ddirect->Print();

	TDirectoryFile* Dirsub;

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


void change(TFile* file, int mod = 0, TDirectoryFile* Fpostb = NULL, TDirectoryFile* Fcorr = NULL)
{
	// Initilazing fitb and setting the names of the folders
	RooFitResult* fitb;
	if(mod == 0){		file->GetObject("nuisances_prefit_res",fitb);
				Fpostb= new TDirectoryFile("Prefit","Prefit Nuisances");
				Fcorr = new TDirectoryFile("Correlation_pre","Correlation_pre");}
	else if(mod == 1){	file->GetObject("fit_b",fitb);
				Fpostb= new TDirectoryFile("background","background Nuisances");
				Fcorr = new TDirectoryFile("Correlation_bac","Correaltion_bac");}
	else if(mod == 2){	file->GetObject("fit_s",fitb);
				Fpostb= new TDirectoryFile("signal","signal Nuisances");
				Fcorr = new TDirectoryFile("Correlation_sig","Correlation_sig");}
	
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

bool checkFitStatus(TFile* file){
	bool storeExperiment = true;
	int fit_status=7;
	TString fit_trees[2] = {"tree_fit_sb", "tree_fit_b"};
	bool fit_flags[2] = {fitSBmustConverge_, fitBmustConverge_};
	for(int nTrees=0; nTrees<2; nTrees++)
	{
		TTree* tree = (TTree*)file->Get(fit_trees[nTrees].Data());
		if(tree != NULL)
		{
			if(tree->SetBranchAddress("fit_status",&fit_status)>= 0)
			{
				tree->GetEntry(0);
				if((fit_status != 0))
				{
					std::cout << "WARNING fit_status in " << fit_trees[nTrees].Data() << " did not converge!\n";
					if(fit_flags[nTrees]){ storeExperiment = false;}
				}
			}
			fit_status=7;
		}
	else{
	std::cerr << "ERROR   could not load tree " << fit_trees[nTrees].Data() << " from file " << file->GetName() << std::endl;
	storeExperiment = false;
	}
	}
	return storeExperiment;
}



bool checkCovarianceMatrix(TFile* file){
  bool accurateCovariance = false;
  TString rooFitObjects[2] = {"fit_b", "fit_s"};
  int quality = -1;
  bool qualities[2] = {false, false};
  for(int currentObject=0; currentObject < 2; currentObject++){
    RooFitResult* fitObject = 0;
    file->GetObject(rooFitObjects[currentObject].Data(),fitObject);
    if( fitObject == 0 ) {
      std::cerr << "ERROR getting '" << rooFitObjects[currentObject].Data() << "' from file '" << file->GetName() << "'" << std::endl;
      //throw std::exception();
    }
    else{
      quality=-1;
      quality=fitObject->covQual();
      bool debug_ = true;
      if(debug_)
      {
        std::cout << "    DEBUG: quality of covariance matrix in " << rooFitObjects[currentObject].Data() << " is " << quality;
        if(quality==-1) std::cout << rooFitObjects[currentObject].Data() << ": Unknown, matrix was externally provided\n";
        if(quality==0) std::cout << rooFitObjects[currentObject].Data() << ": Not calculated at all\n";
        if(quality==1) std::cout << rooFitObjects[currentObject].Data() << ": Approximation only, not accurate\n";
        if(quality==2) std::cout << rooFitObjects[currentObject].Data() << ": Full matrix, but forced positive-definite\n";
      }
      if(quality==3) {
        if(debug_) std::cout << rooFitObjects[currentObject].Data() << ": Full, accurate covariance matrix\n";
        qualities[currentObject] = true;
      }
    }
    delete fitObject;
  }
  if(!fitSBmustConverge_ && !fitBmustConverge_) return true;
  if(fitBmustConverge_){
    if(qualities[0]) accurateCovariance = true;
  }
  if(fitSBmustConverge_){
    if(qualities[1]) accurateCovariance = true;
    else accurateCovariance = false;
  }
  return accurateCovariance;
}


void TConvert(TString filename = "fitDiagnostics",TString savehere = "none" , TString outputopt = "_test")
{
	TString readfile = filename;
	readfile.Append(".root");
	TFile* read = new TFile(readfile.Data());
	if( read->IsOpen() && !read->IsZombie() && !read->TestBit(TFile::kRecovered)){
		if( checkCovarianceMatrix(read) && checkFitStatus(read))
		{
			
			filename.Append(outputopt.Data());
			TString savedir;
			// This will change the directory in which the resulting file is saved. This is usefull if you need to collect several files in a single folder. By default it is switched off
			if(!savehere.Contains("none"))
				{
				savedir = TString("/nfs/dust/cms/user/firin/bachelor/CombineFitTests/PseudoDataTests/scripts/");
				savedir.Append(savehere.Data());
				savedir.Append("/");
				savedir.Append(filename.Data());
				}
			else savedir = filename;
			savedir.Append(".root");
			TFile* output = new TFile(savedir.Data(),"RECREATE");
			TDirectoryFile* Ffitpre;
			TDirectoryFile* Ffitbac;
			TDirectoryFile* Ffitsig;
			TDirectoryFile* Fcorpre;
			TDirectoryFile* Fcorbac;
			TDirectoryFile* Fcorsig;
			TDirectoryFile* Fshapepre = shapes(read,0);
			TDirectoryFile* Fshapebac = shapes(read,1);
			TDirectoryFile* Fshapesig = shapes(read,2);
			change(read,0,Ffitpre,Fcorpre);
			change(read,1,Ffitbac,Fcorbac);
			change(read,2,Ffitsig,Fcorsig);

			Fshapepre->Write();
			Fshapebac->Write();
			Fshapesig->Write();
			output->Close();

		}
		read->Close();
	}
}
