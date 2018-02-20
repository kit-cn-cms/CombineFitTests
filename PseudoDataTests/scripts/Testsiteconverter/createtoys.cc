#include "iostream"
#include "TString.h"
#include "cstdlib"
#include "iomanip"
#include "TRandom.h"
#include "sstream"

int createfiles(int dirs = 20, int toys = 30, TString save ="/toytest/", TString origin = "/signal_background_example/datacard.txt", float signal = 1.)
{
	TString workdir = TString("/nfs/dust/cms/user/firin/bachelor/CombineFitTests/PseudoDataTests/scripts");
	TRandom generator = TRandom();

	stringstream s;
	s << fixed << setprecision(1) << signal;
	string s1 = s.str();
	char const * sign = s1.c_str();

	int temp1l =0;
	int temp1i =dirs;
	do{++temp1l; temp1i/=10;}
	while(temp1i);
	char temp1c[temp1l];
	sprintf(temp1c, "%d", dirs);

	int seeds[dirs];

	std::cout << "Before for loop\n";

	for(int i = 1; i <= dirs; ++i)
	{
		int seed =0;
		while(seed <1)
		{
		std::cout << "In while loop\n";
		seed = generator.Uniform() *1000;
		}
		std::cout << "Left while loop\n";
		seeds[i] = seed;
		int templ =0;
		int tempi =i;
		do{++templ; tempi/=10;}
		while(tempi);
		char tempc[templ];
		sprintf(tempc, "%d",i);

		int temp2l=0;
		int temp2i = seed;
		do{++temp2l; temp2i/=10;}
		while(temp2i);
		char temp2c[temp2l];
		sprintf(temp2c, "%d", seed);

		TString command = TString("python tth_analysis_study.py -d ");
		command.Append(workdir).Append(origin).Append(" -o ").Append(workdir).Append(save).Append("some").Append(tempc).Append("/").Append(" -s ").Append(sign).Append(" -n ").Append(temp1c).Append(" --startSeed ").Append(temp2c);
		system(command.Data());
	}
	std::cout << "Used Seeds: " << std::endl;
	for(int i =1; i<=dirs;++i){std::cout << i << " | " << seeds[i] << std::endl;}
	std::cout << "DONE" << std::endl;
	return 0;
}