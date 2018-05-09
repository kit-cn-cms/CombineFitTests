#include "iostream"
#include "TString.h"
#include "cstdlib"
#include "iomanip"
#include "sstream"

int createfiles(int dirs = 20, int toys = 30, TString save ="toytest", TString origin = "signal_background_example/datacard.txt", float signal = 1.)
{
	TString workdir = TString("/nfs/dust/cms/user/firin/bachelor/CombineFitTests/PseudoDataTests/scripts/");

	stringstream s;
	s << fixed << setprecision(1) << signal;
	string s1 = s.str();
	char const * sign = s1.c_str();

	int temp1l =0;
	int temp1i =toys;
	do{++temp1l; temp1i/=10;}
	while(temp1i);
	char temp1c[temp1l];
	sprintf(temp1c, "%d", toys);

	for(int i = 1; i <= dirs; ++i)
	{
		int templ =0;
		int tempi =i;
		do{++templ; tempi/=10;}
		while(tempi);
		char tempc[templ];
		sprintf(tempc, "%d",i);

		TString command = TString("python tth_analysis_study.py -d ");
		command+=workdir + origin + " -o " + workdir + save + "/some" + tempc + " -s " + sign + " -n " + temp1c + " --addToyCommand \"--toysNoSystematics  --rMin=-10 --rMax=10\" ";
		system(command.Data());
	}
	std::cout << "DONE" << std::endl;
	return 0;
}
