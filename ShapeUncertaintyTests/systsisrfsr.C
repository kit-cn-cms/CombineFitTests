#include <cmath>
#include <exception>
#include <fstream>
#include <iostream>
#include <vector>

#include "TCanvas.h"
#include "TF1.h"
#include "TFile.h"
#include "TH1.h"
#include "TH1D.h"
#include "TLegend.h"
#include "TString.h"
#include "TStyle.h"
#include "TUUID.h"


// container to pass info
class Template {
public:
  static TString labelProcess(const TString& str) {
    if( str == "ttbarPlusBBbar" ) return "tt+bb";
    if( str == "ttbarPlusB"     ) return "tt+b";
    if( str == "ttbarPlus2B"    ) return "tt+2b";
    if( str == "ttbarPlusCCbar" ) return "tt+cc";
    if( str == "ttbarOther"     ) return "tt+LF";
    return str;
  }
  static TString labelCategory(const TString& str) {
    if( str == "jge6_tge4" ) return "6j4t";
    if( str == "jge6_t3"   ) return "6j3t";
    if( str == "jge6_t2"   ) return "6j2t";
    if( str == "j5_tge4"   ) return "5j4t";
    if( str == "j5_t3"     ) return "5j3t";
    if( str == "j5_t2"     ) return "5j2t";
    if( str == "j4_t4"     ) return "4j4t";
    if( str == "j4_t3"     ) return "4j3t";
    if( str == "j4_t2"     ) return "4j2t";
    return str;
  }
  static TString labelSyst(const TString& str) {
    if( str.Contains("FSR") ) return "FSR";
    if( str.Contains("ISR") ) return "ISR";
    return str;
  }


  Template()
    : process_(""), category_(""), systematic_(""),
      p1dn_(0.), p1up_(0.), p1Fitdn_(0.), p1Fitup_(0.),
      p2dn_(0.), p2up_(0.), p2Fitdn_(0.), p2Fitup_(0.) {}

  void setProcess(const TString& theProcess) { process_ = theProcess; }
  TString process() const { return process_; }
  void setCategory(const TString& theCategory) { category_ = theCategory; }
  TString category() const { return category_; }
  void setSystematic(const TString& theSystematic) { systematic_ = theSystematic; }
  TString systematic() const { return systematic_; }

  // coeff of linear variation (slope)
  void setp1dn(const double val) { p1dn_ = val; }
  void setp1up(const double val) { p1up_ = val; }
  void setp1Fitdn(const double val) { p1Fitdn_ = val; }
  void setp1Fitup(const double val) { p1Fitup_ = val; }
  double p1dn() const { return p1dn_; }
  double p1up() const { return p1up_; }
  double p1Fitdn() const { return p1Fitdn_; }
  double p1Fitup() const { return p1Fitup_; }

  // coeff of quadratic variation
  void setp2dn(const double val) { p2dn_ = val; }
  void setp2up(const double val) { p2up_ = val; }
  void setp2Fitdn(const double val) { p2Fitdn_ = val; }
  void setp2Fitup(const double val) { p2Fitup_ = val; }
  double p2dn() const { return p2dn_; }
  double p2up() const { return p2up_; }
  double p2Fitdn() const { return p2Fitdn_; }
  double p2Fitup() const { return p2Fitup_; }

  TString labelProcess() const  { return labelProcess( process_ ); }
  TString labelCategory() const { return labelCategory( category_ ); }
  TString labelSyst() const     { return labelSyst( systematic_ ); }

  TString histName() const {
    return process_+"_finaldiscr_ljets_"+category_;
  }
  TString histNameUp() const {
    return histName()+"_"+systematic_+"Up";
  }
  TString histNameDn() const {
    return histName()+"_"+systematic_+"Down";
  }

  
private:
  TString process_;
  TString category_;
  TString systematic_;

  double p1dn_, p1up_;			// coeff of linear variation (slope)
  double p1Fitdn_, p1Fitup_;		// coeff of linear variation (slope)
  double p2dn_, p2up_;			// coeff of quadratic variation
  double p2Fitdn_, p2Fitup_;		// coeff of quadratic variation
};


