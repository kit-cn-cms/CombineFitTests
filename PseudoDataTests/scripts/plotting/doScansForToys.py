from ROOT import TFile, RooFitResult
import os
import sys
import glob
import subprocess

scriptDir = os.path.dirname(os.path.abspath(sys.argv[0]))
toywildcard = sys.argv[1]
pathToWorkspace = sys.argv[2]
resultfile = sys.argv[3]
cmdList = None
if len(sys.argv) > 4:
    cmdList = sys.argv[4:]


def fit_converged(results):
    f = TFile.Open(results)
    if isinstance(f, TFile) and not f == None:
        if f.IsOpen() and not f.IsZombie() and not f.TestBit(TFile.kRecovered):
            fit_s = f.Get("fit_s")
            if isinstance(fit_s, RooFitResult):
                if fit_s.status() == 0 and fit_s.covQual() == 3:
                    return True
    return False


if cmdList:
    indeces = [i for i,x in enumerate(cmdList) if (x == "-a" or x =="--addCommand" or x == "--setXtitle" or x == "--setYtitle")]
    for index in indeces:
        cmdList[index+1] = '"{0}"'.format(cmdList[index+1])

if not os.path.exists(pathToWorkspace):
    sys.exit("Could not find workspace to perform fits with!")
else:
    pathToWorkspace = os.path.abspath(pathToWorkspace)

basedir = os.getcwd()

for toy in glob.glob(toywildcard):
    toy = os.path.abspath(toy)
    toydir = os.path.dirname(toy)
    outputdir = toydir + "/nllscan"
    if cmdList:
        suffix = [cmdList[i+1] for i,x in enumerate(cmdList) if x == "-n" or x == "--suffix" or x == "-x" or x == "-y"]
        if not len(suffix) == 0:
            outputdir += "_" + "_".join(suffix)
    if not os.path.exists(outputdir):
        os.makedirs(outputdir)
    cmd = "python {0}/nllscan.py".format(scriptDir)
    cmd += " -d " + pathToWorkspace
    cmd += " -o " + outputdir
    if "_-1toys_" in toy:
        cmd += ' -a "-t -1 --toysFile {0}"'.format(toy)
    else:
        cmd += ' -a "-t 1 --toysFile {0}"'.format(toy)
    fitresultfile = toydir + "/"+resultfile
    if os.path.exists(fitresultfile):
        fitresultfile = os.path.abspath(fitresultfile)
        cmd += " --loadErrorsFrom " + fitresultfile
        if not fit_converged(fitresultfile):
            print "skipping toy in", toy
            continue
    else: 
        print "could not find file", fitresultfile
        continue
    if cmdList:
        cmd += " " + " ".join(cmdList)
    print cmd
    subprocess.call([cmd], shell=True)
