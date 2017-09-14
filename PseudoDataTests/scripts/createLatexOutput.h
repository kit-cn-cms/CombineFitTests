#ifndef CREATE_LATEX_OUTPUT_H
#define CREATE_LATEX_OUTPUT_H

#include "TH1.h"
#include "TString.h"

namespace createLatexOutput{
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
}
#endif