void setStyle() {
  // Zero horizontal error bars
  gStyle->SetErrorX(0);

  //  For the canvas
  gStyle->SetCanvasBorderMode(0);
  gStyle->SetCanvasColor(kWhite);
  gStyle->SetCanvasDefH(800); //Height of canvas
  gStyle->SetCanvasDefW(800); //Width of canvas
  gStyle->SetCanvasDefX(0);   //Position on screen
  gStyle->SetCanvasDefY(0);
  
  //  For the frame
  gStyle->SetFrameBorderMode(0);
  gStyle->SetFrameBorderSize(10);
  gStyle->SetFrameFillColor(kBlack);
  gStyle->SetFrameFillStyle(0);
  gStyle->SetFrameLineColor(kBlack);
  gStyle->SetFrameLineStyle(0);
  gStyle->SetFrameLineWidth(2);
  gStyle->SetLineWidth(3);
    
  //  For the Pad
  gStyle->SetPadBorderMode(0);
  gStyle->SetPadColor(kWhite);
  gStyle->SetPadGridX(false);
  gStyle->SetPadGridY(false);
  gStyle->SetGridColor(0);
  gStyle->SetGridStyle(3);
  gStyle->SetGridWidth(1);
  
  //  Margins
  gStyle->SetPadTopMargin(0.08);
  gStyle->SetPadBottomMargin(0.15);
  gStyle->SetPadLeftMargin(0.18);
  gStyle->SetPadRightMargin(0.05);

  //  For the histo:
  gStyle->SetHistLineColor(kBlack);
  gStyle->SetHistLineStyle(0);
  gStyle->SetHistLineWidth(2);
  gStyle->SetMarkerSize(1.2);
  gStyle->SetEndErrorSize(4);
  gStyle->SetHatchesLineWidth(1);

  //  For the statistics box:
  gStyle->SetOptStat(0);
  
  //  For the axis
  gStyle->SetAxisColor(1,"XYZ");
  gStyle->SetTickLength(0.03,"XYZ");
  gStyle->SetNdivisions(510,"XYZ");
  gStyle->SetPadTickX(1);
  gStyle->SetPadTickY(1);
  gStyle->SetStripDecimals(kFALSE);
  
  //  For the axis labels and titles
  gStyle->SetTitleColor(1,"XYZ");
  gStyle->SetLabelColor(1,"XYZ");
  gStyle->SetLabelFont(42,"XYZ");
  gStyle->SetLabelOffset(0.007,"XYZ");
  gStyle->SetLabelSize(0.04,"XYZ");
  gStyle->SetTitleFont(42,"XYZ");
  gStyle->SetTitleSize(0.047,"XYZ");
  gStyle->SetTitleXOffset(1.5);
  gStyle->SetTitleYOffset(1.9);

  //  For the legend
  gStyle->SetLegendBorderSize(0);
}


TH1* getTH1(const TString& fileName, const TString& histName) {
  TFile file(fileName,"READ");
  TH1* h = 0;
  file.GetObject(histName,h);
  if( h == 0 ) {
    std::cerr << "\n\nERROR: no TH1 '" << histName << "' in file '" << fileName << "'" << std::endl;
    throw std::exception();
  }
  h->SetDirectory(0);
  file.Close();
  h->UseCurrentStyle();
  TUUID id;
  h->SetName(id.AsString());
    
  return h;
}


double getMax(const TH1* h) {
  double max = -999999.;
  for(int bin = 1; bin <= h->GetNbinsX(); ++bin) {
    if( h->GetBinContent(bin) > max ) max = h->GetBinContent(bin);
  }
  
  return max;
}


double getMax(const std::vector<TH1*>& hists) {
  double max = -999999.;
  for(auto& h: hists) {
    const double tmax = getMax(h);
    if( tmax > max ) max = tmax;
  }
  return max;
}


void setRange(std::vector<TH1*>& hists, const double min, const double max) {
  for(auto& h: hists) {
    h->GetYaxis()->SetRangeUser(min,max);
  }
}


TLegend* createLegend(const unsigned int nLines) {
  const double relWidth = 0.4;
  const double lineHeight = 0.046;
  const double x0 = gStyle->GetPadLeftMargin()+0.03;
  const double x1 = 1.-gStyle->GetPadRightMargin();
  const double y1 = 1.-gStyle->GetPadTopMargin()-0.03;
  const double y0 = y1-nLines*lineHeight;

  TLegend* leg = new TLegend(x0,y0,x1,y1);
  leg->SetBorderSize(0);
  leg->SetFillColor(0);
  leg->SetFillStyle(0);
  leg->SetTextFont(42);
  leg->SetTextSize(0.04);

  return leg;
}


// get ratios to first histogram in vector
std::vector<TH1*> createRatios(const std::vector<TH1*>& hists) {
  std::vector<TH1*> ratios;
  for(size_t i = 1; i < hists.size(); ++i) {
    TUUID id;
    TH1* hRatio = static_cast<TH1*>( hists.at(i)->Clone("ratio_"+TString(id.AsString())) );
    hRatio->Divide(hists.front());
    ratios.push_back( hRatio );
  }

  return ratios;
}



