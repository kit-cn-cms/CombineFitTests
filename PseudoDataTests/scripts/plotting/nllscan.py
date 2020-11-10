import ROOT
ROOT.PyConfig.IgnoreCommandLineOptions = True
import os
import sys
import glob
import subprocess
import imp
import shutil
from optparse import OptionParser
from math import fsum

basedir = os.path.dirname(os.path.abspath(sys.argv[0]))
helperpath = os.path.join(basedir, "../base")
if not helperpath in sys.path:
    sys.path.append(helperpath)
import batchConfig
batch = batchConfig.batchConfig()
helperpath += "/helpfulFuncs.py"
if os.path.exists(helperpath):
    helperfuncs = imp.load_source('helpfulFuncs', helperpath)
else:
    sys.exit("Could not load helperfuncs from %s" % helperpath)
ROOT.gROOT.SetBatch(True)
pathToCMSSWsetup="/nfs/dust/cms/user/pkeicher/setup_combine_cmssw_new.sh"


#======================================================================

def check_workspace(pathToDatacard):
    workspacePath = ""
    parts = pathToDatacard.split(".")
    outputPath = ".".join(parts[:len(parts)-1]) + ".root"
    if not os.path.exists(outputPath) or doWorkspaces:
        print "generating workspace for", pathToDatacard
        
        bashCmd = ["source {0} ;".format(pathToCMSSWsetup)]
        bashCmd.append("text2workspace.py -m 125.38 " + pathToDatacard)
        bashCmd.append("-o " + outputPath)
        print bashCmd
        subprocess.call([" ".join(bashCmd)], shell = True)
   
    workspacePath = outputPath
   
    if os.path.exists(workspacePath):
        f = ROOT.TFile(workspacePath)
        if not (f.IsOpen() and not f.IsZombie() and not f.TestBit(ROOT.TFile.kRecovered)):
            workspacePath = ""
        else:
            test = f.Get("w")
            if not isinstance(test, ROOT.RooWorkspace):
                print "could not find workspace in", workspacePath
                workspacePath = ""
    else:
        print "could not find", workspacePath
        workspacePath = ""
    return workspacePath

def make_mdf_command(   datacard, nPoints, unconstrained, params, xVar,
                        yVar, bonly, suffix, additionalCmds = None):
    """#______________combine stuff______________________________"""


    fitresFile = "higgsCombine"
    
    multidimfitcmd = ['combine -M MultiDimFit ' + datacard]
    multidimfitcmd.append('--algo=grid --points=' + str(nPoints))
    # multidimfitcmd += ' --minimizerStrategy 1 --minimizerTolerance 0.3'
    # multidimfitcmd += ' --cminApproxPreFitTolerance=25'
    # multidimfitcmd += ' --cminFallbackAlgo "Minuit2,migrad,0:0.3"'
    # multidimfitcmd += ' --cminOldRobustMinimize=0'
    # multidimfitcmd += ' --X-rtd MINIMIZER_MaxCalls=9999999'
    multidimfitcmd.append('-m 125.38')
    if "--firstPoint=0" in additionalCmds or not any("--firstPoint" in x for x in additionalCmds):
         multidimfitcmd.append('--saveWorkspace')
    if additionalCmds:
        multidimfitcmd.append(" ".join(additionalCmds))
        
    # multidimfitcmd.append('--rMin -10 --rMax 10')
    multidimfitcmd.append('--saveFitResult')
    multidimfitcmd.append('--saveInactivePOI 1')
        
    if unconstrained:
        multidimfitcmd.append("--redefineSignalPOIs " + ",".join(params))
    else:
        multidimfitcmd += ["-P " + str(x) for x in params]
    if not xVar in params and not xVar == "r":
        multidimfitcmd.append("--trackParameters " + xVar)
    if not yVar == "deltaNLL" and not yVar == "r" and not yVar in params:
        multidimfitcmd.append("--trackParameters " + yVar)
    if "r" in [xVar, yVar] and not "r" in params and not any("--floatOtherPOIs 1" in x for x in multidimfitcmd):
        multidimfitcmd.append("--floatOtherPOIs 1")
    if bonly:
        print "will perform background-only fit"
        suffix += "_bonly"
        multidimfitcmd.append("--setParameters r=0 --freezeParameters r")
        
    if not suffix == "":
        multidimfitcmd.append("-n " + suffix)
        fitresFile += suffix
    else:
        fitresFile += "Test"
    mdfcmd = " ".join(multidimfitcmd)
    mdfcmd = mdfcmd.replace("  ", " ")
    print mdfcmd

    fitresFile = os.path.abspath(fitresFile + ".MultiDimFit.mH125.root")
    return fitresFile, mdfcmd

