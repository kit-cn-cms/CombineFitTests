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

int amount(TString folder)
{
	int amount = 0;
	DIR *dir =opendir(folder.Data());
	while(dir = readdir(dir)){++amount;}
	return amount;
}

int createfiles(TString origin = "datacard.txt", TString output = "toytest/bla_some", float signal = 1, int files = 100, TString ToyCom = "")
{
	/*
	std::cout << "All data is read. Displaying:\nOrigin/Datacard: " << origin.Data() << "\nSignalstrength: " << signal << std::endl;
	std::cout << "amount of files: " << files << "\nAdditional Toy Commands: " << ToyCom.Data() << std::endl;

	TString TArgs = TString("-o ");
	*/
	stringstream s;
	s << fixed << setprecision(1) << signal;
	string s1 = s.str();
	char const *signalchar = s1.c_str();
	/*
	int ftemp=files;
	int length_files=0;
	do{++length_files;
	ftemp /=10;} while(ftemp);
	char chfiles[length_files-1];

	sprintf(chfiles,"%d",files);
	TArgs.Append(output.Data()).Append(" -d ").Append(origin.Data()).Append(" -n ").Append(chfiles).Append(" -s ").Append(signalchar).Append(" --startSeed 200");

	TString callpy = TString("python tth_analysis_study.py ");
	callpy.Append(TArgs.Data());

	system(callpy.Data());
*/
        // NEEDS FIXING ON OTHER MACHINES!!!
	TString directory = TString("/nfs/dust/cms/user/firin/bachelor/CombineFitTests/PseudoDataTests/scripts/");
	/*
	// Waiting for files to be created
	TString temp = directory;
	temp.Append(output.Data()).Append("/sig").Append(signalchar);
	TString Command = TString("stat -c %h ");
	Command.Append(directory.Data()).Append(output.Data()).Append("/sig").Append(signalchar).Append("/");
	int filenr =0;
	*/
//	TString Tfile = TString("nfs/dust/cms/user/firin/bachelor/CombineFitTests/PseudoDataTests/scripts/");
	TString Tfile = output;
	Tfile.Append(output).Append("/sig").Append(TString(signalchar)).Append("/PseudoExperiemnt/");

	/*

	while(filenr!=files+3)
	{
		clock_t next = clock();
		if(clock() >next)
		{
			filenr=count(Tfile.Data());
			next += 60000;
		}
	}
	*/
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
	TString savecomb = combining;
	savecomb.Append("combined1.root");
	combining.Append("fitDiagnostics*.root");
	chaining(combining.Data(),savecomb.Data());
	
	return 0;
}
