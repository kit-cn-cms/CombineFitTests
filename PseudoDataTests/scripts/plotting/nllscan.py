import ROOT
import os
import sys
import glob
import subprocess
import imp
from optparse import OptionParser

basedir = os.path.dirname(os.path.abspath(sys.argv[0]))
helperpath = os.path.join(basedir, "../base")
helperpath += "/helpfulFuncs.py"
if os.path.exists(helperpath):
    helperfuncs = imp.load_source('helpfulFuncs', helperpath)
else:
    sys.exit("Could not load helperfuncs from %s" % helperpath)
ROOT.gROOT.SetBatch(True)

parser = OptionParser()
parser.add_option("-d", "--datacard", help = "path to datacard to use for toy generation (and fit if no workspace is given)", dest = "datacard", metavar = "path/to/datacard")
parser.add_option("-a","--addCommand", help = "add option to standard combine command 'MultiDimFit' (can be used multiple times)", dest = "addCommand", action = "append")
parser.add_option("-x", "--xVariable", help = "variable for x axis", dest="x", default="r")
parser.add_option("-y", "--yVariable", help = "variable for y axis (default = deltaNLL)", dest="y", default = "deltaNLL")
parser.add_option("--scan2D", help = "perform 2D NLL scan", dest="scan2D", action="store_true", default=False)
parser.add_option("-n", "--suffix", help = "add suffix to output name", dest = "suffix", default = "")
parser.add_option("-o", "--outputDirectory", metavar = "path/to/save/plots/in", dest = "outputDirectory", help = "path to save output plots in (default = here)", default = "./")
parser.add_option("--directlyDrawFrom", metavar = "path/to/scan/results", dest = "directDraw", help = "skip multi dim fit and draw plots directly from this file (needs to contain limit tree)")
parser.add_option("--points", dest = "points", metavar = "numberOfPoints", help = "number of points to scan (default = 1000)", type = "int", default = "1000")
parser.add_option("--bonly", help = "perform background only fit (creates multiSignalModel where signal strength is not mapped to anything)", action = "store_true", default = False, dest = "bonly")
parser.add_option("-w", "--workspace", dest = "workspace", metavar = "path/to/workspace", help = "path to workspace to use for fit")
parser.add_option("--scanUnconstrained", dest = "unconstrained", action = "store_true", default = False, help = "Drop constraints for scaned parameters")
parser.add_option("--floatR", dest = "floatR", action = "store_true", default = False, help = "keep other POIs (e.g. signal strength r) floating. If not set, r is fixed to one value (can be set via '--setPhysicsModelParameter r=Value', should be added to '--addFitCommand')")

(options, args) = parser.parse_args()

directDrawPath = options.directDraw
if directDrawPath is not None:
    directDrawPath = os.path.abspath(directDrawPath)
    if not os.path.exists(directDrawPath):
        parser.error("File with scan results does not exist!")

datacard = options.datacard
additionalCmds = options.addCommand

nPoints = options.points
unconstrained = options.unconstrained
float_r = options.floatR

if directDrawPath == None:
    
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


if directDrawPath is None:
        
    if workspace:
        datacard = workspace

#______________combine stuff_____________________________________


    basepath = os.getcwd()
    fitresFile = outputDirectory + "/higgsCombine"
    
    multidimfitcmd = 'combine -M MultiDimFit ' + datacard
    multidimfitcmd += ' --algo=grid --points=' + str(nPoints)
    # multidimfitcmd += ' --minimizerStrategy 1 --minimizerTolerance 0.3'
    # multidimfitcmd += ' --cminApproxPreFitTolerance=25'
    # multidimfitcmd += ' --cminFallbackAlgo "Minuit2,migrad,0:0.3"'
    # multidimfitcmd += ' --cminOldRobustMinimize=0'
    # multidimfitcmd += ' --X-rtd MINIMIZER_MaxCalls=9999999'
    multidimfitcmd += ' -m 125 --saveWorkspace'
    if additionalCmds:
        multidimfitcmd += " " + " ".join(additionalCmds)
        
    multidimfitcmd += ' --rMin -10 --rMax 10 --saveFitResult'
    multidimfitcmd += ' --saveInactivePOI 1'
    
    if float_r:
        multidimfitcmd += ' --floatOtherPOIs 1'
    
    
    if unconstrained:
        if not xVar == "r":
            multidimfitcmd += " --redefineSignalPOIs " + xVar
            if not yVar == "deltaNLL":
                multidimfitcmd += "," + yVar
        else:
            if not yVar == "deltaNLL":
                multidimfitcmd += " --redefineSignalPOIs " + yVar
    else:
        if not xVar == "r":
            multidimfitcmd += " -P " + xVar
        if not yVar == "deltaNLL":
            multidimfitcmd += " -P " + yVar
    if bonly:
        print "will perform background-only fit"
        suffix += "_bonly"
        multidimfitcmd += " --setParameters r=0 --freezeParameters r"
        
    if not suffix == "":
        multidimfitcmd += ' -n ' + suffix
        fitresFile += suffix
    else:
        fitresFile += "Test"
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

