#include <iostream>
#include <iomanip>
#include <cmath>
#include "TCanvas.h"
#include "TH1D.h"
#include "TFile.h"
#include "TString.h"

int finishDraw(TString location = "toytest1", float signal = 1.)
{
	// Loading files
	TFile * file1 = TFile::Open(TString(location+"/Values/final_histo.root").Data());
	TFile * file2 = TFile::Open(TString(location+"/Means/final_histo.root").Data());

	// Get Histograms
	TH1D * val =	(TH1D*) ((TDirectoryFile*) file1->Get("signal"))->Get("Value_r");
	TH1D * mea =	(TH1D*) ((TDirectoryFile*) file2->Get("signal"))->Get("Value_r");
	TH1D * errval = (TH1D*) ((TDirectoryFile*) file1->Get("signal"))->Get("Error_r");
	TH1D * errmea = (TH1D*) ((TDirectoryFile*) file2->Get("signal"))->Get("Er_Me_r");
	TH1D * valasy = (TH1D*) ((TDirectoryFile*) file1->Get("signal"))->Get("Er_As_r");
	TH1D * meaasy = (TH1D*) ((TDirectoryFile*) file2->Get("signal"))->Get("Er_As_r");

	double merr = errmea->GetMean();
	double verr = errval->GetMean();

	// Calculating p-Values
	double mmea = mea->GetMean();
	double mval = val->GetMean();
	double nmea = mea->GetEntries();
	double nval = val->GetEntries();
	double smea = mea->GetStdDev();
	double sval = val->GetStdDev();
	double zmea = std::abs((mmea-signal)*sqrt(nmea)/(smea*2));
	double zval = std::abs((mval-signal)/sqrt(nval)/(sval*2));
	double pmea = (1-erf(zmea));
	double pval = (1-erf(zval));
	double masy = meaasy->GetMean();
	double vasy = valasy->GetMean();
	std::cout << "Mean of Means: " << mmea << "\nMean of Values: " << mval << std::endl;
	std::cout << "Mean of Meanerrors: " << merr << "\nMean of Errors: " << verr << std::endl;
	std::cout << "Std of Means: " << smea << "\nStd of Values: " << sval << std::endl;
	std::cout << "Number of Means: " << nmea << "\nNumber of Values: " << nval << std::endl;
	std::cout << "z-value: " << zmea << " | " << zval << "\nerf: " << erf(zmea) << " | " << erf(zval) << std::endl;
	std::cout << "p-Value of Means: " << std::setprecision(4) << pmea << "\np-Value of Values: " << std::setprecision(4) << pval << std::endl;
	std::cout << "Mean of Meanasymmetry: " << masy << "\nMean of Valueasymmetry: " << vasy << std::endl;

	val->SetTitle("");
	mea->SetTitle("");
	errval->SetTitle("");
	errmea->SetTitle("");
	valasy->SetTitle("");
	meaasy->SetTitle("");

	val->SetLineColor(3);
	errval->SetLineColor(3);
	valasy->SetLineColor(3);

	val->GetYaxis()->SetTitle("Frequenz");
	mea->GetYaxis()->SetTitle("Frequenz");
	errmea->GetYaxis()->SetTitle("Frequenz");
	errval->GetYaxis()->SetTitle("Frequenz");
	valasy->GetYaxis()->SetTitle("Frequenz");
	val->GetXaxis()->SetTitle("Signalstaerke r");
	mea->GetXaxis()->SetTitle("Signalstaerke r");
	errval->GetXaxis()->SetTitle("Fehler der Signalstaerke r");
	errmea->GetXaxis()->SetTitle("Fehler der Signalstaerke r");
	valasy->GetXaxis()->SetTitle("Asymmetrie der Signalstaerke r");
	meaasy->GetXaxis()->SetTitle("Asymmetrie der Signalstaerke r");

	val->SetStats(0);
	mea->SetStats(0);
	errval->SetStats(0);
	errmea->SetStats(0);
	valasy->SetStats(0);
	meaasy->SetStats(0);

        // Setting both histograms to the same hight
	mea->Scale(val->GetMaximum()/(mea->GetMaximum()));
	errmea->Scale(errval->GetMaximum()/(errmea->GetMaximum()));
	meaasy->Scale(valasy->GetMaximum()/(meaasy->GetMaximum()));

	// Building Canvases
	TCanvas * values = new TCanvas("Comparison_of_values","Comparison_of_values",700,600);
	val->Draw();
	mea->Draw("HISTsame");
        values->SaveAs(TString(location+"/Values.png").Data());	
	TCanvas * errors = new TCanvas("Comparison_of_errors","Comparison_of_errors",700,600);
	errval->Draw();
	errmea->Draw("HISTsame");
        errors->SaveAs(TString(location+"/Errors.png").Data());
	TCanvas * asymet = new TCanvas("Asymetry_of_errors","Asymetry_of_errors",700,600);
	valasy->Draw();
	meaasy->Draw("HISTsame");
        asymet->SaveAs(TString(location+"/Asymet.png").Data());
	TCanvas * all    = new TCanvas("Comparison_of_valerr","Comparison_of_valerr",1200,600);
	all->Divide(2,1);
	all->cd(1);
	val->Draw();
	mea->Draw("HISTsame");
	all->cd(2);
	errval->Draw();
	errmea->Draw("HISTsame");
	//all->cd(3);
	//valasy->Draw();
	//meaasy->Draw("HISTsame");
	all->SaveAs(TString(location+"/ValErr.png").Data());

	return 0;
}
