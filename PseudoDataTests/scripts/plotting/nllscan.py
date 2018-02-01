import ROOT
ROOT.PyConfig.IgnoreCommandLineOptions = True
import os
import sys
import glob
import subprocess
import imp
import shutil
from optparse import OptionParser

basedir = os.path.dirname(os.path.abspath(sys.argv[0]))
helperpath = os.path.join(basedir, "../base")
if not helperpath in sys.path:
    sys.path.append(helperpath)
import batchConfig
batch = batchConfig.batchConfig(queue = "short")
helperpath += "/helpfulFuncs.py"
if os.path.exists(helperpath):
    helperfuncs = imp.load_source('helpfulFuncs', helperpath)
else:
    sys.exit("Could not load helperfuncs from %s" % helperpath)
ROOT.gROOT.SetBatch(True)
pathToCMSSWsetup="/nfs/dust/cms/user/pkeicher/tth_analysis_study/CombineFitTests/PseudoDataTests/scripts/setupCMSSW_8_1_0.txt"


#======================================================================

def check_workspace(pathToDatacard):
    workspacePath = ""
    parts = pathToDatacard.split(".")
    outputPath = ".".join(parts[:len(parts)-1]) + ".root"
    if not os.path.exists(outputPath) or doWorkspaces:
        print "generating workspace for", pathToDatacard
        
        bashCmd = ["source {0} ;".format(pathToCMSSWsetup)]
        bashCmd.append("text2workspace.py -m 125 " + pathToDatacard)
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
    

#______________combine stuff_____________________________________


    fitresFile = "higgsCombine"
    
    multidimfitcmd = ['combine -M MultiDimFit ' + datacard]
    multidimfitcmd.append('--algo=grid --points=' + str(nPoints))
    # multidimfitcmd += ' --minimizerStrategy 1 --minimizerTolerance 0.3'
    # multidimfitcmd += ' --cminApproxPreFitTolerance=25'
    # multidimfitcmd += ' --cminFallbackAlgo "Minuit2,migrad,0:0.3"'
    # multidimfitcmd += ' --cminOldRobustMinimize=0'
    # multidimfitcmd += ' --X-rtd MINIMIZER_MaxCalls=9999999'
    multidimfitcmd.append('-m 125')
    if "--firstPoint=0" in additionalCmds or not any("--firstPoint" in x for x in additionalCmds):
         multidimfitcmd.append('--saveWorkspace')
    if additionalCmds:
        multidimfitcmd.append(" ".join(additionalCmds))
        
    multidimfitcmd.append('--rMin -10 --rMax 10 --saveFitResult')
    multidimfitcmd.append('--saveInactivePOI 1')
        
    if unconstrained:
        multidimfitcmd.append("--redefineSignalPOIs " + ",".join(params))
    else:
        multidimfitcmd += ["-P " + str(x) for x in params]
    if not xVar in params and not xVar == "r":
        multidimfitcmd.append("--saveSpecifiedNuis " + xVar)
    if not yVar == "deltaNLL" and not yVar == "r" and not yVar in params:
        multidimfitcmd.append("--saveSpecifiedNuis " + yVar)
    if "r" in [xVar, yVar] and not "r" in params:
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
    if os.path.exists(foldername):
        print "resetting folder for scripts"
        shutil.rmtree(foldername)
    os.makedirs(foldername)
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
        arrayid = batch.submitArrayToBatch( scripts = scripts, 
                                        arrayscriptpath = "arrayJob.sh")
    else:
        sys.exit("Unable to create any scripts!")
    
    os.chdir(base)
    
    lines = ["#!/bin/bash"]
    lines.append("pathToCMSSW="+pathToCMSSWsetup)
    lines.append('if [ -f "$pathToCMSSW" ]; then')
    lines.append('  source "$pathToCMSSW"')
    cmds = ["python"]
    cmds += sys.argv
    indeces = [i for i,x in enumerate(cmds) if (x == "-a" or x =="--addCommand")]
    for index in indeces:
        cmds[index+1] = '"{0}"'.format(cmds[index+1])
    cmds.append("--directlyDrawFrom")
    cmds.append(",".join(results))
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
        mergeid = batch.submitJobToBatch(script = mergescript, jobid = arrayid)
        sys.exit("Everything submitted! Jobids: {0} {1}".format(arrayid, mergeid))
    else:
        sys.exit("Could not write script to merge files!")

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
        
def find_crossing(graph, cl, start, stop, granularity = 1e-3):
    if graph and cl:
        stepsize = 0.0001
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