void plot(std::vector<TH1*>& hists, const std::vector<TString>& labels, const bool isRatio, const TString& outName) {
  TCanvas* can = new TCanvas("can","",600,600);
  can->cd();

  if( isRatio ) {
    setRange(hists,0.5,1.8);
  } else {
    setRange(hists,0.,1.5*getMax(hists));
  }

  TH1* hFrame = static_cast<TH1*>( hists.front()->Clone("frame") );
  if( isRatio ) {
    for(int bin = 1; bin <= hFrame->GetNbinsX(); ++bin) {
      hFrame->SetBinContent( bin, 1. );
      hFrame->SetBinError( bin, 0. );
    }
    hFrame->SetLineColor(kBlack);
    hFrame->SetLineStyle(2);
    hFrame->GetYaxis()->SetTitle("ratio to nominal");
  }

  hFrame->Draw("HIST");
  for(std::vector<TH1*>::reverse_iterator it = hists.rbegin();
      it != hists.rend(); ++it) {
    (*it)->Draw("HISTEsame");
  }

  TLegend* leg = createLegend(hists.size());
  for(size_t i = 0; i < hists.size(); ++i) {
    leg->AddEntry( hists.at(i), labels.at(i), "L" );
  }
  leg->Draw("same");

  if( isRatio ) {
    can->SaveAs(outName+"_Ratios.pdf");
  } else {
    can->SaveAs(outName+".pdf");
  }

  delete hFrame;
  delete leg;
  delete can;
}

void plotTemplates(std::vector<TH1*>& hists, const std::vector<TString>& labels, const TString& outName) {
  plot(hists,labels,false,outName);
}

void plotRatios(std::vector<TH1*>& hists, const std::vector<TString>& labels, const TString& outName) {
  plot(hists,labels,true,outName);
}



TH1* skewPol(const TH1* hOrig, const unsigned int order, const double coeff) {
  if( !(order == 1 || order == 2) ) {
    std::cerr << "ERROR in skewPol: order " << order << " not supported" << std::endl;
    throw std::exception();
  }
  
  TUUID id;
  TH1* hSkewed = static_cast<TH1*>( hOrig->Clone(id.AsString()) );

  const double xmin = hOrig->GetBinCenter(1);
  const double xmax = hOrig->GetBinCenter(hOrig->GetNbinsX());
  const double x0 = 0.5*(xmin+xmax);

  if( order == 1 ) {
    // skew linearly
    // coeff is slope
    //const double slopeAbs = (xmax-xmin)*coeff;
    for(int bin = 1; bin <= hOrig->GetNbinsX(); ++bin) {
      const double dx = hOrig->GetBinCenter(bin)-x0;
      //const double sf = 1. + slopeAbs*dx;
      const double sf = 1. + coeff*dx;
      hSkewed->SetBinContent( bin, sf*hOrig->GetBinContent(bin) );
    }

  } else if( order == 2 ) {
    // skew as parabola
    const double dxmax = xmax-x0;
    const double offset = -0.5*coeff*dxmax*dxmax;
    for(int bin = 1; bin <= hOrig->GetNbinsX(); ++bin) {
      const double dx = hOrig->GetBinCenter(bin)-x0;
      const double sf = 1 + offset + coeff*dx*dx;
      hSkewed->SetBinContent( bin, sf*hOrig->GetBinContent(bin) );
    }
    hSkewed->Scale( hOrig->Integral() / hSkewed->Integral() );
  }

  return hSkewed;
}



double findSkewingCoeffWithSameKS(const TH1* hNom, const double ks, const unsigned int order, int sign) {
  sign /= std::abs(sign);
  //  std::cout << "\n\nKS(obs) = " << ks << "  sign = " << sign << std::endl;
  
  const double coeffmax = 0.5;
  const double stepsize = 0.005;
  double coeff = 0.;
  while( std::abs(coeff) < std::abs(coeffmax) ) {
    coeff += sign*stepsize;
    TH1* hSkewed = skewPol(hNom,order,coeff);
    const double thisks = hNom->KolmogorovTest(hSkewed);
    //    std::cout << "  slope = " << slope << " --> KS = " << thisks << std::endl;    
    if( thisks < ks ) break;
  }

  return coeff;
}


double fitPol(const TH1* hRatio, const unsigned int order) {
  if( !(order == 1 || order == 2) ) {
    std::cerr << "ERROR in fitPol: order " << order << " not supported" << std::endl;
    throw std::exception();
  }

  double coeff = 0.;
  
  const double xmin = hRatio->GetBinCenter(1);
  const double xmax = hRatio->GetBinCenter(hRatio->GetNbinsX());
  const double x0 = 0.5*(xmin+xmax);
  
  TH1* hFit = static_cast<TH1*>( hRatio->Clone("forfit") );

  if( order == 1 ) {
    TF1* fit = new TF1("fit","1 + [1]*(x-[0])");
    fit->FixParameter(0,x0);
    fit->SetParameter(1,0.);
    hFit->Fit(fit,"0QNB");
    coeff = fit->GetParameter(1);
    delete fit;

  } else if( order == 2 ) {
    const double dxmax = xmax-x0;
    TF1* fit = new TF1("fit","1 - [2]*( (x-[0])*(x-[0]) - 0.5*[1]*[1] )");
    fit->FixParameter(0,x0);
    fit->FixParameter(1,dxmax);
    fit->SetParameter(2,0.);
    hFit->Fit(fit,"0QNB");
    coeff = fit->GetParameter(2);
    delete fit;
  }

  delete hFit;

  return coeff;
}



