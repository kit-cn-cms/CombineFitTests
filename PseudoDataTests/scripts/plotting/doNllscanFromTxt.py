import os
import sys
import glob
import subprocess
from ROOT import TFile, RooFitResult, RooRealVar
from shutil import rmtree

scriptDir = os.path.dirname(os.path.abspath(sys.argv[0]))
pathToTxt = sys.argv[1]
pathToDatacards = sys.argv[2]
cmdList = None
if len(sys.argv) > 3:
    cmdList = sys.argv[3:]

print "input:"
print "\tpathToTxt:", pathToTxt
print "\tpathToDatacards:", pathToDatacards
print "\tcmdList:\n", cmdList

basepath = os.getcwd()

def reset_directory(outputDirectory):
    if os.path.exists(outputDirectory):
        print "resetting ", outputDirectory
        rmtree(outputDirectory)
    os.makedirs(outputDirectory)

def do_prefit(datacard, cmdlist = None):
    
    # cmd = "combine -M FitDiagnostics -m 125 --cminDefaultMinimizerStrategy 0 --cminDefaultMinimizerTolerance 1e-5"
    cmd = "combine -M FitDiagnostics -m 125"
    if cmdlist:
        cmd += " "+ " ".join(cmdlist)
    cmd += " " + datacard
    print cmd
    subprocess.call([cmd], shell = True)
    if os.path.exists("fitDiagnostics.root"):
        infile = TFile("fitDiagnostics.root")
        if infile.IsOpen() and not infile.IsZombie() and not infile.TestBit(TFile.kRecovered):
            fit_s = infile.Get("fit_s")
            if isinstance(fit_s, RooFitResult):
                r = fit_s.floatParsFinal().find("r")
                if isinstance(r, RooRealVar):
                    return r.getVal()
                    
    print "could not load parameter r!"
    return None

if os.path.exists(pathToTxt):
    
    with open(pathToTxt) as f:
        lines = f.read().splitlines()

    for datacard in glob.glob(pathToDatacards):
        datacard = os.path.abspath(datacard)
        parts = os.path.basename(datacard).split(".")
        outputDirectory = ".".join(parts[:len(parts)-1])
        reset_directory(outputDirectory = outputDirectory)
        os.chdir(outputDirectory)
        r = None
        r = do_prefit(datacard = datacard, cmdlist = cmdList)
        for param in lines:
            print param
            if param.startswith("#"):
                continue
            
            reset_directory(param)
            os.chdir(param)
            cmd = "python {0}/nllscan.py".format(scriptDir)
            cmd += " -d " + datacard
            cmd += ' -x {0}'.format(param)
            cmd += ' -a "--setParameterRanges {0}=-3,3"'.format(param)
            if r is not None:
                cmd += ' -a "--setParameters r={0} --floatOtherPOIs 1"'.format(r)
            # cmd += " -n _" + os.path.basename(outputDirectory)
            if cmdList:
                cmd += " -a \"" + " ".join(cmdList) + "\""
            print cmd
            subprocess.call([cmd], shell=True)
            os.chdir("../")
        os.chdir(basepath)
    
