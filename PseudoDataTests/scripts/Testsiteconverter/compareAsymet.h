#include <iostream>
#include <iomanip>
#include <cmath>
#include "TCanvas.h"
#include "TH1D.h"
#include "TFile.h"
#include "TString.h"

int compare(TString location1 = "toytest1",TString location2 = "toytest2")
{
	TFile * f1 = TFile::Open(TString(location1+"/Means/final_histo.root").Data());
	TFile * f2 = TFile::Open(TString(location1+"/Values/final_histo.root").Data());
	TFile * f3 = TFile::Open(TString(location2+"/Means/final_histo.root").Data());
	TFile * f4 = TFile::Open(TString(location2+"/Values/final_histo.root").Data());

	// Histogramms 
	TH1D* asy1 = (TH1D*) ((TDirectoryFile*) f1->Get("signal"))->Get("Er_As_r");
	TH1D* asy2 = (TH1D*) ((TDirectoryFile*) f2->Get("signal"))->Get("Er_As_r");
	TH1D* asy3 = (TH1D*) ((TDirectoryFile*) f3->Get("signal"))->Get("Er_As_r");
	TH1D* asy4 = (TH1D*) ((TDirectoryFile*) f4->Get("signal"))->Get("Er_As_r");

	// Scaling
	asy1->Scale(asy2->GetMaximum()/(asy1->GetMaximum()));
	asy3->Scale(asy4->GetMaximum()/(asy3->GetMaximum()));

	asy1->SetLineColor(4);
	asy2->SetLineColor(3);
	asy3->SetLineColor(4);
	asy4->SetLineColor(3);

	asy1->SetTitle("");
	asy2->SetTitle("");
	asy3->SetTitle("");
	asy4->SetTitle("");

	asy1->GetYaxis()->SetTitle("Frequenz");
	asy2->GetYaxis()->SetTitle("Frequenz");
	asy3->GetYaxis()->SetTitle("Frequenz");
	asy4->GetYaxis()->SetTitle("Frequenz");

	asy1->GetXaxis()->SetTitle("Asymmetrie der Signalstaerke");
	asy2->GetXaxis()->SetTitle("Asymmetrie der Signalstaerke");
	asy3->GetXaxis()->SetTitle("Asymmetrie der Signalstaerke");
	asy4->GetXaxis()->SetTitle("Asymmetrie der Signalstaerke");

	asy1->SetStats(0);
	asy2->SetStats(0);
	asy3->SetStats(0);
	asy4->SetStats(0);

	TCanvas * c1 = new TCanvas("new","new",1400,600);
	c1->Divide(2,1);
	c1->cd(1);
	asy2->Draw();
	asy1->Draw("HISTsame");
	c1->cd(2);
	asy4->Draw();
	asy3->Draw("HISTsame");

	c1->SaveAs("plots/comparisonAsymet.png");

	TFile* f5 = TFile::Open("signal_background_example/simple-shapes-TH1.root");

	TCanvas* call = new TCanvas("all","all",1400,1200);
	call->Divide(2,2);
	TCanvas* cs = new TCanvas("sig","sig",700,600);
	TH1D* ssig = (TH1D*) f5->Get("signal");
	TH1D* ssigup = (TH1D*) f5->Get("signal_sigmaUp");
	TH1D* ssigdo = (TH1D*) f5->Get("signal_sigmaDown");

	ssigdo->SetTitle("");
	ssig->SetTitle("");
	ssigup->SetTitle("");

	ssig->GetYaxis()->SetTitle("Frequenz");
	ssigdo->GetYaxis()->SetTitle("Frequenz");
	ssigup->GetYaxis()->SetTitle("Frequenz");

	ssig->GetXaxis()->SetTitle("Bins");
	ssigup->GetXaxis()->SetTitle("Bins");
	ssigdo->GetXaxis()->SetTitle("Bins");

	ssigdo->SetStats(0);
	ssigup->SetStats(0);
	ssig->SetStats(0);

	cs->cd();
	ssigdo->Draw();
	ssig->Draw("HISTsame");
	ssigup->Draw("HISTsame");
	cs->SaveAs("/nfs/dust/cms/user/firin/bachelor/CombineFitTests/PseudoDataTests/scripts/plots/datasig.png");
	call->cd(1);
	ssigdo->Draw();
	ssigup->Draw("HISTsame");
	ssig->Draw("HISTsame");

	TCanvas* cbe = new TCanvas("backe","backe",700,600);
	TCanvas* cba = new TCanvas("backa","backa",700,600);
	TH1D* sback = (TH1D*) f5->Get("background");
	TH1D* sbackexpup = (TH1D*) f5->Get("background_exp_2Up");
	TH1D* sbackexpdo = (TH1D*) f5->Get("background_exp_2Down");
	TH1D* sbackalpup = (TH1D*) f5->Get("background_alphaUp");
	TH1D* sbackalpdo = (TH1D*) f5->Get("background_alphaDown");

	sbackexpup->GetYaxis()->SetTitle("Frequenz");
	sbackexpdo->GetYaxis()->SetTitle("Frequenz");
	sbackalpdo->GetYaxis()->SetTitle("Frequenz");
	sbackalpup->GetYaxis()->SetTitle("Frequenz");
	sback->GetYaxis()->SetTitle("Frequenz");

	sbackexpup->GetXaxis()->SetTitle("Bins");
	sbackexpdo->GetXaxis()->SetTitle("Bins");
	sback->GetXaxis()->SetTitle("Bins");
	sbackalpup->GetXaxis()->SetTitle("Bins");
	sbackalpdo->GetXaxis()->SetTitle("Bins");
	sback->GetXaxis()->SetTitle("Bins");

	sbackexpup->SetTitle("");
	sbackexpdo->SetTitle("");
	sbackalpup->SetTitle("");
	sbackalpdo->SetTitle("");
	sback->SetTitle("");

	sbackexpup->SetStats(0);
	sbackexpdo->SetStats(0);
	sbackalpup->SetStats(0);
	sbackalpdo->SetStats(0);
	sback->SetStats(0);

	sbackexpup->SetLineColor(2);
	sbackexpdo->SetLineColor(4);
	cbe->cd();
	sbackexpup->Draw();
	sbackexpdo->Draw("HISTsame");
	sback->Draw("HISTsame");
	cbe->SaveAs("/nfs/dust/cms/user/firin/bachelor/CombineFitTests/PseudoDataTests/scripts/plots/databacke.png");
	cba->cd();
	sbackalpdo->Draw();
	sbackalpup->Draw("HISTsame");
	sback->Draw("HISTsame");
	cba->SaveAs("/nfs/dust/cms/user/firin/bachelor/CombineFitTests/PseudoDataTests/scripts/plots/databacka.png");
	call->cd(2);
	sbackexpup->Draw();
	sbackexpdo->Draw("HISTsame");
	sback->Draw("HISTsame");
	call->cd(3);
	sbackalpdo->Draw();
	sbackalpup->Draw("HISTsame");
	sback->Draw("HISTsame");

	TCanvas* cda = new TCanvas("data","data",700,600);
	TH1D* sdatas = (TH1D*) f5->Get("data_obs");
	TH1D* sdatao = (TH1D*) f5->Get("data_sig");
	sdatao->GetYaxis()->SetTitle("Frequenz");
	sdatas->GetYaxis()->SetTitle("Frequenz");
	sdatao->SetTitle("");
	sdatas->SetTitle("");
	sdatao->SetStats(0);
	sdatao->SetStats(0);
	cda->cd();
	sdatao->Draw();
	sdatas->Draw("HISTsame");
	cda->SaveAs("/nfs/dust/cms/user/firin/bachelor/CombineFitTests/PseudoDataTests/scripts/plots/datadat.png");
	call->cd(4);
	sdatao->Draw();
	sdatas->Draw("HISTsame");
	call->SaveAs("/nfs/dust/cms/user/firin/bachelor/CombineFitTests/PseudoDataTests/scripts/plots/dataall.png");
	return 0;
}