def create_lines(   graph, xbest, clStyles, granularity,
                    ybest = None, ymin = None, ymax = None):
    
    clresults = {}
    
    parabel = None
    if isinstance(graph, ROOT.TGraph) and not isinstance(graph, ROOT.TGraph2D):
        npoints = graph.GetN()
        xmin = graph.GetXaxis().GetXmin()
        xmax = graph.GetXaxis().GetXmax()
        
        parabel = create_parabola(xmin = xmin, xmax = xmax, xbest = xbest, ybest = ybest)
        graph.Fit(parabel, "R")
        print "fitted parabola with #chi^2/ndf =", parabel.GetChisquare()/parabel.GetNDF()
        print "probability:", parabel.GetProb()
        parabel.Draw("same")
    for clname in clStyles:
        lines = []
        vals = []
        if isinstance(parabel, ROOT.TF1):
            x_down = None
            x_up = None
            if not xbest == None:
                x_down = find_crossing( graph = parabel,
                                        cl = get_cl_value(clname), 
                                        start = xbest, 
                                        stop = xmin,
                                        granularity = granularity)
                if not x_down == None:
                    vals.append(x_down)
                else:
                    vals.append("none")
                x_up = find_crossing(   graph = parabel,
                                        cl = get_cl_value(clname), 
                                        start = xbest, 
                                        stop = xmax,
                                        granularity = granularity)
                if not x_up == None:
                    vals.append(x_up)
                else:
                    vals.append("none")
                print vals
            line_hor = create_straight_line(val = get_cl_value(clname),
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
            
        elif isinstance(graph, ROOT.TGraph2D):
            hist = graph.GetHistogram().Clone("{0}_{1}".format(graph.GetName, clname))
            hist.SetContour(1)
            hist.SetContourLevel(0,get_cl_value(cl = clname))
            hist.SetLineColor(ROOT.kBlack)
            hist.SetLineWidth(3)
            hist.SetLineStyle(clStyles[clname])
            lines.append(hist.Clone("clone_" + hist.GetName()))
            clresults[clname] = lines
                    
    return clresults

def save_output(canvas, graph, name):
    canvas.SaveAs(name + ".pdf")
    canvas.SaveAs(name+".png")
    canvas.SaveAs(name + "_canvas.root")
    graph.SaveAs(name + ".root")

def do1DScan(   limit, xVar, yVar, outputDirectory, suffix, granularity,
                xtitle = None, ytitle = None):
    cls = { "68%" : 2,  #68%
            "95%" : 3}  #95%     
    if xtitle == None:
        xtitle = xVar
    if ytitle == None:
        ytitle = yVar
    xVals = []
    yVals = []
    xbest = None
    ybest = 999999
    for i, e in enumerate(limit):
        x = eval("e." + xVar)
        y = eval("e." + yVar)
        # print "current values: {0}, {1}".format(x, y)
        
        if yVar == "deltaNLL":
            if y >= 0 and y < 10:
                # print "\tsaving values {0}, {1}".format(x, 2*y)
                xVals.append(x)
                yVals.append(2*y)
        else:
            xVals.append(x)
            yVals.append(y)
            
        if i == 0:
            xbest = xVals[-1]
            ybest = yVals[-1]
    
    print "found best fit point at (x, y) = ({0}, {1})".format(xbest, ybest)
    
    # c = helperfuncs.getCanvas()
    c = ROOT.TCanvas()
    graph = ROOT.TGraph(len(xVals))
    for i in range(len(xVals)):
        graph.SetPoint(i, xVals[i], yVals[i])
    graph.GetXaxis().SetTitle(xtitle)
    if ytitle == "deltaNLL":
        ytitle = '2#Delta NLL'
        
    xmin = min(xVals)
    xmax = max(xVals)
    ymin = min(yVals)
    ymax = max(yVals)
    leg = helperfuncs.getLegend()
    
    graph.GetYaxis().SetTitle(ytitle)
    graph.SetTitle("Scan of {0} over {1}".format(ytitle, xtitle))
    graph.Sort()
    graph.Draw()
    if ytitle == '2#Delta NLL':
        print "creating TF1 in range [{0}, {1}]".format(xmin,xmax)
        print "y-axis range: [{0}, {1}]".format(ymin, ymax)
        results = create_lines( graph = graph, xbest = xbest, 
                                clStyles = cls, ymin = ymin, 
                                ymax = ymax, ybest = ybest,
                                granularity = granularity
                                )
        for cl in results:
            lines = results[cl]["lines"]
            vals = results[cl]["vals"]
            
            # print lines
            for i, line in enumerate(lines):
                if i == 0:
                    label = "{0}: {1}".format(cl, round(xbest,3))
                    
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
                
    bestfit = ROOT.TGraph(1)
    bestfit.SetPoint(0, xbest, ybest)

    bestfit.SetMarkerStyle(34)
    bestfit.SetMarkerSize(1.8)
    bestfit.Sort()
    bestfit.Draw("P")
    c.Modified()
    leg.AddEntry(bestfit, "Best Fit Value", "p")
    
    leg.Draw("Same")
    filename = "nllscan_{0}_{1}{2}".format(xtitle,ytitle, suffix)
    filename = filename.replace("#", "")
    filename = filename.replace(" ", "_")
    filename = outputDirectory + "/" + filename
    
    save_output(canvas = c, graph = graph, name = filename)
    
def do2DScan(   limit, xVar, yVar, outputDirectory, suffix, 
                xtitle= None, ytitle = None):
    cls = { "68%" : 2,  #68%
            "95%" : 3}  #95%
    if xtitle == None:
        xtitle = xVar
    if ytitle == None:
        ytitle = yVar
    xVals = []
    yVals = []
    zVals = []
    xbest = 0
    ybest = 0
    zbest = 99999999
    for i, e in enumerate(limit):
        x = eval("e." + xVar)
        y = eval("e." + yVar)
        z = e.deltaNLL
        #print "current values: {0}, {1}".format(x, y)

        if z >= 0 and z < 10:
            #print "saving values {0}, {1}".format(x, 2*y)
            xVals.append(x)
            yVals.append(y)
            zVals.append(2*z)
            if i == 0:
                xbest = xVals[-1]
                ybest = yVals[-1]
                zbest = zVals[-1]

    c = ROOT.TCanvas()

    graph = ROOT.TGraph2D()
    for i in range(len(xVals)):
        graph.SetPoint(i, xVals[i], yVals[i], zVals[i])
    bestfit = ROOT.TGraph()
    bestfit.SetPoint(0, xbest, ybest)
    bestfit.SetMarkerStyle(34)
    bestfit.SetMarkerSize(1)
    graph.GetHistogram().GetXaxis().SetTitle(xtitle)
    graph.GetHistogram().GetYaxis().SetTitle(ytitle)
    graph.GetHistogram().GetZaxis().SetTitle("2#Delta NLL")
    graph.SetTitle("Scan of {0} over {1}".format(ytitle, xtitle))
    graph.Draw("COLZ")
    bestfit.Draw("P")
    label = helperfuncs.getLegend()
    label.AddEntry(bestfit, "Best Fit Value", "p")
    contours = []
    for cl in cls:
        contourname = "countour_" + cl
        contourname = contourname.replace("%", "")
        contours.append(graph.GetHistogram().Clone(contourname))
        contours[-1].SetContour(1)
        contours[-1].SetContourLevel(0,get_cl_value(cl = cl))
        contours[-1].SetLineStyle(cls[cl])
        contours[-1].SetLineWidth(3)
        contours[-1].Draw('same cont3')
        label.AddEntry(contours[-1], "{0} CL".format(cl), "l")
    
    label.Draw("Same")
    c.SetMargin(0.25, 0.15, 0.15, 0.08);
    filename = ("nllscan_2D_{0}_{1}{2}").format(xtitle,ytitle, suffix)
    filename = filename.replace("#", "")
    filename = filename.replace(" ", "_")
    filename = outputDirectory + "/" + filename
    
    save_output(canvas = c, graph = graph, name = filename)

def merge_files(filelist):
    cmd = "hadd -f merged_combine_output.root " + " ".join(filelist)
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
    parser.add_option("--bonly", help = "perform background only fit (creates multiSignalModel where signal strength is not mapped to anything)", action = "store_true", default = False, dest = "bonly")
    parser.add_option("-w", "--workspace", dest = "workspace", metavar = "path/to/workspace", help = "path to workspace to use for fit")
    parser.add_option("--scanUnconstrained", dest = "unconstrained", action = "store_true", default = False, help = "Drop constraints for scaned parameters")
    parser.add_option("--doWorkspaces", dest = "doWorkspaces", action = "store_true", default = False, help = "Force creation of workspaces even if they exist already (default = false)")
    parser.add_option("-p", "--paramsToScan", metavar = "par1,par2,...", dest = "paramsToScan", help = "scan these parameters. Default is scanning x (and y if '--scan2D'). Can be used multiple times", action = "append")
    parser.add_option("--runLocally", help = "do not perform fits on batch system (default = false)", dest = "runLocally", action = "store_true", default = False)
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
    

    if directDrawPath == None:
        
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
        
        if "*" in directDrawPath:
            filelist = glob.glob(directDrawPath)
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
                                ytitle = ytitle)
                else:
                    do1DScan(   limit, xVar = xVar, yVar = yVar, 
                                outputDirectory = outputDirectory, 
                                suffix = suffix,
                                granularity = granularity,
                                xtitle = xtitle,
                                ytitle = ytitle)
    
    
            else:
                print "Could not load limit tree!"
            infile.Close()
    else:
        print "Could not find multidim fit output", fitresFile
    os.chdir(workdir)
