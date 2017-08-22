import ROOT
import os
import sys
import glob
import math

ROOT.gROOT.SetBatch(True)

path1 = sys.argv[1]
path2 = None
if len(sys.argv) == 1:
    path2 = sys.argv[2]
else:
    path2 = path1


filesToCheck = ["mlfit.root", "mlfit_MS_mlfit.root"]

if os.path.exists(os.path.abspath(path1)):
    path1 = os.path.abspath(path1)
else:
    sys.exit("%s does not exist!" % path1)


if os.path.exists(os.path.abspath(path2)):
    path2 = os.path.abspath(path2)
else:
    sys.exit("%s does not exist!" % path2)



if path1.endswith("/"):
    path1 = path1 + "*"
else:
    path1 = path1 + "/*"

if not path2.endswith("/"):
    path2 = path2 + "/"

def loadVariable(pathToLoad, takeTree = False):
    val = -9999
    error = -9999
    if os.path.exists(pathToLoad):
        results = ROOT.TFile(pathToLoad)
        if results.IsOpen() and not results.TestBit(ROOT.TFile.kRecovered) and not results.IsZombie():
            if not takeTree:
                fit_s = results.Get("fit_s")
                if not isinstance(fit_s,ROOT.RooFitResult):
                    print pathToLoad,"does not contain RooFitResult"
                    return val, error
                ROOT.gDirectory.cd('PyROOT:/')
                var = fit_s.floatParsFinal().find("r")
                val = var.getVal()
                error = var.getError()
            else:
                tree = results.Get("tree_fit_sb")
                if not isinstance(tree,ROOT.TTree):
                    print pathToLoad,"does not contain limit tree"
                    return val, error
                ROOT.gDirectory.cd('PyROOT:/')
                for entry in tree:
                    val = entry.mu
                    error = entry.muErr
            results.Close()
    else:
        print "Could not load file", pathToLoad
    #return math.ceil(val*10**6)/10**6, math.ceil(error*10**6)/10**6
    return val, error

def compareVals(val1, error1, val2, error2, errormessage):
    if not (val1 == val2 and error1 == error2):
        print errormessage
        print "\tCOMBINE FUCKED UP!"
        print "\tval1 = {0} +- {1}\n\tval2 = {2} +- {3}".format(val1, error1, val2, error2)

mu1 = {}
muError1 = {}
mu2 = {}
muError2 = {}

for path in sorted(glob.glob(path1)):
    if os.path.exists(os.path.abspath(path)):
        if not "PseudoExperiment" in os.path.basename(path):
            continue
        path = os.path.abspath(path)
        siblingPath = path2 + os.path.basename(path)
        if os.path.exists(siblingPath):
            for infile in filesToCheck:
                r1, error1 = loadVariable(path + "/" + infile)
                tree_r1, tree_error1 = loadVariable(path + "/" + infile, True)
                r2, error2 = loadVariable(siblingPath + "/" + infile)
                tree_r2, tree_error2 = loadVariable(siblingPath + "/" + infile, True)
                if r1 == -9999 or error1 == -9999:
                    print "Could not load variable r1 or error1 from {0}/{1}".format(path, infile)
                    continue
                if r2 == -9999 or error2 == -9999:
                    print "Could not load variable r2 or error2 from {0}/{1}".format(siblingPath, infile)
                    continue

                if tree_r1 == -9999 or tree_error1 == -9999:
                    print "Could not load variable tree_r1 or tree_error1 from {0}/{1}".format(path, infile)
                    continue
                if tree_r2 == -9999 or tree_error2 == -9999:
                    print "Could not load variable tree_r2 or tree_error2 from {0}/{1}".format(siblingPath, infile)
                    continue

                if not infile in mu1:
                    mu1[infile] = ROOT.TH1D("mu1_"+infile, "", 400, -10, 10)
                    muError1[infile] = ROOT.TH1D("muError1_"+infile, "", 400, -10, 10)
                    mu2[infile] = ROOT.TH1D("mu2_"+infile, "", 400, -10, 10)
                    muError2[infile] = ROOT.TH1D("muError2_"+infile, "", 400, -10, 10)

                mu1[infile].Fill(r1)
                muError1[infile].Fill(error1)
                #print infile, "in", os.path.basename(path)
                #print "\tr1 = {0} +- {1}\n\tr2 = {2} +- {3}".format(r1, error1, r2, error2)
                mu2[infile].Fill(r2)
                muError2[infile].Fill(error2)
                compareVals(r1, error1, r2, error2, "while checking {0} in folder {1}: error in RooFitResult values".format(infile, os.path.basename(path)))

                compareVals(tree_r1, tree_error1, tree_r2, tree_error2, "while checking {0} in folder {1}: error in TTree values".format(infile, os.path.basename(path)))

                #print "\tcomparing RooFitResult and TTree values - 1"
                #compareVals(r1, error1, tree_r1, tree_error1)
                #print "\tcomparing RooFitResult and TTree values - 2"
                #compareVals(r2, error2, tree_r2, tree_error2)

        else:
            print "Could not find sibling path in {0}".format(siblingPath)


    else:
        print path, "does not exist!"
for infile in mu1:
    print "computed signal strength for {5} in path {0}: mu = {1} +- {2} +- {3} +- {4}".format(path1, mu1[infile].GetMean(), mu1[infile].GetMeanError(), mu1[infile].GetRMS(), muError1[infile].GetMean(), infile)
    print "computed signal strength for {5} in path {0}: mu = {1} +- {2} +- {3} +- {4}".format(path2, mu2[infile].GetMean(), mu2[infile].GetMeanError(), mu2[infile].GetRMS(), muError2[infile].GetMean(), infile)
