#include <exception>
#include <iostream>
#include <vector>
#include <math.h>

#include "TCanvas.h"
#include "TColor.h"
#include "TLegend.h"
#include "TH1.h"
#include "TH1D.h"
#include "TPad.h"
#include "TString.h"
#include "TUUID.h"
#include "TMath.h"
#include "TColor.h"

#include "LabelMaker.h"
#include "PseudoExperiments.h"
#include "TheLooks.h"

double findMaxValue(std::vector<TH1*> histos)
{
  double maxVal = -999;
  for(int bin=0; bin<histos[0]->GetNbinsX(); bin++){
    for(int histogram=0; histogram < int(histos.size()); histogram++) if(histos[histogram]->GetBinContent(bin+1) > maxVal) maxVal = histos[histogram]->GetBinContent(bin+1);
  }
  return maxVal;
}

double findMinValue(std::vector<TH1*> histos)
{
  double minVal = 999;
  for(int bin=0; bin<histos[0]->GetNbinsX(); bin++){
    for(int histogram=0; histogram < int(histos.size()); histogram++) if(histos[histogram]->GetBinContent(bin+1) < minVal) minVal = histos[histogram]->GetBinContent(bin+1);
  }
  return minVal;
}

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

    TLegend* leg = LabelMaker::legend("top left",labels.size(),0.6);
    TString legendEntry;

    for(size_t iH = 0; iH < hists.size(); ++iH) {
      legendEntry.Form("#splitline{%s}{#mu_{mean}=%.2f #pm %.2f_{(mean error)} #pm %.2f_{(RMS)}}", labels.at(iH).Data(), hists.at(iH)->GetMean(), hists.at(iH)->GetMeanError(), hists.at(iH)->GetRMS() );
      leg->AddEntry(hists.at(iH),legendEntry,"L");
    }
    leg->Draw("same");

    gPad->RedrawAxis();
    std::cout << "printing distribution as " << outLabel << ".pdf\n";
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

std::vector<TH1*> histoList;
histoList.push_back(hFittedValues);
histoList.push_back(hInitValues);
double windowSize = 1.5;

if(hFittedMedianValues != 0){
hFittedMedianValues->SetMarkerColor(kBlack);
hFittedMedianValues->SetMarkerStyle(25);
hFittedMedianValues->SetLineStyle(2);
histoList.push_back(hFittedMedianValues);
//set y axis range of the plot depending on the highest RMS value
windowSize = hFittedMedianValues->GetBinError(1)+0.5;
for(int bin=2; bin< hFittedMedianValues->GetNbinsX(); bin ++) if(hFittedMedianValues->GetBinError(bin)+0.5 > windowSize) windowSize = hFittedMedianValues->GetBinError(bin)+0.5;
}

hInitValues->SetLineColor(kBlack);
hInitValues->SetLineWidth(2);
hInitValues->SetLineStyle(2);
//std::cout << "# of labels: " << labels.size() << std::endl;
//for(int i=0; i<int(labels.size());i++) std::cout << "\t" << labels[i].Data() << std::endl;
//std::cout << "# of points in hInitValues: " << hInitValues->GetEntries() << "\t# of bins: " << hInitValues->GetNbinsX() << std::endl;
//for(int i=0; i<hInitValues->GetNbinsX(); i++) std::cout << "\t bin: " << i << "\t entry: " << hInitValues->GetBinContent(i) << std::endl;
//std::cout << "# of points in hFittedValues: " << hFittedValues->GetEntries() << "\t# of bins: " << hInitValues->GetNbinsX() << std::endl;
//for(int i=0; i<hFittedValues->GetNbinsX(); i++) std::cout << "\t bin: " << i << "\t entry: " << hFittedValues->GetBinContent(i) << std::endl;

//find minimum and maximum value on y axis to make sure everything will be shown
double maxVal = findMaxValue(histoList);
double minVal = findMinValue(histoList);
hInitValues->GetYaxis()->SetRangeUser(minVal - windowSize, maxVal + windowSize);



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
std::cout << "Printing Mean Value plot as " << outLabel << ".pdf\n";
can->SaveAs(outLabel+".pdf");

delete can;
}


