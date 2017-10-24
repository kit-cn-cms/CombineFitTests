import ROOT
import os
import sys
import glob
import subprocess
from optparse import OptionParser

ROOT.gROOT.SetBatch(True)

parser = OptionParser()
parser.add_option("--toysFile", help= "file with toys that datacard is supposed to be fitted to", dest = "toysFile", metavar = "path/to/toys/file")
parser.add_option("-d", "--datacard", help = "path to datacard to use for toy generation (and fit if no workspace is given)", dest = "datacard", metavar = "path/to/datacard")
parser.add_option("-t", help = "number of toys in toysFile, -1 for asimov toys (default = -1)", dest = "t", type = "int", default = -1)
parser.add_option("--addFitCommand", help = "add option to standard combine command 'MultiDimFit' (can be used multiple times)", dest = "addFitCommand", action = "append")
parser.add_option("--addToyCommand", help = "add option to standard combine command 'GenerateOnly' (can be used multiple times)", dest = "addToyCommand", action = "append")
parser.add_option("-x", "--xVariable", help = "variable for x axis", dest="x")
parser.add_option("-y", "--yVariable", help = "variable for y axis (default = deltaNLL)", dest="y", default = "deltaNLL")
parser.add_option("--scan2D", help = "perform 2D NLL scan", dest="scan2D", action="store_true", default=False)
parser.add_option("-n", "--suffix", help = "add suffix to output name", dest = "suffix", default = "")
parser.add_option("-o", "--outputDirectory", metavar = "path/to/save/plots/in", dest = "outputDirectory", help = "path to save output plots in (default = here)", default = "./")
parser.add_option("--directlyDrawFrom", metavar = "path/to/scan/results", dest = "directDraw", help = "skip multi dim fit and draw plots directly from this file (needs to contain limit tree)")
parser.add_option("--points", dest = "points", metavar = "numberOfPoints", help = "number of points to scan (default = 1000)", type = "int", default = "1000")
parser.add_option("--bonly", help = "perform background only fit (creates multiSignalModel where signal strength is not mapped to anything)", action = "store_true", default = False, dest = "bonly")
parser.add_option("-w", "--workspace", dest = "workspace", metavar = "path/to/workspace", help = "path to workspace to use for fit")

(options, args) = parser.parse_args()

directDrawPath = options.directDraw
if directDrawPath is not None:
    directDrawPath = os.path.abspath(directDrawPath)
    if not os.path.exists(directDrawPath):
        parser.error("File with scan results does not exist!")

toysFile = options.toysFile
datacard = options.datacard
nToys = options.t
additionalCmds = options.addFitCommand
additionalToyCmds = options.addToyCommand
nPoints = options.points

if directDrawPath == None:
    if toysFile == None:
        print "will generate toys on the fly"
    else:
        toysFile = os.path.abspath(toysFile)
        if not os.path.exists(toysFile):
            parser.error("toy file does not exist!")

    if datacard == None:
        parser.error("datacard for toy generation/fitting must be specified!")
    else:
        datacard = os.path.abspath(datacard)
        if not os.path.exists(datacard):
            parser.error("datacard does not exist!")

xVar = options.x
if xVar == None:
    parser.error("variable for x axis of scan needs to be specified!")

yVar = options.y
suffix = options.suffix
outputDirectory = os.path.abspath(options.outputDirectory)
if not os.path.exists(outputDirectory):
    parser.error("output directory does not exist!")

scan2D = options.scan2D
if scan2D and (xVar == None or yVar == None):
    parser.error("Both x and y variable must be defined for 2D NLL scan!")

bonly = options.bonly
workspace = options.workspace
if workspace:
    workspace = os.path.abspath(workspace)
    if not os.path.exists(workspace):
        print "Could not find workspace, will ignore input"
        workspace = None

if bonly:
    print "will perform background-only fit"
    suffix += "_bonly"
    
    if not workspace:
        print "creating b-only workspace"
        pathToWorkspace = os.path.dirname(datacard) + "/"
        filename = os.path.basename(datacard)
        parts = filename.split(".")
        filename = ".".join(parts[:len(parts) - 1])
        pathToWorkspace += filename + "_bonly.root"
        cmd = "text2workspace.py " + datacard
        cmd += " -P HiggsAnalysis.CombinedLimit.PhysicsModel:multiSignalModel"
        cmd += " --PO verbose  --PO 'map=.*/(0):r[1,-10,10]'"
        cmd += " -o " + pathToWorkspace
        
        print cmd
        subprocess.call([cmd], shell=True)
        if not os.path.exists(pathToWorkspace):
            sys.exit("Could not generate bonly workspace in %s! Aborting" % pathToWorkspace)
        workspace = pathToWorkspace