void writeToNewInputs(const TH1* hDn, const TH1* hUp, const TString& fileName, const Template& temp, const TString& id) {
  TString name = temp.histNameDn();
  name.ReplaceAll("Down",id+"Down");
  TH1* hTmpDn = static_cast<TH1*>( hDn->Clone( name ) );

  name = temp.histNameUp();
  name.ReplaceAll("Up",id+"Up");
  TH1* hTmpUp = static_cast<TH1*>( hUp->Clone( name ) );

  TFile file(fileName,"UPDATE");
  file.WriteTObject(hTmpDn);
  file.WriteTObject(hTmpUp);
  file.Close();

  delete hTmpDn;
  delete hTmpUp;
}


void analyzeTemplate(const TString& fileName, Template& temp, const TString& newInputsFileName) {
  // variations order:
  const std::vector<TString> vars { "dn", "up" };
  
  // get templates
  TH1* hTempNom = getTH1(fileName,temp.histName());
  TH1* hTempDn = getTH1(fileName,temp.histNameDn());
  TH1* hTempUp = getTH1(fileName,temp.histNameUp());

  // get norms
  const double normNom = hTempNom->Integral();
  const std::vector<double> normVar {
    hTempDn->Integral(),
    hTempUp->Integral()
  };
  
  // run Kolmogorov tests
  const std::vector<double> ksObs {
    hTempNom->KolmogorovTest( hTempDn ),
    hTempNom->KolmogorovTest( hTempUp )
  };
  const std::vector<double> chi2Obs {
    hTempNom->Chi2Test( hTempDn, "WW" ),
    hTempNom->Chi2Test( hTempUp, "WW" )
  };

  
  // store as vectors
  std::vector<TH1*> hTemps { hTempNom, hTempDn, hTempUp };
  std::vector<TString> labelsTemps;
  char entry[100];
  sprintf( entry, "nominal:  N=%.1f", normNom );
  labelsTemps.push_back( entry );
  for(size_t i = 0; i < 2; ++i) {
    sprintf( entry, "%s %s:  N=%.1f  KS=%.2f  #chi^{2}=%.2f", temp.labelSyst().Data(), vars.at(i).Data(), normVar.at(i), ksObs.at(i), chi2Obs.at(i) );
    labelsTemps.push_back( entry );
  }

  // set styles
  for(auto& h: hTemps) {
    h->SetTitle( temp.labelSyst()+": "+temp.labelProcess()+" "+temp.labelCategory() );
    h->SetLineWidth(2);
    h->GetXaxis()->SetTitle("BDT output");
    h->GetYaxis()->SetTitle("N events");
  }
  hTempNom->SetLineColor(kBlack);
  hTempDn->SetLineColor(kRed);
  hTempUp->SetLineColor(kBlue);


  // get ratios of variations to nominal
  std::vector<TH1*> hRatios = createRatios( hTemps );
  std::vector<TString> labelsRatios { temp.labelSyst()+" dn", temp.labelSyst()+" up" };
    
  // plot templates and ratios
  const TString outName = temp.systematic() + "_" + temp.process() + "_" + temp.category();
  plotTemplates(hTemps,labelsTemps,outName);
  plotRatios(hRatios,labelsRatios,outName);


  // produce templates normalized to nominal
  TH1* hTempNormDn = static_cast<TH1*>( hTempDn->Clone("temp_dn") );
  hTempNormDn->Scale( normNom / normVar.at(0) );
  TH1* hTempNormUp = static_cast<TH1*>( hTempUp->Clone("temp_up") );
  hTempNormUp->Scale( normNom / normVar.at(1) );

  // run Kolmogorov tests
  const std::vector<double> ksNorm {
    hTempNom->KolmogorovTest( hTempNormDn ),
    hTempNom->KolmogorovTest( hTempNormUp )
  };
  const std::vector<double> chi2Norm {
    hTempNom->Chi2Test( hTempNormDn, "WW" ),
    hTempNom->Chi2Test( hTempNormUp, "WW" )
  };
  
  // store as vectors
  std::vector<TH1*> hTempsNorm { hTempNom, hTempNormDn, hTempNormUp };
  std::vector<TString> labelsTempsNorm;
  sprintf( entry, "nominal" );
  labelsTempsNorm.push_back( entry );
  for(size_t i = 0; i < 2; ++i) {
    sprintf( entry, "%s (norm) dn:  KS=%.2f  #chi^{2}=%.2f", temp.labelSyst().Data(), ksNorm.at(i), chi2Norm.at(i) );
    labelsTempsNorm.push_back( entry );
  }

  // set styles
  for(auto& h: hTemps) {
    h->GetYaxis()->SetTitle("N events (normalized to nominal)");
  }

  std::vector<TH1*> hRatiosNorm = createRatios( hTempsNorm );
  std::vector<TString> labelsRatiosNorm { temp.labelSyst()+" (norm) dn", temp.labelSyst()+" (norm) up" };
    
  // plot templates and ratios of normalized templates
  const TString outNameNorm = temp.systematic() + "_" + temp.process() + "_" + temp.category()+ "_normalized";
  plotTemplates(hTempsNorm,labelsTempsNorm,outNameNorm);
  plotRatios(hRatiosNorm,labelsRatiosNorm,outNameNorm);

  // write normalized varied templates to new inputs file
  // (uncertainty will have 'NormalizedToNominal' in name)
  writeToNewInputs(hTempsNorm.at(1),hTempsNorm.at(2),newInputsFileName,temp,"NormalizedToNominal");

  

  ///////////// LINEAR SHAPE VARIATION //////////////////////////////////////////////

  // find slope with same KS
  std::vector<TH1*> histsSkewedPol1 { hTempNom };
  std::vector<TString> labelsSkewedPol1 { "nominal" };
  std::vector<TH1*> histsSkewedPol1Ratio;
  std::vector<TString> labelsSkewedPol1Ratio;

  // slope for up/down variation depending on mean of original templates:
  // variation with larger mean gets positive slope
  std::vector<int> signPol1 { -1, +1 };
  if( hTempDn->GetMean() < hTempUp->GetMean() ) {
    signPol1.at(0) = -1;
    signPol1.at(1) = +1;
  } else {
    signPol1.at(0) = +1;
    signPol1.at(1) = -1;
  }

  for(unsigned int i = 0; i < 2; ++i) { // loop over down/up variations
    const double slope = findSkewingCoeffWithSameKS(hTempNom,ksObs.at(i),1,signPol1.at(i));
    i==0 ? temp.setp1dn( slope ) : temp.setp1up( slope );

    TH1* hSkew = skewPol(hTempNom,1,slope);
    const int color = i==0 ? kRed : kBlue;
    hSkew->SetLineColor(color);
    histsSkewedPol1.push_back(hSkew);

    const double ks   = hTempNom->KolmogorovTest(hSkew);
    const double chi2 = hTempNom->Chi2Test(hSkew,"WW"); // weighted-weighted correct comparison? https://root.cern.ch/root/html528/TH1.html#TH1:Chi2Test

    char kstxt[100];
    if( i==0 ) {		// down
      sprintf(kstxt,"dn: p_{1} %.2f  KS=%.2f  #chi^{2}=%.2f",slope,ks,chi2);
    } else {			// up
      sprintf(kstxt,"up: p_{1} %.2f  KS=%.2f  #chi^{2}=%.2f",slope,ks,chi2);
    }
    labelsSkewedPol1.push_back(kstxt);

    TUUID id;
    TH1* hRatio = static_cast<TH1*>( hSkew->Clone(id.AsString()) );
    hRatio->Divide( hTempNom );
    histsSkewedPol1Ratio.push_back( hRatio );

    if( i==0 ) {		// down
      sprintf(kstxt,"dn: p_{1} %.2f",slope);
    } else {			// up
      sprintf(kstxt,"up: p_{1} %.2f",slope);
    }
    labelsSkewedPol1Ratio.push_back(kstxt);
  }

  // for comparison: fit pol1 to ratio
  for(size_t i = 0; i < hRatiosNorm.size(); ++i) {
    const double slope = fitPol(hRatiosNorm.at(i),1);
    i==0 ? temp.setp1Fitdn( slope ) : temp.setp1Fitup( slope );
    
    TH1* hFit = skewPol(hTempNom,1,slope);
    hFit->SetLineColor( hRatiosNorm.at(i)->GetLineColor() );
    hFit->SetLineStyle(2);
    histsSkewedPol1.push_back( hFit );
    if( i == 0 ) {
      labelsSkewedPol1.push_back("linear fit to dn");
    } else {
      labelsSkewedPol1.push_back("linear fit to up");
    }

    TUUID id;
    TH1* hRatio = static_cast<TH1*>( hFit->Clone(id.AsString()) );
    hRatio->Divide( hTempNom );
    histsSkewedPol1Ratio.push_back( hRatio );
    if( i == 0 ) {
      labelsSkewedPol1Ratio.push_back("linear fit to dn");
    } else {
      labelsSkewedPol1Ratio.push_back("linear fit to up");
    }
  }  
  
  const TString outNameSkewedPol1 = temp.systematic() + "_" + temp.process() + "_" + temp.category() + "_skewedPol1";
  plotTemplates(histsSkewedPol1,labelsSkewedPol1,outNameSkewedPol1);
  plotRatios(histsSkewedPol1Ratio,labelsSkewedPol1Ratio,outNameSkewedPol1);

  // write skewed templates to new inputs file
  // (uncertainty will have 'Linear' in name)
  writeToNewInputs(histsSkewedPol1.at(1),histsSkewedPol1.at(2),newInputsFileName,temp,"Linear");



  
  ///////////// QUADRATIC SHAPE VARIATION //////////////////////////////////////////////

  // find slope with same KS
  std::vector<TH1*> histsSkewedPol2 { hTempNom };
  std::vector<TString> labelsSkewedPol2 { "nominal" };
  std::vector<TH1*> histsSkewedPol2Ratio;
  std::vector<TString> labelsSkewedPol2Ratio;

  // orientation of parabola for up/down variation depending on behaviour of original
  // templates in central BDT region: normalized original template with larger bin content
  // will get up-side-down parabola
  double bcCentralDn = 0.;
  double bcCentralUp = 0.;
  const int binStart = hTempNom->GetNbinsX()/4 + 1;
  const int binEnd   = hTempNom->GetNbinsX() + 1 - binStart;
  //  std::cout << hTempNom->GetNbinsX() << ": " << binStart << ": " << binEnd << std::endl;
  for(int bin = binStart; bin <= binEnd; ++bin) {
    bcCentralDn += hTempNormDn->GetBinContent(bin);
    bcCentralUp += hTempNormUp->GetBinContent(bin);
  }
  std::vector<int> signPol2 { -1, +1 };
  if( bcCentralDn < bcCentralUp ) {
    signPol2.at(0) = +1;
    signPol2.at(1) = -1;
  } else {
    signPol2.at(0) = -1;
    signPol2.at(1) = +1;
  }

  for(unsigned int i = 0; i < 2; ++i) { // loop over down/up variations
    const double coeff = findSkewingCoeffWithSameKS(hTempNom,ksObs.at(i),2,signPol2.at(i));
    i==0 ? temp.setp2dn( coeff ) : temp.setp2up( coeff );

    TH1* hSkew = skewPol(hTempNom,2,coeff);
    const int color = i==0 ? kRed : kBlue;
    hSkew->SetLineColor(color);
    histsSkewedPol2.push_back(hSkew);

    const double ks = hTempNom->KolmogorovTest(hSkew);

    char kstxt[100];
    if( i==0 ) {		// down
      sprintf(kstxt,"dn: p_{2} %.2f:  KS=%.2f",coeff,ks);
    } else {			// up
      sprintf(kstxt,"up: p_{2} %.2f:  KS=%.2f",coeff,ks);
    }
    labelsSkewedPol2.push_back(kstxt);

    TUUID id;
    TH1* hRatio = static_cast<TH1*>( hSkew->Clone(id.AsString()) );
    hRatio->Divide( histsSkewedPol2.front() );
    histsSkewedPol2Ratio.push_back( hRatio );

    if( i==0 ) {		// down
      sprintf(kstxt,"dn: p_{2} %.2f",coeff);
    } else {			// up
      sprintf(kstxt,"up: p_{2} %.2f",coeff);
    }
    labelsSkewedPol2Ratio.push_back(kstxt);
  }

  // for comparison: fit pol2 to ratio
  for(size_t i = 0; i < hRatiosNorm.size(); ++i) {
    const double coeff = fitPol(hRatiosNorm.at(i),2);
    i==0 ? temp.setp2Fitdn( coeff ) : temp.setp2Fitup( coeff );
    
    TH1* hFit = skewPol(hTempNom,2,coeff);
    hFit->SetLineColor( hRatiosNorm.at(i)->GetLineColor() );
    hFit->SetLineStyle(2);
    histsSkewedPol2.push_back( hFit );
    if( i == 0 ) {
      labelsSkewedPol2.push_back("quadratic fit to dn");
    } else {
      labelsSkewedPol2.push_back("quadratic fit to up");
    }

    TUUID id;
    TH1* hRatio = static_cast<TH1*>( hFit->Clone(id.AsString()) );
    hRatio->Divide( hTempNom );
    histsSkewedPol2Ratio.push_back( hRatio );
    if( i == 0 ) {
      labelsSkewedPol2Ratio.push_back("quadratic fit to dn");
    } else {
      labelsSkewedPol2Ratio.push_back("quadratic fit to up");
    }
  }
  
  const TString outNameSkewedPol2 = temp.systematic() + "_" + temp.process() + "_" + temp.category() + "_skewedPol2";
  plotTemplates(histsSkewedPol2,labelsSkewedPol2,outNameSkewedPol2);
  plotRatios(histsSkewedPol2Ratio,labelsSkewedPol2Ratio,outNameSkewedPol2);
  
  // write skewed templates to new inputs file
  // (uncertainty will have 'Quadratic' in name)
  writeToNewInputs(histsSkewedPol2.at(1),histsSkewedPol2.at(2),newInputsFileName,temp,"Quadratic");


  
  // clean up
  for(auto& h: hTemps) {
    delete h;
  }
  for(auto& h: hRatios) {
    delete h;
  }
  for(size_t i = 1; i < hTempsNorm.size(); ++i) {
    delete hTempsNorm.at(i);
  }
  for(auto& h: hRatiosNorm) {
    delete h;
  }
  for(size_t i = 1; i < histsSkewedPol1.size(); ++i) {
    delete histsSkewedPol1.at(i);
  }
  for(auto& h: histsSkewedPol1Ratio) {
    delete h;
  }
  for(size_t i = 1; i < histsSkewedPol2.size(); ++i) {
    delete histsSkewedPol2.at(i);
  }
  for(auto& h: histsSkewedPol2Ratio) {
    delete h;
  }
}