def make_script(low, up, datacard, nPoints, unconstrained, params, xVar,
                yVar, bonly, suffix, additionalCmds = None):
    
    print "creating script for points {0} to {1}".format(low, up)
    currentSuffix = suffix + "_{0}_to_{1}".format(low, up)
    print currentSuffix
    currentCmds = ["--firstPoint=" + str(low)]
    currentCmds.append("--lastPoint="+ str(up))
    if additionalCmds:
        currentCmds += additionalCmds
    script = "do_fits" + currentSuffix + ".sh"
    result, mdfcmd = make_mdf_command(  datacard = datacard, 
                                        nPoints = nPoints, 
                                        unconstrained = unconstrained, 
                                        params = params, xVar = xVar,
                                        yVar = yVar, bonly = bonly,
                                        suffix = currentSuffix, 
                                        additionalCmds = currentCmds)
    
    lines = ["#!/bin/bash"]
    lines.append("ulimit -s unlimited")
    lines.append("pathToCMSSW="+pathToCMSSWsetup)
    lines.append('if [ -f "$pathToCMSSW" ]; then')
    lines.append('  source "$pathToCMSSW"')
    lines.append("  cmd='{0}'".format(mdfcmd))
    lines.append('  echo "$cmd"')
    lines.append('  eval "$cmd"')

    lines.append('else')
    lines.append('  echo "could not find CMSSW source path!"')
    lines.append('fi')
    
    with open(script, "w") as output:
        output.write("\n".join(lines))
    
    return result, script

def do_fits():
    foldername = "fit_parts"
    helperfuncs.check_for_reset(foldername)
    base = os.getcwd()
    os.chdir(foldername)
    scripts = []
    results = []
    devision = int(nPoints / nPointsPerJob)
    print range(devision)
    for i in range(devision):
        low = nPointsPerJob*i
        up = nPointsPerJob*(i+1)
        resfile, script = make_script(  low = low, up = up,
                                        datacard = datacard,
                                        nPoints = nPoints, 
                                        unconstrained = unconstrained,
                                        params = params, xVar = xVar,
                                        yVar = yVar, bonly = bonly, 
                                        suffix = suffix, 
                                        additionalCmds = additionalCmds)
        results.append(resfile)
        if os.path.exists(script):
            scripts.append(script)
    low = devision * nPointsPerJob
    up = low + nPoints % nPointsPerJob
    if low != up:
        resfile, script = make_script(  low = low, up = up,
                                        datacard = datacard,
                                        nPoints = nPoints, 
                                        unconstrained = unconstrained,
                                        params = params, xVar = xVar,
                                        yVar = yVar, bonly = bonly, 
                                        suffix = suffix, 
                                        additionalCmds = additionalCmds)
        results.append(resfile)
        if os.path.exists(script):
            scripts.append(script)
    if len(scripts) != 0:
        # batch.runtime = 
        arrayid = batch.submitArrayToBatch( scripts = scripts, 
                                        arrayscriptpath = "arrayJob.sh")
    else:
        sys.exit("Unable to create any scripts!")
    
    os.chdir(base)
    
    lines = ["#!/bin/bash"]
    lines.append("ulimit -s unlimited")
    lines.append("pathToCMSSW="+pathToCMSSWsetup)
    lines.append('if [ -f "$pathToCMSSW" ]; then')
    lines.append('  source "$pathToCMSSW"')
    cmds = ["python"]
    cmds += sys.argv
    indeces = [i for i,x in enumerate(cmds) if (x == "-a" or x =="--addCommand" or x == "--setXtitle" or x == "--setYtitle")]
    for index in indeces:
        cmds[index+1] = '"{0}"'.format(cmds[index+1])
    index = None
    if "-o" in cmds:
        index = cmds.index("-o")
    elif "--outputDirectory" in cmds:
        index = cmds.index("--outputDirectory")
    if index != None:
        cmds = cmds[:index] + cmds[index+2:]
    cmds.append("--directlyDrawFrom")
    # cmds.append(",".join(results))
    cmds.append('"{0}/higgsCombine*.MultiDimFit.*.root"'.format(foldername))
    cmds.append("--runLocally")
    cmd = " ".join(cmds)
    lines.append("  cmd='{0}'".format(cmd))
    lines.append('  echo "$cmd"')
    lines.append('  eval "$cmd"')

    lines.append('else')
    lines.append('  echo "could not find CMSSW source path!"')
    lines.append('fi')
    
    mergescript = "merge_files.sh"
    
    with open(mergescript,"w") as out:
        out.write("\n".join(lines))
    if os.path.exists(mergescript):
        logdir = os.path.join(os.path.dirname(mergescript),)
        mergeid = batch.submitJobToBatch(script = mergescript, jobid = arrayid)
        sys.exit("Everything submitted! Jobids: {0} {1}".format(arrayid, mergeid))
    else:
        sys.exit("Could not write script to merge files!") 

