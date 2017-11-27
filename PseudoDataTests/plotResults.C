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

  hists.front()->GetYaxis()->SetRangeUser( 0., 2.*(hists.front()->GetBinContent(hists.front()->GetMaximumBin())) );
  hists.front()->Draw("HIST");
  if( superimposeNorm ) hNorm->Draw("HISTsame");
  for(auto& h: hists) {
    h->Draw("HISTsame");
  }

  TLegend* leg = LabelMaker::legendTL(labels.size(),1.);
  for(size_t iH = 0; iH < hists.size(); ++iH) {
    const double meanval = hists.at(iH)->GetMean();
    const double meanerr = hists.at(iH)->GetMeanError();
    char text[200];
    sprintf(text,"%s: %.2f #pm %.2f",labels.at(iH).Data(),meanval,meanerr);
    leg->AddEntry(hists.at(iH),text,"L");
  }
  leg->Draw("same");

  gPad->RedrawAxis();

  can->SaveAs(outLabel+".pdf");
  
  if( hNorm ) delete hNorm;
  delete leg;
  delete can;
}


void compareMeanValues(TH1* hFittedValuesMeanError,
		       TH1* hFittedValuesRMS,
		       TH1* hInitValues,
		       const std::vector<TString>& labels,
		       const TString& outLabel) {
  TCanvas* can = new TCanvas("can","",500,500);
  can->SetBottomMargin(0.25);
  can->cd();

  hFittedValuesMeanError->SetMarkerColor(kBlack);
  hFittedValuesMeanError->SetMarkerStyle(24);

  hFittedValuesRMS->SetMarkerColor( hFittedValuesMeanError->GetMarkerColor() );
  hFittedValuesRMS->SetMarkerStyle( hFittedValuesMeanError->GetMarkerStyle() );

  hInitValues->SetLineColor(kBlack);
  hInitValues->SetLineWidth(2);
  hInitValues->SetLineStyle(2);
  for(size_t iB = 0; iB < labels.size(); ++iB) {
    hInitValues->GetXaxis()->SetBinLabel(1+iB,labels.at(iB));
  }
  hInitValues->GetXaxis()->LabelsOption("v");
  hInitValues->GetXaxis()->SetLabelSize(0.06);
  

  hInitValues->Draw("HIST");
  hFittedValuesRMS->Draw("PEsame");
  hFittedValuesMeanError->Draw("PE1same");

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
    for(auto& exp: exps) {
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
  TH1* hPOIsMeanError = new TH1D("hPOIsMeanError",";;#mu",exps.size(),0,exps.size());
  TH1* hPOIsRMS = static_cast<TH1*>(hPOIsMeanError->Clone("hPOIsRMS"));
  TH1* hInit = static_cast<TH1*>(hPOIsMeanError->Clone("hInit"));
  std::vector<TH1*> hists;
  std::vector<TString> labels;

  for(size_t iE = 0; iE < exps.size(); ++iE) {
    const int bin = iE+1;
    const PseudoExperiments& exp = exps.at(iE);

    hPOIsMeanError->SetBinContent(bin,exp.muMean());
    hPOIsRMS->SetBinContent(bin,exp.muMean());
    hPOIsMeanError->SetBinError(bin,exp.muMeanError());
    hPOIsRMS->SetBinError(bin,exp.muRMS());
    hInit->SetBinContent(bin,exp.muInjected());

    hists.push_back( exp.mu() );
    hists.back()->Sumw2();
    hists.back()->GetXaxis()->SetTitle("#mu");
    hists.back()->SetTitle("");
    hists.back()->SetLineColor( exp.color() );
    norm(hists.back());
    setXRange(hists.back(),-3,5);
    
    labels.push_back(exp());
  }
  hInit->GetYaxis()->SetRangeUser(-1.1,3.1);

  compareMeanValues(hPOIsMeanError,hPOIsRMS,hInit,labels,outLabel+"_POImeans");
  compareDistributions(hists,labels,outLabel+"_POI",false);
  
  delete hPOIsMeanError;
  delete hPOIsRMS;
  delete hInit;
  for(auto& h: hists) {
    delete h;
  }
}



void plotResults() {
  TheLooks::set();
  
  // set inputs
  std::vector<PseudoExperiments> expSet;

  expSet.push_back( PseudoExperiments("64h_onlyttbar_onlyLumi_onlyHbb",1.) );
  expSet.back().addExperiments("toys_nominal_64h_onlyttbar_onlyLumi_onlyHbb/PseudoExperiment*/fitDiagnostics.root",500);
  expSet.back().setColor(kBlack);

  expSet.push_back( PseudoExperiments("64h_onlyttbar_onlyHbb",1.) );
  expSet.back().addExperiments("toys_nominal_64h_onlyttbar_onlyHbb//PseudoExperiment*/fitDiagnostics.root",500);
  expSet.back().setColor(kRed);

  expSet.push_back( PseudoExperiments("64h_onlyttbar_only50pc_onlyHbb",1.) );
  expSet.back().addExperiments("toys_nominal_64h_onlyttbar_only50pc_onlyHbb/PseudoExperiment*/fitDiagnostics.root",500);
  expSet.back().setColor(kBlue);

  // expSet.push_back( PseudoExperiments("64h_onlyttbar",1.) );
  // expSet.back().addExperiments("toys_nominal_64h_onlyttbar/PseudoExperiment*/fitDiagnostics.root",500);
  // expSet.back().setColor(kYellow);
  
  
  // expSet.push_back( PseudoExperiments("64h",1.) );
  // expSet.back().addExperiments("toys_nominal_64h/PseudoExperiment*/fitDiagnostics.root",500);
  // expSet.back().setColor(kBlack);

  // expSet.push_back( PseudoExperiments("64h_no50pc",1.) );
  // expSet.back().addExperiments("toys_nominal_64h_no50pc/PseudoExperiment*/fitDiagnostics.root",500);
  // expSet.back().setColor(kBlue);
  
  // expSet.push_back( PseudoExperiments("64h_noISR",1.) );
  // expSet.back().addExperiments("toys_nominal_64h_noISR/PseudoExperiment*/fitDiagnostics.root",500);
  // expSet.back().setColor(kRed);
  
  // expSet.push_back( PseudoExperiments("64h_noCSV",1.) );
  // expSet.back().addExperiments("toys_nominal_64h_noCSV/PseudoExperiment*/fitDiagnostics.root",500);
  // expSet.back().setColor(kSpring);
  
  // expSet.push_back( PseudoExperiments("64h_noJEC",1.) );
  // expSet.back().addExperiments("toys_nominal_64h_noJEC/PseudoExperiment*/fitDiagnostics.root",500);
  // expSet.back().setColor(kMagenta);
  
  // expSet.push_back( PseudoExperiments("64h_onlyLumi",1.) );
  // expSet.back().addExperiments("toys_nominal_64h_onlyLumi/PseudoExperiment*/fitDiagnostics.root",500);
  // expSet.back().setColor(kOrange);

  
  comparePOIs(expSet,"test");
  //compareNuisanceParameters(expSet,"test",true);
}