void analyzeTemplates(const TString& fileName, std::vector<Template>& templates, const bool addNewTemplatesToOriginalInput=false) {

  TString newInputsFileName = fileName;
  if( !addNewTemplatesToOriginalInput ) {
    // output templates written to new file:
    newInputsFileName = "templates.root";
    TFile file(newInputsFileName,"RECREATE");
    file.Close();
  }
  for( auto& temp: templates ) {
    analyzeTemplate(fileName,temp,newInputsFileName);
  }
}


TString getTableOfCoefficients(const std::vector<Template>& templates, const TString& syst, const std::vector<TString>& cats, const std::vector<TString>& procs, const int order) {
  TString table = "";

  bool hasSyst = false;
  for(auto& temp: templates) {
    if( temp.systematic() == syst ) {
      hasSyst = true;
      break;
    }
  }
  if( !hasSyst ) return table;


  // header
  TString nCols = "";
  nCols += procs.size()+1;
  table += "\\begin{tabular}{*{" + nCols + "}{c}}\n";
  table += "\\toprule\n";
  for(size_t ip = 0; ip < procs.size(); ++ip) {
    table += " & " + Template::labelProcess( procs.at(ip) );
  }
  table += "  \\\\ \n";
  table += "\\midrule\n";

  // body    
  for(size_t ic = 0; ic < cats.size(); ++ic) {
    table += Template::labelCategory( cats.at(ic) );
    for(size_t ip = 0; ip < procs.size(); ++ip) {
      table += " & ";
      for(auto& temp: templates) {
	if( temp.systematic() == syst &&
	    temp.category() == cats.at(ic) &&
	    temp.process() == procs.at(ip) ) {
	  if( order == 1 ) {
	    table += ( temp.p1dn()<0 ? "-" : "+" );
	    table += "/";
	    table += ( temp.p1up()<0 ? "-" : "+" );
	  } else if( order == 2 ) {
	    table += ( temp.p2dn()<0 ? "-" : "+" );
	    table += "/";
	    table += ( temp.p2up()<0 ? "-" : "+" );
	  }
	  break;
	}
      }
    } // end of loop over processes
    table += "  \\\\ \n";
  }   // end of loop over categories

  table += "\\bottomrule\n";
  table += "\\end{tabular}";

  return table;
}