def get_cl_value(cl, npois = None, ybest = None):
    """
    Find confidence level value \Delta\chi^2/2*\Delta\ln L for
    specific coverage probability. 
    Source: http://pdg.lbl.gov/2017/reviews/rpp2017-rev-statistics.pdf
    If requested probabilty is not defined in the dictionaries, the
    return value is None.
    
    cl      -   coverage probability string (e.g. 68%)
    """
    if not npois:
        npois = 1
        #open workspace to get ModelConfig

        infile = ROOT.TFile(fitresFile)
        w = infile.Get("w")
        if isinstance(w, ROOT.RooWorkspace):
            mc = w.obj("ModelConfig")
            if isinstance(mc, ROOT.RooStats.ModelConfig):
                npois = mc.GetParametersOfInterest().getSize()
    
    #confidence level dictionary. Keys are the number of fitted POI
    cldict = {
        1: {
            "68.27%"       : 1,
            "90%"       : 2.71,
            "95%"       : 3.84,
            "95.45%"    : 4,
            "99%"       : 6.63,
            "99.73%"    : 9
        },
        2: {
            "68.27%"       : 2.30,
            "90%"       : 4.61,
            "95%"       : 5.99,
            "95.45%"    : 6.18,
            "99%"       : 9.21,
            "99.73%"    : 11.83
        },
        3: {
            "68.27%"       : 3.53,
            "90%"       : 6.25,
            "95%"       : 7.82,
            "95.45%"    : 8.03,
            "99%"       : 11.34,
            "99.73%"    : 14.16
        }
    }
    
    #check if #POIs is in the confidence level dictionary
    if npois in cldict:
        #check if requested coverage probability is in the dictionary
        cls = cldict[npois]
        if cl in cls:
            return cls[cl] + ybest if not ybest is None else cls[cl]
        else:
            print "WARNING:\tunknown confidence level! Cannot compute errors"
    else:
        print "WARNING:\tconfidence levels for {0} POIs are unknown, cannot compute errors".format(npois)
    return None
        
def find_crossing(graph, cl, start, stop, granularity = 1e-3):
    if graph and cl:
        stepsize = 1e-6
        deltabest = 9999
        epsilon = granularity
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

def create_contours(h, clStyles):
    lines = []
    for clname in clStyles:
        hist = graph.GetHistogram().Clone("{0}_{1}".format(graph.GetName(), clname))
        hist.SetContour(1)
        hist.SetContourLevel(0,get_cl_value(cl = clname))
        hist.SetLineColor(ROOT.kBlack)
        hist.SetLineWidth(3)
        hist.SetLineStyle(clStyles[clname])
        lines.append(hist.Clone("clone_" + hist.GetName()))
    return lines

def create_parabola(xmin, xmax, xbest, ybest=None):
    
    # formular = formular.replace("\n", "")
    # while "  " in formular: formular = formular.replace("  ", " ")
    # print formular
    # sys.exit(0)
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
    return parabel

def create_lines_from_RooFitResult(var, pathToErrors, xmin, xmax, ymin,ymax, ybest=None, bonly = False):
    clresults = {}
    style = 2
    clname = "68.27%"
    if intact_root_file(pathToErrors):
        f = ROOT.TFile.Open(pathToErrors)
        if bonly:
            fit = f.Get("fit_b")
        else:
            fit = f.Get("fit_s")
        if fit == None:
            fit = f.Get("fit_mdf")
        if isinstance(fit, ROOT.RooFitResult):
            results = fit.floatParsFinal().find(var)
            if isinstance(results, ROOT.RooRealVar):

                x_down = results.getVal() + results.getErrorLo()
                x_up = results.getVal() + results.getErrorHi()
                vals = [x_down, x_up]
                print results.getErrorLo(), "\t", results.getErrorHi()
                print vals
                lines = []
                line_hor = create_straight_line(val = get_cl_value(cl = clname, npois = 1, ybest = ybest),
                                                                minVal = xmin,
                                                                maxVal = xmax,
                                                                mode = "horizontal",
                                                                style = style)
                if line_hor:
                    lines.append(line_hor.Clone())
                
                line_down = create_straight_line(   val = x_down,
                                                    minVal = ymin,
                                                    maxVal = ymax,
                                                    mode = "vertical",
                                                    style = style)                                 
                if line_down:
                    lines.append(line_down.Clone())
                
                line_up = create_straight_line( val = x_up,
                                                minVal = ymin,
                                                maxVal = ymax,
                                                mode = "vertical",
                                                style = style)
                if line_up:
                    lines.append(line_up.Clone())
                clresults[clname] = {"lines" : lines, "vals" : vals}
            else:
                print "could not load var {0} from {1}".format(var, pathToErrors)
            fit.Delete()
        else:
            print "could not load RooFitResult object from", pathToErrors
    else:
        print "root file is damaged!"
    return clresults

