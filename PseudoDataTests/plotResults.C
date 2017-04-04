#include <exception>
#include <iostream>
#include <vector>

#include "TCanvas.h"
#include "TColor.h"
#include "TLegend.h"
#include "TH1.h"
#include "TH1D.h"
#include "TPad.h"
#include "TString.h"
#include "TUUID.h"
#include "TMath.h"

#include "LabelMaker.h"
#include "PseudoExperiments.h"
#include "TheLooks.h"


void norm(TH1* h) {
  if( h->Integral() > 0 ) {
    h->Scale( 1./h->Integral() );
  }
}

void setXRange(TH1* h, const double min, const double max) {
  h->GetXaxis()->SetRangeUser(min,max);
}

void setLineStyle(TH1* h, const int color, const int style) {
  h->SetLineColor(color);
  h->SetLineStyle(style);
  h->SetLineWidth(2);
}

double getMedian(TH1* histo1) {
  int numBins = histo1->GetXaxis()->GetNbins();
  Double_t *x = new Double_t[numBins];
  Double_t* y = new Double_t[numBins];
  for (int i = 0; i < numBins; i++) {
    x[i] = histo1->GetBinCenter(i);
    y[i] = histo1->GetBinContent(i);
  }
  return TMath::Median(numBins, x, y);
}

void compareDistributions(const std::vector<TH1*>& hists,
			  const std::vector<TString>& labels,
			  const TString& outLabel,
			  const bool superimposeNorm = false) {
  TCanvas* can = new TCanvas("can","",500,500);
  can->cd();

  TH1* hNorm = 0;
  if( superimposeNorm ) {
    hNorm = static_cast<TH1*>(hists.front()->Clone("norm"));
    hNorm->Reset();
    hNorm->FillRandom("gaus",1E5);
    norm(hNorm);
    hNorm->SetLineColor(kGray);
    hNorm->SetFillColor(kGray);
  }

  hists.front()->Draw("HIST");
  if( superimposeNorm ) hNorm->Draw("HISTsame");
  for(auto& h: hists) {
    h->Draw("HISTsame");
  }

  TLegend* leg = LabelMaker::legend(labels.size(),0.4);
  for(size_t iH = 0; iH < hists.size(); ++iH) {
    leg->AddEntry(hists.at(iH),labels.at(iH),"L");
  }
  leg->Draw("same");

  gPad->RedrawAxis();

  can->SaveAs(outLabel+".pdf");

  if( hNorm ) delete hNorm;
  delete leg;
  delete can;
}


void compareMeanValues(TH1* hFittedValues,
		       TH1* hInitValues,
          	       const std::vector<TString>& labels,
		       const TString& outLabel,
		       TH1* hFittedMedianValues=0) {
  TCanvas* can = new TCanvas("can","",500,500);
  can->SetBottomMargin(0.25);
  can->cd();

  TLegend legend(0.2,0.1,0.5,0.3);

  hFittedValues->SetMarkerColor(kBlack);
  hFittedValues->SetMarkerStyle(24);

  if(hFittedMedianValues != 0){
  hFittedMedianValues->SetMarkerColor(kBlack);
  hFittedMedianValues->SetMarkerStyle(25);
  hFittedMedianValues->SetLineStyle(2);
  }

  hInitValues->SetLineColor(kBlack);
  hInitValues->SetLineWidth(2);
  hInitValues->SetLineStyle(2);
  //std::cout << "# of labels: " << labels.size() << std::endl;
  //std::cout << "# of points in hInitValues: " << hInitValues->GetEntries() << "\t# of bins: " << hInitValues->GetNbinsX() << std::endl;
  //std::cout << "# of points in hFittedValues: " << hFittedValues->GetEntries() << "\t# of bins: " << hInitValues->GetNbinsX() << std::endl;
  hInitValues->GetYaxis()->SetRangeUser(hInitValues->GetBinContent(1)-1,hInitValues->GetBinContent(1)+1);



  for(size_t iB = 0; iB < labels.size(); ++iB) {
    hInitValues->GetXaxis()->SetBinLabel(1+iB,labels.at(iB));
  }
  hInitValues->GetXaxis()->LabelsOption("v");
  hInitValues->GetXaxis()->SetLabelSize(0.06);

  legend.AddEntry(hFittedValues, "mean values+mean error", "lp");

  hInitValues->Draw("HIST");
  hFittedValues->Draw("PE1same");
  if(hFittedMedianValues != 0) {
    legend.AddEntry(hFittedMedianValues, "median values+RMS", "lp");
    hFittedMedianValues->Draw("PE1same");
  }
  legend.Draw("Same");

  can->SaveAs(outLabel+".pdf");

  delete can;
}