void writeBeamerSlides(const std::vector<Template>& templates, const TString& outName, const std::vector<TString>& cats, const std::vector<TString>& procs) {
  ofstream out(outName.Data());
  if( !out.is_open() ) {
    std::cerr << "ERROR writing to text file '" << outName << "'" << std::endl;
    throw std::exception();
  }
  
  out << "\\documentclass{beamer}" << std::endl;
  out << "\\usepackage{overpic}" << std::endl;
  out << "\\usepackage{booktabs}" << std::endl;
  out << "\\begin{document}" << std::endl;

  for(auto& temp: templates) {
    const TString plotName = temp.systematic() + "_" + temp.process() + "_" + temp.category();
    out << "\n\n\% --------------------------------------------------" << std::endl;
    out << "\\begin{frame}[t]" << std::endl;
    TString frametitle = temp.labelSyst() + ": " + temp.labelProcess() + " " + temp.labelCategory();
    frametitle.ReplaceAll("_","\\_");
    out << "  \\frametitle{" << frametitle << "}" << std::endl;
    out << "  \\vskip-8pt" << std::endl;
    out << "  \\begin{columns}[T]" << std::endl;
    out << "    \\begin{column}{0.25\\textwidth}" << std::endl;
    out << "      \\centering{\\footnotesize original}" << std::endl;
    out << "      \\begin{overpic}[width=\\textwidth]{" << plotName << ".pdf}" << std::endl;
    out << "      \\end{overpic}" << std::endl;
    out << "      \\begin{overpic}[width=\\textwidth]{" << plotName << "_Ratios.pdf}" << std::endl;
    out << "      \\end{overpic}" << std::endl;
    out << "    \\end{column}" << std::endl;

    out << "    \\begin{column}{0.25\\textwidth}" << std::endl;
    out << "      \\centering{\\footnotesize normalized}" << std::endl;
    out << "      \\begin{overpic}[width=\\textwidth]{" << plotName << "_normalized.pdf}" << std::endl;
    out << "      \\end{overpic}" << std::endl;
    out << "      \\begin{overpic}[width=\\textwidth]{" << plotName << "_normalized_Ratios.pdf}" << std::endl;
    out << "      \\end{overpic}" << std::endl;
    out << "    \\end{column}" << std::endl;

    out << "    \\begin{column}{0.25\\textwidth}" << std::endl;
    out << "      \\centering{\\footnotesize linear}" << std::endl;
    out << "      \\begin{overpic}[width=\\textwidth]{" << plotName << "_skewedPol1.pdf}" << std::endl;
    out << "      \\end{overpic}" << std::endl;
    out << "      \\begin{overpic}[width=\\textwidth]{" << plotName << "_skewedPol1_Ratios.pdf}" << std::endl;
    out << "      \\end{overpic}" << std::endl;
    out << "    \\end{column}" << std::endl;

    out << "    \\begin{column}{0.25\\textwidth}" << std::endl;
    out << "      \\centering{\\footnotesize quadratic}" << std::endl;
    out << "      \\begin{overpic}[width=\\textwidth]{" << plotName << "_skewedPol2.pdf}" << std::endl;
    out << "      \\end{overpic}" << std::endl;
    out << "      \\begin{overpic}[width=\\textwidth]{" << plotName << "_skewedPol2_Ratios.pdf}" << std::endl;
    out << "      \\end{overpic}" << std::endl;
    out << "    \\end{column}" << std::endl;

    out << "  \\end{columns}" << std::endl;
    out << "\\end{frame}" << std::endl;
  }


  const std::vector<TString> systs { "CMS_ttH_FSR", "CMS_ttH_ISR" };
  for(auto& syst: systs) {
    out << "\n\n\% --------------------------------------------------" << std::endl;
    out << "\\begin{frame}[t]" << std::endl;
    TString frametitle = "Coefficients "+Template::labelSyst( syst )+": Linear";
    frametitle.ReplaceAll("_","\\_");
    out << "  \\frametitle{" << frametitle << "}" << std::endl;
    out << "  \\vskip-5pt" << std::endl;
    out << "  \\begin{center}" << std::endl;
    out << getTableOfCoefficients(templates,syst,cats,procs,1) << std::endl;
    out << "  \\end{center}" << std::endl;
    out << "\\end{frame}" << std::endl;

    out << "\n\% --------------------------------------------------" << std::endl;
    out << "\\begin{frame}[t]" << std::endl;
    frametitle = "Coefficients "+Template::labelSyst( syst )+": Quadratic";
    frametitle.ReplaceAll("_","\\_");
    out << "  \\frametitle{" << frametitle << "}" << std::endl;
    out << "  \\vskip-5pt" << std::endl;
    out << "  \\begin{center}" << std::endl;
    out << getTableOfCoefficients(templates,syst,cats,procs,2) << std::endl;
    out << "  \\end{center}" << std::endl;
    out << "\\end{frame}" << std::endl;
  }
  
  out << "\\end{document}" << std::endl;

  out.close();
}


