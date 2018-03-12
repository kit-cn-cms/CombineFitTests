#ifndef HELPER_FUNCS_H
#define HELPER_FUNCS_H

#include "TH1.h"
#include "TH1D.h"
#include "TGraph.h"
#include "TGraphErrors.h"
#include "TTree.h"
#include "TString.h"
#include "TColor.h"
#include "TLine.h"
#include "TFile.h"
#include "TObjArray.h"
#include "RooFitResult.h"
#include "RooRealVar.h"
#include <TMath.h>
#include <vector>

namespace helperFuncs{
  
  TH1D* createHistoFromVector(const TString& histName, const std::vector<Double_t>& vec, const TString& title="", const Double_t defaultXmin = -5, const Double_t defaultXmax = 5){
    int position = std::distance(vec.begin(), std::min_element(vec.begin(), vec.end()));
    double minVal = vec[position];
    position = std::distance(vec.begin(), std::max_element(vec.begin(), vec.end()));
    double maxVal = vec[position];
    if(minVal == maxVal){
      minVal = minVal + defaultXmin;
      maxVal = maxVal + defaultXmax;
    }
    int nBins = 2000;
    TH1D* histo = new TH1D(histName, title.Data(), nBins, minVal, maxVal);
    for(int i=0; i<int(vec.size()); i++) histo->Fill(vec[i]);
    return histo;
  }
  
  void saveTreeVals(TTree* tree, const TString& branchName, std::vector<Double_t>& vec){
    double value = 0;
    std::cout << "reading branch " << branchName << std::endl;
    tree->SetBranchAddress(branchName.Data(), &value);
    for(int i = 0; i< tree->GetEntries(); i++){
      tree->GetEntry(i);
      std::cout << "\t" << value << std::endl;
      vec.push_back(value);
    }
  }
  
  void saveTreeVals(TTree* tree, const TString& branchName, std::map<TString,TH1*>& vec, const TString& histName, const int nbins = 400, const double xmin = -1, const double xmax = 1){
    std::map<TString, TH1*>::const_iterator it = vec.find(branchName.Data());
    if(it == vec.end()){
      vec[branchName] = new TH1D(histName.Data(), "", nbins, xmin, xmax);
      vec[branchName]->SetDirectory(0);      
    }
    double value = 0;
    tree->SetBranchAddress(branchName.Data(), &value);
    for(int i = 0; i< tree->GetEntries(); i++){
      std::cout << "\t" << branchName << ":\t" << value << std::endl;
      tree->GetEntry(i);
      vec[branchName]->Fill(value);
    }
  }
  
  
  TH1D* branch2histo(TTree* tree, const TString& branchName, const TString& title = "", const TString& hName = ""){
    std::vector<Double_t> vec;
    saveTreeVals(tree, branchName, vec);
    const TUUID id;
    TString histoName = hName;
    if(histoName.EqualTo(""))
    {
      histoName.Form("%s_%s_%s", tree->GetName(), branchName.Data(), id.AsString());
    }
    return createHistoFromVector(histoName, vec, title);
  }
  
  Double_t checkValues(Double_t x, Double_t cut = 100000000){
      if(std::isnan(x) || std::isinf(x) || x>cut) {
        std::cout << "WARNING:\tchecked value is either nan, inf or > "<< cut;
        std::cout << "! Setting it to 0\n";
        return 0;}
      else return x;
  }

  double findMaxValue(const std::vector<TH1*>& histos, const TString mode = "y")
  {
    std::cout << "looking for maximum value; comparing " << histos.size() << " histos; mode: " << mode << std::endl;
    double maxVal = -999;
    double current = 0;
    if(mode.EqualTo("y"))
    {
      for(int histogram=0; histogram < int(histos.size()); histogram++)
      {
        if(histos[histogram] != NULL){
          current = histos[histogram]->GetBinContent(histos[histogram]->GetMaximumBin());
          if (maxVal < current) maxVal = current;
        }
      }
    }
    if(mode.EqualTo("x"))
    {
      for(int histogram=0; histogram < int(histos.size()); histogram++)
      {
        if(histos[histogram] != NULL){
          current = histos[histogram]->GetXaxis()->GetBinUpEdge(histos[histogram]->GetNbinsX());
          if(current > maxVal) maxVal = current;
        }
      }
    }
    std::cout << "found maximum at " << maxVal << std::endl;
    return maxVal;
  }