def get_cl_value(cl):
    infile = ROOT.TFile(fitresFile)
    w = infile.Get("w")
    npois = 1
    if isinstance(w, ROOT.RooWorkspace):
        mc = w.obj("ModelConfig")
        if isinstance(mc, ROOT.RooStats.ModelConfig):
            npois = mc.GetParametersOfInterest().getSize()
    
    cldict = {
        1: {
            "68%" : 1,
            "95%" : 4
        },
        2: {
            "68%" : 2.297,
            "95%" : 5.991
        }
    }
    
    if npois in cldict:
        cls = cldict[npois]
        if cl in cls:
            return cls[cl]
        else:
            print "WARNING:\tunknown confidence level! Cannot compute errors"
    else:
        print "WARNING:\tconfidence levels for {0} POIs are unknown, cannot compute errors".format(npois)
    return None
        


def find_crossing(graph, clname, start, stop):
    cl = get_cl_value(clname)
    if graph and cl:
        stepsize = 0.0001
        deltabest = 9999
        epsilon = 1e-3
        print "looking for crossing at {0} in interval [{1}, {2}]".format(cl, start, stop)
        xbest = start
        if stop >= start:
            for i in range(0, int(abs(start - stop)/stepsize)):
                x = start + i*stepsize
                # print "\tx =", x
                yval = graph.Eval(x)
                delta = abs(cl - yval)
                # print "\tcurrent delta =", delta
                if delta < deltabest:
                    if deltabest <= epsilon:
                        print "found crossing at", xbest
                        return xbest
                    else:
                        # print "setting deltabest to", deltabest
                        deltabest = delta
                        xbest = x
        else:
            for i in range(0, int(abs(start - stop)/stepsize)):
                x = start - i*stepsize
                # print "\tx =", x
                yval = graph.Eval(x)
                delta = abs(cl - yval)
                # print "\tcurrent delta =", delta
                if delta < deltabest:
                    if deltabest <= epsilon:
                        print "found crossing at", xbest
                        return xbest
                    else:
                        # print "setting deltabest to", deltabest
                        deltabest = delta
                        xbest = x
    print "could not find crossing in interval"
    return None

def create_straight_line(val, minVal, maxVal, style, mode = "horizontal"):
    if not val == None and not minVal == None and not maxVal == None:
        print "drawing {0} line at {1} from {2} to {3}".format(mode, val, minVal, maxVal)
        if mode is "horizontal":
            line = ROOT.TLine(minVal, val, maxVal, val)
            line.SetLineStyle(style)
        elif mode is "vertical":
            line = ROOT.TLine(val, minVal, val, maxVal)
            line.SetLineStyle(style)
        else:
            print "error! Did not recognize mode"
            return None
        return line
    else:
        if not val:
            print "received no value to draw line for!"
        if not minVal or not maxVal:
            print "received no bounds for line to draw in!"
        return None

def create_lines(graph, xbest, clStyles, ybest = None, ymin = None, ymax = None):
    
    clresults = {}
    
    parabel = None
    if isinstance(graph, ROOT.TGraph) and not isinstance(graph, ROOT.TGraph2D):
        npoints = graph.GetN()
        xmin = graph.GetXaxis().GetXmin()
        xmax = graph.GetXaxis().GetXmax()
        
        parabel = ROOT.TF1("parabel", "[0]*(x*x*x*x - [1]*[1]*[1]*[1]) + [2]*(x*x*x - [1]*[1]*[1]) + [3]*(x*x - [1]*[1]) + [4]*(x - [1]) + [5] + [6]*(x*x*x*x*x - [1]*[1]*[1]*[1]*[1]) + [7]*(x*x*x*x*x*x - [1]*[1]*[1]*[1]*[1]*[1])", xmin, xmax)
        parabel.SetParNames("a", "best fit x", "b", "c", "d", "best fit y")
        parabel.SetParameter(0, 0.5)
        parabel.SetParameter(2, 0.5)
        parabel.SetParameter(3, 0.5)
        parabel.SetParameter(4, 0.5)
        parabel.SetParameter(6, 0.5)
        parabel.SetParameter(7, 0.5)
        parabel.FixParameter(1, xbest)
        # parabel.SetParameter(1,xbest)
        if not ybest == None:
            parabel.FixParameter(5, ybest)
            # parabel.SetParameter(5,ybest)
        else:
            print "got no best fit y!"
            parabel.SetParameter(5, 0.5)
        parabel.SetNpx(1000)
        # parabel.SetParameter(1, xbest)
        parabel.SetLineStyle(3)
        parabel.SetLineColor(ROOT.kBlack)
        graph.Fit(parabel, "R")
        print "fitted parabola with #chi^2/ndf =", parabel.GetChisquare()/parabel.GetNDF()
        parabel.Draw("same")
    for cl in clStyles:
        lines = []
        vals = []
        if isinstance(parabel, ROOT.TF1):
            x_down = None
            x_up = None
            if not xbest == None:
                x_down = find_crossing( graph = parabel,
                                        clname = cl, 
                                        start = xbest, 
                                        stop = xmin)
                if not x_down == None:
                    vals.append(x_down)
                else:
                    vals.append("none")
                x_up = find_crossing(   graph = parabel,
                                        clname = cl, 
                                        start = xbest, 
                                        stop = xmax)
                if not x_up == None:
                    vals.append(x_up)
                else:
                    vals.append("none")
                print vals
            line_hor = create_straight_line(val = get_cl_value(cl),
                                            minVal = xmin,
                                            maxVal = xmax,
                                            mode = "horizontal",
                                            style = clStyles[cl])
            if line_hor:
                lines.append(line_hor.Clone())
            
            line_down = create_straight_line(   val = x_down,
                                                minVal = ymin,
                                                maxVal = ymax,
                                                mode = "vertical",
                                                style = clStyles[cl])                                 
            if line_down:
                lines.append(line_down.Clone())
            
            line_up = create_straight_line( val = x_up,
                                            minVal = ymin,
                                            maxVal = ymax,
                                            mode = "vertical",
                                            style = clStyles[cl])
            if line_up:
                lines.append(line_up.Clone())
            clresults[cl] = {"lines" : lines, "vals" : vals}
            
        elif isinstance(graph, ROOT.TGraph2D):
            hist = graph.GetHistogram().Clone("{0}_{1}".format(graph.GetName, cl))
            hist.SetContour(1)
            hist.SetContourLevel(0,cl)
            hist.SetLineColor(ROOT.kBlack)
            hist.SetLineWidth(3)
            lines.append(hist.Clone("clone_" + hist.GetName()))
            clresults[cl] = lines
                    
    return clresults
            
