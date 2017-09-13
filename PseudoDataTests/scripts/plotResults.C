#include <exception>
#include <iostream>
#include <fstream>
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
#include "TLine.h"
<<<<<<< HEAD
=======
#include "TStyle.h"
>>>>>>> 81509c8b530b5f11e714f8f281f434aa1387a365

#include "LabelMaker.h"
#include "PseudoExperiments.h"
#include "TheLooks.h"

<<<<<<< HEAD
double isNaN(double x){
  if(std::isnan(x)) return 0;
  else return x;
}
=======
// double checkValues(double x){
//     if(std::isnan(x)) return 0;
//     else return x;
// }
>>>>>>> 81509c8b530b5f11e714f8f281f434aa1387a365

double findMaxValue(const std::vector<TH1*> histos, const TString mode = "y")
{
  double maxVal = -999;
<<<<<<< HEAD
  if(mode.EqualTo("y"))
  {
    for(int bin=0; bin<histos[0]->GetNbinsX(); bin++){
      for(int histogram=0; histogram < int(histos.size()); histogram++) if(histos[histogram]->GetBinContent(bin+1) > maxVal) maxVal = histos[histogram]->GetBinContent(bin+1);
=======
  double current = 0;
  if(mode.EqualTo("y"))
  {
    for(int histogram=0; histogram < int(histos.size()); histogram++)
    {
      if(histos[histogram] != NULL){
        current = histos[histogram]->GetBinContent(histos[histogram]->GetMaximumBin());
        if (maxVal < current) maxVal = current;
      }
>>>>>>> 81509c8b530b5f11e714f8f281f434aa1387a365
    }
  }
  if(mode.EqualTo("x"))
  {
<<<<<<< HEAD
    int maxBin = 0;
    for(int histogram=0; histogram < int(histos.size()); histogram++)
    {
      maxBin = histos[histogram]->GetNbinsX();
      if(histos[histogram]->GetXaxis()->GetBinUpEdge(maxBin) > maxVal) maxVal = histos[histogram]->GetXaxis()->GetBinUpEdge(maxBin);
=======
    for(int histogram=0; histogram < int(histos.size()); histogram++)
    {
      if(histos[histogram] != NULL){
        current = histos[histogram]->GetXaxis()->GetBinUpEdge(histos[histogram]->GetNbinsX());
        if(current > maxVal) maxVal = current;
      }
>>>>>>> 81509c8b530b5f11e714f8f281f434aa1387a365
    }
  }
  return maxVal;
}

double findMinValue(const std::vector<TH1*> histos, const TString mode = "y")
{
  double minVal = 999;
<<<<<<< HEAD
  if(mode.EqualTo("y"))
  {
    for(int bin=0; bin<histos[0]->GetNbinsX(); bin++){
      for(int histogram=0; histogram < int(histos.size()); histogram++) if(histos[histogram]->GetBinContent(bin+1) < minVal) minVal = histos[histogram]->GetBinContent(bin+1);
=======
  double current = 0;
  if(mode.EqualTo("y"))
  {
    for(int histogram=0; histogram < int(histos.size()); histogram++) {
      if(histos[histogram] != NULL){
        current = histos[histogram]->GetBinContent(histos[histogram]->GetMinimumBin());
        if(current < minVal) minVal = current;
      }
>>>>>>> 81509c8b530b5f11e714f8f281f434aa1387a365
    }
  }
  if(mode.EqualTo("x"))
  {
    for(int histogram=0; histogram < int(histos.size()); histogram++){
<<<<<<< HEAD
      if(histos[histogram]->GetXaxis()->GetBinLowEdge(1) < minVal) minVal = histos[histogram]->GetXaxis()->GetBinLowEdge(1);
=======
      if(histos[histogram] != NULL)
      {
        current = histos[histogram]->GetXaxis()->GetBinLowEdge(1);
        if(current < minVal) minVal = current;
      }
>>>>>>> 81509c8b530b5f11e714f8f281f434aa1387a365
    }
  }
  return minVal;
}

void norm(TH1* h) {
<<<<<<< HEAD
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
void setLineStyle(TH1* h, const Color_t color, const int style, const int markerstyle=1) {
  h->SetLineColor(color);
  h->SetLineStyle(style);
  h->SetMarkerStyle(markerstyle);
  h->SetMarkerColor(color);
  h->SetLineWidth(2);
}

double getMedian(const TH1* histo1) {
  int numBins = histo1->GetXaxis()->GetNbins();
  Double_t *x = new Double_t[numBins];
  Double_t* y = new Double_t[numBins];
  for (int i = 0; i < numBins; i++) {
    x[i] = histo1->GetBinCenter(i);
    y[i] = histo1->GetBinContent(i);
  }
  return TMath::Median(numBins, x, y);
=======
    if(h != NULL){
        if( h->Integral() > 0 ) {
            h->Scale( 1./h->Integral() );
        }
    }
}

void setXRange(TH1* h, const double min, const double max) {
    if(h!= NULL) h->GetXaxis()->SetRangeUser(min,max);
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
>>>>>>> 81509c8b530b5f11e714f8f281f434aa1387a365
}

void setupHistogramBin(TH1* histo, const int& bin, const TString binLabel, const Double_t binContent, const Double_t binError = 0 )
{
<<<<<<< HEAD
  //std::cout << "current histogram: " << histo->GetName() << std::endl;
  //std::cout << "\tsetting label of bin " << bin << " to " << binLabel << std::endl;
  TString finalLabel = binLabel;
  if(finalLabel.BeginsWith("CMS_ttH_")) finalLabel.ReplaceAll("CMS_ttH_","");
  histo->GetXaxis()->SetBinLabel(bin, finalLabel);
  histo->GetXaxis()->LabelsOption("v");
  histo->GetXaxis()->SetLabelSize(0.04);
  //std::cout << "\tsetting content of bin " << bin << " to " << binContent << std::endl;

  histo->SetBinContent(bin, binContent);
  //std::cout << "\tsetting error of bin " << bin << " to " << binError << std::endl;

  histo->SetBinError(bin, binError);
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
        returnLine = new TLine(var->getVal(), 0, var->getVal(), 10);
        delete var;
      }
    }
    inputRootFile->Close();
  }
  if(inputRootFile != NULL) delete inputRootFile;
  return returnLine;
}

void writePOILatexTable(const TH1* hMeans, const TH1* hMedians, const TH1* hMeansWithFittedError, const TString outputPath, const TString tableCaption, const TString& testName){
  std::ofstream output(outputPath.Data(), std::ofstream::out);
  //output.open();
  if(output.is_open() && hMeans != NULL && hMedians != NULL && hMeansWithFittedError != NULL){
    int nSignalStrengths = hMeans->GetNbinsX();
    output << "\\begin{table}\n";
    output << "\\centering\n";
    output << "\\caption{" << tableCaption.Data() << "}\n";
    output << "\\begin{tabular}{c";
    for (int i=0; i<nSignalStrengths; i++) output << "c";
    output << "}\n";
    output << "\\toprule\n";
    output << "Process & \\multicolumn{" << nSignalStrengths << "}{c}{Mean $\\pm$ Mean Error $\\pm$ RMS $\\pm$ Fitted Error}\\\\\n";
    for(int i=1; i<=nSignalStrengths; i++) output << " & " << hMeans->GetXaxis()->GetBinLabel(i);
    output << "\\\\\n";
    output << "\\midrule\n";
    TString processName = testName;
    if(processName.Contains("_")) processName.ReplaceAll("_", "\\_");
    output << processName.Data();
    for(int bin=1; bin <= int(hMeans->GetNbinsX()); bin++)
    {
      output << " & \\num{" << isNaN(hMeans->GetBinContent(bin)) << "} $\\pm$ \\num{"<< isNaN(hMeans->GetBinError(bin)) << "}";
      output << " $\\pm$ \\num{"<< isNaN(hMedians->GetBinError(bin)) << "} $\\pm$ \\num{" << isNaN(hMeansWithFittedError->GetBinError(bin)) << "}";
    }
    output << "\\\\\n";
    output << "\\bottomrule\n";
    output << "\\end{tabular}\n\\end{table}";
    output.close();
  }
  else std::cerr << "ERROR: could not open .tex table!\n";
}

void writeLatexTable(TH1* hMeansB, TH1* hMediansB, TH1* hMeansSB, TH1* hMediansSB, TString outputPath, TString tableCaption, TH1* hExpectation=NULL, TH1* hMeansBwithFittedError= NULL, TH1* hMeansSBwithFittedError = NULL){
  std::ofstream output(outputPath.Data(), std::ofstream::out);
  //output.open();
  if(output.is_open()){
    output << "\\begin{table}\n";
    output << "\\centering\n";
    output << "\\caption{" << tableCaption.Data() << "}\n";
    output << "\\begin{tabular}{ccc";
    if(hExpectation != NULL) output << "c";
    output << "}\n";
    output << "\\toprule\n";
    output << "Parameter & \\multicolumn{2}{c}{Mean $\\pm$ Mean Error $\\pm$ RMS";
    if(hMeansBwithFittedError != NULL && hMeansSBwithFittedError != NULL) output << " $\\pm$ Fitted Error";
    output << "}";
    if(hExpectation != NULL) output << " & Expectation";
    output << "\\\\\n";
    output << " & B-Only Fit & S+B Fit";
    if(hExpectation != NULL) output << " & ";
    output << "\\\\\n";

    output << "\\midrule\n";
    TString processName;
    for(int bin=1; bin <= int(hMeansB->GetNbinsX()); bin++)
    {
      processName = hMeansB->GetXaxis()->GetBinLabel(bin);
      if(processName.Contains("_")) processName.ReplaceAll("_", "\\_");
      output << processName << " & \\num{" << isNaN(hMeansB->GetBinContent(bin)) << "} $\\pm$ \\num{"<< isNaN(hMeansB->GetBinError(bin)) << "} $\\pm$ \\num{" << isNaN(hMediansB->GetBinError(bin)) << "}";
      if(hMeansBwithFittedError != NULL && hMeansSBwithFittedError != NULL) output << " $\\pm$ \\num{"<< isNaN(hMeansBwithFittedError->GetBinError(bin)) << "}";
      output << " & \\num{" << isNaN(hMeansSB->GetBinContent(bin)) << "} $\\pm$ \\num{"<< isNaN(hMeansSB->GetBinError(bin)) << "} $\\pm$ \\num{" << isNaN(hMediansSB->GetBinError(bin)) << "}";
      if(hMeansBwithFittedError != NULL && hMeansSBwithFittedError != NULL)output << " $\\pm$ \\num{"<< isNaN(hMeansSBwithFittedError->GetBinError(bin)) << "}";

      if(hExpectation != NULL) output << " & \\num{" << isNaN(hExpectation->GetBinContent(bin)) << "}";
      output << "\\\\\n";
    }
    output << "\\bottomrule\n";
    output << "\\end{tabular}\n\\end{table}";
    output.close();
  }
  else std::cerr << "ERROR: could not open .tex table!\n";
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

void compareDistributions(const std::vector<TH1*>& hists,
  const std::vector<TString>& labels,
  const TString& outLabel,
  const bool superimposeNorm = false) {
    TCanvas* can = new TCanvas("can","",500,500);
=======
    if(histo != NULL){
        //std::cout << "current histogram: " << histo->GetName() << std::endl;
        //std::cout << "\tsetting label of bin " << bin << " to " << binLabel << std::endl;
        TString finalLabel = binLabel;
        if(finalLabel.BeginsWith("CMS_ttH_")) finalLabel.ReplaceAll("CMS_ttH_","");
        histo->GetXaxis()->SetBinLabel(bin, finalLabel);
        histo->GetXaxis()->LabelsOption("v");
        histo->GetXaxis()->SetLabelSize(0.04);
        //std::cout << "\tsetting content of bin " << bin << " to " << binContent << std::endl;

        histo->SetBinContent(bin, binContent);
        double finalBinError = binError;
        //std::cout << "\tsetting error of bin " << bin << " to " << finalBinError << std::endl;

        if( finalBinError == 0) finalBinError = checkValues(histo->GetMeanError());
        histo->SetBinError(bin, finalBinError);
    }
}

void printCorrelationPlots(TH2D* correlationPlot, const TString& outlabel, const TString label){
    TString outputName = label;
    if(outputName.Contains(" = ")) outputName.ReplaceAll(" = ", "_");
    if(outputName.Contains("=")) outputName.ReplaceAll("=","_");
    if(outputName.Contains(" ")) outputName.ReplaceAll(" ", "_");
    if(outputName.Contains(".")) outputName.ReplaceAll(".","p");
    outputName.Prepend(outlabel);

    TFile* output = TFile::Open(outputName+".root", "RECREATE");
    TCanvas can;

    can.SetMargin(0.25, 0.15, 0.15, 0.08);
    correlationPlot->SetStats(kFALSE);
    correlationPlot->Draw("coltzTEXTE");
    correlationPlot->Write();
    can.Write(outputName);
    can.SaveAs(outputName+".pdf");
    output->Close();
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

void writePOILatexTable(const TH1* hMeans, const TH1* hMedians, const TH1* hMeansWithFittedError, const TString outputPath, const TString tableCaption, const TString& testName){
    if(hMeans != NULL && hMedians != NULL && hMeansWithFittedError != NULL){
        std::ofstream output(outputPath.Data(), std::ofstream::out);
        //output.open();
        if(output.is_open()){
            int nSignalStrengths = hMeans->GetNbinsX();
            output << "\\begin{table}\n";
            output << "\\centering\n";
            TString processName = testName;
            if(processName.Contains("_")) processName.ReplaceAll("_", "\\_");
            processName.Prepend(tableCaption + " for ");
//             output << "\\caption{" << tableCaption.Data() << "}\n";
            output << "\\caption{" << processName.Data() << "}\n";

            output << "\\begin{tabular}{cc";
            //for (int i=0; i<nSignalStrengths; i++) output << "c";
            output << "}\n";
            output << "\\toprule\n";
            //output << "Process & \\multicolumn{" << nSignalStrengths << "}{c}{Mean $\\pm$ Mean Error $\\pm$ RMS $\\pm$ Fitted Error}\\\\\n";
            output << "Pseudo Experiment & Mean $\\pm$ Mean Error $\\pm$ RMS $\\pm$ Fitted Error\\\\\n";
            output << "\\midrule\n";
            /*for(int i=1; i<=nSignalStrengths; i++) output << " & " << hMeans->GetXaxis()->GetBinLabel(i);
             * output << "\\\\\n";
             * output << "\\midrule\n";*/
            //             TString processName = testName;
            //             if(processName.Contains("_")) processName.ReplaceAll("_", "\\_");
            //             output << processName.Data();
            //             for(int bin=1; bin <= int(hMeans->GetNbinsX()); bin++)
            //             {
            //                 output << " & \\num{" << checkValues(hMeans->GetBinContent(bin)) << "} $\\pm$ \\num{"<< checkValues(hMeans->GetBinError(bin)) << "}";
            //                 output << " $\\pm$ \\num{"<< checkValues(hMedians->GetBinError(bin)) << "} $\\pm$ \\num{" << checkValues(hMeansWithFittedError->GetBinError(bin)) << "}";
            //             }
            //             output << "\\\\\n";
            for(int bin=1; bin <= int(hMeans->GetNbinsX()); bin++)
            {
                output <<  hMeans->GetXaxis()->GetBinLabel(bin) << " & \\num{" << checkValues(hMeans->GetBinContent(bin)) << "} $\\pm$ \\num{"<< checkValues(hMeans->GetBinError(bin)) << "}";
                output << " $\\pm$ \\num{"<< checkValues(hMedians->GetBinError(bin)) << "} $\\pm$ \\num{" << checkValues(hMeansWithFittedError->GetBinError(bin)) << "}\\\\\n";
            }
            output << "\\bottomrule\n";
            output << "\\end{tabular}\n\\end{table}";
            output.close();
        }
        else std::cerr << "ERROR: could not open .tex table!\n";

    }
    else std::cerr << "ERROR: Histogram pointer(s) is(are) NULL!\n";
}

void writeLatexTable(TH1* hMeansB, TH1* hMediansB, TH1* hMeansSB, TH1* hMediansSB, TString outputPath, TString tableCaption, TH1* hExpectation=NULL, TH1* hMeansBwithFittedError= NULL, TH1* hMeansSBwithFittedError = NULL){
    std::ofstream output(outputPath.Data(), std::ofstream::out);
    //output.open();
    if(output.is_open() && hMeansB != NULL && hMediansB != NULL && hMeansSB != NULL && hMediansSB != NULL){
        output << "\\begin{table}\n";
        output << "\\centering\n";
        output << "\\caption{" << tableCaption.Data() << "}\n";
        output << "\\begin{tabular}{ccc";
        if(hExpectation != NULL) output << "c";
        output << "}\n";
        output << "\\toprule\n";
        output << "Parameter & \\multicolumn{2}{c}{Mean $\\pm$ Mean Error $\\pm$ RMS";
        if(hMeansBwithFittedError != NULL && hMeansSBwithFittedError != NULL) output << " $\\pm$ Fitted Error";
        output << "}";
        if(hExpectation != NULL) output << " & Expectation";
        output << "\\\\\n";
        output << " & B-Only Fit & S+B Fit";
        if(hExpectation != NULL) output << " & ";
        output << "\\\\\n";

        output << "\\midrule\n";
        TString processName;
        for(int bin=1; bin <= int(hMeansB->GetNbinsX()); bin++)
        {
            processName = hMeansB->GetXaxis()->GetBinLabel(bin);
            if(processName.Contains("_")) processName.ReplaceAll("_", "\\_");
            output << processName << " & \\num{" << checkValues(hMeansB->GetBinContent(bin)) << "} $\\pm$ \\num{"<< checkValues(hMeansB->GetBinError(bin)) << "} $\\pm$ \\num{" << checkValues(hMediansB->GetBinError(bin)) << "}";
            if(hMeansBwithFittedError != NULL && hMeansSBwithFittedError != NULL) output << " $\\pm$ \\num{"<< checkValues(hMeansBwithFittedError->GetBinError(bin)) << "}";
            output << " & \\num{" << checkValues(hMeansSB->GetBinContent(bin)) << "} $\\pm$ \\num{"<< checkValues(hMeansSB->GetBinError(bin)) << "} $\\pm$ \\num{" << checkValues(hMediansSB->GetBinError(bin)) << "}";
            if(hMeansBwithFittedError != NULL && hMeansSBwithFittedError != NULL)output << " $\\pm$ \\num{"<< checkValues(hMeansSBwithFittedError->GetBinError(bin)) << "}";

            if(hExpectation != NULL) output << " & \\num{" << checkValues(hExpectation->GetBinContent(bin)) << "}";
            output << "\\\\\n";
        }
        output << "\\bottomrule\n";
        output << "\\end{tabular}\n\\end{table}";
        output.close();
    }
    else std::cerr << "ERROR: could not open .tex table!\n";
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

void compareDistributions(const std::vector<TH1*>& hists,
                          const std::vector<TString>& labels,
                          const TString& outLabel,
                          const bool superimposeNorm = false) {
    gStyle->SetOptStat(000110000);
    TCanvas* can = new TCanvas("can","",900,500);
>>>>>>> 81509c8b530b5f11e714f8f281f434aa1387a365
    can->cd();
    std::vector<TLine*> lines;
    TH1* hNorm = 0;
    if( superimposeNorm ) {
<<<<<<< HEAD
      hNorm = static_cast<TH1*>(hists.front()->Clone("norm"));
      hNorm->Reset();
      hNorm->FillRandom("gaus",1E5);
      norm(hNorm);
      hNorm->SetLineColor(kGray);
      hNorm->SetFillColor(kGray);
    }
    double xmin = findMinValue(hists,"x")*0.5;
    double xmax = findMaxValue(hists, "x")*1.5;
    int nBins = int((xmax-xmin)/2);
    if(nBins ==0) nBins = 100;
    TH1F dummy("dummy","",nBins, xmin, xmax);
    dummy.GetXaxis()->SetTitle(hists.front()->GetXaxis()->GetTitle());
    dummy.GetYaxis()->SetRangeUser(findMinValue(hists), findMaxValue(hists));
    dummy.Draw();
    hists.front()->Draw("HISTsame");
    if( superimposeNorm ) hNorm->Draw("HISTsame");
    for(int histogram = 1; histogram < int(hists.size()); histogram++) {
      hists[histogram]->Draw("HISTsame");
=======
        if(hists.front() != NULL){
            hNorm = static_cast<TH1*>(hists.front()->Clone("norm"));
            hNorm->Reset();
            hNorm->FillRandom("gaus",1E5);
            norm(hNorm);
            hNorm->SetLineColor(kGray);
            hNorm->SetFillColor(kGray);
        }
    }
    double xmin = findMinValue(hists,"x")*0.5;
    double xmax = findMaxValue(hists, "x")*1.5;
    double ymin = findMinValue(hists);
    double ymax = findMaxValue(hists);
    int nBins = int((xmax-xmin)/2);
    if(nBins ==0) nBins = 100;
    std::cout << "creating dummy histo with nBins = " << nBins << "\txmin = " << xmin << "\txmax = " << xmax << std::endl;
    TH1D dummy("dummy","",nBins, xmin, xmax);
    dummy.GetXaxis()->SetTitle(hists.front()->GetXaxis()->GetTitle());
    dummy.GetYaxis()->SetRangeUser(ymin, ymax);
    dummy.Draw();
    if(hists.front() != NULL) hists.front()->Draw("HIST");
    if( hNorm != NULL ) hNorm->Draw("HISTsame");
    for(int histogram = 1; histogram < int(hists.size()); histogram++) {
        if(hists[histogram]!= NULL) hists[histogram]->Draw("HISTsame");
>>>>>>> 81509c8b530b5f11e714f8f281f434aa1387a365
    }
    TString whichfit = outLabel;
    whichfit.Remove(0, whichfit.Last('_')+1);
    if(whichfit.EqualTo("PostfitB")) whichfit = "fit_b";
    if(whichfit.EqualTo("PostfitS")) whichfit = "fit_s";
    TString helper;
    TString linePath;
    double signalStrength = 0;
<<<<<<< HEAD
    for(int nLabel = 0; nLabel<int(labels.size()); nLabel++){
      helper = labels[nLabel];
      helper.Remove(0, helper.Last('=')+1);
      signalStrength = convertTStringToDouble(helper);
      helper = outLabel;
      helper.Remove(helper.Last('/'), helper.Length());
      linePath.Form("%s/sig%.1f/asimov/mlfit_asimov_sig%.0f.root", helper.Data(), signalStrength, signalStrength);
      if(whichfit.EqualTo("POI")) lines.push_back(createLine(linePath, "fit_s", "r"));
      else lines.push_back(createLine(linePath, whichfit, hists.front()->GetXaxis()->GetTitle()));
      if(lines.back() != NULL){
        lines.back()->SetLineColor(hists[nLabel]->GetLineColor());
        lines.back()->Draw("Same");
      }
    }
    std::cout << "number of saved lines: " << lines.size() << std::endl;


    TLegend* leg = LabelMaker::legend("top left",labels.size(),0.6);
=======
    // for(int nLabel = 0; nLabel<int(labels.size()); nLabel++){
    //   helper = labels[nLabel];
    //   helper.Remove(0, helper.Last('=')+1);
    //   signalStrength = convertTStringToDouble(helper);
    //   helper = outLabel;
    //   helper.Remove(helper.Last('/'), helper.Length());
    //   linePath.Form("%s/sig%.0f/asimov/mlfit.root", helper.Data(), signalStrength);
    //   if(whichfit.EqualTo("POI")) lines.push_back(createLine(linePath, "fit_s", "r"));
    //   else lines.push_back(createLine(linePath, whichfit, hists.front()->GetXaxis()->GetTitle()));
    //   if(lines.back() != NULL){
    //     lines.back()->SetLineColor(hists[nLabel]->GetLineColor());
    //     lines.back()->SetY2(ymax);
    //     lines.back()->Draw("Same");
    //   }
    // }
    std::cout << "number of saved lines: " << lines.size() << std::endl;


    TLegend* leg = LabelMaker::legend("top right",labels.size(),0.6);
>>>>>>> 81509c8b530b5f11e714f8f281f434aa1387a365
    TString legendEntry;

    TString greekLetter;
    if(outLabel.Contains("POI")) greekLetter = "mu";
    else greekLetter = "theta";

    for(size_t iH = 0; iH < hists.size(); ++iH) {
<<<<<<< HEAD
      legendEntry.Form("#splitline{%s}{#%s_{mean}=%.2f #pm %.2f_{(mean error)} #pm %.2f_{(RMS)}}", labels.at(iH).Data(), greekLetter.Data(), hists.at(iH)->GetMean(), hists.at(iH)->GetMeanError(), hists.at(iH)->GetRMS() );
      leg->AddEntry(hists.at(iH),legendEntry,"L");
      if(lines.at(iH)!= NULL){
        legendEntry.Form("Asimov Val for %s", labels.at(iH).Data());
        leg->AddEntry(lines.at(iH), legendEntry, "l");
      }
=======
        legendEntry.Form("#splitline{%s}{#%s_{mean}=%.2f #pm %.2f_{(mean error)} #pm %.2f_{(RMS)}}", labels.at(iH).Data(), greekLetter.Data(), hists.at(iH)->GetMean(), hists.at(iH)->GetMeanError(), hists.at(iH)->GetRMS() );
        if(hists.at(iH) != NULL){
            leg->AddEntry(hists.at(iH),legendEntry,"L");
            // if(lines.at(iH)!= NULL){
            //   legendEntry.Form("Asimov Val for %s", labels.at(iH).Data());
            //   leg->AddEntry(lines.at(iH), legendEntry, "l");
            // }
        }
>>>>>>> 81509c8b530b5f11e714f8f281f434aa1387a365
    }

    leg->Draw("same");

    gPad->RedrawAxis();
    std::cout << "printing distribution as " << outLabel << ".pdf\n";
    can->SaveAs(outLabel+".pdf");
    can->SaveAs(outLabel+".root");

    if( hNorm ) delete hNorm;
    if(leg != NULL) delete leg;
    if(can != NULL) delete can;
    for(int nLine=0; nLine<int(lines.size()); nLine++){
<<<<<<< HEAD
      if(lines[nLine] != NULL) delete lines[nLine];
    }
  }


void compareMeanValues(TH1* hFittedValues,
                      TH1* hInitValues,
                      const std::vector<TString>& labels,
                      const TString& outLabel,
                      TH1* hFittedMedianValues=0) {
  TCanvas* can = new TCanvas("can","",500,500);
  can->SetBottomMargin(0.25);
  can->cd();

  TLegend* legend = LabelMaker::legend("top right",labels.size(),0.3);
  legend->SetTextSize(0.02);

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
    //std::cout << "set bin label " << 1+iB << " to " << labels.at(iB) << std::endl;
    //std::cout << "\tcorresponding entry in " << hFittedValues->GetName() << " = " << hFittedValues->GetBinContent(iB+1) << std::endl;
  }
  hInitValues->GetXaxis()->LabelsOption("v");
  hInitValues->GetXaxis()->SetLabelSize(0.04);

  legend->AddEntry(hFittedValues, "mean values+mean error", "lp");
  hInitValues->Draw("HIST");
  std::cout << "drawing " << hFittedValues->GetName() << std::endl;
  //for (int i=1; i<=hFittedValues->GetEntries(); i++) std::cout << "\tValue in bin " << i << " = " << hFittedValues->GetBinContent(i) << std::endl;
  hFittedValues->Draw("PE1same");
  if(hFittedMedianValues != 0) {
    legend->AddEntry(hFittedMedianValues, "median values+RMS", "lp");
    std::cout << "drawing " << hFittedMedianValues->GetName() << std::endl;
    //for (int i=1; i<=hFittedMedianValues->GetEntries(); i++) std::cout << "\tValue in bin " << i << " = " << hFittedMedianValues->GetBinContent(i) << std::endl;
    hFittedMedianValues->Draw("PE1same");
  }
  legend->Draw("Same");
  std::cout << "Printing Mean Value plot as " << outLabel << ".pdf\n";
  can->SaveAs(outLabel+".pdf");
  can->SaveAs(outLabel+".root");

  if(can != NULL) delete can;
}

void drawPullPlots(const std::vector<TString>& listOfNPs,
   const std::vector<TString>& labels,
   const std::vector<std::vector<Double_t> >* PostfitBvalsAndErrors,
   const std::vector<std::vector<Double_t> >* PostfitSBvalsAndErrors,
   const std::vector<std::vector<Double_t> >* PrefitValsAndErrors,
   const TString outLabel,
   const double lowerBound=-3,
   const double upperBound=3,
   const TString& pathToShapeExpectationRootfile="",
   const TString& categoryName=""){
  if(PostfitBvalsAndErrors != NULL && PostfitSBvalsAndErrors != NULL && PrefitValsAndErrors != NULL)
  {
    TString* canvasName = NULL;
    TCanvas canvas;

    const int nParameters = int(listOfNPs.size());
    //std::cout << "Init hPostFitSBmeans with " << nParameters << "bins\n";
    TH1D* hPostfitSBmeans = new TH1D("hPostfitSBmeans", ";;Value", nParameters, 0, nParameters);
    TH1D* hPostfitSBmeansWithFitErrors= NULL;
    if(PostfitSBvalsAndErrors[0].front().size()>4) hPostfitSBmeansWithFitErrors = new TH1D("hPostfitSBmeansWithFitErrors", ";;Value", nParameters, 0, nParameters);
    //std::cout << hPostfitSBmeans->GetName() << std::endl;
    //std::cout << "Init hPostfitSBmedians with " << nParameters << "bins\n";

    TH1D* hPostfitSBmedians = new TH1D("hPostfitSBmedians", ";;Value", nParameters, 0, nParameters);
    //std::cout << "Init hPostfitBmeans with " << nParameters << "bins\n";

    TH1D* hPostfitBmeans = new TH1D("hPostfitBmeans", ";;Value", nParameters, 0, nParameters);
    TH1D* hPostfitBmeansWithFitErrors= NULL;
    if(PostfitBvalsAndErrors[0].front().size()>4) hPostfitBmeansWithFitErrors = new TH1D("hPostfitBmeansWithFitErrors", ";;Value", nParameters, 0, nParameters);

    //std::cout << hPostfitBmeans->GetName() << std::endl;
    //std::cout << "Init hPostfitBmedians with " << nParameters << "bins\n";

    TH1D* hPostfitBmedians = new TH1D("hPostfitBmedians", ";;Value", nParameters, 0, nParameters);
    //std::cout << "Init hPrefitMeans with " << nParameters << "bins\n";

    TH1D* hPrefitMeans = new TH1D("hPrefitMeans", ";;Value", nParameters, 0, nParameters);
    //std::cout << hPrefitMeans->GetName() <<std::endl;
    TH1D* hPrefitMedians = new TH1D("hPrefitMedians", ";;Value", nParameters, 0, nParameters);
    TH1D* hExpectation = NULL;



    TLegend* legend = LabelMaker::legend("top left",labels.size(),0.4);
    TFile* expectationFile = new TFile(pathToShapeExpectationRootfile, "READ");
    TTree* tree = NULL;
    if(expectationFile->IsOpen())
    {
      std::cout << "Init hExpectation with " << nParameters << "bins\n";
      hExpectation = new TH1D("hExpectation", ";;Value", nParameters, 0, nParameters);
      hExpectation->SetDirectory(0);
      tree = (TTree*)expectationFile->Get(categoryName);
    }
    double prescaleVal = 0;
    double postscaleVal = 0;
    double signal_prescale = 0;
    double signal_postscale = 0;
    double background_prescale = 0;
    double background_postscale = 0;
    double ratio = 0;
    double signalStrength = 0;
    TString* helperString = NULL;

    //std::vector<std::vector<Double_t> >* expectations = new std::vector<std::vector<Double_t>[int(labels.size())];
    //std::vector<Double_t> vectorToSafe;

    for(size_t nExperiment=0; nExperiment<labels.size(); nExperiment++)
    {
      std::cout << "filling np histograms for " << labels[nExperiment] << std::endl;
      helperString = new TString(labels[nExperiment]);
      std::cout << "helperString: " << helperString->Data() << std::endl;

      helperString->Remove(0,helperString->Index("=")+1);
      std::cout << "helperString: " << helperString->Data() << std::endl;
      signalStrength = convertTStringToDouble(*helperString);
      if(helperString != NULL) delete helperString;
      for(int np=0; np<nParameters; np++)
      {
        if(hExpectation != NULL)
        {
          if(tree != NULL)
          {
            //std::cout << "Set Branch address for " << listOfNPs[np] << "\n";

            if(!listOfNPs[np].EqualTo("total"))
            {
              if(tree->SetBranchAddress(TString(listOfNPs[np]+"_prescale").Data(), &prescaleVal) >= 0 && tree->SetBranchAddress(TString(listOfNPs[np]+"_postscale").Data(), &postscaleVal) >= 0) tree->GetEntry(0);
            }
            else{
              if(tree->SetBranchAddress("total_signal_prescale", &signal_prescale) >= 0 &&
              tree->SetBranchAddress("total_signal_postscale", &signal_postscale) >= 0 &&
              tree->SetBranchAddress("total_background_prescale", &background_prescale) >= 0 &&
              tree->SetBranchAddress("total_background_postscale", &background_postscale) >= 0
            )
            {
              tree->GetEntry(0);
            }
            prescaleVal = signalStrength*signal_prescale + background_prescale;
            postscaleVal = signalStrength*signal_postscale + background_postscale;
          }
          if(listOfNPs[np].BeginsWith("ttH") || listOfNPs[np].Contains("signal"))
          {
            if(prescaleVal != 0) ratio = signalStrength*postscaleVal/prescaleVal;
            else ratio = 0;
          }
          else{
            if(prescaleVal != 0) ratio = postscaleVal/prescaleVal;
            else ratio = 0;
          }
          //std::cout << "fill histogram with value " << ratio << std::endl;
          //hExpectation->SetBinContent(np+1, ratio);
          setupHistogramBin(hExpectation, np+1, listOfNPs[np], ratio, 0);
          //vectorToSafe.push_back(ratio);
          //std::cout << "reset branch address\n";
          tree->ResetBranchAddresses();
          prescaleVal = 0;
          postscaleVal = 0;
          ratio = 0;
        }
        else std::cerr << "TTree " << categoryName << " could not be loaded from file " << expectationFile->GetName() << "!\n";
      }

      setupHistogramBin(hPostfitBmeans, np+1, listOfNPs[np], PostfitBvalsAndErrors[nExperiment][np][0], PostfitBvalsAndErrors[nExperiment][np][1]);
      setupHistogramBin(hPostfitBmedians, np+1, listOfNPs[np], PostfitBvalsAndErrors[nExperiment][np][2], PostfitBvalsAndErrors[nExperiment][np][3]);
      if(hPostfitBmeansWithFitErrors != NULL) setupHistogramBin(hPostfitBmeansWithFitErrors, np+1, listOfNPs[np], PostfitBvalsAndErrors[nExperiment][np][0], PostfitBvalsAndErrors[nExperiment][np][4]);

      setupHistogramBin(hPostfitSBmeans, np+1, listOfNPs[np], PostfitSBvalsAndErrors[nExperiment][np][0], PostfitSBvalsAndErrors[nExperiment][np][1]);
      if(hPostfitSBmeansWithFitErrors != NULL) setupHistogramBin(hPostfitSBmeansWithFitErrors, np+1, listOfNPs[np], PostfitSBvalsAndErrors[nExperiment][np][0], PostfitSBvalsAndErrors[nExperiment][np][4]);

      setupHistogramBin(hPostfitSBmedians, np+1, listOfNPs[np], PostfitSBvalsAndErrors[nExperiment][np][2], PostfitSBvalsAndErrors[nExperiment][np][3]);

      setupHistogramBin(hPrefitMeans, np+1, listOfNPs[np], PrefitValsAndErrors[nExperiment][np][0], PrefitValsAndErrors[nExperiment][np][1]);
      setupHistogramBin(hPrefitMedians, np+1, listOfNPs[np], PrefitValsAndErrors[nExperiment][np][2], PrefitValsAndErrors[nExperiment][np][3]);

    }
    std::cout << "done!\n";

    if(hExpectation != NULL)
    {
      setLineStyle(hExpectation, kRed, 2);
      legend->AddEntry(hExpectation, "Expected Norm Ratio", "l");
    }
    setLineStyle(hPrefitMeans, kBlack, 2);
    setLineStyle(hPrefitMedians, kBlack, 2);

    setLineStyle(hPostfitBmeans, kBlue, 1, 20);
    if(hPostfitBmeansWithFitErrors != NULL) setLineStyle(hPostfitBmeansWithFitErrors, kBlue, 4, 1);
    setLineStyle(hPostfitBmedians, kBlue, 2, 21);

    setLineStyle(hPostfitSBmeans, kRed, 1, 20);
    if(hPostfitSBmeansWithFitErrors != NULL) setLineStyle(hPostfitSBmeansWithFitErrors, kRed, 4, 1);
    setLineStyle(hPostfitSBmedians, kRed, 2, 21);

    std::cout << "set line style successfully\n";

    hPrefitMeans->GetYaxis()->SetRangeUser(lowerBound,upperBound);
    hPrefitMeans->GetXaxis()->SetLabelSize(0.02);
    hPrefitMeans->Draw("HIST");
    if(hExpectation != NULL) hExpectation->Draw("HISTsame");
    hPrefitMedians->Draw("PE1same");
    std::cout << "drew prefit histo\n";
    hPostfitBmeans->Draw("PE1same");
    if(hPostfitBmeansWithFitErrors != NULL) hPostfitBmeansWithFitErrors->Draw("PE1same");
    hPostfitBmedians->Draw("PE1same");
    std::cout << "drew postfit b\n";
    hPostfitSBmeans->Draw("PE1same");
    if(hPostfitSBmeansWithFitErrors != NULL) hPostfitSBmeansWithFitErrors->Draw("PE1same");
    hPostfitSBmedians->Draw("PE1same");
    std::cout << "drew postfit s+b\n";

    legend->AddEntry(hPrefitMeans, "Prefit Means", "l");
    legend->AddEntry(hPostfitBmeans, "B-only fit Means + Mean Error", "lp");
    legend->AddEntry(hPostfitBmedians, "B-only fit Medians + RMS", "lp");
    if(hPostfitBmeansWithFitErrors != NULL) legend->AddEntry(hPostfitBmeansWithFitErrors, "B-only fit Means + Fitted Error", "l");
    legend->AddEntry(hPostfitSBmeans, "S+B fit Means + Mean Error", "lp");
    legend->AddEntry(hPostfitSBmedians, "S+B fit Medians + RMS", "lp");
    if(hPostfitSBmeansWithFitErrors != NULL) legend->AddEntry(hPostfitSBmeansWithFitErrors, "S+B fit Means + Fitted Error", "l");


    legend->Draw("Same");
    std::cout << "drew legend\n";

    //std::cout << "setting canvas name to " << labels[nExperiment] << std::endl;
    canvasName = new TString(labels[nExperiment]);
    //std::cout << "\tsuccess!\n";
    //std::cout << "canvasName = " << canvasName->Data() << std::endl;
    if(canvasName->Contains(" ")) canvasName->ReplaceAll(" ", "_");
    if(canvasName->Contains("=")) canvasName->ReplaceAll("=", "_");
    if(canvasName->Contains(".")) canvasName->ReplaceAll(".", "p");
    //std::cout << "prepending path: " << outLabel << "pullplot_" << std::endl;
    writeLatexTable(hPostfitBmeans, hPostfitBmedians, hPostfitSBmeans, hPostfitSBmedians, outLabel+"values_"+*canvasName+".tex", "Values for "+labels[nExperiment], hExpectation, hPostfitBmeansWithFitErrors, hPostfitSBmeansWithFitErrors);

    canvasName->Prepend(outLabel+"pullplot_");
    //canvasName->Append(".pdf");
    //std::cout << "canvas name: " << *canvasName << std::endl;
    canvas.SaveAs(*canvasName+".pdf");
    canvas.SaveAs(*canvasName+".root");
    std::cout << "saved canvas successfully\n";

    //writeLatexTable(hPostfitBmeans, hPostfitBmedians, outLabel+"values_PostfitB_"+labels[nExperiment]+".tex", "Values for "+labels[nExperiment], hExpectation);

    hPostfitBmeans->Reset();
    if(hPostfitBmeansWithFitErrors != NULL) hPostfitBmeansWithFitErrors->Reset();
    hPostfitBmedians->Reset();


    hPostfitSBmeans->Reset();
    if(hPostfitSBmeansWithFitErrors != NULL) hPostfitSBmeansWithFitErrors->Reset();
    hPostfitSBmedians->Reset();

    hPrefitMeans->Reset();
    hPrefitMedians->Reset();

    if(hExpectation != NULL)
    {
      //expectations[nExperiment].push_back(vectorToSafe);
      //vectorToSafe.clear();
      hExpectation->Reset();
    }
    legend->Clear();
    canvas.Clear();
    if(canvasName != NULL) delete canvasName;
    //std::cout << "cleared everything\n";

  }

  if(expectationFile->IsOpen()) expectationFile->Close();
  if(hPostfitSBmeans != NULL) delete hPostfitSBmeans;
  if(hPostfitSBmeansWithFitErrors != NULL) delete hPostfitSBmeansWithFitErrors;
  if(hPostfitSBmedians != NULL) delete hPostfitSBmedians;

  if(hPostfitBmeans != NULL) delete hPostfitBmeans;
  if(hPostfitBmeansWithFitErrors != NULL) delete hPostfitBmeansWithFitErrors;
  if(hPostfitBmedians != NULL)delete hPostfitBmedians;

  if(hPrefitMeans != NULL) delete hPrefitMeans;
  if(hPrefitMedians != NULL) delete hPrefitMedians;

  if(hExpectation != 0) delete hExpectation;
  if(legend != NULL)delete legend;
}
}

void compareNuisanceParameters(const std::vector<PseudoExperiments>& exps,
                               const TString& outLabel,
                               const bool ignoreBinByBinNPs){
  std::vector<TString> listOfNPs;
  std::vector<std::vector<Double_t> >* PostfitBvalsAndErrors = new std::vector<std::vector<Double_t> >[exps.size()];
  std::vector<std::vector<Double_t> >* PostfitSBvalsAndErrors = new std::vector<std::vector<Double_t> >[exps.size()];
  std::vector<std::vector<Double_t> >* PrefitValsAndErrors = new std::vector<std::vector<Double_t> >[exps.size()];
  std::vector<Double_t> vectorToSafe;
  std::vector<TString> labels;


  for( auto& np: exps.begin()->nps() ) {
    if( ignoreBinByBinNPs && np.Contains("BDTbin") ) continue;
    labels.clear();

    std::cout << "adding np " << np << "to the list\n";
    listOfNPs.push_back(np);
    std::vector<TH1*> histsPrefit;
    std::vector<TH1*> histsPostfitB;
    std::vector<TH1*> histsPostfitS;

    TH1* hNPs = new TH1D("hNPs",TString(";;"+np).Data(),exps.size(),0,exps.size()); //Histogram to collect one np from all pseudo experiments

    TH1D* hCompareNPMeansList[2] = {(TH1D*)hNPs->Clone(TString(np+"_mean_PostfitB").Data()),(TH1D*)hNPs->Clone(TString(np+"_mean_PostfitS").Data())};
    TH1D* hCompareNPMediansList[2] = {(TH1D*)hNPs->Clone(TString(np+"_median_PostfitB").Data()),(TH1D*)hNPs->Clone(TString(np+"_median_PostfitS").Data())};
    TH1D* hPrefit = (TH1D*)(hNPs->Clone(TString("hPrefit_"+np).Data()));

    for(size_t iE = 0; iE < exps.size(); ++iE) {
      const int bin = iE+1;
      const PseudoExperiments& exp = exps.at(iE);

      labels.push_back( exp() );
      std::cout << "\tsaving PostfitB\n";
      //std::cout << "\t\tMean value in PseudoExperiment " << labels.back()  << " in bin " << iE << ": " << exp.npPostfitBMean(np) << " +- " << exp.npPostfitB(np)->GetMeanError() << std::endl;
      //hCompareNPMeansList[0]->SetBinContent(bin,exp.npPostfitBMean(np) );
      //hCompareNPMeansList[0]->SetBinError(bin,exp.npPostfitB(np)->GetMeanError() );
      setupHistogramBin(hCompareNPMeansList[0], bin, labels.back(), exp.npPostfitBMean(np), exp.npPostfitB(np)->GetMeanError());
      vectorToSafe.push_back(exp.npPostfitBMean(np));
      vectorToSafe.push_back(exp.npPostfitB(np)->GetMeanError());

      //std::cout << "\t\tMedian value in PseudoExperiment " << labels.back()  << " in bin " << iE << ": " << getMedian(exp.npPostfitB(np) ) << " +- " << exp.npPostfitBRMS(np) << std::endl;
      //hCompareNPMediansList[0]->SetBinContent(bin,getMedian(exp.npPostfitB(np) ));
      //hCompareNPMediansList[0]->SetBinError(bin,exp.npPostfitBRMS(np));
      setupHistogramBin(hCompareNPMediansList[0], bin, labels.back(), getMedian(exp.npPostfitB(np) ), exp.npPostfitBRMS(np));

      vectorToSafe.push_back(getMedian(exp.npPostfitB(np)));
      vectorToSafe.push_back(exp.npPostfitBRMS(np));
      vectorToSafe.push_back(exp.npPostfitBerror(np));

      PostfitBvalsAndErrors[iE].push_back(vectorToSafe);
      vectorToSafe.clear();

      std::cout << "\tsaving PostfitS\n";
      //std::cout << "\t\tMean value in PseudoExperiment " << labels.back()  << " in bin " << iE << ": " << exp.npPostfitSMean(np) << " +- " << exp.npPostfitS(np)->GetMeanError() << std::endl;
      //hCompareNPMeansList[1]->SetBinContent(bin,exp.npPostfitSMean(np) );
      //hCompareNPMeansList[1]->SetBinError(bin,exp.npPostfitS(np)->GetMeanError() );
      setupHistogramBin(hCompareNPMeansList[1], bin, labels.back(), exp.npPostfitSMean(np), exp.npPostfitS(np)->GetMeanError());
      vectorToSafe.push_back(exp.npPostfitSMean(np));
      vectorToSafe.push_back(exp.npPostfitS(np)->GetMeanError());

      //std::cout << "\t\tMedian value in PseudoExperiment " << labels.back()  << " in bin " << iE << ": " << getMedian(exp.npPostfitS(np) ) << " +- " << exp.npPostfitSRMS(np) << std::endl;
      //hCompareNPMediansList[1]->SetBinContent(bin,getMedian(exp.npPostfitS(np) ));
      //hCompareNPMediansList[1]->SetBinError(bin,exp.npPostfitSRMS(np));
      setupHistogramBin(hCompareNPMediansList[1], bin, labels.back(), getMedian(exp.npPostfitS(np) ), exp.npPostfitSRMS(np));

      vectorToSafe.push_back(getMedian(exp.npPostfitS(np)));
      vectorToSafe.push_back(exp.npPostfitSRMS(np));
      vectorToSafe.push_back(exp.npPostfitSerror(np));


      PostfitSBvalsAndErrors[iE].push_back(vectorToSafe);
      vectorToSafe.clear();
      //hPrefit->SetBinContent(bin,exp.npPrefitMean(np));
      //hPrefit->SetBinError(bin,exp.npPrefitRMS(np));
      setupHistogramBin(hPrefit, bin, labels.back(), exp.npPrefitMean(np), exp.npPrefitRMS(np));

      vectorToSafe.push_back(exp.npPrefitMean(np));
      vectorToSafe.push_back(exp.npPrefit(np)->GetMeanError());
      vectorToSafe.push_back(getMedian(exp.npPrefit(np)));
      vectorToSafe.push_back(exp.npPrefitRMS(np));

      PrefitValsAndErrors[iE].push_back(vectorToSafe);
      vectorToSafe.clear();

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

    }
    // plot np distributions
    //std::cout << "\tPostFitB histogram entries:\n";
    //for(int i=1; i<=int(exps.size()); i++) std::cout << "\t\tSaved mean val for " << labels[i-1]  << " in bin " << i << ": " << hCompareNPMeansList[0]->GetBinContent(i) << std::endl;
    //std::cout << "\tPostFitS histogram:\n";
    //for(int i=1; i<=int(exps.size()); i++) std::cout << "\t\tContent for " << labels[i-1]  << " in bin" <<  i << ": " << hCompareNPMeansList[1]->GetBinContent(i) << std::endl;
    compareDistributions(histsPrefit,labels,outLabel+np+"_Prefit",true);
    compareDistributions(histsPostfitB,labels,outLabel+np+"_PostfitB",true);
    compareDistributions(histsPostfitS,labels,outLabel+np+"_PostfitS",true);

    // plot np means
    for(int i=0; i<2; i++) compareMeanValues(hCompareNPMeansList[i],hPrefit,labels,outLabel+hCompareNPMeansList[i]->GetName(), hCompareNPMediansList[i]);

    for(int i=0; i<2; i++){
      if(hCompareNPMeansList[i] != NULL)delete hCompareNPMeansList[i];
      if(hCompareNPMediansList[i] != NULL) delete hCompareNPMediansList[i];
    }
    if(hPrefit != 0) delete hPrefit;
    if(hNPs != 0) delete hNPs;

    for(size_t iH = 0; iH < histsPrefit.size(); ++iH) {
      if(histsPrefit.at(iH) != 0) delete histsPrefit.at(iH);
      if(histsPostfitB.at(iH) != 0) delete histsPostfitB.at(iH);
      if(histsPostfitS.at(iH) != 0) delete histsPostfitS.at(iH);
    }
  }
  std::cout << "collected " << listOfNPs.size() << " names of nps!\n";
  if(listOfNPs.size() != 0) drawPullPlots(listOfNPs, labels, PostfitBvalsAndErrors, PostfitSBvalsAndErrors, PrefitValsAndErrors, outLabel);
  delete[] PostfitBvalsAndErrors;
  delete[] PostfitSBvalsAndErrors;
  delete[] PrefitValsAndErrors;
=======
        if(lines[nLine] != NULL) delete lines[nLine];
    }
    gStyle->SetOptStat(000000000);
}


void compareMeanValues(TH1* hFittedValues,
                       TH1* hInitValues,
                       const std::vector<TString>& labels,
                       const TString& outLabel,
                       TH1* hFittedMedianValues=0)
{
    if(hFittedValues != NULL && hInitValues != NULL){
        TCanvas* can = new TCanvas("can","",500,500);
        can->SetBottomMargin(0.25);
        can->cd();

        TLegend* legend = LabelMaker::legend("top right",labels.size(),0.3);
        legend->SetTextSize(0.02);

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
            //std::cout << "set bin label " << 1+iB << " to " << labels.at(iB) << std::endl;
            //std::cout << "\tcorresponding entry in " << hFittedValues->GetName() << " = " << hFittedValues->GetBinContent(iB+1) << std::endl;
        }
        hInitValues->GetXaxis()->LabelsOption("v");
        hInitValues->GetXaxis()->SetLabelSize(0.04);

        legend->AddEntry(hFittedValues, "mean values+mean error", "lp");
        hInitValues->Draw("HIST");
        std::cout << "drawing " << hFittedValues->GetName() << std::endl;
        //for (int i=1; i<=hFittedValues->GetEntries(); i++) std::cout << "\tValue in bin " << i << " = " << hFittedValues->GetBinContent(i) << std::endl;
        hFittedValues->Draw("PE1same");
        if(hFittedMedianValues != 0) {
            legend->AddEntry(hFittedMedianValues, "median values+RMS", "lp");
            std::cout << "drawing " << hFittedMedianValues->GetName() << std::endl;
            //for (int i=1; i<=hFittedMedianValues->GetEntries(); i++) std::cout << "\tValue in bin " << i << " = " << hFittedMedianValues->GetBinContent(i) << std::endl;
            hFittedMedianValues->Draw("PE1same");
        }
        legend->Draw("Same");
        std::cout << "Printing Mean Value plot as " << outLabel << ".pdf\n";
        can->SaveAs(outLabel+".pdf");
        can->SaveAs(outLabel+".root");

        if(can != NULL) delete can;
    }
}

void drawPullPlots(const std::vector<TString>& listOfNPs,
                   const TString& label,
                   const std::vector<std::vector<Double_t> >& PostfitBvalsAndErrors,
                   const std::vector<std::vector<Double_t> >& PostfitSBvalsAndErrors,
                   const std::vector<std::vector<Double_t> >& PrefitValsAndErrors,
                   const TString outLabel,
                   const double lowerBound=-3,
                   const double upperBound=3,
                   const TString& pathToShapeExpectationRootfile="",
                   const TString& categoryName="")
{
    if(PostfitBvalsAndErrors.size() != 0 && PostfitSBvalsAndErrors.size() != 0 && PrefitValsAndErrors.size() != 0)
    {
        TString* canvasName = NULL;
        TCanvas canvas;


        TH1D* hExpectation = NULL;

        TH1D* hPostfitSBmeans = NULL;
        TH1D* hPostfitSBmeansWithFitErrors= NULL;

        TH1D* hPostfitSBmedians = NULL;
        TH1D* hPostfitBmeans = NULL;
        TH1D* hPostfitBmeansWithFitErrors= NULL;
        TH1D* hPostfitBmedians = NULL;
        TH1D* hPrefitMeans = NULL;
        TH1D* hPrefitMedians = NULL;


        TLegend* legend = LabelMaker::legend("top left",3,0.1);
        TFile* expectationFile = NULL;
        if(!pathToShapeExpectationRootfile.EqualTo("")) expectationFile = new TFile(pathToShapeExpectationRootfile, "READ");
        TTree* tree = NULL;

        double prescaleVal = 0;
        double postscaleVal = 0;
        double signal_prescale = 0;
        double signal_postscale = 0;
        double background_prescale = 0;
        double background_postscale = 0;
        double ratio = 0;
        double signalStrength = 0;
        TString* helperString = NULL;

        std::cout << "filling np histograms for " << label << std::endl;
            helperString = new TString(label);
            std::cout << "helperString: " << helperString->Data() << std::endl;

            helperString->Remove(0,helperString->Index("=")+1);
            std::cout << "helperString: " << helperString->Data() << std::endl;
            signalStrength = convertTStringToDouble(*helperString);
            if(helperString != NULL) delete helperString;

            int nParameters = int(listOfNPs.size());
            //std::cout << "Init hPostFitSBmeans with " << nParameters << "bins\n";
            hPostfitSBmeans = new TH1D("hPostfitSBmeans", ";;Value", nParameters, 0, nParameters);
            if(PostfitSBvalsAndErrors.front().size()>4) hPostfitSBmeansWithFitErrors = new TH1D("hPostfitSBmeansWithFitErrors", ";;Value", nParameters, 0, nParameters);
            //std::cout << hPostfitSBmeans->GetName() << std::endl;
            //std::cout << "Init hPostfitSBmedians with " << nParameters << "bins\n";

            hPostfitSBmedians = new TH1D("hPostfitSBmedians", ";;Value", nParameters, 0, nParameters);
            //std::cout << "Init hPostfitBmeans with " << nParameters << "bins\n";

            hPostfitBmeans = new TH1D("hPostfitBmeans", ";;Value", nParameters, 0, nParameters);
            if(PostfitBvalsAndErrors.front().size()>4) hPostfitBmeansWithFitErrors = new TH1D("hPostfitBmeansWithFitErrors", ";;Value", nParameters, 0, nParameters);

            //std::cout << hPostfitBmeans->GetName() << std::endl;
            //std::cout << "Init hPostfitBmedians with " << nParameters << "bins\n";

            hPostfitBmedians = new TH1D("hPostfitBmedians", ";;Value", nParameters, 0, nParameters);
            //std::cout << "Init hPrefitMeans with " << nParameters << "bins\n";

            hPrefitMeans = new TH1D("hPrefitMeans", ";;Value", nParameters, 0, nParameters);
            //std::cout << hPrefitMeans->GetName() <<std::endl;
            hPrefitMedians = new TH1D("hPrefitMedians", ";;Value", nParameters, 0, nParameters);

            if(expectationFile != NULL && expectationFile->IsOpen())
            {
                std::cout << "Init hExpectation with " << nParameters << "bins\n";
                hExpectation = new TH1D("hExpectation", ";;Value", nParameters, 0, nParameters);
                hExpectation->SetDirectory(0);
                tree = (TTree*)expectationFile->Get(categoryName);
            }

            for(int np=0; np<nParameters; np++)
            {
                if(hExpectation != NULL)
                {
                    if(tree != NULL)
                    {
                        //std::cout << "Set Branch address for " << listOfNPs[np] << "\n";

                        if(!listOfNPs[np].EqualTo("total"))
                        {
                            if(tree->SetBranchAddress(TString(listOfNPs[np]+"_prescale").Data(), &prescaleVal) >= 0 && tree->SetBranchAddress(TString(listOfNPs[np]+"_postscale").Data(), &postscaleVal) >= 0) tree->GetEntry(0);
                        }
                        else{
                            if(tree->SetBranchAddress("total_signal_prescale", &signal_prescale) >= 0 &&
                                tree->SetBranchAddress("total_signal_postscale", &signal_postscale) >= 0 &&
                                tree->SetBranchAddress("total_background_prescale", &background_prescale) >= 0 &&
                                tree->SetBranchAddress("total_background_postscale", &background_postscale) >= 0
                            )
                            {
                                tree->GetEntry(0);
                            }
                            prescaleVal = signal_prescale + background_prescale;
                            postscaleVal = signalStrength*signal_postscale + background_postscale;
                        }
                        if(listOfNPs[np].BeginsWith("ttH") || listOfNPs[np].Contains("signal"))
                        {
                            if(prescaleVal != 0) ratio = signalStrength*postscaleVal/prescaleVal;
                            else ratio = 0;
                        }
                        else{
                            if(prescaleVal != 0) ratio = postscaleVal/prescaleVal;
                            else ratio = 0;
                        }
                        //std::cout << "fill histogram with value " << ratio << std::endl;
                        //hExpectation->SetBinContent(np+1, ratio);
                        setupHistogramBin(hExpectation, np+1, listOfNPs[np], ratio, 0);
                        //vectorToSafe.push_back(ratio);
                        //std::cout << "reset branch address\n";
                        tree->ResetBranchAddresses();
                        prescaleVal = 0;
                        postscaleVal = 0;
                        ratio = 0;
                    }
                    else std::cerr << "TTree " << categoryName << " could not be loaded from file " << expectationFile->GetName() << "!\n";
                }

                setupHistogramBin(hPostfitBmeans, np+1, listOfNPs[np], PostfitBvalsAndErrors[np][0], PostfitBvalsAndErrors[np][1]);
                setupHistogramBin(hPostfitBmedians, np+1, listOfNPs[np], PostfitBvalsAndErrors[np][2], PostfitBvalsAndErrors[np][3]);
                if(hPostfitBmeansWithFitErrors != NULL) setupHistogramBin(hPostfitBmeansWithFitErrors, np+1, listOfNPs[np], PostfitBvalsAndErrors[np][0], PostfitBvalsAndErrors[np][4]);

                setupHistogramBin(hPostfitSBmeans, np+1, listOfNPs[np], PostfitSBvalsAndErrors[np][0], PostfitSBvalsAndErrors[np][1]);
                if(hPostfitSBmeansWithFitErrors != NULL) setupHistogramBin(hPostfitSBmeansWithFitErrors, np+1, listOfNPs[np], PostfitSBvalsAndErrors[np][0], PostfitSBvalsAndErrors[np][4]);

                setupHistogramBin(hPostfitSBmedians, np+1, listOfNPs[np], PostfitSBvalsAndErrors[np][2], PostfitSBvalsAndErrors[np][3]);

                setupHistogramBin(hPrefitMeans, np+1, listOfNPs[np], PrefitValsAndErrors[np][0], PrefitValsAndErrors[np][1]);
                setupHistogramBin(hPrefitMedians, np+1, listOfNPs[np], PrefitValsAndErrors[np][2], PrefitValsAndErrors[np][3]);

            }
            std::cout << "done!\n";

            if(hExpectation != NULL)
            {
                setLineStyle(hExpectation, kRed, 2);
                legend->AddEntry(hExpectation, "Expected Norm Ratio", "l");
            }
            setLineStyle(hPrefitMeans, kBlack, 2);
            setLineStyle(hPrefitMedians, kBlack, 2);

            setLineStyle(hPostfitBmeans, kBlue, 1, 20);
            if(hPostfitBmeansWithFitErrors != NULL) setLineStyle(hPostfitBmeansWithFitErrors, kBlue, 4, 1);
            setLineStyle(hPostfitBmedians, kBlue, 2, 21);

            setLineStyle(hPostfitSBmeans, kRed, 1, 20);
            if(hPostfitSBmeansWithFitErrors != NULL) setLineStyle(hPostfitSBmeansWithFitErrors, kRed, 4, 1);
            setLineStyle(hPostfitSBmedians, kRed, 2, 21);

            std::cout << "set line style successfully\n";

            hPrefitMeans->GetYaxis()->SetRangeUser(lowerBound,upperBound);
            hPrefitMeans->GetXaxis()->SetLabelSize(0.02);
            hPrefitMeans->Draw("HIST");
            if(hExpectation != NULL) hExpectation->Draw("HISTsame");
            //hPrefitMedians->Draw("PE1same");
            std::cout << "drew prefit histo\n";
            hPostfitBmeans->Draw("PE1same");
            if(hPostfitBmeansWithFitErrors != NULL) hPostfitBmeansWithFitErrors->Draw("PE1same");
            //hPostfitBmedians->Draw("PE1same");
            std::cout << "drew postfit b\n";
            hPostfitSBmeans->Draw("PE1same");
            if(hPostfitSBmeansWithFitErrors != NULL) hPostfitSBmeansWithFitErrors->Draw("PE1same");
            //hPostfitSBmedians->Draw("PE1same");
            std::cout << "drew postfit s+b\n";

            legend->AddEntry(hPrefitMeans, "Prefit Means", "l");
            legend->AddEntry(hPostfitBmeans, "B-only fit Means + Mean Error", "lp");
            //legend->AddEntry(hPostfitBmedians, "B-only fit Medians + RMS", "lp");
            if(hPostfitBmeansWithFitErrors != NULL) legend->AddEntry(hPostfitBmeansWithFitErrors, "B-only fit Means + Fitted Error", "l");
            legend->AddEntry(hPostfitSBmeans, "S+B fit Means + Mean Error", "lp");
            //legend->AddEntry(hPostfitSBmedians, "S+B fit Medians + RMS", "lp");
            if(hPostfitSBmeansWithFitErrors != NULL) legend->AddEntry(hPostfitSBmeansWithFitErrors, "S+B fit Means + Fitted Error", "l");


            legend->Draw("Same");
            std::cout << "drew legend\n";

            //std::cout << "setting canvas name to " << label << std::endl;
            canvasName = new TString(label);
            //std::cout << "\tsuccess!\n";
            //std::cout << "canvasName = " << canvasName->Data() << std::endl;
            if(canvasName->Contains(" ")) canvasName->ReplaceAll(" ", "_");
            if(canvasName->Contains("=")) canvasName->ReplaceAll("=", "_");
            if(canvasName->Contains(".")) canvasName->ReplaceAll(".", "p");
            //std::cout << "prepending path: " << outLabel << "pullplot_" << std::endl;
            writeLatexTable(hPostfitBmeans, hPostfitBmedians, hPostfitSBmeans, hPostfitSBmedians, outLabel+"values_"+*canvasName+".tex", "Values for "+label, hExpectation, hPostfitBmeansWithFitErrors, hPostfitSBmeansWithFitErrors);

            canvasName->Prepend(outLabel+"pullplot_");
            //canvasName->Append(".pdf");
            //std::cout << "canvas name: " << *canvasName << std::endl;
            canvas.SaveAs(*canvasName+".pdf");
            canvas.SaveAs(*canvasName+".root");
            std::cout << "saved canvas successfully\n";

            if(hPostfitSBmeans != NULL) delete hPostfitSBmeans;
            if(hPostfitSBmeansWithFitErrors != NULL) delete hPostfitSBmeansWithFitErrors;
            if(hPostfitSBmedians != NULL) delete hPostfitSBmedians;

            if(hPostfitBmeans != NULL) delete hPostfitBmeans;
            if(hPostfitBmeansWithFitErrors != NULL) delete hPostfitBmeansWithFitErrors;
            if(hPostfitBmedians != NULL)delete hPostfitBmedians;

            if(hPrefitMeans != NULL) delete hPrefitMeans;
            if(hPrefitMedians != NULL) delete hPrefitMedians;

            if(hExpectation != NULL)
            {
                hExpectation->Reset();
            }
            legend->Clear();
            canvas.Clear();
            if(canvasName != NULL) delete canvasName;
            std::cout << "cleared everything\n";
        std::cout << "checking expectationFile\n";
        if(expectationFile != NULL && expectationFile->IsOpen()){
            std::cout << "closing expectationFile\n";
            expectationFile->Close();
        }
        std::cout << "checking expectation histo\n";
        if(hExpectation != 0){
            std::cout << "deleting expectation histo\n";
            delete hExpectation;
        }
        std::cout << "checking legend\n";
        if(legend != NULL){
            std::cout << "deleting legend\n";
            delete legend;
        }
    }
    else std::cout << "WARNING! Could not load values to draw pull plot\n";
}

std::vector<TString> getParameterList(const std::vector<PseudoExperiments>& exps, const bool ignoreBinByBinNPs){
    std::vector<TString> list;
    bool push_back = true;
    for(auto& exp : exps)
    {
        for(auto& np : exp.nps()){
            push_back = true;
            if( ignoreBinByBinNPs && np.Contains("BDTbin") ) continue;
            for(auto& entry: list){
                if(np.EqualTo(entry)){
                    push_back = false;
                    break;
                }
            }
            if(push_back) list.push_back(np);
        }
    }
    return list;
}

void analyzeNPDistributions(const std::vector<TString>& listOfParameters, const std::vector<PseudoExperiments>& exps, const TString& outLabel, const bool& ignoreBinByBinNPs = true){
    std::vector<TString> labels;
    std::vector<TH1*> histsPrefit;
    std::vector<TH1*> histsPostfitB;
    std::vector<TH1*> histsPostfitS;

    for( auto& np: listOfParameters ) {

        labels.clear();

        std::cout << "processing np " << np << " to the list\n";


        TH1* hNPs = new TH1D("hNPs",TString(";;"+np).Data(),exps.size(),0,exps.size()); //Histogram to collect one np from all pseudo experiments


        TH1D* hCompareNPMeansList[2] = {(TH1D*)hNPs->Clone(TString(np+"_mean_PostfitB").Data()),(TH1D*)hNPs->Clone(TString(np+"_mean_PostfitS").Data())};
        TH1D* hCompareNPMediansList[2] = {(TH1D*)hNPs->Clone(TString(np+"_median_PostfitB").Data()),(TH1D*)hNPs->Clone(TString(np+"_median_PostfitS").Data())};
        TH1D* hPrefit = (TH1D*)(hNPs->Clone(TString("hPrefit_"+np).Data()));

        for(size_t iE = 0; iE < exps.size(); ++iE) {
            const int bin = iE+1;
            const PseudoExperiments& exp = exps.at(iE);

            if(exp.npPostfitB(np) != NULL){
                std::cout << "\tsaving PostfitB\n";

                labels.push_back( exp() );

                setupHistogramBin(hCompareNPMeansList[0], bin, labels.back(), exp.npPostfitBMean(np));

                setupHistogramBin(hCompareNPMediansList[0], bin, labels.back(), getMedian(exp.npPostfitB(np) ), exp.npPostfitBRMS(np));

                histsPostfitB.push_back( exp.npPostfitB(np) );
                norm( histsPostfitB.back() );
                setXRange( histsPostfitB.back(), -4, 4 );
                setLineStyle( histsPostfitB.back(), exp.color(), 1 );

            }

            if(exp.npPostfitS(np) != NULL){
                std::cout << "\tsaving PostfitS\n";

                setupHistogramBin(hCompareNPMeansList[1], bin, labels.back(), exp.npPostfitSMean(np), exp.npPostfitS(np)->GetMeanError());

                setupHistogramBin(hCompareNPMediansList[1], bin, labels.back(), getMedian(exp.npPostfitS(np) ), exp.npPostfitSRMS(np));

                histsPostfitS.push_back( exp.npPostfitS(np) );
                norm( histsPostfitS.back() );
                setXRange( histsPostfitS.back(), -4, 4 );
                setLineStyle( histsPostfitS.back(), exp.color(), 1 );

            }
            //hPrefit->SetBinContent(bin,exp.npPrefitMean(np));
            //hPrefit->SetBinError(bin,exp.npPrefitRMS(np));
            if(exp.npPrefit(np) != NULL){
                std::cout << "\tsaving Prefit vals\n";
                setupHistogramBin(hPrefit, bin, labels.back(), exp.npPrefitMean(np), exp.npPrefitRMS(np));

                histsPrefit.push_back( exp.npPrefit(np) );
                norm( histsPrefit.back() );
                setXRange( histsPrefit.back(), -4, 4 );
                setLineStyle( histsPrefit.back(), exp.color(), 1 );
            }

        }
        compareDistributions(histsPrefit,labels,outLabel+np+"_Prefit",true);
        compareDistributions(histsPostfitB,labels,outLabel+np+"_PostfitB",true);
        compareDistributions(histsPostfitS,labels,outLabel+np+"_PostfitS",true);

        // plot np means
        for(int i=0; i<2; i++) if(hCompareNPMeansList[i] != NULL) compareMeanValues(hCompareNPMeansList[i],hPrefit,labels,outLabel+hCompareNPMeansList[i]->GetName(), hCompareNPMediansList[i]);

        for(int i=0; i<2; i++){
            if(hCompareNPMeansList[i] != NULL)delete hCompareNPMeansList[i];
            if(hCompareNPMediansList[i] != NULL) delete hCompareNPMediansList[i];
        }
        if(hPrefit != 0) delete hPrefit;
        if(hNPs != 0) delete hNPs;

        for(auto& h : histsPrefit) if(h != 0) delete h;
        histsPrefit.clear();
        for(auto& h : histsPostfitB) if(h != 0) delete h;
        histsPostfitB.clear();
        for(auto& h : histsPostfitS) if(h != 0) delete h;
        histsPostfitS.clear();
    }
}

void analyzeNPPulls(const PseudoExperiments& exp, const std::vector<TString>& listOfParameters, const TString& outLabel, const bool& ignoreBinByBinNPs,
                    const double lowerBound=-3,
                    const double upperBound=3,
                    const TString& pathToShapeExpectationRootfile="",
                    const TString& categoryName="")
{

    std::vector<std::vector<Double_t> > PostfitBvalsAndErrors;
    std::vector<std::vector<Double_t> > PostfitSBvalsAndErrors;
    std::vector<std::vector<Double_t> > PrefitValsAndErrors;
    std::vector<Double_t> vectorToSafe;

    for(auto& np : listOfParameters){
        if( ignoreBinByBinNPs && np.Contains("BDTbin") ) continue;

        std::cout << "\tsaving PostfitB\n";

        vectorToSafe.push_back(exp.npPostfitBMean(np));
        vectorToSafe.push_back(exp.npPostfitB(np)->GetMeanError());

        vectorToSafe.push_back(getMedian(exp.npPostfitB(np)));
        vectorToSafe.push_back(exp.npPostfitBRMS(np));
        vectorToSafe.push_back(exp.npPostfitBerror(np));

        PostfitBvalsAndErrors.push_back(vectorToSafe);
        vectorToSafe.clear();

        std::cout << "\tsaving PostfitS\n";

        vectorToSafe.push_back(exp.npPostfitSMean(np));
        vectorToSafe.push_back(exp.npPostfitS(np)->GetMeanError());
        vectorToSafe.push_back(getMedian(exp.npPostfitS(np)));
        vectorToSafe.push_back(exp.npPostfitSRMS(np));
        vectorToSafe.push_back(exp.npPostfitSerror(np));


        PostfitSBvalsAndErrors.push_back(vectorToSafe);
        vectorToSafe.clear();

        std::cout << "\tsaving Prefit vals\n";

        vectorToSafe.push_back(exp.npPrefitMean(np));
        vectorToSafe.push_back(exp.npPrefit(np)->GetMeanError());
        vectorToSafe.push_back(getMedian(exp.npPrefit(np)));
        vectorToSafe.push_back(exp.npPrefitRMS(np));

        PrefitValsAndErrors.push_back(vectorToSafe);
        vectorToSafe.clear();

    }
    drawPullPlots(listOfParameters, exp(), PostfitBvalsAndErrors, PostfitSBvalsAndErrors, PrefitValsAndErrors, outLabel, lowerBound, upperBound, pathToShapeExpectationRootfile, categoryName);
    std::cout << "clearing value vectors\n";
    PostfitBvalsAndErrors.clear();
    PostfitSBvalsAndErrors.clear();
    PrefitValsAndErrors.clear();

}

void compareNuisanceParameters(const std::vector<PseudoExperiments>& exps,
                               const TString& outLabel,
                               const bool& ignoreBinByBinNPs)
{
    analyzeNPDistributions(getParameterList(exps, ignoreBinByBinNPs), exps, outLabel,ignoreBinByBinNPs);
    for(auto& exp : exps){
        analyzeNPPulls(exp, exp.nps(), outLabel, ignoreBinByBinNPs);
    }
>>>>>>> 81509c8b530b5f11e714f8f281f434aa1387a365
}


void comparePOIs(const std::vector<PseudoExperiments>& exps,
<<<<<<< HEAD
const TString& outLabel, const TString& testName) {
  // mean values and distributions of POIs
  TH1* hPOIs = new TH1D("hPOIs",";;#mu",exps.size(),0,exps.size());
  TH1* hInit = static_cast<TH1*>(hPOIs->Clone("hInit"));
  TH1* hPOImedians = static_cast<TH1*>(hPOIs->Clone("hPOImedians"));
  TH1* hPOIwithFittedError = static_cast<TH1*>(hPOIs->Clone("hPOIwithFittedError"));

  std::vector<TH1*> hists;
  std::vector<TString> labels;
  //std::cout << "size of exps vector: " << exps.size() << std::endl;
  for(size_t iE = 0; iE < exps.size(); ++iE) {
    const int bin = iE+1;
    const PseudoExperiments& exp = exps.at(iE);
    labels.push_back(exp());

    //hPOIs->SetBinContent(bin,exp.muMean());
    //std::cout << "filling mu = " << exp.muMean() << " into bin " << bin <<std::endl;
    //hPOIs->SetBinError(bin, exp.mu()->GetMeanError());
    setupHistogramBin(hPOIs, bin, labels.back(), exp.muMean(), exp.mu()->GetMeanError());
    //hPOImedians->SetBinContent(bin,getMedian(exp.mu()));
    //hPOImedians->SetBinError(bin,exp.muRMS());
    setupHistogramBin(hPOImedians, bin, labels.back(), getMedian(exp.mu()), exp.muRMS());
    setupHistogramBin(hPOIwithFittedError, bin, labels.back(), exp.muMean(), exp.muError());
    //hInit->SetBinContent(bin,exp.muInjected());
    setupHistogramBin(hInit, bin, labels.back(), exp.muInjected());

    hists.push_back( exp.mu() );
    hists.back()->GetXaxis()->SetTitle("#mu");
    hists.back()->SetTitle("");
    hists.back()->SetLineColor( exp.color() );
    norm(hists.back());
    //setXRange(hists.back(),-3,5);

  }
  //hInit->GetYaxis()->SetRangeUser(-1.1,3.1);

  std::cout << "comparing mean values for POI\n";
  compareMeanValues(hPOIs,hInit,labels,outLabel+"POImeans",hPOImedians);
  writePOILatexTable(hPOIs, hPOImedians, hPOIwithFittedError, outLabel+"POI.tex", "Signal Strength Parameters", testName);
  compareDistributions(hists,labels,outLabel+"POI",false);

  if(hPOIs != NULL) delete hPOIs;
  if(hInit != NULL) delete hInit;
  for(auto& h: hists) {
    if(h != NULL) delete h;
  }
}

void saveFitValues(const int bin, TH1D* hMeans, double mean, double meanError, std::vector<std::vector<Double_t> >& fitResultsContainer, TH1D* hMedians=NULL, double median=0., double rms=0.){
  //std::cout << "\tFilling histogram " << hMeans->GetName() << std::endl;
  std::vector<Double_t> vectorToSafe;
  //std::cout << "\t\tMean value in bin " << bin << ": " << mean << " +- " << meanError << std::endl;

  hMeans->SetBinContent(bin, mean );
  hMeans->SetBinError(bin,meanError );

  vectorToSafe.push_back(mean);
  vectorToSafe.push_back(meanError);
  if(hMedians != NULL)
  {
    //std::cout << "\t\tMedian value in bin " << bin << ": " << median << " +- " << rms << std::endl;
    hMedians->SetBinContent(bin,median);
    hMedians->SetBinError(bin,rms);

    vectorToSafe.push_back(median);
    vectorToSafe.push_back(rms);
  }
  fitResultsContainer.push_back(vectorToSafe);
}

void normVals(std::vector<std::vector<Double_t> >* toNorm, std::vector<std::vector<Double_t> >* norm, int nAllExps){
  for(int nExp=0; nExp<nAllExps; nExp++){
    //std::cout << "\tnorming experiment #" << nExp << std::endl;
    for(int i=0; i<int(norm[nExp].size()); i++)
    {
      //std::cout << "\t\tnorming process #" << i << std::endl;
      double postFitMeanVal = toNorm[nExp][i][0];
      double postFitMeanErr = toNorm[nExp][i][1];
      double postFitMedian = toNorm[nExp][i][2];
      double postFitRMS = toNorm[nExp][i][3];
      double preFitMeanVal = norm[nExp][i][0];
      double preFitMeanErr = norm[nExp][i][1];
      double preFitMedian = norm[nExp][i][2];
      double preFitRMS = norm[nExp][i][3];
      if(norm[nExp][i][0] != 0 && norm[nExp][i][2] != 0)
      {
        toNorm[nExp][i][0] = postFitMeanVal/preFitMeanVal;
        //std::cout << "\t\t\tnew mean val = " << toNorm[nExp][i][0] << std::endl;
        toNorm[nExp][i][1] = toNorm[nExp][i][0]*TMath::Sqrt(TMath::Power(postFitMeanErr/postFitMeanVal,2) + TMath::Power(preFitMeanErr/preFitMeanVal,2));
        //std::cout << "\t\t\tnew mean err = " << toNorm[nExp][i][1] << std::endl;
        toNorm[nExp][i][2] = postFitMedian/preFitMedian;
        //std::cout << "\t\t\tnew median val = " << toNorm[nExp][i][2] << std::endl;

        toNorm[nExp][i][3] = toNorm[nExp][i][2]*TMath::Sqrt(TMath::Power(postFitRMS/postFitMedian,2) + TMath::Power(preFitRMS/preFitMedian,2));
        //std::cout << "\t\t\tnew RMS = " << toNorm[nExp][i][3] << std::endl;
      }
      else
      {
        //std::cout << "\tmean value to norm to is 0! Setting all values to 0\n";
        for(int i=0; i<int(norm[nExp][i].size()); i++) toNorm[nExp][i][i] = 0;
      }
    }
  }
}

void resizeHisto(TH1* hist)
{
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
    hist->SetBins(nBins, xmin, xmax);
  }
}

void compareShapes(const std::vector<PseudoExperiments>& exps, const TString& outLabel, const TString& pathToShapeExpectationRootfile=""){
  std::vector<TString> listOfProcesses;
  std::vector<std::vector<Double_t> >* PostfitBvalsAndErrors =NULL;
  std::vector<std::vector<Double_t> >* PostfitSBvalsAndErrors = NULL;
  std::vector<std::vector<Double_t> >* PrefitValsAndErrors = NULL;
  std::vector<TString> labels;
  TString np;

  std::vector<TH1*> histsPrefit;
  std::vector<TH1*> histsPostfitB;
  std::vector<TH1*> histsPostfitS;

  TH1* hNPs = NULL; //Histogram to collect one np from all pseudo experiments

  TH1D* hCompareNPMeansList[2] = {};
  TH1D* hCompareNPMediansList[2] = {};
  TH1D* hPrefit = NULL;

  ShapeContainer* prefitShapes = NULL;
  ShapeContainer* postfitSshapes = NULL;
  ShapeContainer* postfitBshapes = NULL;

  TString categoryName;
  for(int nCategory=0; nCategory< int(exps.begin()->getPrefitShapes().size()); nCategory++)
  {
    listOfProcesses = exps.begin()->getPrefitShapes()[nCategory]->getListOfProcesses();
    PostfitBvalsAndErrors = new std::vector<std::vector<Double_t> >[exps.size()];
    PostfitSBvalsAndErrors = new std::vector<std::vector<Double_t> >[exps.size()];
    PrefitValsAndErrors = new std::vector<std::vector<Double_t> >[exps.size()];

    categoryName = exps.begin()->getPrefitShapes()[nCategory]->getName();

    for(int nProcess=0; nProcess<int(listOfProcesses.size()); nProcess++ ) {
      labels.clear();
      histsPrefit.clear();
      histsPostfitB.clear();
      histsPostfitS.clear();


      np = listOfProcesses[nProcess];

      hNPs = new TH1D("hNPs",TString(";;"+np).Data(),exps.size(),0,exps.size()); //Histogram to collect one np from all pseudo experiments

      hCompareNPMeansList[0] = (TH1D*)hNPs->Clone(TString(categoryName + "_normalisation_"+np+"_mean_PostfitB").Data());
      hCompareNPMeansList[1] = (TH1D*)hNPs->Clone(TString(categoryName + "_normalisation_"+np+"_mean_PostfitS").Data());
      hCompareNPMediansList[0] = (TH1D*)hNPs->Clone(TString(categoryName + "_normalisation_"+np+"_median_PostfitB").Data());
      hCompareNPMediansList[1] = (TH1D*)hNPs->Clone(TString(categoryName + "_normalisation_"+np+"_median_PostfitS").Data());
      hPrefit = (TH1D*)(hNPs->Clone(TString("hPrefit_"+categoryName + "_normalisation_"+np).Data()));


      for(size_t iE = 0; iE < exps.size(); ++iE) {
        const int bin = iE+1;
        const PseudoExperiments& exp = exps.at(iE);

        prefitShapes = exp.getPrefitShapes()[nCategory];
        resizeHisto(prefitShapes->getDist(np));
        postfitSshapes = exp.getPostfitSshapes()[nCategory];
        resizeHisto(postfitSshapes->getDist(np));
        postfitBshapes = exp.getPostfitBshapes()[nCategory];
        resizeHisto(postfitBshapes->getDist(np));
        //std::cout << "DEBUG\t current process: " << np.Data() << std::endl;
        //std::cout << "DEBUG\t prefitShapes:\n";
        //std::cout << "\t number of saved processes: " << prefitShapes->getNumberOfProcesses() << std::endl;
        //std::cout << "\t number of saved normalisations: " << prefitShapes->getDist(np)->GetEntries() << std::endl;

        //std::cout << "DEBUG\t postfitSshapes:\n";
        //std::cout << "\t number of saved processes: " << postfitSshapes->getNumberOfProcesses() << std::endl;
        //std::cout << "\t number of saved normalisations: " << postfitSshapes->getDist(np)->GetEntries() << std::endl;

        //std::cout << "DEBUG\t postfitBshapes:\n";
        //std::cout << "\t number of saved processes: " << postfitBshapes->getNumberOfProcesses() << std::endl;
        //std::cout << "\t number of saved normalisations: " << postfitBshapes->getDist(np)->GetEntries() << std::endl;

        //std::cout << "DEBUG\t current category: " << categoryName.Data() << std::endl;
        labels.push_back( exp() );

        //std::cout << "\tsaving PostfitB of PseudoExperiment with " << labels.back()  << " \n";

        saveFitValues(bin, hCompareNPMeansList[0], postfitBshapes->getMean(np), postfitBshapes->getMeanError(np), PostfitBvalsAndErrors[iE], hCompareNPMediansList[0], getMedian(postfitBshapes->getDist(np) ), postfitBshapes->getRMS(np));
        //std::cout << "\tsaving PostfitS of PseudoExperiment with " << labels.back()  << " \n";

        saveFitValues(bin, hCompareNPMeansList[1], postfitSshapes->getMean(np), postfitSshapes->getMeanError(np), PostfitSBvalsAndErrors[iE], hCompareNPMediansList[1], getMedian(postfitSshapes->getDist(np) ), postfitSshapes->getRMS(np));

        //std::cout << "\tsaving Prefit of PseudoExperiment with " << labels.back()  << " \n";

        saveFitValues(bin, hPrefit, prefitShapes->getMean(np), prefitShapes->getMeanError(np), PrefitValsAndErrors[iE]);
        PrefitValsAndErrors[iE].back().push_back(getMedian(prefitShapes->getDist(np) ));
        PrefitValsAndErrors[iE].back().push_back(prefitShapes->getRMS(np));

        std::cout << "saving norm distributions...\n";
        histsPrefit.push_back( prefitShapes->getDist(np) );
        norm( histsPrefit.back() );
        setLineStyle( histsPrefit.back(), exp.color(), 1 );

        histsPostfitB.push_back( postfitBshapes->getDist(np) );
        norm( histsPostfitB.back() );
        setLineStyle( histsPostfitB.back(), exp.color(), 1 );

        histsPostfitS.push_back( postfitSshapes->getDist(np) );
        norm( histsPostfitS.back() );
        setLineStyle( histsPostfitS.back(), exp.color(), 1 );
      }
      // plot np distributions
      // std::cout << "\tPostFitB histogram entries:\n";
      // for(int i=1; i<=int(exps.size()); i++) std::cout << "\t\tSaved mean val for " << labels[i-1]  << " in bin " << i << ": " << hCompareNPMeansList[0]->GetBinContent(i) << std::endl;
      // std::cout << "\tPostFitS histogram:\n";
      // for(int i=1; i<=int(exps.size()); i++) std::cout << "\t\tContent for " << labels[i-1]  << " in bin" <<  i << ": " << hCompareNPMeansList[1]->GetBinContent(i) << std::endl;
      // std::cout << "\tPrefit histograms: " << histsPrefit.size() << "\n";
      // for(int i=0; i<int(histsPrefit.size()); i++) std::cout << "\t\tEntries for " << labels[i]  << " in histo" <<  histsPrefit[i]->GetName() << ": " << histsPrefit[i]->GetEntries() << std::endl;
      compareDistributions(histsPrefit,labels,outLabel+categoryName + "_normalisation_"+np+"_Prefit");
      //std::cout << "\tPostfitB histograms: " << histsPostfitB.size() << "\n";
      //for(int i=0; i<int(histsPostfitB.size()); i++) std::cout << "\t\tEntries for " << labels[i]  << " in histo" <<  histsPostfitB[i]->GetName() << ": " << histsPostfitB[i]->GetEntries() << std::endl;
      compareDistributions(histsPostfitB,labels,outLabel+categoryName + "_normalisation_"+np+"_PostfitB");
      //std::cout << "\t PostfitS histograms: " << histsPostfitS.size() << "\n";
      //for(int i=0; i<int(histsPostfitS.size()); i++) std::cout << "\t\tEntries for " << labels[i]  << " in histo" <<  histsPostfitS[i]->GetName() << ": " << histsPostfitS[i]->GetEntries() << std::endl;
      compareDistributions(histsPostfitS,labels,outLabel+categoryName + "_normalisation_"+np+"_PostfitS");

      // plot np means
      for(int i=0; i<2; i++) compareMeanValues(hCompareNPMeansList[i],hPrefit,labels,outLabel+hCompareNPMeansList[i]->GetName(), hCompareNPMediansList[i]);

      for(int i=0; i<2; i++){
        if(hCompareNPMeansList[i] != 0) delete hCompareNPMeansList[i];
        if(hCompareNPMediansList[i] != 0) delete hCompareNPMediansList[i];
      }
      if(hPrefit != 0) delete hPrefit;
      if(hNPs != 0) delete hNPs;

      for(size_t iH = 0; iH < histsPrefit.size(); ++iH) {
        if(histsPrefit.at(iH) != 0) delete histsPrefit.at(iH);
        if(histsPostfitB.at(iH) != 0) delete histsPostfitB.at(iH);
        if(histsPostfitS.at(iH) != 0) delete histsPostfitS.at(iH);
      }
    }
    std::cout << "collected " << listOfProcesses.size() << " names of processes!\n";

    std::cout << "norming PostfitBvals to PrefitVals\n";
    normVals(PostfitBvalsAndErrors, PrefitValsAndErrors, int(labels.size()));
    std::cout << "norming PostfitSBvals to PrefitVals\n";
    normVals(PostfitSBvalsAndErrors, PrefitValsAndErrors, int(labels.size()));
    std::cout << "norming PrefitVals to PrefitVals\n";
    normVals(PrefitValsAndErrors, PrefitValsAndErrors, int(labels.size()));

    std::cout << "drawing norm pullplots\n";
    if(listOfProcesses.size() != 0) drawPullPlots(listOfProcesses, labels, PostfitBvalsAndErrors, PostfitSBvalsAndErrors, PrefitValsAndErrors, outLabel+categoryName +"_normalisation_", -1, 3, pathToShapeExpectationRootfile, categoryName);
    delete[] PostfitBvalsAndErrors;
    delete[] PostfitSBvalsAndErrors;
    delete[] PrefitValsAndErrors;
    listOfProcesses.clear();
  }
}



void loadPseudoExperiments(TString pathToPseudoExperiments, TString containsSignalStrength, std::vector<PseudoExperiments>& expSet, Color_t color = kBlue, const TString suffix = "", const TString sourceFile = "mlfit.root", double injectedMu = -999){
  double nominalMu=0;
  TString helper;

  if(injectedMu != -999) nominalMu = injectedMu;
  else
  {


    helper = containsSignalStrength;
    helper.Remove(0,helper.Index("sig")+3);
    if(helper.Length() > 3) helper.Remove(3, helper.Length());
    nominalMu=convertTStringToDouble(helper);
  }
  TString finalSuffix = suffix;
  if(!finalSuffix.EqualTo("")) finalSuffix.Append(" ");
  helper.Form("%snominal S=%.1f",suffix.Data(), nominalMu);
  expSet.push_back( PseudoExperiments(helper,nominalMu) );
  expSet.back().addExperiments(pathToPseudoExperiments, sourceFile);

  if((expSet.back().mu() == NULL) ){
    std::cout << "Could not load any mus for experiment " << expSet.back()() << " from directory " << pathToPseudoExperiments.Data() << ", deleting current experiment set...\n";
    expSet.pop_back();
  }
  else
  {
    std::cout << "obtained number of mus: " << expSet.back().mu()->GetEntries() << std::endl;
    expSet.back().setColor(color);
  }
=======
                 const TString& outLabel, const TString& testName) {
    // mean values and distributions of POIs
    TH1* hPOIs = new TH1D("hPOIs",";;#mu",exps.size(),0,exps.size());
    TH1* hInit = static_cast<TH1*>(hPOIs->Clone("hInit"));
    TH1* hPOImedians = static_cast<TH1*>(hPOIs->Clone("hPOImedians"));
    TH1* hPOIwithFittedError = static_cast<TH1*>(hPOIs->Clone("hPOIwithFittedError"));

    std::vector<TH1*> hists;
    std::vector<TString> labels;
    TH2D* correlationPlot = NULL;
    //std::cout << "size of exps vector: " << exps.size() << std::endl;
    for(size_t iE = 0; iE < exps.size(); ++iE) {
        const int bin = iE+1;
        const PseudoExperiments& exp = exps.at(iE);
        labels.push_back(exp());

        //hPOIs->SetBinContent(bin,exp.muMean());
        //std::cout << "filling mu = " << exp.muMean() << " into bin " << bin <<std::endl;
        //hPOIs->SetBinError(bin, exp.mu()->GetMeanError());
        setupHistogramBin(hPOIs, bin, labels.back(), exp.muMean(), exp.mu()->GetMeanError());
        //hPOImedians->SetBinContent(bin,getMedian(exp.mu()));
        //hPOImedians->SetBinError(bin,exp.muRMS());
        setupHistogramBin(hPOImedians, bin, labels.back(), getMedian(exp.mu()), exp.muRMS());
        setupHistogramBin(hPOIwithFittedError, bin, labels.back(), exp.muMean(), exp.muError());
        //hInit->SetBinContent(bin,exp.muInjected());
        setupHistogramBin(hInit, bin, labels.back(), exp.muInjected());

        hists.push_back( exp.mu() );
        hists.back()->GetXaxis()->SetTitle("#mu");
        hists.back()->SetTitle("");
        hists.back()->SetLineColor( exp.color() );
        norm(hists.back());
        //setXRange(hists.back(),-3,5);
        correlationPlot = exps.at(iE).getCorrelationPlotPostfitB();
        if(correlationPlot != NULL){
          printCorrelationPlots(correlationPlot, outLabel, "correlationPlot_PostfitB_" + labels.back());
          delete correlationPlot;
          correlationPlot = NULL;
        }
        correlationPlot = exps.at(iE).getCorrelationPlotPostfitS();
        if(correlationPlot != NULL){
          printCorrelationPlots(correlationPlot, outLabel, "correlationPlot_PostfitS_" + labels.back());
          delete correlationPlot;
          correlationPlot = NULL;
        }

    }
    //hInit->GetYaxis()->SetRangeUser(-1.1,3.1);

    std::cout << "comparing mean values for POI\n";
    compareMeanValues(hPOIs,hInit,labels,outLabel+"POImeans",hPOImedians);
    writePOILatexTable(hPOIs, hPOImedians, hPOIwithFittedError, outLabel+"POI.tex", "Signal Strength Parameters", testName);
    compareDistributions(hists,labels,outLabel+"POI",false);

    if(hPOIs != NULL) delete hPOIs;
    if(hInit != NULL) delete hInit;
    for(auto& h: hists) {
        if(h != NULL) delete h;
    }

}

void saveFitValues(const int bin, TH1D* hMeans, double mean, double meanError, std::vector<std::vector<Double_t> >& fitResultsContainer, TH1D* hMedians=NULL, double median=0., double rms=0.){
    //std::cout << "\tFilling histogram " << hMeans->GetName() << std::endl;
    std::vector<Double_t> vectorToSafe;
    //std::cout << "\t\tMean value in bin " << bin << ": " << mean << " +- " << meanError << std::endl;
    if(hMeans != NULL){
        hMeans->SetBinContent(bin, mean );
        hMeans->SetBinError(bin,meanError );

        vectorToSafe.push_back(mean);
        vectorToSafe.push_back(meanError);
    }
    if(hMedians != NULL)
    {
        //std::cout << "\t\tMedian value in bin " << bin << ": " << median << " +- " << rms << std::endl;
        hMedians->SetBinContent(bin,median);
        hMedians->SetBinError(bin,rms);

        vectorToSafe.push_back(median);
        vectorToSafe.push_back(rms);
    }
    fitResultsContainer.push_back(vectorToSafe);
}

void normVals(std::vector<std::vector<Double_t> >* toNorm, std::vector<std::vector<Double_t> >* norm, int nAllExps){
    if(toNorm != NULL && norm != NULL){
        for(int nExp=0; nExp<nAllExps; nExp++){
            //std::cout << "\tnorming experiment #" << nExp << std::endl;
            for(int i=0; i<int(norm[nExp].size()); i++)
            {
                //std::cout << "\t\tnorming process #" << i << std::endl;
                double postFitMeanVal = toNorm[nExp][i][0];
                double postFitMeanErr = toNorm[nExp][i][1];
                double postFitMedian = toNorm[nExp][i][2];
                double postFitRMS = toNorm[nExp][i][3];
                double preFitMeanVal = norm[nExp][i][0];
                double preFitMeanErr = norm[nExp][i][1];
                double preFitMedian = norm[nExp][i][2];
                double preFitRMS = norm[nExp][i][3];
                if(norm[nExp][i][0] != 0 && norm[nExp][i][2] != 0)
                {
                    toNorm[nExp][i][0] = postFitMeanVal/preFitMeanVal;
                    //std::cout << "\t\t\tnew mean val = " << toNorm[nExp][i][0] << std::endl;
                    toNorm[nExp][i][1] = toNorm[nExp][i][0]*TMath::Sqrt(TMath::Power(postFitMeanErr/postFitMeanVal,2) + TMath::Power(preFitMeanErr/preFitMeanVal,2));
                    //std::cout << "\t\t\tnew mean err = " << toNorm[nExp][i][1] << std::endl;
                    toNorm[nExp][i][2] = postFitMedian/preFitMedian;
                    //std::cout << "\t\t\tnew median val = " << toNorm[nExp][i][2] << std::endl;

                    toNorm[nExp][i][3] = toNorm[nExp][i][2]*TMath::Sqrt(TMath::Power(postFitRMS/postFitMedian,2) + TMath::Power(preFitRMS/preFitMedian,2));
                    //std::cout << "\t\t\tnew RMS = " << toNorm[nExp][i][3] << std::endl;
                }
                else
                {
                    //std::cout << "\tmean value to norm to is 0! Setting all values to 0\n";
                    for(int i=0; i<int(norm[nExp][i].size()); i++) toNorm[nExp][i][i] = 0;
                }
            }
        }
    }
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


void compareShapes(const std::vector<PseudoExperiments>& exps, const TString& outLabel, const TString& pathToShapeExpectationRootfile=""){
    std::vector<TString> listOfProcesses;
    std::vector<std::vector<Double_t> >* PostfitBvalsAndErrors =NULL;
    std::vector<std::vector<Double_t> >* PostfitSBvalsAndErrors = NULL;
    std::vector<std::vector<Double_t> >* PrefitValsAndErrors = NULL;
    std::vector<TString> labels;

    TH1* hNPs = NULL; //Histogram to collect one np from all pseudo experiments
    std::vector<TH1*> histsPrefit;
    std::vector<TH1*> histsPostfitB;
    std::vector<TH1*> histsPostfitS;

    TH1D* hCompareNPMeansList[2] = {};
    TH1D* hCompareNPMediansList[2] = {};
    TH1D* hPrefit = NULL;



    ShapeContainer* prefitShapes = NULL;
    ShapeContainer* postfitSshapes = NULL;
    ShapeContainer* postfitBshapes = NULL;

    TString categoryName;
    TString np;

    for(int nCategory=0; nCategory< int(exps.begin()->getPrefitShapes().size()); nCategory++)
    {
        listOfProcesses = exps.begin()->getPrefitShapes()[nCategory]->getListOfProcesses();
        PostfitBvalsAndErrors = new std::vector<std::vector<Double_t> >[exps.size()];
        PostfitSBvalsAndErrors = new std::vector<std::vector<Double_t> >[exps.size()];
        PrefitValsAndErrors = new std::vector<std::vector<Double_t> >[exps.size()];

        categoryName = exps.begin()->getPrefitShapes()[nCategory]->getName();

        for(int nProcess=0; nProcess<int(listOfProcesses.size()); nProcess++ ) {
            labels.clear();
            histsPrefit.clear();
            histsPostfitB.clear();
            histsPostfitS.clear();


            np = listOfProcesses[nProcess];

            hNPs = new TH1D("hNPs",TString(";;"+np).Data(),exps.size(),0,exps.size()); //Histogram to collect one np from all pseudo experiments

            hCompareNPMeansList[0] = (TH1D*)hNPs->Clone(TString(categoryName + "_normalisation_"+np+"_mean_PostfitB").Data());
            hCompareNPMeansList[1] = (TH1D*)hNPs->Clone(TString(categoryName + "_normalisation_"+np+"_mean_PostfitS").Data());
            hCompareNPMediansList[0] = (TH1D*)hNPs->Clone(TString(categoryName + "_normalisation_"+np+"_median_PostfitB").Data());
            hCompareNPMediansList[1] = (TH1D*)hNPs->Clone(TString(categoryName + "_normalisation_"+np+"_median_PostfitS").Data());
            hPrefit = (TH1D*)(hNPs->Clone(TString("hPrefit_"+categoryName + "_normalisation_"+np).Data()));


            for(size_t iE = 0; iE < exps.size(); ++iE) {
                const int bin = iE+1;
                const PseudoExperiments& exp = exps.at(iE);

                prefitShapes = exp.getPrefitShapes()[nCategory];
                //resizeHisto(prefitShapes->getDist(np));
                postfitSshapes = exp.getPostfitSshapes()[nCategory];
                //resizeHisto(postfitSshapes->getDist(np));
                postfitBshapes = exp.getPostfitBshapes()[nCategory];
                //resizeHisto(postfitBshapes->getDist(np));
                //std::cout << "DEBUG\t current process: " << np.Data() << std::endl;
                //std::cout << "DEBUG\t prefitShapes:\n";
                //std::cout << "\t number of saved processes: " << prefitShapes->getNumberOfProcesses() << std::endl;
                //std::cout << "\t number of saved normalisations: " << prefitShapes->getDist(np)->GetEntries() << std::endl;

                //std::cout << "DEBUG\t postfitSshapes:\n";
                //std::cout << "\t number of saved processes: " << postfitSshapes->getNumberOfProcesses() << std::endl;
                //std::cout << "\t number of saved normalisations: " << postfitSshapes->getDist(np)->GetEntries() << std::endl;

                //std::cout << "DEBUG\t postfitBshapes:\n";
                //std::cout << "\t number of saved processes: " << postfitBshapes->getNumberOfProcesses() << std::endl;
                //std::cout << "\t number of saved normalisations: " << postfitBshapes->getDist(np)->GetEntries() << std::endl;

                //std::cout << "DEBUG\t current category: " << categoryName.Data() << std::endl;
                labels.push_back( exp() );

                //std::cout << "\tsaving PostfitB of PseudoExperiment with " << labels.back()  << " \n";

                saveFitValues(bin, hCompareNPMeansList[0], postfitBshapes->getMean(np), postfitBshapes->getMeanError(np), PostfitBvalsAndErrors[iE], hCompareNPMediansList[0], getMedian(postfitBshapes->getDist(np) ), postfitBshapes->getRMS(np));
                //std::cout << "\tsaving PostfitS of PseudoExperiment with " << labels.back()  << " \n";

                saveFitValues(bin, hCompareNPMeansList[1], postfitSshapes->getMean(np), postfitSshapes->getMeanError(np), PostfitSBvalsAndErrors[iE], hCompareNPMediansList[1], getMedian(postfitSshapes->getDist(np) ), postfitSshapes->getRMS(np));

                //std::cout << "\tsaving Prefit of PseudoExperiment with " << labels.back()  << " \n";

                saveFitValues(bin, hPrefit, prefitShapes->getMean(np), prefitShapes->getMeanError(np), PrefitValsAndErrors[iE]);
                PrefitValsAndErrors[iE].back().push_back(getMedian(prefitShapes->getDist(np) ));
                PrefitValsAndErrors[iE].back().push_back(prefitShapes->getRMS(np));

                std::cout << "saving norm distributions...\n";
                histsPrefit.push_back( prefitShapes->getDist(np) );
                norm( histsPrefit.back() );
                setLineStyle( histsPrefit.back(), exp.color(), 1 );

                histsPostfitB.push_back( postfitBshapes->getDist(np) );
                norm( histsPostfitB.back() );
                setLineStyle( histsPostfitB.back(), exp.color(), 1 );

                histsPostfitS.push_back( postfitSshapes->getDist(np) );
                norm( histsPostfitS.back() );
                setLineStyle( histsPostfitS.back(), exp.color(), 1 );
            }
            // plot np distributions
            // std::cout << "\tPostFitB histogram entries:\n";
            // for(int i=1; i<=int(exps.size()); i++) std::cout << "\t\tSaved mean val for " << labels[i-1]  << " in bin " << i << ": " << hCompareNPMeansList[0]->GetBinContent(i) << std::endl;
            // std::cout << "\tPostFitS histogram:\n";
            // for(int i=1; i<=int(exps.size()); i++) std::cout << "\t\tContent for " << labels[i-1]  << " in bin" <<  i << ": " << hCompareNPMeansList[1]->GetBinContent(i) << std::endl;
            // std::cout << "\tPrefit histograms: " << histsPrefit.size() << "\n";
            // for(int i=0; i<int(histsPrefit.size()); i++) std::cout << "\t\tEntries for " << labels[i]  << " in histo" <<  histsPrefit[i]->GetName() << ": " << histsPrefit[i]->GetEntries() << std::endl;
            compareDistributions(histsPrefit,labels,outLabel+categoryName + "_normalisation_"+np+"_Prefit");
            //std::cout << "\tPostfitB histograms: " << histsPostfitB.size() << "\n";
            //for(int i=0; i<int(histsPostfitB.size()); i++) std::cout << "\t\tEntries for " << labels[i]  << " in histo" <<  histsPostfitB[i]->GetName() << ": " << histsPostfitB[i]->GetEntries() << std::endl;
            compareDistributions(histsPostfitB,labels,outLabel+categoryName + "_normalisation_"+np+"_PostfitB");
            //std::cout << "\t PostfitS histograms: " << histsPostfitS.size() << "\n";
            //for(int i=0; i<int(histsPostfitS.size()); i++) std::cout << "\t\tEntries for " << labels[i]  << " in histo" <<  histsPostfitS[i]->GetName() << ": " << histsPostfitS[i]->GetEntries() << std::endl;
            compareDistributions(histsPostfitS,labels,outLabel+categoryName + "_normalisation_"+np+"_PostfitS");

            // plot np means
            for(int i=0; i<2; i++) compareMeanValues(hCompareNPMeansList[i],hPrefit,labels,outLabel+hCompareNPMeansList[i]->GetName(), hCompareNPMediansList[i]);

            for(int i=0; i<2; i++){
                if(hCompareNPMeansList[i] != 0) delete hCompareNPMeansList[i];
                if(hCompareNPMediansList[i] != 0) delete hCompareNPMediansList[i];
            }
            if(hPrefit != 0) delete hPrefit;
            if(hNPs != 0) delete hNPs;

            for(auto& h : histsPrefit) if(h != 0) delete h;
            histsPrefit.clear();
            for(auto& h : histsPostfitB) if(h != 0) delete h;
            histsPostfitB.clear();
            for(auto& h : histsPostfitS) if(h != 0) delete h;
            histsPostfitS.clear();
        }
        std::cout << "collected " << listOfProcesses.size() << " names of processes!\n";

        std::cout << "norming PostfitBvals to PrefitVals\n";
        normVals(PostfitBvalsAndErrors, PrefitValsAndErrors, int(labels.size()));
        std::cout << "norming PostfitSBvals to PrefitVals\n";
        normVals(PostfitSBvalsAndErrors, PrefitValsAndErrors, int(labels.size()));
        std::cout << "norming PrefitVals to PrefitVals\n";
        normVals(PrefitValsAndErrors, PrefitValsAndErrors, int(labels.size()));

        std::cout << "drawing norm pullplots\n";
        if(listOfProcesses.size() != 0){
            for(int i = 0; i<int(exps.size()); i++){
                drawPullPlots(listOfProcesses, labels[i], PostfitBvalsAndErrors[i], PostfitSBvalsAndErrors[i], PrefitValsAndErrors[i], outLabel+categoryName +"_normalisation_", -1, 3, pathToShapeExpectationRootfile, categoryName);
            }

        }
        delete[] PostfitBvalsAndErrors;
        delete[] PostfitSBvalsAndErrors;
        delete[] PrefitValsAndErrors;
        listOfProcesses.clear();
    }

}

void loadPseudoExperiments(TString pathToPseudoExperiments, TString containsSignalStrength, std::vector<PseudoExperiments>& expSet, Color_t color = kBlue, double injectedMu = -999, const TString suffix = "", const TString sourceFile = "mlfit.root"){
    double nominalMu=0;
    TString helper;
    std::cout << "in 'loadPseudoExperiments': injectedMu = " << injectedMu << std::endl;
    if(injectedMu != -999) nominalMu = injectedMu;
    else
    {


        helper = containsSignalStrength;
        helper.Remove(0,helper.Index("sig")+3);
        if(helper.Length() > 3) helper.Remove(3, helper.Length());
        nominalMu=convertTStringToDouble(helper);
    }
    TString finalSuffix = suffix;
    if(!finalSuffix.EqualTo("")) finalSuffix.Append(" ");
    helper.Form("%snominal S=%.1f",suffix.Data(), nominalMu);
    expSet.push_back( PseudoExperiments(helper,nominalMu) );
    expSet.back().addExperiments(pathToPseudoExperiments, sourceFile);

    if((expSet.back().mu() == NULL) ){
        std::cout << "Could not load any mus for experiment " << expSet.back()() << " from directory " << pathToPseudoExperiments.Data() << ", deleting current experiment set...\n";
        expSet.pop_back();
    }
    else
    {
        std::cout << "obtained number of mus: " << expSet.back().mu()->GetEntries() << std::endl;
        expSet.back().setColor(color);
    }
>>>>>>> 81509c8b530b5f11e714f8f281f434aa1387a365

}

void plotResults(TString pathname, TString pathToShapeExpectationRootfile = "", double injectedMu = -999) {
  TheLooks::set();
<<<<<<< HEAD
=======
  gStyle->SetPaintTextFormat(".2f");

>>>>>>> 81509c8b530b5f11e714f8f281f434aa1387a365
  std::vector<PseudoExperiments> expSet;
  if(pathname.EndsWith("/")) pathname.Chop();
  TString outputPath = pathname;
  if(outputPath.Contains("/")) outputPath.Remove(0, outputPath.Last('/')+1);
  TString testName = outputPath;
  if(outputPath.Contains(".")) outputPath.ReplaceAll(".", "p");
  outputPath.Prepend(pathname + "/");
  outputPath.Append("_");
<<<<<<< HEAD
  //if(!pathToShapeExpectationRootfile.Contains("/")) pathToShapeExpectationRootfile.Form("%s/temp/%s", pathname.Data(), pathToShapeExpectationRootfile.Data());
  //std::cout << "getting expectation from " << pathToShapeExpectationRootfile << std::endl;
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
      if(ncolor > 4) ncolor = 0;
      Color_t colors[5] = {kBlue, kRed, kBlack, kGreen, kOrange};
      if (folder->IsDirectory() && folderName.Contains("sig")) {
        loadPseudoExperiments(pathname+"/"+folderName, folderName, expSet, colors[ncolor]);
        ncolor++;
        //loadPseudoExperiments(pathname+"/"+folderName, folderName, expSet, colors[ncolor], "MDF", "mlfit_MDF.root");
        //ncolor++;
      }
      if (folder->IsDirectory() && folderName.Contains("PseudoExperiment")){
        loadPseudoExperiments(pathname, pathname, expSet, colors[ncolor], injectedMu);
        ncolor++;
        //loadPseudoExperiments(pathname, pathname, expSet, colors[ncolor], "MDF", "mlfit_MDF.root");
        //ncolor++;
        //break;
      }
    }
    if(folder != NULL) delete folder;
    if(folders != NULL) delete folders;

=======

  int ncolor=0;
  Color_t colors[5] = {kBlue, kRed, kBlack, kGreen, kOrange};

  //if(!pathToShapeExpectationRootfile.Contains("/")) pathToShapeExpectationRootfile.Form("%s/temp/%s", pathname.Data(), pathToShapeExpectationRootfile.Data());
  //std::cout << "getting expectation from " << pathToShapeExpectationRootfile << std::endl;
  TSystemDirectory dir(pathname.Data(), pathname.Data());
  if(pathname.Contains("PseudoExperiment")){
    loadPseudoExperiments(pathname, pathname, expSet, colors[ncolor], injectedMu);
    ncolor++;
    loadPseudoExperiments(pathname, pathname, expSet, colors[ncolor], injectedMu, "MDF", "mlfit_MS_mlfit.root");
    ncolor++;
  }
  else{
    TList *folders = dir.GetListOfFiles();
    //if folders are found, go through each one an look for the mlfitFile
    if (folders) {
      TSystemFile *folder;
      TString folderName;
      TIter next(folders);
      while ((folder=(TSystemFile*)next())) {
        folderName = folder->GetName();
        if(ncolor > 4) ncolor = 0;
        if (folder->IsDirectory() && folderName.Contains("sig")) {
          loadPseudoExperiments(pathname+"/"+folderName, folderName, expSet, colors[ncolor]);
          ncolor++;
          loadPseudoExperiments(pathname+"/"+folderName, folderName, expSet, colors[ncolor], injectedMu, "MDF", "mlfit_MS_mlfit.root");
          ncolor++;
        }
        if (folder->IsDirectory() && folderName.Contains("PseudoExperiment")){
          loadPseudoExperiments(pathname, pathname, expSet, colors[ncolor], injectedMu);
          ncolor++;
          loadPseudoExperiments(pathname, pathname, expSet, colors[ncolor], injectedMu, "MDF", "mlfit_MS_mlfit.root");
          ncolor++;
          break;
        }
      }
    }
>>>>>>> 81509c8b530b5f11e714f8f281f434aa1387a365
  }
  // set inputs
  std::cout << "loaded Experiments: " << expSet.size() << std::endl;
  if(expSet.size() != 0)
  {
    pathname += "/";
<<<<<<< HEAD
    comparePOIs(expSet,outputPath, testName);
    compareNuisanceParameters(expSet,outputPath,true);
    compareShapes(expSet, outputPath, pathToShapeExpectationRootfile);
=======

    comparePOIs(expSet,outputPath, testName);
    compareNuisanceParameters(expSet,outputPath,true);
    compareShapes(expSet, outputPath, pathToShapeExpectationRootfile);
    for(auto& exp : expSet){
      std::cout << "label " << exp() << std::endl;
      std::cout << "\tr = " << exp.muMean() << " +- " << exp.muMeanError() << " +- " << exp.muRMS() << " +- " << exp.muError() << std::endl;
    }

>>>>>>> 81509c8b530b5f11e714f8f281f434aa1387a365

  }
  else std::cerr << "was unable to load any Pseudo Experiments!\n";
}
<<<<<<< HEAD
=======

//
// # ifndef __CINT__
// int main()
// {
//   plotResults("/nfs/dust/cms/user/pkeicher/tth_analysis_study/CombineFitTests/PseudoDataTests/test/officialHiggsCombine/JTBDT_Spring17v10/test_msfit/wo_NP/PseudoData/r_ttbb/63445464_ttHbb_N1000_r_ttbb_ttbarPlusBBbar_1.2_test1", "/nfs/dust/cms/user/pkeicher/tth_analysis_study/CombineFitTests/PseudoDataTests/test/officialHiggsCombine/JTBDT_Spring17v10/test_msfit/wo_NP/PseudoData/r_ttbb/63445464_ttHbb_N1000_r_ttbb_ttbarPlusBBbar_1.2_test1/temp/temp_shapeExpectation.root");
//   return 0;
// }
// # endif
>>>>>>> 81509c8b530b5f11e714f8f281f434aa1387a365