if not toysFile:
    print "starting toy generation"
    cmd = "combine -M GenerateOnly -m 125"
    cmd += " -t " + str(nToys)
    cmd += " --saveToys -n " + suffix
    if additionalToyCmds:
        cmd += " " + " ".join(additionalToyCmds)
    cmd += " " + datacard
    
    print "creating new toy for b-only fit"
    print cmd
    subprocess.call([cmd], shell=True)
    toysFile = "higgsCombine" + suffix + ".GenerateOnly.mH125.123456.root"
    toysFile = os.path.abspath(toysFile)
    if not os.path.exists(toysFile):
        sys.exit("Could not generate toy file %s! Aborting" % toysFile)

if workspace:
    datacard = workspace

#______________combine stuff_____________________________________

if directDrawPath is None:
    basepath = os.getcwd()
    fitresFile = "higgsCombine"
    if toysFile:
        fitresFile = os.path.dirname(toysFile) + "/" + fitresFile
        print "changing into directory", os.path.dirname(toysFile)
        os.chdir(os.path.dirname(toysFile))

    multidimfitcmd = 'combine -M MultiDimFit ' + datacard
    multidimfitcmd += ' --algo=grid --points=' + str(nPoints)
    # multidimfitcmd += ' --minimizerStrategy 1 --minimizerTolerance 0.3'
    # multidimfitcmd += ' --cminApproxPreFitTolerance=25'
    # multidimfitcmd += ' --cminFallbackAlgo "Minuit2,migrad,0:0.3"'
    # multidimfitcmd += ' --cminOldRobustMinimize=0'
    # multidimfitcmd += ' --X-rtd MINIMIZER_MaxCalls=9999999'
    multidimfitcmd += ' -m 125 -t ' + str(nToys)
    if toysFile:
        multidimfitcmd += ' --toysFile ' + toysFile
    else:
        if additionalToyCmds:
            multidimfitcmd += " " + " ".join(additionalToyCmds)
    multidimfitcmd += ' --rMin -10 --rMax 10 --saveFitResult --saveInactivePOI 1'
    if not suffix == "":
        multidimfitcmd += ' -n ' + suffix
        fitresFile += suffix
    else:
        fitresFile += "Test"
    if additionalCmds:
        for cmd in additionalCmds:
            multidimfitcmd += " " + cmd

    print multidimfitcmd
    subprocess.call([multidimfitcmd], shell=True)
    for workspace in glob.glob("roostat*.root"):
        os.remove(workspace)
    if os.path.exists(fitresFile + ".MultiDimFit.mH125.123456.root"):
        os.remove(fitresFile + ".MultiDimFit.mH125.123456.root")
    fitresFile = os.path.abspath(fitresFile + ".MultiDimFit.mH125.root")
    os.chdir(basepath)
else:
    fitresFile = directDrawPath

#________________________________________________________________

#======================================================================

def do1DScan(limit, xVar, yVar, outputDirectory, suffix):
    xVals = []
    yVals = []
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

def do2DScan(limit, xVar, yVar, outputDirectory, suffix):
    xVals = []
    yVals = []
    zVals = []
    for e in limit:
        x = eval("e." + xVar)
        y = eval("e." + yVar)
        z = e.deltaNLL
        #print "current values: {0}, {1}".format(x, y)

        if z > 0 and z < 10:
                #print "saving values {0}, {1}".format(x, 2*y)
                xVals.append(x)
                yVals.append(y)
                zVals.append(2*z)

    c = ROOT.TCanvas()

    graph = ROOT.TGraph2D()
    for i in range(len(xVals)):
        graph.SetPoint(i, xVals[i], yVals[i], zVals[i])
    graph.GetHistogram().GetXaxis().SetTitle(xVar)
    graph.GetHistogram().GetYaxis().SetTitle(yVar)
    graph.GetHistogram().GetZaxis().SetTitle("2#Delta NLL")
    graph.SetTitle("Scan of {0} over {1}".format(yVar, xVar))
    graph.Draw("COLZ")
    c.SetMargin(0.25, 0.15, 0.15, 0.08);
    filename = ("nllscan_2D_{0}_{1}{2}").format(xVar,yVar.replace("#", "x"), suffix)
    filename = filename.replace(" ", "_")
    c.SaveAs(outputDirectory + "/" + filename + ".pdf")
    graph.SaveAs(outputDirectory + "/" + filename + ".root")

#=======================================================================

if os.path.exists(fitresFile):
    infile = ROOT.TFile(fitresFile)

    if infile.IsOpen() and not infile.IsZombie() and not infile.TestBit(ROOT.TFile.kRecovered):
        limit = infile.Get("limit")
        if isinstance(limit, ROOT.TTree):
            print "loaded limit TTree with {0} events".format(limit.GetEntries())

            if scan2D:
                do2DScan(limit, xVar = xVar, yVar = yVar, outputDirectory = outputDirectory, suffix = suffix)
            else:
                do1DScan(limit, xVar = xVar, yVar = yVar, outputDirectory = outputDirectory, suffix = suffix)


        else:
            print "Could not load limit tree!"
        infile.Close()
else:
    print "Could not find multidim fit output", fitresFile
