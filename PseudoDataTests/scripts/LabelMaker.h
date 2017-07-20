#ifndef LABELMAKER_H
#define LABELMAKER_H

#include "TColor.h"
#include "TLegend.h"
#include "TPaveText.h"
#include "TString.h"

#include "TheLooks.h"

class LabelMaker {
public:
  // text as specified
  static TPaveText* title(const TString& title);


  // Returns a TPaveText object that fits into the top-right corner
  // of the current pad and that can be used for additional labels.
  // Its width, relative to the pad size (without margins), can be
  // specified. Its height is optimized for nEntries entries.
  static TPaveText* label(const int nEntries, const double relWidth=0.5) {
    return labelTR(nEntries,relWidth);
  }

  static TPaveText* label(TString position, const int nEntries, const double relWidth=0.5);

  // Same but explicitly state position on pad
  static TPaveText* labelTL(const int nEntries, const double relWidth=0.5) {
    return label(nEntries,relWidth,true,true);
  }
  static TPaveText* labelTR(const int nEntries, const double relWidth=0.5) {
    return label(nEntries,relWidth,false,true);
  }
  static TPaveText* labelBL(const int nEntries, const double relWidth=0.5) {
    return label(nEntries,relWidth,true,false);
  }
  static TPaveText* labelBR(const int nEntries, const double relWidth=0.5) {
    return label(nEntries,relWidth,false,false);
  }


  // Returns a TLeged object that fits into the top-right corner
  // of the current pad and that can be used for additional labels.
  // Its width, relative to the pad size (without margins), can be
  // specified. Its height is optimized for nEntries entries.
  static TLegend* legend(const int nEntries, const double relWidth=0.5) {
    return legendTR(nEntries,relWidth);
  }

  static TLegend* legend(TString position, const int nEntries, const double relWidth=0.5);

  // Same but explicitly state position on pad
  static TLegend* legendTL(const int nEntries, const double relWidth=0.5) {
    return legend(nEntries,relWidth,true,true);
  }
  static TLegend* legendTR(const int nEntries, const double relWidth=0.5) {
    return legend(nEntries,relWidth,false,true);
  }
  static TLegend* legendBL(const int nEntries, const double relWidth=0.5) {
    return legend(nEntries,relWidth,true,false);
  }
  static TLegend* legendBR(const int nEntries, const double relWidth=0.5) {
    return legend(nEntries,relWidth,false,false);
  }


private:
  // NDC coordinates for TPave, TLegend,...
  static void setXCoordinatesL(const double relWidth, double& x0, double& x1);
  static void setXCoordinatesR(const double relWidth, double& x0, double& x1);
  static void setYCoordinatesT(const int nEntries, double& y0, double& y1);
  static void setYCoordinatesB(const int nEntries, double& y0, double& y1);

  static TPaveText* label(const int nEntries, const double relWidth, const bool leftt, const bool top);
  static TLegend* legend(const int nEntries, const double relWidth, const bool leftt, const bool top);
  static TPaveText* tPaveText(const double x0, const double y0, const double x1, const double y1, const double textSize);
};


// --------------------------------------------------------------
TPaveText* LabelMaker::title(const TString &title) {
  double x0 = gStyle->GetPadLeftMargin();
  double x1 = 1.-gStyle->GetPadRightMargin();
  double y0 = 1.-gStyle->GetPadTopMargin()+0.01;
  double y1 = 1.;
  TPaveText *header = LabelMaker::tPaveText(x0,y0,x1,y1,0.044);
  header->AddText(title);
  
  return header;
}


// --------------------------------------------------------------
TPaveText* LabelMaker::label(TString position, const int nEntries, const double relWidth) {
  position.ToLower();
  if( !( position.Contains("top") || position.Contains("bottom") ) )
    position += "top";
  if( !( position.Contains("left") || position.Contains("right") ) )
    position += "right";
  TPaveText* label = 0;
  if(        position.Contains("top")    && position.Contains("right") ) {
    label = labelTR(nEntries,relWidth);
  } else if( position.Contains("top")    && position.Contains("left")  ) {
    label = labelTL(nEntries,relWidth);
  } else if( position.Contains("bottom") && position.Contains("right") ) {
    label = labelBR(nEntries,relWidth);
  } else if( position.Contains("bottom") && position.Contains("left")  ) {
    label = labelBL(nEntries,relWidth);
  } else {
    label = labelTR(nEntries,relWidth);
  }
  
  return label;
}


