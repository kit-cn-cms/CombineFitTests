#include "iomanip"
#include "iostream"
#include "test_bundle.h"
#include "TString.h"
#include "cstdlib"
#include "sstream"
#include "fstream"
#include "Draw_fit.h"
//#include "TChaining.h"

int repeat(TString savesite = "toytest/some*", int runs = 20, int files = 100, float signal = 1)
{
	stringstream s;
	s << fixed << setprecision(1) << signal;
	string s1 = s.str();
	char const * sign = s1.c_str();

	for(int i = 1; i <= runs; ++i)
	{
		TString current = savesite;
		int templ =0;
		int tempi = i;
		do{++templ;
		tempi/=10;} while(tempi);
		char tempc[templ];
		sprintf(tempc,"%d",i);
		current.Replace(current.Index("*",1),1,tempc);

		createfiles(current.Data(), signal, files);

		current.Append("/sig").Append(sign).Append("/converted/combined1.root");

		CreateHist(current.Data());
	}

	TString savedir = TString("/toytest/converted/");
	TString createdir = TString("mkdir ");
	createdir.Append(savedir.Data());
	system(createdir.Data());

	TString working = TString("/nfs/dust/cms/user/firin/bachelor/CombineFitTests/PseudoDataTests/scripts%");

	TString collect = savesite;
	collect.Append("/sig").Append(sign).Append("/converted/combined_histo.root");
	savedir.Append("combined").Append(sign).Append(".root");
	savedir.Prepend(working.Data());
	collect.Prepend(working.Data());
	// The idea here is to include TChaining and collect all the converted files in one last file. There seems to be a problem with having TChaining in the header of repeat and bundle...
	chaining(savedir.Data(),collect.Data());

	return 0;
}
