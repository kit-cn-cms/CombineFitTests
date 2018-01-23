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
#include "TStyle.h"

#include "LabelMaker.h"
#include "PseudoExperiments.h"
#include "TheLooks.h"
#include "drawPullPlots.h"
#include "helperFuncs.h"
#include "createLatexOutput.h"

void printCorrelationPlots(TH2D* correlationPlot, const TString& outlabel, const TString label){
    if(correlationPlot)
    {
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
        // correlationPlot->Draw("coltzTEXTE");
        correlationPlot->GetXaxis()->SetLabelSize(0.5*correlationPlot->GetXaxis()->GetLabelSize());
        correlationPlot->GetYaxis()->SetLabelSize(0.5*correlationPlot->GetYaxis()->GetLabelSize());
        correlationPlot->Draw("coltz");
        correlationPlot->Write();
        can.Write(outputName);
        std::cout << "creating " << outputName << ".pdf\n";
        can.SaveAs(outputName+".pdf");
        output->Close();
    }
    else{
        std::cerr << "was unable to load correlation matrix!\n";
    }
}

void compareDistributions(const std::vector<TH1*>& hists,
                          const std::vector<TString>& labels,
                          const TString& outLabel,
                          const bool superimposeNorm = false) {
    gStyle->SetOptStat("e");
    TFile* outfile = TFile::Open(outLabel+".root", "RECREATE");
    TCanvas* can = new TCanvas("can","",900,500);
    can->cd();
    std::vector<TLine*> lines;
    TH1* hNorm = 0;
    if( superimposeNorm ) {
        if(hists.front() != NULL){
            hNorm = static_cast<TH1*>(hists.front()->Clone("norm"));
            hNorm->Reset();
            hNorm->FillRandom("gaus",1E5);
            helperFuncs::norm(hNorm);
            hNorm->SetLineColor(kGray);
            hNorm->SetFillColor(kGray);
        }
    }
    
    double xmin = helperFuncs::findMinValue(hists,"x")*0.5;
    double xmax = helperFuncs::findMaxValue(hists, "x")*1.5;
    double ymin = helperFuncs::findMinValue(hists);
    double ymax = helperFuncs::findMaxValue(hists);
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
        if(hists[histogram]!= NULL) 
        {
            hists[histogram]->Draw("HISTsame");
            hists[histogram]->Write();
        }
    }
    // TString whichfit = outLabel;
    // whichfit.Remove(0, whichfit.Last('_')+1);
    // if(whichfit.EqualTo("PostfitB")) whichfit = "fit_b";
    // if(whichfit.EqualTo("PostfitS")) whichfit = "fit_s";
    // TString helper;
    // TString linePath;
    // double signalStrength = 0;
    // for(int nLabel = 0; nLabel<int(labels.size()); nLabel++){
    //   helper = labels[nLabel];
    //   helper.Remove(0, helper.Last('=')+1);
    //   signalStrength = helperFuncs::convertTStringToDouble(helper);
    //   helper = outLabel;
    //   helper.Remove(helper.Last('/'), helper.Length());
    //   linePath.Form("%s/sig%.0f/asimov/fitDiagnostics.root", helper.Data(), signalStrength);
    //   if(whichfit.EqualTo("POI")) lines.push_back(helperFuncs::createLine(linePath, "fit_s", "r"));
    //   else lines.push_back(helperFuncs::createLine(linePath, whichfit, hists.front()->GetXaxis()->GetTitle()));
    //   if(lines.back() != NULL){
    //     lines.back()->SetLineColor(hists[nLabel]->GetLineColor());
    //     lines.back()->SetY2(ymax);
    //     lines.back()->Draw("Same");
    //   }
    // }
    // std::cout << "number of saved lines: " << lines.size() << std::endl;


    TLegend* leg = LabelMaker::legend("top right",labels.size(),0.6);
    TString legendEntry;

    TString greekLetter;
    if(outLabel.Contains("POI")) greekLetter = "mu";
    else greekLetter = "theta";

    for(size_t iH = 0; iH < hists.size(); ++iH) {
        legendEntry.Form("#splitline{%s}{#%s_{mean}=%.2f #pm %.2f_{(mean error)} #pm %.2f_{(RMS)}}", labels.at(iH).Data(), greekLetter.Data(), hists.at(iH)->GetMean(), hists.at(iH)->GetMeanError(), hists.at(iH)->GetRMS() );
        if(hists.at(iH) != NULL){
            leg->AddEntry(hists.at(iH),legendEntry,"L");
            // if(lines.at(iH)!= NULL){
            //   legendEntry.Form("Asimov Val for %s", labels.at(iH).Data());
            //   leg->AddEntry(lines.at(iH), legendEntry, "l");
            // }
        }
    }

    leg->Draw("same");

    gPad->RedrawAxis();
    std::cout << "printing distribution as " << outLabel << ".pdf\n";
    can->SaveAs(outLabel+".pdf");
    std::cout << "printing distribution as " << outLabel << ".root\n";
    can->Write();
    
    std::cout << "deleting temporary objects\n";
    if( hNorm ) delete hNorm;
    if(leg != NULL) delete leg;
    if(can != NULL) delete can;
    for(int nLine=0; nLine<int(lines.size()); nLine++){
        if(lines[nLine] != NULL) delete lines[nLine];
    }
    outfile->Close();
    gStyle->SetOptStat(000000000);
    std::cout << "done comparing distributions\n";
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
        double maxVal = helperFuncs::findMaxValue(histoList);
        double minVal = helperFuncs::findMinValue(histoList);
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