  double findMinValue(const std::vector<TH1*>& histos, const TString mode = "y")
  {
    std::cout << "looking for minimum value; comparing " << histos.size() << " histos; mode: " << mode << std::endl;

    double minVal = 999;
    double current = 0;
    if(mode.EqualTo("y"))
    {
      for(int histogram=0; histogram < int(histos.size()); histogram++) {
        if(histos[histogram] != NULL){
          current = histos[histogram]->GetBinContent(histos[histogram]->GetMinimumBin());
          if(current < minVal) minVal = current;
        }
      }
    }
    if(mode.EqualTo("x"))
    {
      for(int histogram=0; histogram < int(histos.size()); histogram++){
        if(histos[histogram] != NULL)
        {
          current = histos[histogram]->GetXaxis()->GetBinLowEdge(1);
          if(current < minVal) minVal = current;
        }
      }
    }
    std::cout << "found minimum at " << minVal << std::endl;
    return minVal;
  }


  void norm(TH1* h) {
    if(h != NULL){
      if( h->Integral() > 0 ) {
        h->Scale( 1./h->Integral() );
      }
    }
  }
  void treat_special_chars(TString& name){
    if(name.Contains(".")) name.ReplaceAll(".", "p");
    if(name.Contains(":")) name.ReplaceAll(":", "_");
    if(name.Contains(" = ")) name.ReplaceAll(" = ", "_");
    if(name.Contains(" ")) name.ReplaceAll(" ", "_");
  }
  void setXRange(TH1* h, const double min, const double max) {
    if(h!= NULL) 
    {
        TString name = h->GetName();
        treat_special_chars(name);
        name.Append(".root");
        // h->SaveAs(name.Data());
        std::cout << "setting histo range to " << min << "\t" << max << std::endl;
        h->GetXaxis()->SetRangeUser(min,max);
    }
  }
  double find_min_x(TH1* h){
      double min = 0;
      int nbins = h->GetNbinsX();
      for(int i=1; i<=nbins; i++){
          if(h->GetBinContent(i) > 0){
              min = h->GetBinLowEdge(i);
              break;
          }
      }
      return min;
  }
  double find_max_x(TH1* h){
      double max = 0;
      int nbins = h->GetNbinsX();
      for(int i=1; i<=nbins; i++){
          if(h->GetBinContent(i) > 0){
              max = h->GetXaxis()->GetBinUpEdge(i);
          }
      }
      return max;
  }

  void setXRange(TH1* h) {
    if(h!= NULL) 
    {
        double additionalRange = 0.2;
        double max = find_max_x(h);
        max += TMath::Abs(max*additionalRange);
        double min = find_min_x(h);
        min -= TMath::Abs(min*additionalRange);
        setXRange(h, min, max);
    }
  }
  

  void setLineStyle(TH1* h, const int color, const int style) {
    if(h != NULL){
      h->SetLineColor(color);
      h->SetLineStyle(style);
      h->SetLineWidth(2);
    }
  }

  void setLineStyle(TH1* h, const Color_t color, const int style, const int markerstyle=1) {
    if(h!= NULL){
      h->SetLineColor(color);
      h->SetLineStyle(style);
      h->SetMarkerStyle(markerstyle);
      h->SetMarkerColor(color);
      h->SetLineWidth(2);
    }
  }
  void setLineStyle(TGraph* h, const int color, const int style) {
    if(h != NULL){
      h->SetLineColor(color);
      h->SetLineStyle(style);
      h->SetLineWidth(2);
    }
  }

  void setLineStyle(TGraph* h, const Color_t color, const int style, const int markerstyle=1) {
    if(h!= NULL){
      h->SetLineColor(color);
      h->SetLineStyle(style);
      h->SetMarkerStyle(markerstyle);
      h->SetMarkerColor(color);
      h->SetLineWidth(2);
    }
  }