def create_lines(   graph, xbest, clStyles, granularity, ybest = None,
                    ymin = None, ymax = None):
    
    clresults = {}
    
    parabel = None
    if isinstance(graph, ROOT.TGraph) or isinstance(graph, ROOT.TProfile):
        xmin = graph.GetXaxis().GetXmin()
        xmax = graph.GetXaxis().GetXmax()
        
        parabel = create_parabola(xmin = xmin, xmax = xmax, xbest = xbest, ybest = ybest)
        graph.Fit(parabel, "R")
        if parabel.GetNDF() != 0:
            print "fitted parabola with #chi^2/ndf =", parabel.GetChisquare()/parabel.GetNDF()

        print "probability:", parabel.GetProb()
        parabel.Draw("same")
        for clname in clStyles:
            lines = []
            vals = []
            if isinstance(parabel, ROOT.TF1):
                x_down = None
                x_up = None
                cl = get_cl_value(clname, 1, ybest)
                if not xbest == None:
                    x_down = find_crossing( graph = parabel,
                                            cl = cl, 
                                            start = xbest, 
                                            stop = xmin,
                                            granularity = granularity)
                    if not x_down == None:
                        vals.append(x_down)
                    else:
                        vals.append("none")
                    x_up = find_crossing(   graph = parabel,
                                            cl = cl, 
                                            start = xbest, 
                                            stop = xmax,
                                            granularity = granularity)
                    if not x_up == None:
                        vals.append(x_up)
                    else:
                        vals.append("none")
                    print vals
                line_hor = create_straight_line(val = cl,
                                                minVal = xmin,
                                                maxVal = xmax,
                                                mode = "horizontal",
                                                style = clStyles[clname])
                if line_hor:
                    lines.append(line_hor.Clone())
                
                line_down = create_straight_line(   val = x_down,
                                                    minVal = ymin,
                                                    maxVal = ymax,
                                                    mode = "vertical",
                                                    style = clStyles[clname])                                 
                if line_down:
                    lines.append(line_down.Clone())
                
                line_up = create_straight_line( val = x_up,
                                                minVal = ymin,
                                                maxVal = ymax,
                                                mode = "vertical",
                                                style = clStyles[clname])
                if line_up:
                    lines.append(line_up.Clone())
                clresults[clname] = {"lines" : lines, "vals" : vals}
                    
    return clresults

def save_output(canvas, graph, name):
    canvas.Print(name + ".pdf", "pdf")
    canvas.Print(name+".png", "png")
    canvas.Print(name + "_canvas.root", "root")
    # graph.SaveAs(name + ".root")



def fill_graph(graph, xVals, yVals, zVals = None):
    if isinstance(graph, ROOT.TGraph) or isinstance(graph, ROOT.TGraph2D):
        for i in range(len(xVals)):
            if zVals is not None:
                graph.SetPoint(i, xVals[i], yVals[i], zVals[i])
            else:
                print "setting point {}: ({},{})".format(i, xVals[i], yVals[i])
                graph.SetPoint(i, xVals[i], yVals[i])
    elif isinstance(graph, ROOT.TH1):
        for i in range(len(xVals)):
            if zVals is not None:
                graph.Fill(xVals[i], yVals[i], zVals[i])
            else:
                graph.Fill(xVals[i], yVals[i])
    else:
        sys.exit("could not fill graph! Aborting")

def set_titles(graph, xtitle, ytitle, ztitle = None):
    if isinstance(graph, ROOT.TGraph) or isinstance(graph, ROOT.TGraph2D):
        graph.GetHistogram().GetXaxis().SetTitle(xtitle)
        graph.GetHistogram().GetYaxis().SetTitle(ytitle)
        if ztitle is not None:
            graph.GetHistogram().GetZaxis().SetTitle(ztitle)
        
    elif isinstance(graph, ROOT.TH1):
        graph.GetXaxis().SetTitle(xtitle)
        graph.GetYaxis().SetTitle(ytitle)
        if ztitle is not None:
            graph.GetZaxis().SetTitle(ztitle)
    else:
        print "WARNING! Could not set axis titles!"
    graph.SetTitle("Scan of {0} over {1}".format(ytitle, xtitle))

def find_parameter(bl, var):
    if var in bl:
        return var
    var = "trackedParam_" + var
    if var in bl:
        return var
    msg = "Could not find variable {} in TTree!".format(var)
    msg += " Are you sure you saved it?"
    raise KeyError(msg)