void selectParameters(const PseudoExperiments& exp, std::vector<TString>& list, const bool ignoreBinByBinNPs, const bool includePOIs = false){
  bool push_back = true;

  // for(auto& np : exp.nps()){
      // push_back = true;
      // if( ignoreBinByBinNPs && np.Contains("BDTbin") ) continue;
      // for(auto& entry: list){
          // if(np.EqualTo(entry)){
              // push_back = false;
              // break;
          // }
      // }
      // if(push_back) list.push_back(np);
  // }
  if( includePOIs ){
      for(auto& poi: exp.pois()){
          list.push_back(poi);
      }
  }
}

std::vector<TString> getParameterList(const std::vector<PseudoExperiments>& exps, const bool ignoreBinByBinNPs, const bool includePOIs = false){
    std::vector<TString> list;
    for(auto& exp : exps)
    {
        selectParameters(exp, list, ignoreBinByBinNPs, includePOIs);
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

            if(exp.postfitB(np)){
                std::cout << exp.postfitB(np) << std::endl;
                std::cout << "\tsaving PostfitB\n";

                labels.push_back( exp() );

                helperFuncs::setupHistogramBin(hCompareNPMeansList[0], bin, labels.back(), exp.postfitBMean(np), exp.postfitBMeanError(np));

                helperFuncs::setupHistogramBin(hCompareNPMediansList[0], bin, labels.back(), helperFuncs::getMedian(exp.postfitB(np) ), exp.postfitBRMS(np));

                histsPostfitB.push_back( exp.postfitB(np) );
                helperFuncs::norm( histsPostfitB.back() );
                helperFuncs::setXRange( histsPostfitB.back(), -4, 4 );
                helperFuncs::setLineStyle( histsPostfitB.back(), exp.color(), 1 );

            }

            if(exp.postfitS(np) != NULL){
                std::cout << "\tsaving PostfitS\n";

                helperFuncs::setupHistogramBin(hCompareNPMeansList[1], bin, labels.back(), exp.postfitSMean(np), exp.postfitSMeanError(np));

                helperFuncs::setupHistogramBin(hCompareNPMediansList[1], bin, labels.back(), helperFuncs::getMedian(exp.postfitS(np) ), exp.postfitSRMS(np));

                histsPostfitS.push_back( exp.postfitS(np) );
                helperFuncs::norm( histsPostfitS.back() );
                helperFuncs::setXRange( histsPostfitS.back(), -4, 4 );
                helperFuncs::setLineStyle( histsPostfitS.back(), exp.color(), 1 );

            }
            //hPrefit->SetBinContent(bin,exp.prefitMean(np));
            //hPrefit->SetBinError(bin,exp.prefitRMS(np));
            if(exp.prefit(np) != NULL){
                std::cout << "\tsaving Prefit vals\n";
                helperFuncs::setupHistogramBin(hPrefit, bin, labels.back(), exp.prefitMean(np), exp.prefitRMS(np));

                histsPrefit.push_back( exp.prefit(np) );
                helperFuncs::norm( histsPrefit.back() );
                helperFuncs::setXRange( histsPrefit.back(), -4, 4 );
                helperFuncs::setLineStyle( histsPrefit.back(), exp.color(), 1 );
            }

        }
        compareDistributions(histsPrefit,labels,outLabel+np+"_Prefit");
        compareDistributions(histsPostfitB,labels,outLabel+np+"_PostfitB");
        compareDistributions(histsPostfitS,labels,outLabel+np+"_PostfitS");

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

void analyzeNPPulls(const PseudoExperiments& exp, const TString& outLabel, const bool& ignoreBinByBinNPs,
                    const double lowerBound=-3,
                    const double upperBound=3,
                    const TString& pathToShapeExpectationRootfile="",
                    const TString& categoryName="")
{
    std::cout << "entering 'analyzeNPPulls()'\n";
    std::vector<std::vector<Double_t> > PostfitBvalsAndErrors;
    std::vector<std::vector<Double_t> > PostfitSBvalsAndErrors;
    std::vector<std::vector<Double_t> > PrefitValsAndErrors;
    std::vector<Double_t> vectorToSafe;
    std::vector<TString> listOfParameters;
    selectParameters(exp, listOfParameters, ignoreBinByBinNPs);
    for(auto& np : listOfParameters){
      if (exp.postfitB(np)){
        std::cout << "\tsaving PostfitB\n";

        vectorToSafe.push_back(exp.postfitBMean(np) - exp.prefitMean(np));
        vectorToSafe.push_back(exp.postfitB(np)->GetMeanError());

        vectorToSafe.push_back(helperFuncs::getMedian(exp.postfitB(np)));
        vectorToSafe.push_back(exp.postfitBRMS(np));
        vectorToSafe.push_back(exp.postfitBerror(np));

        PostfitBvalsAndErrors.push_back(vectorToSafe);
        vectorToSafe.clear();

        std::cout << "\tsaving PostfitS\n";

        vectorToSafe.push_back(exp.postfitSMean(np));
        vectorToSafe.push_back(exp.postfitS(np)->GetMeanError());
        vectorToSafe.push_back(helperFuncs::getMedian(exp.postfitS(np)));
        vectorToSafe.push_back(exp.postfitSRMS(np));
        vectorToSafe.push_back(exp.postfitSerror(np));


        PostfitSBvalsAndErrors.push_back(vectorToSafe);
        vectorToSafe.clear();

        std::cout << "\tsaving Prefit vals\n";

        vectorToSafe.push_back(exp.prefitMean(np));
        vectorToSafe.push_back(exp.prefit(np)->GetMeanError());
        vectorToSafe.push_back(helperFuncs::getMedian(exp.prefit(np)));
        vectorToSafe.push_back(exp.prefitRMS(np));

        PrefitValsAndErrors.push_back(vectorToSafe);
        vectorToSafe.clear();
      }
    }
    drawPullPlots::drawPullPlots(listOfParameters, exp(), PostfitBvalsAndErrors, PostfitSBvalsAndErrors, PrefitValsAndErrors, outLabel, lowerBound, upperBound, pathToShapeExpectationRootfile, categoryName);
    std::cout << "clearing value vectors\n";
    PostfitBvalsAndErrors.clear();
    PostfitSBvalsAndErrors.clear();
    PrefitValsAndErrors.clear();

}

void comparePostfitValues(const std::vector<PseudoExperiments>& exps,
                               const TString& outLabel,
                               const bool& ignoreBinByBinNPs)
{
    std::vector<TString> listOfNPs = getParameterList(exps, ignoreBinByBinNPs, true);
    analyzeNPDistributions(listOfNPs, exps, outLabel,ignoreBinByBinNPs);
    for(auto& exp : exps){
        analyzeNPPulls(exp, outLabel, ignoreBinByBinNPs);
    }
}


//void comparePOIs(const std::vector<PseudoExperiments>& exps,
                 //const TString& outLabel, const TString& testName) {
    //// mean values and distributions of POIs
    //TH1* hPOIs = new TH1D("hPOIs",";;#mu",exps.size(),0,exps.size());
    //TH1* hInit = static_cast<TH1*>(hPOIs->Clone("hInit"));
    //TH1* hPOImedians = static_cast<TH1*>(hPOIs->Clone("hPOImedians"));
    //TH1* hPOIwithFittedError = static_cast<TH1*>(hPOIs->Clone("hPOIwithFittedError"));

    //std::vector<TH1*> hists;
    //std::vector<TString> labels;
    //TH2D* correlationPlot = NULL;
    ////std::cout << "size of exps vector: " << exps.size() << std::endl;
    //for(size_t iE = 0; iE < exps.size(); ++iE) {
        //const int bin = iE+1;
        //const PseudoExperiments& exp = exps.at(iE);
        //labels.push_back(exp());

        ////hPOIs->SetBinContent(bin,exp.postfitSMean());
        ////std::cout << "filling mu = " << exp.postfitSMean() << " into bin " << bin <<std::endl;
        ////hPOIs->SetBinError(bin, exp.mu()->GetMeanError());
        //helperFuncs::setupHistogramBin(hPOIs, bin, labels.back(), exp.postfitSMean(), exp.mu()->GetMeanError());
        ////hPOImedians->SetBinContent(bin,helperFuncs::getMedian(exp.mu()));
        ////hPOImedians->SetBinError(bin,exp.postfitSRMS());
        //helperFuncs::setupHistogramBin(hPOImedians, bin, labels.back(), helperFuncs::getMedian(exp.mu()), exp.postfitSRMS());
        //helperFuncs::setupHistogramBin(hPOIwithFittedError, bin, labels.back(), exp.postfitSMean(), exp.postfitSerror());
        ////hInit->SetBinContent(bin,exp.muInjected());
        //helperFuncs::setupHistogramBin(hInit, bin, labels.back(), exp.muInjected());

        //hists.push_back( exp.mu() );
        //hists.back()->GetXaxis()->SetTitle("#mu");
        //hists.back()->SetTitle("");
        //hists.back()->SetLineColor( exp.color() );
        //helperFuncs::norm(hists.back());
        //// //helperFuncs::setXRange(hists.back(),-3,5);
        //// correlationPlot = exps.at(iE).getCorrelationPlotPostfitB();
        //// if(correlationPlot != NULL){
          //// printCorrelationPlots(correlationPlot, outLabel, "correlationPlot_PostfitB_" + labels.back());
          //// delete correlationPlot;
          //// correlationPlot = NULL;
        //// }
        //// correlationPlot = exps.at(iE).getCorrelationPlotPostfitS();
        //// if(correlationPlot != NULL){
          //// printCorrelationPlots(correlationPlot, outLabel, "correlationPlot_PostfitS_" + labels.back());
          //// delete correlationPlot;
          //// correlationPlot = NULL;
        //// }

    //}
    ////hInit->GetYaxis()->SetRangeUser(-1.1,3.1);

    //std::cout << "comparing mean values for POI\n";
    //compareMeanValues(hPOIs,hInit,labels,outLabel+"POImeans",hPOImedians);
    //createLatexOutput::writePOILatexTable(hPOIs, hPOImedians, hPOIwithFittedError, outLabel+"POI.txt", "Signal Strength Parameters", testName);
    //compareDistributions(hists,labels,outLabel+"POI",false);

    //if(hPOIs != NULL) delete hPOIs;
    //if(hInit != NULL) delete hInit;
    //for(auto& h: hists) {
        //if(h != NULL) delete h;
    //}

//}

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
                    toNorm[nExp][i][0] = helperFuncs::checkValues(postFitMeanVal/preFitMeanVal);
                    //std::cout << "\t\t\tnew mean val = " << toNorm[nExp][i][0] << std::endl;
                    //std::cout << "postFitMeanErr = " << postFitMeanErr << "\tpreFitMeanErr = " << preFitMeanErr << std::endl;
                    toNorm[nExp][i][1] = helperFuncs::checkValues(toNorm[nExp][i][0]*TMath::Sqrt(TMath::Power(postFitMeanErr/postFitMeanVal,2) + TMath::Power(preFitMeanErr/preFitMeanVal,2)));
                    //std::cout << "\t\t\tnew mean err = " << toNorm[nExp][i][1] << std::endl;
                    toNorm[nExp][i][2] = postFitMedian/preFitMedian;
                    //std::cout << "\t\t\tnew median val = " << toNorm[nExp][i][2] << std::endl;

                    toNorm[nExp][i][3] = helperFuncs::checkValues(toNorm[nExp][i][2]*TMath::Sqrt(TMath::Power(postFitRMS/postFitMedian,2) + TMath::Power(preFitRMS/preFitMedian,2)));
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
                //helperFuncs::resizeHisto(prefitShapes->getDist(np));
                postfitSshapes = exp.getPostfitSshapes()[nCategory];
                //helperFuncs::resizeHisto(postfitSshapes->getDist(np));
                postfitBshapes = exp.getPostfitBshapes()[nCategory];
                //helperFuncs::resizeHisto(postfitBshapes->getDist(np));
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

                saveFitValues(bin, hCompareNPMeansList[0], postfitBshapes->getMean(np), postfitBshapes->getMeanError(np), PostfitBvalsAndErrors[iE], hCompareNPMediansList[0], helperFuncs::getMedian(postfitBshapes->getDist(np) ), postfitBshapes->getRMS(np));
                //std::cout << "\tsaving PostfitS of PseudoExperiment with " << labels.back()  << " \n";

                saveFitValues(bin, hCompareNPMeansList[1], postfitSshapes->getMean(np), postfitSshapes->getMeanError(np), PostfitSBvalsAndErrors[iE], hCompareNPMediansList[1], helperFuncs::getMedian(postfitSshapes->getDist(np) ), postfitSshapes->getRMS(np));

                //std::cout << "\tsaving Prefit of PseudoExperiment with " << labels.back()  << " \n";

                saveFitValues(bin, hPrefit, prefitShapes->getMean(np), prefitShapes->getMeanError(np), PrefitValsAndErrors[iE]);
                PrefitValsAndErrors[iE].back().push_back(helperFuncs::getMedian(prefitShapes->getDist(np) ));
                PrefitValsAndErrors[iE].back().push_back(prefitShapes->getRMS(np));

                std::cout << "saving norm distributions...\n";
                histsPrefit.push_back( prefitShapes->getDist(np) );
                helperFuncs::norm( histsPrefit.back() );
                helperFuncs::setLineStyle( histsPrefit.back(), exp.color(), 1 );

                histsPostfitB.push_back( postfitBshapes->getDist(np) );
                helperFuncs::norm( histsPostfitB.back() );
                helperFuncs::setLineStyle( histsPostfitB.back(), exp.color(), 1 );

                histsPostfitS.push_back( postfitSshapes->getDist(np) );
                helperFuncs::norm( histsPostfitS.back() );
                helperFuncs::setLineStyle( histsPostfitS.back(), exp.color(), 1 );
            }
            // plot np distributions
            // std::cout << "\tPostFitB histogram entries:\n";
            // for(int i=1; i<=int(exps.size()); i++) std::cout << "\t\tSaved mean val for " << labels[i-1]  << " in bin " << i << ": " << hCompareNPMeansList[0]->GetBinContent(i) << std::endl;
            // std::cout << "\tPostFitS histogram:\n";
            // for(int i=1; i<=int(exps.size()); i++) std::cout << "\t\tContent for " << labels[i-1]  << " in bin" <<  i << ": " << hCompareNPMeansList[1]->GetBinContent(i) << std::endl;
            // std::cout << "\tPrefit histograms: " << histsPrefit.size() << "\n";
            // for(int i=0; i<int(histsPrefit.size()); i++) std::cout << "\t\tEntries for " << labels[i]  << " in histo" <<  histsPrefit[i]->GetName() << ": " << histsPrefit[i]->GetEntries() << std::endl;
            compareDistributions(histsPrefit,labels,outLabel+categoryName + "_normalisation_"+np+"_Prefit");
            std::cout << "\tPostfitB histograms: " << histsPostfitB.size() << "\n";
            //for(int i=0; i<int(histsPostfitB.size()); i++) std::cout << "\t\tEntries for " << labels[i]  << " in histo" <<  histsPostfitB[i]->GetName() << ": " << histsPostfitB[i]->GetEntries() << std::endl;
            compareDistributions(histsPostfitB,labels,outLabel+categoryName + "_normalisation_"+np+"_PostfitB");
            std::cout << "\t PostfitS histograms: " << histsPostfitS.size() << "\n";
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
              drawPullPlots::drawPullPlots(listOfProcesses, labels[i], PostfitBvalsAndErrors[i], PostfitSBvalsAndErrors[i], PrefitValsAndErrors[i], outLabel+categoryName +"_normalisation_", -1, 3, pathToShapeExpectationRootfile, categoryName);
            }

        }
        delete[] PostfitBvalsAndErrors;
        delete[] PostfitSBvalsAndErrors;
        delete[] PrefitValsAndErrors;
        listOfProcesses.clear();
    }

}

