#include "iostream"
#include "TMath.h"
#include "TCanvas.h"
#include "TString.h"
#include "TF1.h"
#include "TGraphErrors.h"
#include "TAttMarker.h"

int uncertain()
{
        TCanvas* c = new TCanvas("Exp_2","Exp_2",1000,600);
	c->Divide(2,1);

	c->cd(1);
        TF1* f1 = new TF1("Exp_2","27*[0]*TMath::Exp(-[1]*x)+1+[2]",0.7,10);
	f1->SetTitle("Uncertainty Exp_2");
        f1->SetParameters(2,1.5,0.5);
	f1->SetLineColor(38);
	f1->SetLineStyle(2);
        f1->DrawCopy();
        f1->SetParameters(0.5,1/1.5,1.5);
        f1->DrawCopy("SAME");

	double x1[10] = {1,2,3,4,5,6,7,8,9,10};
	double y1[10] = {11.49,5.12375,3.21345,2.536,2.256,2.127,2.064,2.033,2.02,2.01};
	double yer1[10] = {2.059,0.93525,1.1135,0.902,0.726,0.625,0.563,0.533,0.52,0.51};

	TGraphErrors * gr1 = new TGraphErrors(10,x1,y1,0,yer1);
	//gr1->SetMarkerStyle(25);
	//gr1->SetMarkerSize(10);
	//gr1->SetMarkerColor(4);
	gr1->SetLineColor(4);
	gr1->Draw("SAME");

	c->cd(2);
	TF1* f2	= new TF1("Exp_3","27*[0]*TMath::Exp(-[1]*x)+2.5",0.7,10);
	f2->SetTitle("Uncertainty Exp_3");
	f2->SetParameters(3,2);
	f2->SetLineColor(43);
	f2->SetLineStyle(2);
	f2->DrawCopy();
	f2->SetParameters(1./3.,1./3.);
	f2->DrawCopy("SAME");

	double y2[10] = {11.206,5.553,4.256,3.700,3.352,3.110,2.937,2.813,2.724,2.661};
	double yer2[10] = {2.2565,1.568,1.555,1.172,0.848,0.608,0.436,0.312,0.224,0.160};

	TGraphErrors* gr2 = new TGraphErrors(10,x1,y2,0,yer2);
	//gr2->SetMarkerStyle(25);
	//gr2->SetMarkerSize(5);
	//gr2->SetMarkerColor(28);
	gr2->SetLineColor(28);
	gr2->Draw("SAME");
	c->SetTitle("Uncertainties");
        c->SaveAs("/nfs/dust/cms/user/firin/bachelor/CombineFitTests/PseudoDataTests/scripts/testplots/Uncertainty1.png");
        return 0;
}
