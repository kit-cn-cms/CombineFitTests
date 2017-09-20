#ifndef DRAW_PULL_PLOTS_H
#define DRAW_PULL_PLOTS_H

#include "TH1.h"
#include "TH1D.h"
#include "TTree.h"
#include "TFile.h"
#include "TLegend.h"
#include "TCanvas.h"
#include "TString.h"
#include <vector>


#include "helperFuncs.h"
#include "createLatexOutput.h"

namespace drawPullPlots{

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


        helperString = new TString(label);
        std::cout << "helperString: " << helperString->Data() << std::endl;

        helperString->Remove(0,helperString->Index("=")+1);
        std::cout << "helperString: " << helperString->Data() << std::endl;
        signalStrength = helperFuncs::convertTStringToDouble(*helperString);
        if(helperString != NULL) delete helperString;

        int nParameters = int(listOfNPs.size());
        // std::cout << "creating pull plot for " << label << " with " << nParameters << " parameters:" << std::endl;
        // for(auto& name : listOfNPs){
        //   std::cout << "\t" << name.Data() << std::endl;
        // }
        // std::cout << "vector PostfitBvalsAndErrors contains " << PostfitBvalsAndErrors.size() << " parameters\n";
        // std::cout << "vector PostfitSBvalsAndErrors contains " << PostfitSBvalsAndErrors.size() << " parameters\n";
        // std::cout << "vector PrefitValsAndErrors contains " << PrefitValsAndErrors.size() << " parameters\n";
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
          if(hExpectation != NULL){
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
                tree->SetBranchAddress("total_background_postscale", &background_postscale) >= 0)
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
              helperFuncs::setupHistogramBin(hExpectation, np+1, listOfNPs[np], ratio, 0);
              //vectorToSafe.push_back(ratio);
              //std::cout << "reset branch address\n";
              tree->ResetBranchAddresses();
              prescaleVal = 0;
              postscaleVal = 0;
              ratio = 0;
            }
            else std::cerr << "TTree " << categoryName << " could not be loaded from file " << expectationFile->GetName() << "!\n";
          }

          // std::cout << "DEBUG:\tgoing to access std::vector hPostfitBmeansWithFitErrors\n\tcontents for parameter" << np << std::endl;
          // for(auto& val : PostfitBvalsAndErrors[np]){
          //   std::cout << "\t\t" << val << std::endl;
          // }
          helperFuncs::setupHistogramBin(hPostfitBmeans, np+1, listOfNPs[np], PostfitBvalsAndErrors[np][0], PostfitBvalsAndErrors[np][1]);
          //helperFuncs::setupHistogramBin(hPostfitBmedians, np+1, listOfNPs[np], PostfitBvalsAndErrors[np][2], PostfitBvalsAndErrors[np][3]);

          if(hPostfitBmeansWithFitErrors != NULL) helperFuncs::setupHistogramBin(hPostfitBmeansWithFitErrors, np+1, listOfNPs[np], PostfitBvalsAndErrors[np][0], PostfitBvalsAndErrors[np][4]);

          helperFuncs::setupHistogramBin(hPostfitSBmeans, np+1, listOfNPs[np], PostfitSBvalsAndErrors[np][0], PostfitSBvalsAndErrors[np][1]);
          if(hPostfitSBmeansWithFitErrors != NULL) helperFuncs::setupHistogramBin(hPostfitSBmeansWithFitErrors, np+1, listOfNPs[np], PostfitSBvalsAndErrors[np][0], PostfitSBvalsAndErrors[np][4]);

          //helperFuncs::setupHistogramBin(hPostfitSBmedians, np+1, listOfNPs[np], PostfitSBvalsAndErrors[np][2], PostfitSBvalsAndErrors[np][3]);

          helperFuncs::setupHistogramBin(hPrefitMeans, np+1, listOfNPs[np], PrefitValsAndErrors[np][0], PrefitValsAndErrors[np][1]);
          //helperFuncs::setupHistogramBin(hPrefitMedians, np+1, listOfNPs[np], PrefitValsAndErrors[np][2], PrefitValsAndErrors[np][3]);

        }
        std::cout << "done!\n";

        if(hExpectation != NULL)
        {
          helperFuncs::setLineStyle(hExpectation, kRed, 2);
          legend->AddEntry(hExpectation, "Expected Norm Ratio", "l");
        }
        helperFuncs::setLineStyle(hPrefitMeans, kBlack, 2);
        helperFuncs::setLineStyle(hPrefitMedians, kBlack, 2);

        helperFuncs::setLineStyle(hPostfitBmeans, kBlue, 1, 20);
        if(hPostfitBmeansWithFitErrors != NULL) helperFuncs::setLineStyle(hPostfitBmeansWithFitErrors, kBlue, 4, 1);
        helperFuncs::setLineStyle(hPostfitBmedians, kBlue, 2, 21);

        helperFuncs::setLineStyle(hPostfitSBmeans, kRed, 1, 20);
        if(hPostfitSBmeansWithFitErrors != NULL) helperFuncs::setLineStyle(hPostfitSBmeansWithFitErrors, kRed, 4, 1);
        helperFuncs::setLineStyle(hPostfitSBmedians, kRed, 2, 21);

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
        createLatexOutput::writeLatexTable(hPostfitBmeans, hPostfitBmedians, hPostfitSBmeans, hPostfitSBmedians, outLabel+"values_"+*canvasName+".tex", "Values for "+label, hExpectation, hPostfitBmeansWithFitErrors, hPostfitSBmeansWithFitErrors);

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
  }
  #endif