def load_values(limit, xVar, yVar, nllcutoff, dim = 1):
    xVals = []
    yVals = []
    nllvals = []
    listxbest = []
    listybest = []
    xbest = None
    ybest = None
    nllbest = None
    branchlist = [x.GetName() for x in limit.GetListOfBranches()]
    xVar = find_parameter(bl = branchlist, var = xVar)
    yVar = find_parameter(bl = branchlist, var = yVar)
    for i, e in enumerate(limit):
        x = eval("e." + xVar)
        y = eval("e." + yVar)
        nll = e.deltaNLL
        print "current values: {0}, {1}".format(x, y)

        if nll <= nllcutoff:
            print "\tsaving values {0}, {1}".format(x, y)
            xVals.append(x)
            yVals.append(y)
            nllvals.append(2*nll)

        if nllbest is None or nll < nllbest:
            xbest = xVals[-1]
            ybest = yVals[-1]
            nllbest = 2*nll
    
    # if len(listxbest) == 0:
    #     xbest = None
    # else:
    #     xbest = fsum(listxbest)/len(listxbest)
    # if len(listybest) == 0:
    #     ybest = None
    # else:
    #     ybest = fsum(listybest)/len(listybest)
    print "found best fit point at (x, y) = ({0}, {1})".format(xbest, ybest)
    bestvalues = [xbest, ybest]
    if dim == 1:
        return xVals, yVals, bestvalues
    elif dim == 2:
        return xVals, yVals, nllvals, bestvalues
    else:
        raise ValueError("Scans are defined for dimensions 1 and 2!")

def do1DScan(   limit, xVar, yVar, outputDirectory, suffix, granularity,
                xtitle = None, ytitle = None, pathToErrors = None,
                doProfile = False, bonly = False, nllcutoff = 10):
    cls = { "68.27%"    : 2,  
            # "95%"       : 3
            }       
    if xtitle == None:
        xtitle = xVar
    if ytitle == None:
        ytitle = yVar
    if ytitle == "deltaNLL":
        ytitle = '2#Delta NLL'
    filename = "nllscan_{0}_{1}{2}".format(xtitle,ytitle, suffix)
    filename = helperfuncs.treat_special_chars(string = filename)
    filename = outputDirectory + "/" + filename
    if bonly:
        filename += "_bonly"
    outfile = ROOT.TFile(filename + ".root", "RECREATE")
    xVals, yVals, bestvalues = load_values(limit = limit, xVar = xVar, 
                                            yVar = yVar, nllcutoff = nllcutoff)
    xbest = bestvalues[0]
    ybest = bestvalues[1]
    # if not ybest is None and ybest != 0 and yVar == "deltaNLL":
    #     print "rebasing y values to ", ybest
    #     yVals = [y - ybest for y in yVals]
    print xVals
    print yVals
    # c = helperfuncs.getCanvas()
    xmin = min(xVals)
    xmax = max(xVals)
    
    c = ROOT.TCanvas()
    graph = None
    if doProfile:
        nx = int(round((xmax - xmin)*100,0))
        graph = ROOT.TProfile("profile1D",";;", nx, xmin, xmax)
    else:
        graph = ROOT.TGraph(len(xVals))
    fill_graph(graph = graph, xVals= xVals, yVals = yVals)
    
    leg = helperfuncs.getLegend()
    set_titles(graph = graph, xtitle = xtitle, ytitle = ytitle)
    if isinstance(graph, ROOT.TGraph):
        graph.Sort()
    for i in range(graph.GetN()):
        print("{}: ({}, {})".format(i, graph.GetX()[i], graph.GetY()[i]))
    graph.Draw()
    ymin = graph.GetYaxis().GetXmin()
    if isinstance(graph, ROOT.TGraph):
        
        ymax = graph.GetYaxis().GetXmax()
    else:
        ymax = graph.GetBinContent(graph.GetMaximumBin())
    graph.SetName("nllscan")
    outfile.WriteObject(graph, graph.GetName())
    if ytitle == '2#Delta NLL':
        print "creating TF1 in range [{0}, {1}]".format(xmin,xmax)
        print "y-axis range: [{0}, {1}]".format(ymin, ymax)
        results = {}
        if pathToErrors is not None:
            results = create_lines_from_RooFitResult(var = xVar, 
                                                    pathToErrors = pathToErrors,
                                                    xmin = xmin, xmax = xmax,
                                                    ymin = ymin, ymax = ymax,
                                                    ybest = ybest,
                                                    bonly = bonly)
        if len(results) == 0:
            results = create_lines( graph = graph, xbest = xbest, 
                                    clStyles = cls, ymin = ymin, 
                                    ymax = ymax, ybest = ybest,
                                    granularity = granularity                                )
        for cl in results:
            lines = results[cl]["lines"]
            vals = results[cl]["vals"]
            
            # print lines
            for i, line in enumerate(lines):
                if i == 0:
                    label = "{0}: {1}".format(cl, round(xbest,2))
                    
                    down = vals[0]
                    if isinstance(down, float):
                        down = round(down-xbest,2)
                        label += '_{'
                        label += "{0}".format(down)
                        label += '}'
                    up = vals[1]
                    if isinstance(up, float):
                        up = round(up-xbest,2)
                        label += '^{'
                        label += "+{0}".format(up)
                        label += '}'
                    
                    leg.AddEntry(line, label, "l")
                line.Draw("Same")
    if xbest != None and ybest != None:     
        bestfit = ROOT.TGraph(1)
        bestfit.SetPoint(0, xbest, ybest)

        bestfit.SetMarkerStyle(34)
        bestfit.SetMarkerSize(1.8)
        bestfit.Sort()
        bestfit.SetName("bestfit")
        outfile.WriteObject(bestfit, bestfit.GetName())
        bestfit.Draw("P")
        c.Modified()
        leg.AddEntry(bestfit, "Best Fit Value", "p")
    else:
        print "could not find bestfit point!"
    
    leg.Draw("Same")
        
    save_output(canvas = c, graph = graph, name = filename)
    c.SetName("canvas")
    outfile.WriteObject(c, c.GetName())
    outfile.Close()
    
