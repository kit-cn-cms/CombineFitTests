import ROOT
import os
import sys
import glob
import subprocess
from optparse import OptionParser

ROOT.gROOT.SetBatch(True)

parser = OptionParser()
parser.add_option("--toysFile", help= "file with toys that datacard is supposed to be fitted to", dest = "toysFile", metavar = "path/to/toys/file")
parser.add_option("-d", "--datacard", help = "path to datacard to use for fit", dest = "datacard", metavar = "path/to/datacard")
parser.add_option("-t", help = "number of toys in toysFile, -1 for asimov toys (default = 1)", dest = "t", type = "int", default = 1)
parser.add_option("--addCommand", help = "add option to standard combine command (can be used multiple times)", dest = "addCommand", action = "append")
parser.add_option("-x", "--xVariable", help = "variable for x axis", dest="x")
parser.add_option("-y", "--yVariable", help = "variable for y axis (default = deltaNLL)", dest="y", default = "deltaNLL")
parser.add_option("-n", "--suffix", help = "add suffix to output name", dest = "suffix")
parser.add_option("-o", "--outputDirectory", metavar = "path/to/save/plots/in", dest = "outputDirectory", help = "path to save output plots in (default = here)", default = "./")

(options, args) = parser.parse_args()
toysFile = options.toysFile
if toysFile == None:
    parser.error("toysFile needs to be specified!")
else:
    toysFile = os.path.abspath(toysFile)
    if not os.path.exists(toysFile):
        parser.error("toy file does not exist!")
datacard = options.datacard
if datacard == None:
    parser.error("datacard for fitting must be specified!")
else:
    datacard = os.path.abspath(datacard)
    if not os.path.exists(datacard):
        parser.error("datacard does not exist!")
nToys = options.t
additionalCmds = options.addCommand
xVar = options.x
if xVar == None:
    parser.error("variable for x axis of scan needs to be specified!")
yVar = options.y
suffix = options.suffix
outputDirectory = os.path.abspath(options.outputDirectory)
if not os.path.exists(outputDirectory):
    parser.error("output directory does not exist!")

basepath = os.getcwd()
print "changing into directory", os.path.dirname(toysFile)
os.chdir(os.path.dirname(toysFile))

fitresFile = "higgsCombine"

multidimfitcmd = 'combine -M MultiDimFit ' + datacard
multidimfitcmd = multidimfitcmd + ' --algo=grid --points=1000 --minimizerStrategy 1 --minimizerTolerance 0.3 --cminApproxPreFitTolerance=25 '
multidimfitcmd = multidimfitcmd + '--cminFallbackAlgo "Minuit2,migrad,0:0.3" --cminOldRobustMinimize=0 --X-rtd MINIMIZER_MaxCalls=9999999 '
multidimfitcmd = multidimfitcmd + '-m 125 -t ' + str(nToys) + ' --toysFile ' + toysFile + ' --saveFitResult'
if not suffix == None:
    multidimfitcmd = multidimfitcmd + ' -n ' + suffix
    fitresFile = fitresFile + suffix
else:
    fitresFile = fitresFile + "Test"
for cmd in additionalCmds:
    multidimfitcmd = multidimfitcmd + " " + cmd

print multidimfitcmd
subprocess.call([multidimfitcmd], shell=True)

fitresFile = os.path.abspath(fitresFile + ".MultiDimFit.mH125.root")


if os.path.exists(fitresFile):
    infile = ROOT.TFile(fitresFile)

    if infile.IsOpen() and not infile.IsZombie() and not infile.TestBit(ROOT.TFile.kRecovered):
        limit = infile.Get("limit")
        if isinstance(limit, ROOT.TTree):
            xVals = []
            yVals = []

            print "loaded limit TTree with {0} events".format(limit.GetEntries())
            for e in limit:
                x = eval("e." + xVar)
                y = eval("e." + yVar)
                #print "current values: {0}, {1}".format(x, y)

                if yVar == "deltaNLL":
                    if y > 0 and y < 10:
                        #print "saving values {0}, {1}".format(x, 2*y)
                        xVals.append(x)
                        yVals.append(2*y)
                else:
                    xVals.append(x)
                    yVals.append(y)
            c = ROOT.TCanvas()
            graph = ROOT.TGraph(len(xVals))
            for i in range(len(xVals)):
                graph.SetPoint(i, xVals[i], yVals[i])
            graph.GetXaxis().SetTitle(xVar)
            if yVar == "deltaNLL":
                yVar = '2#Delta NLL'
            graph.GetYaxis().SetTitle(yVar)
            graph.SetTitle("Scan of {0} over {1}".format(yVar, xVar))
            graph.Sort()
            graph.Draw()
            filename = ("nllscan_{0}_{1}{2}").format(xVar,yVar.replace("#", "x"), suffix)
            filename = filename.replace(" ", "_")
            c.SaveAs(outputDirectory + "/" + filename + ".pdf")
            graph.SaveAs(outputDirectory + "/" + filename + ".root")
        else:
            print "Could not load limit tree!"
else:
    print "Could not find multidim fit output", fitresFile

os.chdir(basepath)