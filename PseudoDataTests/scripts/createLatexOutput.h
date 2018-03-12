#ifndef CREATE_LATEX_OUTPUT_H
#define CREATE_LATEX_OUTPUT_H

#include "TH1.h"
#include "TString.h"
#include "helperFuncs.h"
#include <iostream>
#include <fstream>


namespace createLatexOutput{

void writeValues(std::ofstream& output, const int& bin, const TH1* hMeans, const TH1* hMedians, const TH1* hFittedErrors, const bool writeFittedErrors, const TH1* hExpectation = NULL){
  //get postfit b values to write
  double value = helperFuncs::checkValues(hMeans->GetBinContent(bin));
  double meanError = helperFuncs::checkValues(hMeans->GetBinError(bin));
  double median = helperFuncs::checkValues(hMedians->GetBinContent(bin));
  double rms = helperFuncs::checkValues(hMedians->GetBinError(bin));
  double meanFitError = 0;
  if(writeFittedErrors) meanFitError = helperFuncs::checkValues(hFittedErrors->GetBinError(bin));
  std::cout << "\t" << value << ",\t" << meanError << ",\t" << rms;
  if(writeFittedErrors) std::cout << ",\t" << meanFitError;
  std::cout << "\tmedian: " << median;
  std::cout << std::endl;
  //~ output << " \t& \\num{" << value << "} $\\pm$ \\num{" << meanError << "} $\\pm$ \\num{" << rms << "}";
  //~ if(writeFittedErrors) output << " $\\pm$ \\num{"<< meanFitError << "}";
  output << "\t& " << value << "," << meanError << "," << rms;
  if(writeFittedErrors) output << "," << meanFitError;
  output << "\t" << median;
  if(hExpectation != NULL) output << "," << helperFuncs::checkValues(hExpectation->GetBinContent(bin));
  output << "\n";
}

  void writePOILatexTable(const TH1* hMeans, const TH1* hMedians, const TH1* hMeansWithFittedError, const TString outputPath, const TString tableCaption, const TString& testName){
    if(hMeans != NULL && hMedians != NULL && hMeansWithFittedError != NULL){
      std::ofstream output(outputPath.Data(), std::ofstream::out);
      if(output.is_open()){
        int nSignalStrengths = hMeans->GetNbinsX();
        //~ output << "\\begin{table}\n";
        //~ output << "\\centering\n";
        TString processName = testName;
        //~ if(processName.Contains("_")) processName.ReplaceAll("_", "\\_");
        //~ processName.Prepend(tableCaption + " for ");
        //~ output << "\\caption{" << processName.Data() << "}\n";

        //~ output << "\\begin{tabular}{cc";
        //~ output << "}\n";
        //~ output << "\\toprule\n";
        //~ output << "Pseudo Experiment & Mean $\\pm$ Mean Error $\\pm$ RMS $\\pm$ Fitted Error\\\\\n";
        //~ output << "\\midrule\n";
        
        for(int bin=1; bin <= int(hMeans->GetNbinsX()); bin++)
        {
          output <<  hMeans->GetXaxis()->GetBinLabel(bin);
          writeValues(output, bin, hMeans, hMedians, hMeansWithFittedError, hMeansWithFittedError != NULL);
           //~ << "\t & \\num{" << helperFuncs::checkValues(hMeans->GetBinContent(bin)) << "} $\\pm$ \\num{"<< helperFuncs::checkValues(hMeans->GetBinError(bin)) << "}";
          //~ output << " $\\pm$ \\num{"<< helperFuncs::checkValues(hMedians->GetBinError(bin)) << "} $\\pm$ \\num{" << helperFuncs::checkValues(hMeansWithFittedError->GetBinError(bin)) << "}";
          //~ output << "\\\\\n";
          output << "\n";

        }
        //~ output << "\\bottomrule\n";
        //~ output << "\\end{tabular}\n\\end{table}";
        output.close();
      }
      else std::cerr << "ERROR: could not open .tex table!\n";

    }
    else std::cerr << "ERROR: Histogram pointer(s) is(are) NULL!\n";
  }