def do2DScan(   limit, xVar, yVar, outputDirectory, suffix, 
                xtitle= None, ytitle = None, pathToErrors = None,
                doProfile = False, npois = None, nllcutoff = 10):
    cls = { "68.27%"    : 2,  #68%
            "95%"       : 3}  #95%
    if xtitle == None:
        xtitle = xVar
    if ytitle == None:
        ytitle = yVar
    filename = ("nllscan_2D_{0}_{1}{2}").format(xtitle,ytitle, suffix)
    filename = helperfuncs.treat_special_chars(string = filename)
    filename = os.path.join(outputDirectory, filename)
    outfile = ROOT.TFile(filename+".root", "RECREATE")
    xVals = []
    yVals = []
    zVals = []
    listxbest = []
    listybest = []
    listzbest = []
    for i, e in enumerate(limit):
        x = eval("e." + xVar)
        y = eval("e." + yVar)
        z = e.deltaNLL
        #print "current values: {0}, {1}".format(x, y)

        if z >= 0 and z < nllcutoff:
            print "saving values {0}, {1}".format(x, 2*y)
            xVals.append(x)
            yVals.append(y)
            zVals.append(2*z)
            if z == 0:
                listxbest.append(xVals[-1])
                listybest.append(yVals[-1])
                listzbest.append(zVals[-1])

    c = ROOT.TCanvas()
    xbest = fsum(listxbest)/len(listxbest)
    ybest = fsum(listybest)/len(listybest)
    zbest = fsum(listzbest)/len(listzbest)
    graph = None
    if doProfile:
        xmin = min(xVals)
        xmax = max(xVals)
        nx = int(round((xmax-xmin)*100,0))
        if nx == 0:
            nx = 10
        ymin = min(yVals)
        ymax = max(yVals)
        ny = int(round((ymax-ymin)*100,0))
        if ny == 0:
            ny = 10
        graph = ROOT.TProfile2D("profile2D", ";;", nx, xmin, xmax, ny, ymin, ymax)
    else:
        graph = ROOT.TGraph2D()
    
    fill_graph(graph = graph, xVals = xVals, yVals = yVals, zVals = zVals)
    
    bestfit = ROOT.TGraph()
    bestfit.SetPoint(0, xbest, ybest)
    bestfit.SetMarkerStyle(34)
    bestfit.SetMarkerSize(1)
    
    set_titles( graph = graph, xtitle = xtitle, ytitle = ytitle,
                ztitle = "2#Delta NLL")
    
    
    graph.Draw("COLZ")
    graph.Write("nllscan")
    bestfit.Draw("P")
    bestfit.Write("bestfit")
    label = helperfuncs.getLegend()
    label.AddEntry(bestfit, "Best Fit Value", "p")
    contours = []
    for cl in cls:
        contourname = "countour_" + cl
        contourname = contourname.replace("%", "")
        if not isinstance(graph, ROOT.TH1):
            contours.append(graph.GetHistogram().Clone(contourname))
        else:
            contours.append(graph.Clone(contourname))    
        
        contours[-1].SetContour(1)
        contours[-1].SetContourLevel(0,get_cl_value(cl = cl, npois = npois))
        contours[-1].SetLineStyle(cls[cl])
        contours[-1].SetLineWidth(3)
        contours[-1].Draw('same cont3')
        label.AddEntry(contours[-1], "{0} CL".format(cl), "l")
    
    label.Draw("Same")
    c.SetMargin(0.25, 0.15, 0.15, 0.08);
    
    c.Write("canvas")
    save_output(canvas = c, graph = graph, name = filename)
    outfile.Close()

def merge_files(filelist):
    cmd = "hadd -f merged_combine_output.root " + " ".join(filelist)
    print cmd
    subprocess.call([cmd], shell = True)
    if os.path.exists("merged_combine_output.root"):
        return os.path.abspath("merged_combine_output.root")
    else:
        sys.exit("Could not produce merged combine output file!")
def intact_root_file(infilepath):
    f = ROOT.TFile.Open(infilepath)
    if f.IsOpen() and not f.IsZombie() and not f.TestBit(ROOT.TFile.kRecovered):
        return True
    return False

def check_parameters_in_workspace(workspacepath, paramlist):
    infile = ROOT.TFile(workspacepath)
    w = infile.Get("w")
    if isinstance(w, ROOT.RooWorkspace):
        pars = w.allVars().contentsString().split(",")
        print pars
        if not all(x in pars for x in paramlist):
            sys.exit("ERROR: Not all parameters to scan exist in the given workspace in '%s'!" % workspacepath)