def do1DScan(limit, xVar, yVar, outputDirectory, suffix):
    cls = { "68%" : 2,  #68%
            "95%" : 3}  #95%     
    xVals = []
    yVals = []
    xbest = None
    ybest = 999999
    for e in limit:
        x = eval("e." + xVar)
        y = eval("e." + yVar)
        print "current values: {0}, {1}".format(x, y)
        
        if yVar == "deltaNLL":
            if y >= 0 and y < 10:
                print "\tsaving values {0}, {1}".format(x, 2*y)
                xVals.append(x)
                yVals.append(2*y)
        else:
            xVals.append(x)
            yVals.append(y)
            
        if yVals[-1] < ybest:
            xbest = xVals[-1]
            ybest = yVals[-1]
    
    print "found best fit point at (x, y) = ({0}, {1})".format(xbest, ybest)
    # c = helperfuncs.getCanvas()
    c = ROOT.TCanvas()
    graph = ROOT.TGraph(len(xVals))
    for i in range(len(xVals)):
        graph.SetPoint(i, xVals[i], yVals[i])
    graph.GetXaxis().SetTitle(xVar)
    if yVar == "deltaNLL":
        yVar = '2#Delta NLL'
        
    xmin = min(xVals)
    xmax = max(xVals)
    ymin = min(yVals)
    ymax = max(yVals)
    leg = helperfuncs.getLegend()
    print "creating TF1 in range [{0}, {1}]".format(xmin,xmax)
    print "y-axis range: [{0}, {1}]".format(ymin, ymax)
    graph.GetYaxis().SetTitle(yVar)
    graph.SetTitle("Scan of {0} over {1}".format(yVar, xVar))
    graph.Sort()
    graph.Draw()
    
    
    results = create_lines( graph = graph, xbest = xbest, clStyles = cls,
                            ymin = ymin, ymax = ymax, ybest = ybest)
    
    
    for cl in results:
        lines = results[cl]["lines"]
        vals = results[cl]["vals"]
        
        # print lines
        for i, line in enumerate(lines):
            if i == 0:
                label = "{0}: {1}_{2}^+{3}".format(cl, xbest, vals[0], vals[1])
                label = label.replace("_", "_{")
                label = label.replace("^", "}^{")
                label += "}"
                leg.AddEntry(line, label, "l")
            line.Draw("Same")
    bestfit = ROOT.TGraph(1)
    bestfit.SetPoint(0, xbest, ybest)

    bestfit.SetMarkerStyle(34)
    bestfit.SetMarkerSize(1.5)
    bestfit.Sort()
    bestfit.Draw("Same")
    c.Modified()
    leg.AddEntry(bestfit, "Best Fit Value", "p")
    
    leg.Draw("Same")
    filename = ("nllscan_{0}_{1}{2}").format(xVar,yVar.replace("#", "x"), suffix)
    filename = filename.replace(" ", "_")
    c.SaveAs(outputDirectory + "/" + filename + ".pdf")
    graph.SaveAs(outputDirectory + "/" + filename + ".root")

def do2DScan(limit, xVar, yVar, outputDirectory, suffix):
    cls = { "68%" : 2,  #68%
            "95%" : 3}  #95%
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
                do2DScan(   limit, xVar = xVar, yVar = yVar, 
                            outputDirectory = outputDirectory, 
                            suffix = suffix)
            else:
                do1DScan(   limit, xVar = xVar, yVar = yVar, 
                            outputDirectory = outputDirectory, 
                            suffix = suffix)


        else:
            print "Could not load limit tree!"
        infile.Close()
else:
    print "Could not find multidim fit output", fitresFile