void compareNuisanceParameters(const std::vector<PseudoExperiments>& exps,
			       const TString& outLabel,
			       const bool ignoreBinByBinNPs) {
  for( auto& np: exps.begin()->nps() ) {
    if( ignoreBinByBinNPs && np.Contains("BDTbin") ) continue;

    std::vector<TH1*> histsPrefit;
    std::vector<TH1*> histsPostfitB;
    std::vector<TH1*> histsPostfitS;
    std::vector<TString> labels;

    TH1* hNPs = new TH1D("hNPs",TString(";;"+np).Data(),exps.size(),0,exps.size()); //Histogram to collect one np from all pseudo experiments

    TH1D* hCompareNPMeansList[2] = {(TH1D*)hNPs->Clone(TString("_mean_"+np+"_PostfitB").Data()),(TH1D*)hNPs->Clone(TString("_mean_"+np+"_PostfitS").Data())};
    TH1D* hCompareNPMediansList[2] = {(TH1D*)hNPs->Clone(TString("_median_"+np+"_PostfitB").Data()),(TH1D*)hNPs->Clone(TString("_median_"+np+"_PostfitS").Data())};
    TH1* hPrefit = static_cast<TH1*>(hNPs->Clone(TString("hPrefit_"+np).Data()));

    int iE = 0;
    for(auto& exp: exps) {
      const int bin = iE+1;

      hCompareNPMeansList[0]->SetBinContent(bin,exp.npPostfitBMean(np) );
      hCompareNPMeansList[0]->SetBinError(bin,exp.npPostfitB(np)->GetMeanError() );
      hCompareNPMediansList[0]->SetBinContent(bin,getMedian(exp.npPostfitB(np) ));
      hCompareNPMediansList[0]->SetBinError(bin,exp.npPostfitBRMS(np));

      hCompareNPMeansList[1]->SetBinContent(bin,exp.npPostfitSMean(np) );
      hCompareNPMeansList[1]->SetBinError(bin,exp.npPostfitS(np)->GetMeanError() );
      hCompareNPMediansList[1]->SetBinContent(bin,getMedian(exp.npPostfitS(np) ));
      hCompareNPMediansList[1]->SetBinError(bin,exp.npPostfitSRMS(np));
      hPrefit->SetBinContent(bin,exp.npPrefitMean(np));
      hPrefit->SetBinError(bin,exp.npPrefitRMS(np));
      setXRange(hPrefit, -1, 1);


      histsPrefit.push_back( exp.npPrefit(np) );
      norm( histsPrefit.back() );
      setXRange( histsPrefit.back(), -4, 4 );
      setLineStyle( histsPrefit.back(), exp.color(), 1 );

      histsPostfitB.push_back( exp.npPostfitB(np) );
      norm( histsPostfitB.back() );
      setXRange( histsPostfitB.back(), -4, 4 );
      setLineStyle( histsPostfitB.back(), exp.color(), 1 );

      histsPostfitS.push_back( exp.npPostfitS(np) );
      norm( histsPostfitS.back() );
      setXRange( histsPostfitS.back(), -4, 4 );
      setLineStyle( histsPostfitS.back(), exp.color(), 1 );

      labels.push_back( exp() );
    }
    // plot np distributions
    compareDistributions(histsPrefit,labels,outLabel+"_"+np+"_Prefit",true);
    compareDistributions(histsPostfitB,labels,outLabel+"_"+np+"_PostfitB",true);
    compareDistributions(histsPostfitS,labels,outLabel+"_"+np+"_PostfitS",true);

    // plot np means
    for(int i=0; i<2; i++) compareMeanValues(hCompareNPMeansList[i],hPrefit,labels,outLabel+"_"+hCompareNPMeansList[i]->GetName(), hCompareNPMediansList[i]);
    for(int i=0; i<2; i++){ 
	delete hCompareNPMeansList[i];
	delete hCompareNPMediansList[i];
    }
    delete hPrefit;
    delete hNPs;

    for(size_t iH = 0; iH < histsPrefit.size(); ++iH) {
      delete histsPrefit.at(iH);
      delete histsPostfitB.at(iH);
      delete histsPostfitS.at(iH);
    }
  }
}


void comparePOIs(const std::vector<PseudoExperiments>& exps,
		 const TString& outLabel) {
  // mean values and distributions of POIs
  TH1* hPOIs = new TH1D("hPOIs",";;#mu",exps.size(),0,exps.size());
  TH1* hInit = static_cast<TH1*>(hPOIs->Clone("hInit"));
  TH1* hPOImedians = static_cast<TH1*>(hPOIs->Clone("hPOImedians"));

  std::vector<TH1*> hists;
  std::vector<TString> labels;
  //std::cout << "size of exps vector: " << exps.size() << std::endl;
  for(size_t iE = 0; iE < exps.size(); ++iE) {
    const int bin = iE+1;
    const PseudoExperiments& exp = exps.at(iE);

    hPOIs->SetBinContent(bin,exp.muMean());
    hPOIs->SetBinError(bin, exp.mu()->GetMeanError());
    hPOImedians->SetBinContent(bin,getMedian(exp.mu()));
    hPOImedians->SetBinError(bin,exp.muRMS());
    hInit->SetBinContent(bin,exp.muInjected());

    hists.push_back( exp.mu() );
    hists.back()->GetXaxis()->SetTitle("#mu");
    hists.back()->SetTitle("");
    hists.back()->SetLineColor( exp.color() );
    norm(hists.back());
    setXRange(hists.back(),-3,5);

    labels.push_back(exp());
  }
  hInit->GetYaxis()->SetRangeUser(-1.1,3.1);

  compareMeanValues(hPOIs,hInit,labels,outLabel+"_POImeans",hPOImedians);
  compareDistributions(hists,labels,outLabel+"_POI",false);

  delete hPOIs;
  delete hInit;
  for(auto& h: hists) {
    delete h;
  }
}



void plotResults(TString pathname, const double nominalMu=1.) {
  TheLooks::set();

  // set inputs
  std::vector<PseudoExperiments> expSet;
  TString helper;
  helper.Form("nominal S=%f",nominalMu);
  expSet.push_back( PseudoExperiments(helper,nominalMu) );
  expSet.back().addExperiments(pathname);
  
  std::cout << "obtained number of mus: " << expSet.back().mu()->GetEntries() << std::endl;
  if((expSet.back().mu()->GetEntries() == 0) ){
    std::cout << "Could not load any mus, deleting current experiment set...\n";
    expSet.pop_back();
  }

  expSet.back().setColor(kBlue);
  TString foldername = pathname;
  foldername.Remove(0, foldername.Last('/')+1);
  comparePOIs(expSet,pathname+"/"+foldername);
  compareNuisanceParameters(expSet,pathname+"/"+foldername,true);
}