void systsisrfsr() {
  setStyle();
  
  const TString fileName = "limits_JTBDT_Spring17v11_limitInput_withSkewedTemplates.root";

  const std::vector<TString> systs { "CMS_ttH_FSR", "CMS_ttH_ISR" };
  const std::vector<TString> procs { "ttbarPlusBBbar", "ttbarPlusB", "ttbarPlus2B", "ttbarPlusCCbar", "ttbarOther" };
  //  const std::vector<TString> cats { "j4_t3", "j4_t4", "j5_t3", "j5_tge4", "jge6_t3", "jge6_tge4" };
  const std::vector<TString> cats { "j4_t2", "j4_t3", "j4_t4", "j5_t2", "j5_t3", "j5_tge4", "jge6_t2", "jge6_t3", "jge6_tge4" };

  std::vector<Template> templates;
  for( auto& syst: systs ) {
    for( auto& proc: procs ) {
      for( auto& cat: cats ) {
	Template temp;
	temp.setSystematic( syst );
	temp.setProcess(    proc );
	temp.setCategory(   cat  );
	templates.push_back(temp);
      }
    }
  }

  const bool addNewTemplatesToOriginalInput = true;
  analyzeTemplates(fileName,templates,addNewTemplatesToOriginalInput);
  writeBeamerSlides(templates,"slides.tex",cats,procs);
}