  void setupHistogramBin(TH1* histo, const int& bin, const TString binLabel, const Double_t binContent, const Double_t binError = -99999 )
  {
    if(histo != NULL){
      // std::cout << "current histogram: " << histo->GetName() << std::endl;
      // std::cout << "\tchecking label to set for bin " << bin << ": " << binLabel << std::endl;
      TString finalLabel = binLabel;
      if(finalLabel.BeginsWith("CMS_ttH_")) finalLabel.ReplaceAll("CMS_ttH_","");
      histo->GetXaxis()->SetBinLabel(bin, finalLabel);
      // std::cout << "\tsetting label of bin " << bin << " to " << finalLabel << std::endl;
      histo->GetXaxis()->LabelsOption("v");
      histo->GetXaxis()->SetLabelSize(0.04);

      histo->SetBinContent(bin, binContent);
      // std::cout << "\tsetting content of bin " << bin << " to " << binContent << std::endl;

      double finalBinError = binError;
      // std::cout << "\tchecking error of bin " << bin << ": " << finalBinError << std::endl;
      if( finalBinError == -99999) finalBinError = checkValues(histo->GetMeanError());
      // std::cout << "\tsetting error of bin " << bin << " to " << finalBinError << std::endl;
      histo->SetBinError(bin, finalBinError);
      // std::cout << "done setting up histo bin!\n";
    }
  }

  TLine* createLine(const TString sourceFile, const TString whichfit, const TString parameterToDraw){
    TFile* inputRootFile = new TFile(sourceFile, "READ");
    TLine* returnLine = NULL;
    if(inputRootFile->IsOpen() && !inputRootFile->IsZombie() && !inputRootFile->TestBit(TFile::kRecovered)){
      RooFitResult* result = NULL;
      inputRootFile->GetObject(whichfit.Data(), result);
      if(result != NULL){
        const RooRealVar* var = static_cast<RooRealVar*>( result->floatParsFinal().find(parameterToDraw.Data()) );
        if(var != NULL){
          returnLine = new TLine(var->getVal(), 0, var->getVal(), 1);
          delete var;
        }
      }
      inputRootFile->Close();
    }
    if(inputRootFile != NULL) delete inputRootFile;
    return returnLine;
  }

  double getMedian(const TH1* histo1) {
    if(histo1 != NULL){
      int numBins = histo1->GetXaxis()->GetNbins();
      Double_t *x = new Double_t[numBins];
      Double_t* y = new Double_t[numBins];
      for (int i = 0; i < numBins; i++) {
        x[i] = histo1->GetBinCenter(i);
        y[i] = histo1->GetBinContent(i);
      }
      return TMath::Median(numBins, x, y);
    }
    else return -999;
  }

  double convertTStringToDouble(const TString& tStringToConvert){
    //if(tStringToConvert.IsDigit()){
    if(tStringToConvert.IsFloat()){
      std::cout << "found nominal signal strength to be "<< tStringToConvert.Atof() << std::endl;
      return tStringToConvert.Atof();
    }
    //}
    std::cout << "WARNING:\ttrying to convert TString that is not a digit!\n";
    return -1;
  }

  void resizeHisto(TH1* hist){
      if(hist != NULL){
          double xmin = 999;
          double xmax = -999;
          int counter = 0;
          int boundary = int(hist->GetNbinsX()*0.3);
          if(boundary == 0) boundary = 5;
          for(int i=1; i<=hist->GetNbinsX(); i++)
          {
              if(hist->GetBinContent(i) != 0 && xmin == 999) xmin = hist->GetBinLowEdge(i);
              if(hist->GetBinContent(i) == 0) counter++;
              else counter = 0;
          }
          xmax = hist->GetBinLowEdge(hist->GetNbinsX()-counter) + hist->GetBinWidth(hist->GetNbinsX()-counter);
          int nBins = int((xmax - xmin)/2);
          if(nBins <= 0) nBins = 100;
          std::cout << "Resizing histo " << hist->GetName() << "with nBins = " << nBins << "\txmin = " << xmin << "\txmax = " << xmax << std::endl;
          //hist->SetBins(nBins, xmin, xmax);
      }
  }
  TLatex* getLatex(const TString text = "CMS private work", const double x=0.12, const double y=0.91 ){
    std::cout << "creating header\n";
    TLatex* tests = new TLatex(x, y,text);
    tests->SetTextFont(42);
    tests->SetTextSize(0.04);
    tests->SetNDC();
    return tests;
  }


}
#endif