void loadPseudoExperiments(TString pathToPseudoExperiments, TString containsSignalStrength, std::vector<PseudoExperiments>& expSet, Color_t color = kBlue, double injectedMu = -999, const TString suffix = "", const TString sourceFile = "fitDiagnostics.root"){
    double nominalMu=0;
    TString helper;
    std::cout << "in 'loadPseudoExperiments': injectedMu = " << injectedMu << std::endl;
    if(injectedMu != -999) nominalMu = injectedMu;
    else
    {
        helper = containsSignalStrength;
        helper.Remove(0,helper.Index("sig")+3);
        if(helper.Length() > 3) helper.Remove(3, helper.Length());
        nominalMu=helperFuncs::convertTStringToDouble(helper);
    }
    TString finalSuffix = suffix;
    if(!finalSuffix.EqualTo("")) finalSuffix.Append(" ");
    helper.Form("%snominal S=%.1f",suffix.Data(), nominalMu);
    expSet.push_back( PseudoExperiments(helper,nominalMu) );
    expSet.back().addExperiments(pathToPseudoExperiments, sourceFile);

    if((expSet.back().pois().empty()) ){
        std::cout << "Could not load any POIs for experiment " << expSet.back()() << " from directory " << pathToPseudoExperiments.Data() << ", deleting current experiment set...\n";
        expSet.pop_back();
    }
    else
    {
        for(auto& poi : expSet.back().pois())
        {
            std::cout << "obtained number of " << poi << ": " << expSet.back().postfitS(poi)->GetEntries() << std::endl;
        }
        expSet.back().setColor(color);
    }

}