  void writeLatexTable(TH1* hMeansB, TH1* hMediansB, TH1* hMeansSB, TH1* hMediansSB, TString outputPath, TString tableCaption, TH1* hExpectation=NULL, TH1* hMeansBwithFittedError= NULL, TH1* hMeansSBwithFittedError = NULL){
    //~ outputPath.ReplaceAll(".tex", ".txt");
    std::ofstream output(outputPath.Data(), std::ofstream::out);
    //output.open();
    if(output.is_open() && hMeansB != NULL && hMediansB != NULL && hMeansSB != NULL && hMediansSB != NULL){
      //~ output << "\\begin{table}\n";
      //~ output << "\\centering\n";
      //~ output << "\\caption{" << tableCaption.Data() << "}\n";
      //~ output << "\\begin{tabular}{ccc";
      //~ if(hExpectation != NULL) output << "c";
      //~ output << "}\n";
      //~ output << "\\toprule\n";
      //~ output << "Parameter \t& \\multicolumn{2}{c}{Mean $\\pm$ Mean Error $\\pm$ RMS";
      //~ if(hMeansBwithFittedError != NULL && hMeansSBwithFittedError != NULL) output << " $\\pm$ Fitted Error";
      //~ output << "}";
      //~ if(hExpectation != NULL) output << " \t& Expectation";
      //~ output << "\\\\\n";
      //~ output << " \t& B-Only Fit & S+B Fit";
      //~ if(hExpectation != NULL) output << " & ";
      //~ output << "\\\\\n";

      //~ output << "\\midrule\n";
      TString processName;
      
      bool writeFittedErrors = hMeansBwithFittedError != NULL && hMeansSBwithFittedError != NULL; 
      for(int bin=1; bin <= int(hMeansB->GetNbinsX()); bin++)
      {
        //get Process name and make it latex prove
        processName = hMeansB->GetXaxis()->GetBinLabel(bin);
        //~ if(processName.Contains("_")) processName.ReplaceAll("_", "\\_");
        output << processName;
        std::cout << "parameters to write for PostfitB " << processName <<" (pre check):\n";

        writeValues(output, bin, hMeansB, hMediansB, hMeansBwithFittedError, writeFittedErrors);
        std::cout << "parameters to write for PostfitSB " << processName <<"  (pre check):\n";
        writeValues(output, bin, hMeansSB, hMediansSB, hMeansSBwithFittedError, writeFittedErrors, hExpectation);
        //~ if(hExpectation != NULL) output << " & \\num{" << helperFuncs::checkValues(hExpectation->GetBinContent(bin)) << "}";
        //~ output << "\\\\\n";
        output << "\n";

      }
      //~ output << "\\bottomrule\n";
      //~ output << "\\end{tabular}\n\\end{table}";
      output.close();
    }
    else std::cerr << "ERROR: could not open .tex table!\n";
  }
  void writeTextTable(const TH1* hMeans, const TH1* hMedians, const TString& outputPath, const TH1* hExpectation, const TH1* hMeansWithFittedError = NULL){
    std::cout << "writing values into " << outputPath.Data() << std::endl;
    std::ofstream output(outputPath.Data(), std::ofstream::out);
    if(output.is_open() && hMeans != NULL && hMedians != NULL){
      TString processName;
      
      bool writeFittedErrors = hMeansWithFittedError != NULL; 
      for(int bin=1; bin <= int(hMeans->GetNbinsX()); bin++)
      {
        //get Process name and make it latex prove
        processName = hMeans->GetXaxis()->GetBinLabel(bin);
        output << processName;
        std::cout << "parameters to write for " << processName <<" (pre check):\n";

        writeValues(output, bin, hMeans, hMedians, hMeansWithFittedError, writeFittedErrors, hExpectation);
        output << "\n";

        //~ output << "\\\\\n";
      }
      
      output.close();
      
    }
    else std::cerr << "ERROR: could not open " << outputPath.Data() << std::endl;
    
  }
}
#endif