void compareNuisanceParameters(const std::vector<PseudoExperiments>& exps,
const TString& outLabel,
const bool ignoreBinByBinNPs)
{
for( auto& np: exps.begin()->nps() ) {
  if( ignoreBinByBinNPs && np.Contains("BDTbin") ) continue;

  std::vector<TH1*> histsPrefit;
  std::vector<TH1*> histsPostfitB;
  std::vector<TH1*> histsPostfitS;
  std::vector<TString> labels;

  TH1* hNPs = new TH1D("hNPs",TString(";;"+np).Data(),exps.size(),0,exps.size()); //Histogram to collect one np from all pseudo experiments

  TH1D* hCompareNPMeansList[2] = {(TH1D*)hNPs->Clone(TString(np+"_mean_PostfitB").Data()),(TH1D*)hNPs->Clone(TString(np+"_mean_PostfitS").Data())};
  TH1D* hCompareNPMediansList[2] = {(TH1D*)hNPs->Clone(TString(np+"_median_PostfitB").Data()),(TH1D*)hNPs->Clone(TString(np+"_median_PostfitS").Data())};
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
  compareDistributions(histsPrefit,labels,outLabel+np+"_Prefit",true);
  compareDistributions(histsPostfitB,labels,outLabel+np+"_PostfitB",true);
  compareDistributions(histsPostfitS,labels,outLabel+np+"_PostfitS",true);

  // plot np means
  for(int i=0; i<2; i++) compareMeanValues(hCompareNPMeansList[i],hPrefit,labels,outLabel+hCompareNPMeansList[i]->GetName(), hCompareNPMediansList[i]);
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
    std::cout << "filling mu = " << exp.muMean() << " into bin " << bin <<std::endl;
    hPOIs->SetBinError(bin, exp.mu()->GetMeanError());
    hPOImedians->SetBinContent(bin,getMedian(exp.mu()));
    hPOImedians->SetBinError(bin,exp.muRMS());
    hInit->SetBinContent(bin,exp.muInjected());

    hists.push_back( exp.mu() );
    hists.back()->GetXaxis()->SetTitle("#mu");
    hists.back()->SetTitle("");
    hists.back()->SetLineColor( exp.color() );
    norm(hists.back());
    //setXRange(hists.back(),-3,5);

    labels.push_back(exp());
  }
  //hInit->GetYaxis()->SetRangeUser(-1.1,3.1);

  std::cout << "comparing mean values for POI\n";
  compareMeanValues(hPOIs,hInit,labels,outLabel+"POImeans",hPOImedians);
  compareDistributions(hists,labels,outLabel+"POI",false);

  delete hPOIs;
  delete hInit;
  for(auto& h: hists) {
    delete h;
  }
}

double convertTStringToDouble(const TString& tStringToConvert, double stepSize=0.1){
  TString doubleVal;
  TString integerVal;
  for(double value=0.; value<=10.;){
    doubleVal.Form("%0.1f", value);
    integerVal.Form("%i", int(value));
    //std::cout << "current value: " << value << "\tdoubleVal = " << doubleVal.Data() << "\tintegerVal = " << integerVal.Data() << std::endl;
    if(tStringToConvert.EqualTo(doubleVal))
    {
      std::cout << "Found nominal signal strength to be " << value << std::endl;
      return value;
    }
    if(tStringToConvert.EqualTo(integerVal))
    {
      if(value != 0)
      {
        std::cout << "Found nominal signal strength to be " << value - .1 << std::endl;
        return value - .1;
      }
      else
      {
        std::cout << "Found nominal signal strength to be " << value << std::endl;
        return value;
      }
    }
    value += stepSize;
  }
  std::cerr << "was unable to convert nominal mu value! Aborting...\n";
  throw std::exception();
  return -1;
}

void loadPseudoExperiments(TString pathToPseudoExperiments, TString containsSignalStrength, std::vector<PseudoExperiments>& expSet, Color_t color = kBlue){
  double nominalMu=0;
  TString helper;


  helper = containsSignalStrength;
  helper.Remove(0,helper.Index("sig")+3);
  if(helper.Length() > 3) helper.Remove(3, helper.Length());
  nominalMu=convertTStringToDouble(helper);
  helper.Form("nominal S=%.1f",nominalMu);
  expSet.push_back( PseudoExperiments(helper,nominalMu) );
  expSet.back().addExperiments(pathToPseudoExperiments);

  if((expSet.back().mu() == NULL) ){
    std::cout << "Could not load any mus from directory " << pathToPseudoExperiments.Data() << ", deleting current experiment set...\n";
    expSet.pop_back();
  }
  else
  {
    std::cout << "obtained number of mus: " << expSet.back().mu()->GetEntries() << std::endl;
    expSet.back().setColor(color);
  }

}

void plotResults(TString pathname) {
  TheLooks::set();
  std::vector<PseudoExperiments> expSet;
  if(pathname.EndsWith("/")) pathname.Chop();


  TSystemDirectory dir(pathname.Data(), pathname.Data());
  TList *folders = dir.GetListOfFiles();
  //if folders are found, go through each one an look for the mlfitFile
  if (folders) {
    TSystemFile *folder;
    TString folderName;
    TIter next(folders);
    int ncolor=0;
    while ((folder=(TSystemFile*)next())) {
      folderName = folder->GetName();
      Color_t colors[5] = {kBlue, kRed, kBlack, kGreen, kOrange};
      if (folder->IsDirectory() && folderName.Contains("sig")) {
        loadPseudoExperiments(pathname+"/"+folderName, folderName, expSet, colors[ncolor]);
        ncolor++;
      }
      if (folder->IsDirectory() && folderName.Contains("PseudoExperiment")){
        loadPseudoExperiments(pathname, pathname, expSet);
        break;
      }
    }
    delete folder;
    delete folders;

  }
  // set inputs
  std::cout << "loaded Experiments: " << expSet.size() << std::endl;
  if(expSet.size() != 0)
  {
    pathname += "/";
    comparePOIs(expSet,pathname);
    compareNuisanceParameters(expSet,pathname,true);
  }
  else std::cerr << "was unable to load any Pseudo Experiments!\n";
}
