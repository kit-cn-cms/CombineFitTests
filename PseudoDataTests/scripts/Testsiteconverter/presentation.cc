#include "iostream"
#include "TFile.h"
#include "TF1.h"
#include "TH1D.h"
#include "TCanvas.h"
#include "TPad.h"

int Draw()
{
	TFile* f1 = new TFile("presentation.root","RECREATE");
	TCanvas* c1 = new TCanvas("Templates","Templates", 700,700);
	c1->Divide(2,1);

	TF1* ex2 = new TF1("exp_2","exp(x*[0])*[1]+[2]",0,10);
	TF1* ex3 = new TF1("exp_3","exp(x*[0])*[1]+[2]",0,10);

	ex2->SetParameters(27*2,-1*1.5,1+0.5);
	c1->cd(1);
	ex2->Draw();
	ex2->SetParameters(27*0.5,-1./1.5,1+1.5);
	ex2->Draw("SAME");

	c1->cd(2);
	ex3->SetParameters(27*3,-1*2,1+1.5);
	ex3->Draw();
	ex3->SetParameters(27./3.,1./3.,1+1.5);
	ex3->Draw("SAME");

	f1->Add(c1);
	f1->Close();
	c1->SaveAs("presentation.png");
	return 0;
}
