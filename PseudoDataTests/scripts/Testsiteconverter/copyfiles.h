
#include "iostream"
#include "sstream"
#include "TString.h"

int copy()
{
	for(int i =1; i <= 100; ++i)
	{
		int templ =0;
		int tempi = i;
		do{++templ;
		tempi/=10;}
		while(tempi);
		char tempnr[templ];
		sprintf(tempnr,"%d",i);
		TString origin = TString("/nfs/dust/cms/user/firin/bachelor/CombineFitTests/PseudoDataTests/scripts/toytest12/some") + tempnr + TString("/sig1.0/converted/combined") +tempnr + TString(".root");
		TString dest = TString("/nfs/dust/cms/user/firin/bachelor/CombineFitTests/PseudoDataTests/scripts/toytest12/Values/");
		TString move = "cp " + origin + " " + dest;
		system(move.Data());
	}
	return 0;
}
