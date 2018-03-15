#include "iomanip"
#include "iostream"
#include "test_bundle.h"
#include "TString.h"
#include "cstdlib"
#include "sstream"
#include "fstream"
#include "Draw_fit.h"
#include "Draw_final.h"
#include "ctime"

void repeat(TString save1 = "toytest", TString save2 = "some*",int startruns = 1 , int runs = 20, int files = 100, float signal = 1)
{
	TString savesite = save1;
	savesite.Append("/").Append(save2);
	stringstream s;
	s << fixed << setprecision(1) << signal;
	string s1 = s.str();
	char const * sign = s1.c_str();

        TString working = TString("/nfs/dust/cms/user/firin/bachelor/CombineFitTests/PseudoDataTests/scripts");

        TString savedir = TString(save1);
	savedir.Prepend("/");
	savedir.Append("/converted/");
        TString createdir = TString("mkdir ");
        savedir.Prepend(working);
        createdir.Append(savedir.Data());
        system(createdir.Data());

	double differences[runs+1];

	for(int i = startruns; i <= runs; ++i)
	{
		// Checking time to controll performance
		time_t begin;
		time_t end;
		time(&begin);


		TString moment = save2;

		// Converting int to char
		int templ =0;
		int tempi = i;
		do{++templ;
		tempi/=10;} while(tempi);
		char tempc[templ];
		sprintf(tempc,"%d",i);

		// Creating a pathway to the current file
		moment.Replace(moment.Index("*",1),1,tempc);
                TString current = save1;
                current.Append("/").Append(moment);

		// Executing test_bundle, creating new folder
		createfiles(current.Data(), signal, files);

		TString copy = current;
		TString rename = current;
		rename.Append("/sig").Append(sign).Append("/converted/combined1.root ").Append(current).Append("/sig").Append(sign).Append("/converted/combined").Append(tempc).Append(".root");
		rename.Prepend("mv ");
		system(rename.Data());

		current.Append("/sig").Append(sign).Append("/converted/combined").Append(tempc).Append(".root");
		// Executing Draw_fit, to get Means from TTrees
		CreateHist(current.Data());

		// Copying Histogramdata into a new folder
		copy.Append("/sig").Append(sign).Append("/converted/combined").Append(tempc).Append("_histo.root ").Append(working).Append("/").Append(save1).Append("/converted/");
		copy.Prepend("cp ");
		system(copy.Data());

		// Calculating the time it took to complete one run
		time(&end);
		differences[i]= difftime(end,begin);
	}

	TString collect = savesite;
	collect.Append("/sig").Append(sign).Append("/converted/combined_histo.root");
	savedir.Append("combined").Append(sign).Append(".root");
	savedir.Prepend(working.Data());
	collect.Prepend(working.Data());
	TString call = working + "/" + save1 + "/converted/combined*_histo.root";
	TString savep = working + "/" + save1 + "/converted/collected.root";
	// Chaining the resulting datasets
	chaining(call.Data(),savep.Data());
	CreateFinalHist(savep.Data(),1);

	// Copying and processing values
	system(TString(TString("mkdir ") + working + save1 + TString("/Values/")).Data());
	for(int i = startruns; i < runs; ++i)
	{
		int templ =0;
		int tempi = i;
		do{++templ;
		tempi/=10;}
		while(tempi);
		char tempnr[templ];
		sprintf(tempnr,"%d",i);
		TString temporigin = working + save1 + "/some" + tempnr + "/sig" + sign + "/converted/combined" + tempnr + ".root";
		TString tempsave = working + save1 + "/Values/";
		TString move = "cp " + temporigin + " " + tempsave;
		system(move.Data());
	}
	chaining(TString(working+save1+TString("Values/combined*.root")).Data(),TString(working+save1+TString("Values/final.root")).Data());
	CreateFinalHist(TString(working+save1+TString("Values/final.root")).Data(),signal);
	for(int i = 1; i <= runs; ++i)
	{
		std::cout << "Durchlaufnr. " << i << " : " << differences[i] << std::endl;
	}
}

int main()
{
	TString folder;
	TString sub;
	int runs;
	int files;
	float signal;
	int startrun;
	std::cout << "Geben Sie den Zielordner der Form toytest an: ";
	std::cin >> folder;
	std::cout << "Geben sie den Unterordner an: ";
	std::cin >> sub;
	std::cout << "Erster Run? ";
	std::cin >> startrun;
	std::cout << "Wie viele Runs? ";
	std::cin >> runs;
	std::cout << "Wie viele files? ";
	std::cin >> files;
	std::cout << "Signalstarke? ";
	std::cin >> signal;
	repeat(folder, sub,startrun, runs, files, signal);
	return 0;
}
