#include "iomanip"
#include "iostream"
#include "TChaining.h"
#include "convertToTree.h"
#include "TString.h"
#include "cstdlib"
#include "fstream"
#include "sstream"
#include "iomanip"
#include "dirent.h"
#include "ctime"

int createfiles(TString output = "toytest/bla_some", float signal = 1, int files = 100, int savenr=1)
{
	// Turning floar into char
	stringstream s;
	s << fixed << setprecision(1) << signal;
	string s1 = s.str();
	char const *signalchar = s1.c_str();

	// Turning int into char
	int templ = 0;
	int tempi = savenr;
	do{++templ;
	tempi/=10;}while(tempi);
	char tempc[templ];
	sprintf(tempc,"%d",savenr);
	

        // NEEDS FIXING ON OTHER MACHINES!!!
	TString directory = TString("/nfs/dust/cms/user/firin/bachelor/CombineFitTests/PseudoDataTests/scripts/");
	TString Tfile = output;
	Tfile.Append(output).Append("/sig").Append(TString(signalchar)).Append("/PseudoExperiemnt/");

	// Processing fitDiagnostics (compressing and gathering in one folder) and combining data into Histograms
	
	TString newdir = directory;
	newdir.Append(output).Append("/sig").Append(TString(signalchar)).Append("/converted/");
	newdir.Prepend("mkdir ");
	system(newdir.Data());
	
	for(int i=1; i <= files; ++i)
	{
		int templ=0;
		int tempi = i;
		do{++templ;
		tempi /=10;}while(tempi);
		char tempfiles[templ];
		sprintf(tempfiles,"%d",i);

		TString save = output;
		save.Append("/sig").Append(signalchar).Append("/converted");;
		TString foldername = directory;
		foldername.Append(output).Append("/sig").Append(signalchar).Append("/PseudoExperiment").Append(tempfiles).Append("/fitDiagnostics");
		TConvert(foldername,save,tempfiles);
	}
	TString combining = directory;
	combining.Append(output.Data()).Append("/sig").Append(signalchar).Append("/converted/");
	TString savecomb = directory;
	savecomb.Append("/converted/").Append("combined").Append(tempc).Append(".root");
	combining.Append("fitDiagnostics*.root");
	chaining(combining.Data(),savecomb.Data());
	
	return 0;
}