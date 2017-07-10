
A) Method
-----------------------------------------------------------
Remove bin-by-bin uncertainties because toys take time. This might bias the result however.
Exchange real data with b-only PseudoData !!!
First try the toys were generated from the post-fit of real data. Mixed with all these asimov arguments, the output was really strange.
text2workspace datacard.txt
1) Fit b-only asimov
combine -M MaxLikelihoodFit -t -1 --expectSignal 0 --minimizerStrategy 0 --minimizerTolerance 0.1 --rMin=-5 --rMax=5 -n Name datacard.root

2) Create b-only toys and fit them (see https://twiki.cern.ch/twiki/bin/viewauth/CMS/SWGuideHiggsAnalysisCombinedLimit#Toy_by_toy_diagnostics)
combine -M MaxLikelihoodFit -t 500 --expectSignal 0 --minimizerStrategy 0 --minimizerTolerance 0.1 --rMin=-5 --rMax=5 --toysFrequentist --noErrors --minos none -n Name datacard.root

3) Analyze them with the plotParametersFromToys.C script. This is provided with the combine tool. Actually i used a slightly modified one, which i also put to the sent files.
Do for example:
root plotParametersFromToys
root [1] plotParametersFromToys("mlfitljdlNoBBB_Btoys.root","mlfitljdlcombNoBBB_Basimov.root","LJDLcombNoBBB.root","")

Here the first file is the file with the toys. The second file is the fit file of the asimov toy (usually you would use the fit to real data here) and the third file is datacard workspace. The fourth argument can be used to select subsets of toys only. For example where the best fit value is negative.
You will get 2 pdf files which i will cover in B)
I did similar studied also after Moriond. The explanations for the results are how i remember them and what i understood now, from looking at the code again.
So it is all open for discussion. 