// --------------------------------------------------------------
TLegend* LabelMaker::legend(TString position, const int nEntries, const double relWidth) {
  position.ToLower();
  if( !( position.Contains("top") || position.Contains("bottom") ) )
    position += "top";
  if( !( position.Contains("left") || position.Contains("right") ) )
    position += "right";
  TLegend* legend = 0;
  if(        position.Contains("top")    && position.Contains("right") ) {
    legend = legendTR(nEntries,relWidth);
  } else if( position.Contains("top")    && position.Contains("left")  ) {
    legend = legendTL(nEntries,relWidth);
  } else if( position.Contains("bottom") && position.Contains("right") ) {
    legend = legendBR(nEntries,relWidth);
  } else if( position.Contains("bottom") && position.Contains("left")  ) {
    legend = legendBL(nEntries,relWidth);
  } else {
    legend = legendTR(nEntries,relWidth);
  }
  
  return legend;
}


// --------------------------------------------------------------
void LabelMaker::setXCoordinatesL(const double relWidth, double& x0, double& x1) {
  x0 = gStyle->GetPadLeftMargin()+TheLooks::margin();
  x1 = x0 + relWidth*(1.-gStyle->GetPadLeftMargin()-gStyle->GetPadRightMargin()-2.*TheLooks::margin());
}


// --------------------------------------------------------------
void LabelMaker::setXCoordinatesR(const double relWidth, double& x0, double& x1) {
  x0 = 1.-gStyle->GetPadRightMargin()-TheLooks::margin()-relWidth*(1.-gStyle->GetPadLeftMargin()-gStyle->GetPadRightMargin()-2.*TheLooks::margin());
  x1 = 1.-gStyle->GetPadRightMargin()-TheLooks::margin();
}


// --------------------------------------------------------------
void LabelMaker::setYCoordinatesT(const int nEntries, double& y0, double& y1) {
  y1 = 1.-gStyle->GetPadTopMargin()-TheLooks::margin();
  y0 = y1-nEntries*TheLooks::lineHeight();
}


// --------------------------------------------------------------
void LabelMaker::setYCoordinatesB(const int nEntries, double& y0, double& y1) {
  y1 = gStyle->GetPadBottomMargin()+TheLooks::margin();
  y0 = y1+nEntries*TheLooks::lineHeight();
}


// --------------------------------------------------------------
TPaveText* LabelMaker::label(const int nEntries, const double relWidth, const bool left, const bool top) {
  double x0 = 0.;
  double x1 = 0.;
  double y0 = 0.;
  double y1 = 0.;
  if( left ) setXCoordinatesL(relWidth,x0,x1);
  else       setXCoordinatesR(relWidth,x0,x1);
  if( top  ) setYCoordinatesT(nEntries,y0,y1);
  else       setYCoordinatesB(nEntries,y0,y1);

  TPaveText* label = new TPaveText(x0,y0,x1,y1,"NDC");
  label->SetBorderSize(0);
  label->SetFillColor(0);
  label->SetFillStyle(0);
  label->SetTextFont(42);
  label->SetTextAlign(12);	// left adjusted and vertically centered
  label->SetTextSize(TheLooks::textSize());
  label->SetMargin(0.);

  return label;
}


// --------------------------------------------------------------
TLegend* LabelMaker::legend(const int nEntries, const double relWidth, const bool left, const bool top) {
  double x0 = 0.;
  double x1 = 0.;
  double y0 = 0.;
  double y1 = 0.;
  if( left ) setXCoordinatesL(relWidth,x0,x1);
  else       setXCoordinatesR(relWidth,x0,x1);
  if( top  ) setYCoordinatesT(nEntries,y0,y1);
  else       setYCoordinatesB(nEntries,y0,y1);

  TLegend* leg = new TLegend(x0,y0,x1,y1);
  leg->SetBorderSize(0);
  leg->SetFillColor(0);
  leg->SetFillStyle(0);
  leg->SetTextFont(42);
  leg->SetTextSize(TheLooks::textSize());

  return leg;
}


// --------------------------------------------------------------
TPaveText* LabelMaker::tPaveText(const double x0, const double y0, const double x1, const double y1, const double textSize) {
  TPaveText *txt = new TPaveText(x0,y0,x1,y1,"NDC");
  txt->SetBorderSize(0);
  txt->SetFillColor(0);
  txt->SetTextFont(42);
  txt->SetTextAlign(12);	// left adjusted and vertically centered
  txt->SetTextSize(textSize);
  txt->SetMargin(0.);
  
  return txt;
}  


#endif