void plotResults(TString pathname, TString pathToShapeExpectationRootfile = "", TString injectedMuString = "-999") {
  TheLooks::set();
  gStyle->SetPaintTextFormat(".2f");

  double injectedMu = injectedMuString.Atof();

  std::vector<PseudoExperiments> expSet;
  if(pathname.EndsWith("/")) pathname.Chop();

  int ncolor=0;
  Color_t colors[5] = {kBlue, kRed, kBlack, kGreen, kOrange};

  // if(!pathToShapeExpectationRootfile.Contains("/")) pathToShapeExpectationRootfile.Form("%s/temp/%s", pathname.Data(), pathToShapeExpectationRootfile.Data());
  // std::cout << "getting expectation from " << pathToShapeExpectationRootfile << std::endl;
  TSystemDirectory dir(pathname.Data(), pathname.Data());
  if(pathname.EndsWith(".root")){
      std::cout << "will use " << pathname << " as input\n";
      // TString filename = pathname(pathname.Last('/')+1, pathname.Length() - pathname.Last('/'));
      
      loadPseudoExperiments(pathname, pathname, expSet, colors[ncolor], injectedMu);
      ncolor++;
      pathname.Remove(pathname.Last('/'), pathname.Length()-pathname.Last('/'));
  }
  else
  {
      
      if(pathname.Contains("PseudoExperiment")){
        loadPseudoExperiments(pathname, pathname, expSet, colors[ncolor], injectedMu);
        ncolor++;
        // loadPseudoExperiments(pathname, pathname, expSet, colors[ncolor], injectedMu, "MDF", "fitDiagnostics_MS_mlfit.root");
        // ncolor++;
      }
      else{
        TList *folders = dir.GetListOfFiles();
        //if folders are found, go through each one an look for the fitDiagnosticsFile
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
              // loadPseudoExperiments(pathname+"/"+folderName, folderName, expSet, colors[ncolor], injectedMu, "MDF", "fitDiagnostics_MS_mlfit.root");
              // ncolor++;
            }
            if (folder->IsDirectory() && folderName.Contains("PseudoExperiment")){
              loadPseudoExperiments(pathname, pathname, expSet, colors[ncolor], injectedMu);
              ncolor++;
              // loadPseudoExperiments(pathname, pathname, expSet, colors[ncolor], injectedMu, "MDF", "fitDiagnostics_MS_mlfit.root");
              // ncolor++;
              break;
            }
          }
        }
      }
  }
  // set inputs
  std::cout << "loaded Experiments: " << expSet.size() << std::endl;
  if(expSet.size() != 0)
  {
      TString outputPath = pathname;
      if(outputPath.EndsWith(".root")) outputPath.Remove(outputPath.Last('/'), outputPath.Length()-outputPath.Last('/'));
      if(outputPath.Contains("/")) outputPath.Remove(0, outputPath.Last('/')+1);
      TString testName = outputPath;
      if(outputPath.Contains(".")) outputPath.ReplaceAll(".", "p");
      outputPath.Prepend(pathname + "/");
      outputPath.Append("_");
    pathname += "/";
    std::cout << "saving experiments in directory " << outputPath << std::endl;
    // comparePOIs(expSet,outputPath, testName);
    comparePostfitValues(expSet,outputPath,true);
    // compareShapes(expSet, outputPath, pathToShapeExpectationRootfile);
    TH2D* correlation;
    TString outputString;
    TString temp;
    for(auto& exp : expSet){
      std::cout << "label " << exp() << std::endl;
      outputString = "";
      for(auto& poi : exp.pois())
      {
          temp.Form("%s %f +- %f +- %f +- %f (%f + %f)\n", poi.Data(), exp.postfitSMean(poi), exp.postfitSMeanError(poi), exp.postfitSRMS(poi), exp.postfitSerror(poi), exp.postfitSerrorLo(poi), exp.postfitSerrorHi(poi));
          outputString += temp;
          std::cout << "\t" << temp.Data() << std::endl;
          std::cout << "\tmedian = " << helperFuncs::getMedian(exp.postfitS(poi)) << std::endl;
          std::cout << "\t#Entries: " << exp.postfitS(poi)->GetEntries() << std::endl;
          
      }
      std::cout << "\tprinting correlation for Bonly fit\n";
      correlation = exp.getCorrelationPlotPostfitB();
      printCorrelationPlots(correlation, outputPath, "correlationPlot_PostfitB_" + exp());
      if(correlation) delete correlation;
      
      std::cout << "\tprinting correlation for S+B fit\n";
      correlation = exp.getCorrelationPlotPostfitS();
      printCorrelationPlots(correlation, outputPath, "correlationPlot_PostfitS_" + exp());
      if(correlation) delete correlation;
      
      TString tablename = "POIs_" + exp();
      if(tablename.Contains(".")) tablename.ReplaceAll(".", "p");
      if(tablename.Contains("=")) tablename.ReplaceAll("=", "");
      while(tablename.Contains("  ")) tablename.ReplaceAll("  ", " ");
      if(tablename.Contains(" ")) tablename.ReplaceAll(" ", "_");
      tablename.Prepend(outputPath);
      tablename.Append(".txt");
      std::cout << "opening " << tablename.Data() << std::endl;
      std::ofstream output(tablename.Data(), std::ofstream::out);
      if(output.is_open()){
          std::cout << "saving pois in " << tablename.Data() << std::endl;
          output << outputString.Data();
          output.close();
      }
    }


  }
  else std::cerr << "was unable to load any Pseudo Experiments!\n";
  // expSet.push_back(PseudoExperiments("test", 1.0));
  // expSet.back().addExperiments(pathname);
  
}


 // # ifndef __CINT__
 // int main(int argc, char *argv[])
 // {
   // plotResults(argv[0], argv[1], argv[2]);
   // return 0;
 // }
// # endif