#=======================================================================

if __name__ == '__main__':
    
    parser = OptionParser()
    parser.add_option("-d", "--datacard", help = "path to datacard to use for toy generation (and fit if no workspace is given)", dest = "datacard", metavar = "path/to/datacard")
    parser.add_option("-a","--addCommand", help = "add option to standard combine command 'MultiDimFit' (can be used multiple times)", dest = "addCommand", action = "append")
    parser.add_option("-x", "--xVariable", help = "variable for x axis", dest="x", default="r")
    parser.add_option("-y", "--yVariable", help = "variable for y axis (default = deltaNLL)", dest="y", default = "deltaNLL")
    parser.add_option("--scan2D", help = "perform 2D NLL scan (default is 1D scan of x)", dest="scan2D", action="store_true", default=False)
    parser.add_option("--plot2D", help = "make 2D plot (default is 1D plot)", dest = "plot2D", action="store_true", default = False)
    parser.add_option("-n", "--suffix", help = "add suffix to output name", dest = "suffix", default = "")
    parser.add_option("-o", "--outputDirectory", metavar = "path/to/save/plots/in", dest = "outputDirectory", help = "path to save output plots in (default = here)", default = "./")
    parser.add_option("--directlyDrawFrom", metavar = "path/to/scan/results", dest = "directDraw", help = "skip multi dim fit and draw plots directly from this file (needs to contain limit tree)")
    parser.add_option("--points", dest = "points", metavar = "numberOfPoints", help = "number of points to scan (default = 1000)", type = "int", default = "1000")
    parser.add_option("--nPointsPerJob", dest = "nPointsPerJob", type = "int", default = 30, help = "number of points that is calculated per job (default = 30)")
    parser.add_option("--bonly", help = "perform background only fit (set signal strength to 0 and freezes it there)", action = "store_true", default = False, dest = "bonly")
    parser.add_option("-w", "--workspace", dest = "workspace", metavar = "path/to/workspace", help = "path to workspace to use for fit")
    parser.add_option("--scanUnconstrained", dest = "unconstrained", action = "store_true", default = False, help = "Drop constraints for scaned parameters")
    parser.add_option("--doWorkspaces", dest = "doWorkspaces", action = "store_true", default = False, help = "Force creation of workspaces even if they exist already (default = false)")
    parser.add_option("-p", "--paramsToScan", metavar = "par1,par2,...", dest = "paramsToScan", help = "scan these parameters. Default is scanning x (and y if '--scan2D'). Can be used multiple times", action = "append")
    parser.add_option(  "--runLocally", "-r",
                         help = "do not perform fits on batch system (default = false)", 
                         dest = "runLocally", 
                         action = "store_true", 
                         default = False)
    parser.add_option(  "-g", "--granularity",
                        help = "set granularity for cl crossing scan in 1D NLL scan (default = 1e-3)",
                        type = "float",
                        default = 1e-3,
                        dest = "granularity",
                        metavar = 1e-3
                        )
    parser.add_option(  "--setXtitle", 
                        help = "manually set title for x axis (default = xVariable)",
                        dest = "xtitle")
    parser.add_option(  "--setYtitle", 
                        help = "manually set title for x axis (default = yVariable)",
                        dest = "ytitle")
    parser.add_option(  "--loadErrorsFrom",
                        help = "load uncertainty interval from RooFitResult in this .root file",
                        dest = "pathToErrors",
                        metavar = "path/to/rootfile")
    parser.add_option(  "--doProfile",
                        help = "create intermediary TProfile. Intended to get mean nll values for multiple scans (default = False). You should use this with 'directlyDrawFrom' option",
                        action = "store_true",
                        dest = "doProfile",
                        default = False)
    parser.add_option(  "--npois",
                        help = "in 2D contour, draw cl level for this number of pois",
                        dest = "npois",
                        type = "int")
    parser.add_option(  "--nllcutoff",
                        help = "use this value as cut off for the negative log likelihood",
                        dest = "nllcutoff",
                        type = "float",
                        default = 10
    )
    (options, args) = parser.parse_args()
    
    directDrawPath = options.directDraw
    
    datacard = options.datacard
    additionalCmds = options.addCommand
    doWorkspaces = options.doWorkspaces
    
    nPoints = options.points
    nPointsPerJob = options.nPointsPerJob
    unconstrained = options.unconstrained
    paramsToScan = options.paramsToScan
    workspace = options.workspace
    plot2D = options.plot2D
    granularity = options.granularity
    pathToErrors = options.pathToErrors
    doProfile = options.doProfile
    npois = options.npois
    nllcutoff = options.nllcutoff
    
    if pathToErrors is not None:
        if not os.path.exists(pathToErrors):
            parser.error("could not find root file with uncertainties!")
        else:
            pathToErrors = os.path.abspath(pathToErrors)
    
    if directDrawPath == None:
        if not plot2D and not (additionalCmds and any("--floatOtherPOIs 1" in x for x in additionalCmds)):
            print "WARNING:\tyou might be fixing other POIs! Calculation of uncertainties might not work properly (you should use '--floatOtherPOIs 1')"

        if datacard == None:
            parser.error("datacard for toy generation/fitting must be specified!")
        else:
            datacard = os.path.abspath(datacard)
            if not os.path.exists(datacard):
                parser.error("datacard does not exist!")
        
        if workspace:
            workspace = os.path.abspath(workspace)
            if not os.path.exists(workspace):
                print "Could not find workspace, will ignore input"
                workspace = None
        
        
        if workspace:
            datacard = workspace
        else:
            check_workspace(datacard)
    
    xVar = options.x
    if xVar == None:
        parser.error("variable for x axis of scan needs to be specified!")
    
    yVar = options.y
    
    xtitle = options.xtitle
    if xtitle == None:
        xtitle = xVar
    ytitle = options.ytitle
    if ytitle == None:
        ytitle = yVar
    
    scan2D = options.scan2D
    params = []
    if not paramsToScan == None:
        temp = ",".join(paramsToScan)
        params = temp.split(",")
        if "deltaNLL" in params:
            params.pop(params.index("deltaNLL"))
    else:
        params.append(xVar)
        if yVar != "deltaNLL" and scan2D:
            params.append(yVar) 
    
    check_parameters_in_workspace(workspacepath = datacard, 
                                    paramlist = params)

    suffix = options.suffix
    outputDirectory = os.path.abspath(options.outputDirectory)
    if not os.path.exists(outputDirectory):
        parser.error("output directory does not exist!")
    
    
    if scan2D and not len(params)>1:
        parser.error("Both x and y variable must be defined for 2D NLL scan!")
    
    bonly = options.bonly
    runLocally = options.runLocally
    
    print "changing into target directory"
    
    workdir = os.getcwd()
    os.chdir(outputDirectory)
    fitresFile = None
    if directDrawPath:
        
        if "*" in directDrawPath or "?" in directDrawPath:
            filelist = glob.glob(directDrawPath)
            if len(filelist) == 1:
                subprocess.call(["cp {0} ./merged_combine_output.root".format(filelist[-1])], shell= True)
                fitresFile = "merged_combine_output.root"
            else:
                fitresFile = merge_files(filelist = filelist)
        elif "," in directDrawPath:
            existingResults = []
            results = directDrawPath.split(",")
            for result in results:
                if os.path.exists(result):
                    if intact_root_file(infilepath = result):
                        existingResults.append(result)
                else:
                    print "could not find file %s, you should run the corresponding script again" % result
            fitresFile = merge_files(filelist = existingResults)
        elif os.path.exists(directDrawPath):
            fitresFile = directDrawPath 
    
    if not fitresFile:
        if not runLocally:
            fitresFile = do_fits()
        else:
            fitresFile, mdfcmd = make_mdf_command(  datacard = datacard, 
                                        nPoints = nPoints, 
                                        unconstrained = unconstrained, 
                                        params = params, xVar = xVar,
                                        yVar = yVar, bonly = bonly,
                                        suffix = suffix, 
                                        additionalCmds = additionalCmds)
            subprocess.call([mdfcmd], shell = True)
            if not os.path.exists(fitresFile):
                sys.exit("Could not produce file %s" % fitresFile)
    
    #________________________________________________________________

    if fitresFile and os.path.exists(fitresFile):
        infile = ROOT.TFile(fitresFile)
    
        if infile.IsOpen() and not infile.IsZombie() and not infile.TestBit(ROOT.TFile.kRecovered):
            limit = infile.Get("limit")
            if isinstance(limit, ROOT.TTree):
                print "loaded limit TTree with {0} events".format(limit.GetEntries())
    
                if plot2D:
                    do2DScan(   limit, xVar = xVar, yVar = yVar, 
                                outputDirectory = outputDirectory, 
                                suffix = suffix,
                                xtitle = xtitle,
                                ytitle = ytitle,
                                pathToErrors = pathToErrors,
                                doProfile = doProfile,
                                npois = npois,
                                nllcutoff = nllcutoff)
                else:
                    do1DScan(   limit, xVar = xVar, yVar = yVar, 
                                outputDirectory = outputDirectory, 
                                suffix = suffix,
                                granularity = granularity,
                                xtitle = xtitle,
                                ytitle = ytitle,
                                pathToErrors = pathToErrors,
                                doProfile = doProfile,
                                bonly = bonly,
                                nllcutoff = nllcutoff)
    
    
            else:
                print "Could not load limit tree!"
            infile.Close()
    else:
        print "Could not find multidim fit output", fitresFile
    os.chdir(workdir)
