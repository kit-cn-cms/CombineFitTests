import ROOT
import subprocess
import sys
import os

ROOT.gROOT.SetBatch(True)


if os.path.exists("multidimfit_full_fit.root") and os.path.exists("higgsCombine_full_fit.MultiDimFit.mH125.123456.root"):
    multiFile = ROOT.TFile("multidimfit_full_fit.root")
    fit_mdf = multiFile.Get("fit_mdf")
    signalStrength = fit_mdf.floatParsFinal().find("r")
    r = signalStrength.getVal()
    print "starting MaxLikelihoodFit with r =", r
<<<<<<< HEAD
    combineCmd = 'combine higgsCombine_full_fit.MultiDimFit.mH125.123456.root -w w --snapshotName MultiDimFit -M MaxLikelihoodFit --minimizerStrategy 0 --minimizerTolerance 0.001 --rMin -10 --rMax 10 --minos all --bypassFrequentistFit --toysFrequentist -t -1 --expectSignal ' + str(r) + ' -n _MDF --saveShapes --saveNormalizations'
=======
    combineCmd = 'combine higgsCombine_full_fit.MultiDimFit.mH125.123456.root -w w --snapshotName MultiDimFit -M MaxLikelihoodFit --minimizerStrategy 0 --minimizerTolerance 0.001 --rMin -10 --rMax 10 --minos all --bypassFrequentistFit --toysFrequentist -t -1 --expectSignal ' + str(r) + ' --redefineSignalPOIs r -n _MDF --saveShapes --saveNormalizations'
>>>>>>> 81509c8b530b5f11e714f8f281f434aa1387a365

    subprocess.check_call(combineCmd.split())
    multiFile.Close()
else:
    print "Could not find file higgsCombine_full_fit.MultiDimFit.mH125.123456.root!"
