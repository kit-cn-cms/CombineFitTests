import ROOT
import os
import sys
import glob
import math
from optparse import OptionParser
from shutil import rmtree

ROOT.gROOT.SetBatch(True)

def loadVariable(rootfile, takeTree=False):
    val = None
    error = None
    if not takeTree:
        fit_s = rootfile.Get("fit_s")
        if not isinstance(fit_s,ROOT.RooFitResult):
            print rootfile.GetName(),"does not contain RooFitResult"
            return val, error
        # #ROOT.gDirectory.cd('PyROOT:/')
        
        if not (fit_s.status() == 0 and fit_s.covQual() == 3):
            print "something went wrong in the fit, file", rootfile.GetName()
            print "\tfit status =", fit_s.status()
            print "\tcovQual() =", fit_s.covQual()

            #print "loading values"
        var = fit_s.floatParsFinal().find("r")
        val = var.getVal()
        error = var.getError()
        fit_s.Delete()
        # pass
    else:
        tree = rootfile.Get("tree_fit_sb")
        if not isinstance(tree,ROOT.TTree):
            print rootfile.GetName(),"does not contain limit tree"
            return val, error
        # ROOT.gDirectory.cd('PyROOT:/')
        for entry in tree:
            if entry.fit_status == 0:
                val = entry.r
                error = entry.rErr
            else:
                print "fit status is not 0!"
    return val, error

def fill_value_count(dic, key):
    word = None
    if isinstance(key, list):
        key = [str(round(val, 3)) for val in key]
        word = ",".join(key)
        
    else: word = key
    if not word in dic:
        dic[word] = 0
    dic[word] += 1

def getDataObs(data_obs_dic, scatterplots_dic=None, rootfile = None, 
                scatterplots_bin_dic = None):
    total_int = 0
    nbins = 600
    if rootfile and scatterplots_dic:
        r, error = loadVariable(rootfile = rootfile)
    for cat in data_obs_dic:
        if not cat in data_obs_dic:
            print "creating data_obs_distr for cat", cat
            data_obs_dic[cat] = {}
            if scatterplots_dic:
                scatterplots_dic[cat] = {}
                scatterplots_bin_dic[cat] = {}
            
        data_obs = ROOT.gDirectory.Get(cat+"/data")
        if isinstance(data_obs, ROOT.TGraphAsymmErrors):
            #print "looping over entries"
            s = 0.
            if rootfile and scatterplots_dic:
                r, error = loadVariable(rootfile = rootfile)
            vals = []
            for i in range(data_obs.GetN()):
                val = data_obs.GetY()[i]
                if val < 0:
                    sys.exit("detected negative toy bin content in bin {0}, file {1}".format(i, rootfile.GetName()))
                if not i in data_obs_dic[cat]:
                    
                    data_obs_dic[cat][i] = {}
                    if scatterplots_dic:
                        
                        scatterplots_dic[cat][i] = {}
                        
                fill_value_count(dic = data_obs_dic[cat][i], key = val)
                vals.append(val)
                s += val
                if scatterplots_dic and not r == None:
                    fill_value_count(   dic = scatterplots_dic[cat][i],
                                        key = [r, val])
            if scatterplots_bin_dic and not r == None:
                for i in range(len(vals)):
                    for j in range(i, len(vals)):
                        key = "bin{0}_vs_bin{1}".format(i,j)
                        if not key in scatterplots_bin_dic[cat]:
                            scatterplots_bin_dic[cat][key] = {}
                        fill_value_count(scatterplots_bin_dic[cat][key],
                                            key = [vals[i],vals[j]])
            
            if not "integral" in data_obs_dic[cat]:
                
                data_obs_dic[cat]["integral"] ={}
                if scatterplots_dic:
                    scatterplots_dic[cat]["integral"] = {}

            fill_value_count(dic = data_obs_dic[cat]["integral"],
                                key = s)
            if scatterplots_dic and not r == None:
                fill_value_count(dic = scatterplots_dic[cat]["integral"],
                                    key = [r, s])
                
                # if 2 in scatterplots_dic[cat]["integral"].values():
                    # print "counting was successful!"
                    # print "current scatterplots_dic[{0}][integral]:".format(cat)
                    # print scatterplots_dic[cat]["integral"]
                    # sys.exit("debugging stop")
            total_int += s
    if scatterplots_dic and not r == None:
        if not "total_integral" in scatterplots_dic:
            scatterplots_dic["total_integral"] = {}
        fill_value_count(dic = scatterplots_dic["total_integral"],
                            key = [r, total_int])
    
def loadVariableFromFile(pathToLoad, takeTree = False):
    val = None
    error = None
    if os.path.exists(pathToLoad):
        results = ROOT.TFile(pathToLoad)
        if results.IsOpen() and not results.TestBit(ROOT.TFile.kRecovered) and not results.IsZombie():
            val, error = loadVariable(rootfile = results, takeTree = takeTree)
            results.Close()
    else:
        print "Could not load file", pathToLoad
    #return math.ceil(val*10**6)/10**6, math.ceil(error*10**6)/10**6
    return val, error

def compareVals(val1, error1, val2, error2, errormessage):
    if not (val1 == val2 and error1 == error2):
        print errormessage
        print "\tSomething is strange!"
        print "\tval1 = {0} +- {1}\n\tval2 = {2} +- {3}".format(val1, error1, val2, error2)

def compareHistos(dic1, dic2):
    print "starting comparison of histos"
    for cat in dic1:
        if cat in dic2:
            for i in dic1[cat]:
                if i in dic2[cat]:
                    hist1 = dic1[cat][i]
                    hist2 = dic2[cat][i]
                    if hist1.GetNbinsX() == hist2.GetNbinsX():
                        for xbin in range(1, hist1.GetNbinsX()+1):
                            compareVals( val1 = hist1.GetBinCenter(xbin), error1=0,
                                        val2 = hist2.GetBinCenter(xbin), error2=0,
                                        errormessage = "While checking histos {0} and {1}:\n bin {2} has different centers!".format(hist1.GetName(), hist2.GetName(), xbin))
                            compareVals( val1 = hist1.GetBinContent(xbin), error1 = 0,
                                        val2 = hist2.GetBinContent(xbin), error2 = 0,
                                        errormessage = "While checking histos {0} and {1}:\n bin {2} has different contents!".format(hist1.GetName(), hist2.GetName(), xbin))
                        compareVals(val1 = hist1.GetMean(), error1 = hist1.GetMeanError(),
                                    val2 = hist2.GetMean(), error2 = hist2.GetMeanError(),
                                    errormessage = "While checking histos {0} and {1}:\n different mean detected!".format(hist1.GetName(), hist2.GetName())
                                    )
                        compareVals(val1 = hist1.GetEntries(), error1 = 0,
                                    val2 = hist2.GetEntries(), error2 = 0,
                                    errormessage = "While checking histos {0} and {1}:\n different # entries detected!".format(hist1.GetName(), hist2.GetName())
                                    )
                    else:
                        print "WARNING\tdifferent amount of bins detected!"

def compareMus(path1, pdfOutputPath, path2 = None):

    if os.path.exists(os.path.abspath(path1)):
        path1 = os.path.abspath(path1)
    else:
        sys.exit("%s does not exist!" % path1)

    if path2:
        if os.path.exists(os.path.abspath(path2)):
            path2 = os.path.abspath(path2)
        else:
            sys.exit("%s does not exist!" % path2)
        if not path2.endswith(".root"):
            if not path2.endswith("/"):
                path2 += "/"

    if path1.endswith("/"):
        path1 += "*"
    elif not path1.endswith(".root"):
        path1 += "/*"
    
    

    if not pdfOutputPath.endswith("/"):
        pdfOutputPath += "/"
    
    print "start comparison of mus for:"
    print path1
    print path2
    
    mu1 = {}
    muError1 = {}
    mu2 = {}
    muError2 = {}

    for path in sorted(glob.glob(path1)):
        if os.path.exists(os.path.abspath(path)) and os.path.isdir(os.path.abspath(path)):
                        
            for infile in filesToCheck:
                siblingPath = path2
                pathToLoad = os.path.abspath(path)
                base = os.path.basename(pathToLoad)
                if not base.endswith(infile):
                    if not "PseudoExperiment" in base:
                        continue
                    else:
                        if siblingPath:
                            siblingPath += base + "/" + infile
                        pathToLoad += "/" + infile
                
                if os.path.exists(pathToLoad):
                    
                    r1, error1 = loadVariableFromFile(pathToLoad)
                    tree_r1, tree_error1 = loadVariableFromFile(pathToLoad, True)
                    
                    if r1 == None or error1 == None:
                        print "Could not load variable r1 or error1 from {0}/{1}".format(path, infile)
                        continue
    
                    if tree_r1 == None or tree_error1 == None:
                        print "Could not load variable tree_r1 or tree_error1 from {0}/{1}".format(path, infile)
                        continue
                    if path2:
                        if os.path.exists(siblingPath):
                            r2, error2 = loadVariableFromFile(siblingPath)
                            tree_r2, tree_error2 = loadVariableFromFile(siblingPath, True)
                            
                            if r2 == None or error2 == None:
                                print "Could not load variable r2 or error2 from {0}/{1}".format(siblingPath, infile)
                                continue
                                
                            if tree_r2 == None or tree_error2 == None:
                                print "Could not load variable tree_r2 or tree_error2 from {0}/{1}".format(siblingPath, infile)
                                continue
    
                    if not infile in mu1:
                        mu1[infile] = ROOT.TH1D("mu1_"+infile, "", 400, -10, 10)
                        muError1[infile] = ROOT.TH1D("muError1_"+infile, "", 400, -10, 10)
                        if path2:
                            mu2[infile] = ROOT.TH1D("mu2_"+infile, "", 400, -10, 10)
                            muError2[infile] = ROOT.TH1D("muError2_"+infile, "", 400, -10, 10)
    
                    mu1[infile].Fill(r1)
                    muError1[infile].Fill(error1)
                    #print infile, "in", os.path.basename(path)
                    #print "\tr1 = {0} +- {1}\n\tr2 = {2} +- {3}".format(r1, error1, r2, error2)
                    if path2:
                        mu2[infile].Fill(r2)
                        muError2[infile].Fill(error2)
                        compareVals(r1, error1, r2, error2, "while checking {0} in folder {1}: error in RooFitResult values".format(infile, os.path.basename(path)))
    
                        compareVals(tree_r1, tree_error1, tree_r2, tree_error2, "while checking {0} in folder {1}: error in TTree values".format(infile, os.path.basename(path)))
    
                    #print "\tcomparing RooFitResult and TTree values - 1"
                    #compareVals(r1, error1, tree_r1, tree_error1)
                    #print "\tcomparing RooFitResult and TTree values - 2"
                    #compareVals(r2, error2, tree_r2, tree_error2)
                else:
                    print "Error when loading paths\n{0}\n{1}".format(pathToLoad, siblingPath)

        else:
            print path, "does not exist!"
            
    for infile in mu1:
        outfile = ROOT.TFile(pdfOutputPath + "mus_"+infile, "RECREATE")
        print "computed signal strength for {5} in path {0}: mu = {1} +- {2} +- {3} +- {4}".format(path1, mu1[infile].GetMean(), mu1[infile].GetMeanError(), mu1[infile].GetRMS(), muError1[infile].GetMean(), infile)
        #print "computed signal strength for {5} in path {0}: mu = {1} +- {2} +- {3} +- {4}".format(path2, mu2[infile].GetMean(), mu2[infile].GetMeanError(), mu2[infile].GetRMS(), muError2[infile].GetMean(), infile)
        c = ROOT.TCanvas()
        mu1[infile].Draw()
        mu1[infile].Write("mu_path1")
        c.SaveAs(pdfOutputPath + "mu1_" + infile.replace(".root","") + ".pdf" )
        c.Write()
        c.Clear()
        if path2:
            mu2[infile].Draw()
            mu2[infile].Write("mu_path2")
            c.SaveAs(pdfOutputPath + "mu2_" + infile.replace(".root","") + ".pdf" )
            c.Write()
        outfile.Close()

def create_1Dhisto(histvals, histname, label=""):
    if not isinstance(histvals, dict):
        sys.exit("Error in create_1Dhisto: histvals must be dict!")
    hist = None
    print "creating 1D histogram with name", histname
    shift = -0.5
    values = [x for x in histvals]
    xmin = min(values)
    xmax = max(values)
    nbins = int((xmax - xmin))
    hist = ROOT.TH1D(histname, label, nbins, xmin+shift, xmax+shift)
    for val in histvals:
        for i in range(histvals[val]):
            hist.Fill(val)
        
    return hist

def setup_binning(xmin, xmax):
    n = 10
    if xmin == xmax:
        xmin -= 1
        xmax += 1
    else:
        n = int(xmax - xmin)
    return n, xmin, xmax

def create_2Dhisto(histvals, histname, label = "", floatX = True):
    if not isinstance(histvals, dict):
        sys.exit("Error in create_2Dhisto: histvals must be dict!")
    print "creating 2D histogram with name", histname
    x = []
    y = []
    for key in histvals:
        val_x, val_y = key.split(",")
        if floatX:
            x.append(float(val_x)) #signal strength can be floating
        else:
            x.append(int(float(val_x)))
        y.append(int(float(val_y))) #bin contents must be integers
    # x = [i[0] for i in histvals]
    # y = [i[1] for i in histvals]
    shift = -0.5
    xmin = min(x)
    xmax = max(x)
    ymin = min(y)
    ymax = max(y)
    # if ymin == ymax:
        # print "WARNING:\t ymin == ymax!"
        # print "printing y"
        # print y
        # print "printing list of keys"
        # print histvals.keys()
        # sys.exit()
    nbinsy, ymin, ymax = setup_binning(ymin, ymax)
    if floatX:
        nbinsx = 400
    else:
        nbinsx, xmin, xmax = setup_binning(xmin, xmax)
    hist = ROOT.TH2D(histname, label, nbinsx, xmin+shift, xmax+shift, nbinsy, ymin+shift, ymax+shift)
    hist.SetDirectory(0)
    for n, key in enumerate(histvals):
        for i in range(histvals[key]):
            hist.Fill(x[n], y[n])
    
    return hist

def check_for_reset(foldername):
    if os.path.exists(foldername):
        print "resetting", foldername
        rmtree(foldername)
    os.makedirs(foldername)

def save_histo(hist, pdfOutputPath, ext):
    if isinstance(hist, ROOT.TH1):
        name = hist.GetName()
        parts = name.split("_")
        name = "_".join(parts[0:len(parts)-1])
        
        # outFile = ROOT.TFile(pdfOutputPath+name+".root", "RECREATE")
        # if outFile.IsOpen():
        canvas = ROOT.TCanvas()
        
        hist.Draw()
        # hist.Write()
        # canvas.Write()
        
        canvas.Print(pdfOutputPath + "/"+name+ext)
            # outFile.Close()
        if isinstance(hist, ROOT.TH2):
            filename = pdfOutputPath
            index = -2
            if "vs" in parts:
                index = parts.index("vs")
                txtname = "_".join(parts[0:index])
                index += 1
            else:
                txtname = "_".join(parts[0:len(parts)-2])
            filename = pdfOutputPath+txtname+ "_correlations.txt"
            f = None
            if os.path.exists(filename):
                f = open(filename, "a")
            else:
                f = open(filename, "w")
            f.write("{0}\t{1}\n".format(parts[index], hist.GetCorrelationFactor()))
            f.close()
    else:
        print "not a histogram, skipping"

def print2dDictionary(data_obs_dic, pdfOutputPath, labelbase = ";"):
    print "saving 2d dictionary"
    outputdic = {pdfOutputPath + "pdfs" : ".pdf",
                    pdfOutputPath + "pngs": ".png"}
    
    for i, foldername in enumerate(outputdic):
        for cat in data_obs_dic:
            outfolder = pdfOutputPath + "/" + cat
            # if i == 0:
            #     check_for_reset(outfolder)
            if cat == "total_integral":
                    uid = ROOT.TUUID()
                    histname = "scatterplot_total_integral_"+uid.AsString()
                    label = labelbase + "total_integral"
                    hist = create_2Dhisto(histvals = data_obs_dic[cat], 
                                        histname = histname,
                                        label = label)
            else:
                for i in data_obs_dic[cat]:
                    uid = ROOT.TUUID()
                    histname = "scatterplot_" + cat+ "_" + str(i)+"_"+uid.AsString()
                    label = labelbase
                    floatX = True
                    if not isinstance(i, str):
                        label += "bin content {0}".format(i)
                    else: 
                        if label == ";" and "_vs_" in i:
                            label += i.replace("_vs_", ";")
                            floatX = False
                        else:
                            label += i
                    hist = create_2Dhisto(histvals = data_obs_dic[cat][i],
                                        histname = histname,
                                        label = label,
                                        floatX = floatX)   
            save_histo( hist = hist, 
                        pdfOutputPath = outfolder + "/",
                        ext = outputdic[foldername])

def print1dDictionary(data_obs_dic, pdfOutputPath, doFits = False, origFile = None, scatterplots = False):
    print "saving dictionary:"
    # print data_obs_dic
    for cat in data_obs_dic:
        orig_data_obs = None
        if origFile and origFile.IsOpen() and not origFile.IsZombie() and not origFile.TestBit(ROOT.TFile.kRecovered):
            orig_data_obs = origFile.Get("data_obs_finaldiscr_" + cat)
        elif origFile is not None: print "could not open original file"
        canvasname = ""
        label = ";bin content;frequency"
        for n, i in enumerate(sorted(data_obs_dic[cat])):
            if isinstance(data_obs_dic[cat][i], dict):
                # name = data_obs_dic[cat][i].GetName()
                uid = ROOT.TUUID()
                name = "data_obs_distr_"+cat+"_"+str(i)+"_"+uid.AsString()
                # print name
                hist = create_1Dhisto(histvals = data_obs_dic[cat][i],
                                    histname = name,
                                    label = label)       
                print "current name:", name
                parts = name.split("_")
                name = "_".join(parts[0:len(parts)-2])
                
                outFile = ROOT.TFile(pdfOutputPath+"_".join(parts[0:len(parts)-1])+".root", "RECREATE")
                canvas = ROOT.TCanvas()
    
                hist.Draw()
                if doFits:
                    fit = ROOT.TF1("fit", "[0]*TMath::Poisson(x, [1]) + [2]", hist.GetBinCenter(1), hist.GetBinCenter(hist.GetNbinsX()))
                    fit.SetParNames("Amp", "Mean", "Offset")
                    fit.SetParameters(hist.GetEntries(), hist.GetMean(), 0)
                    fit.SetNpx(500)
                    
                    hist.Fit(fit,"RL")
                    fit.Draw("Same")
                
                if not canvasname:
                    canvasname = pdfOutputPath+name.replace("_{0}".format(i), "")+".pdf"
                
                if isinstance(orig_data_obs, ROOT.TH1) and not isinstance(i, str):
                    val = orig_data_obs.GetBinContent(i+1)
                    leg = ROOT.TLegend(0.1,0.7,0.3,0.9)
                    line = ROOT.TLine(val, 0, val, hist.GetMaximum())
                    line.SetLineWidth(3)
                    leg.AddEntry(line,"orig_val = {0}".format(val),"l")
                    line.Draw("Same")
                    leg.Draw("Same")
                    
                    
                elif isinstance(orig_data_obs, ROOT.TH1) and isinstance(i, str):
                    print "saving distribution for toy yields"
                    val = orig_data_obs.Integral()
                    leg = ROOT.TLegend(0.1,0.7,0.3,0.9)
                    line = ROOT.TLine(val, 0, val, hist.GetMaximum())
                    line.SetLineWidth(3)
                    leg.AddEntry(line,"orig_val = {0}".format(val),"l")
                    line.Draw("Same")
                    leg.Draw("Same")
                else:
                    print "could not load data_obs object \'{0}\' from original file".format("data_obs_finaldiscr_" + cat)
                if n == 0:
                    canvas.Print(canvasname + "(","pdf")
                elif n == len(data_obs_dic[cat])-1:
                    canvas.Print(canvasname + ")","pdf")
                else:
                    canvas.Print(canvasname,"pdf")
                canvas.Write()
                #data_obs_dic[cat].SetDirectory(outFile)
                hist.Write()
                outFile.Close()

def getDataObss(pathToPseudoExps, dataObsFile, pdfOutputPath = "./", suffix = None, doScatterPlots = False):
    if os.path.exists(os.path.abspath(pathToPseudoExps)):
        pathToPseudoExps = os.path.abspath(pathToPseudoExps)
    else:
        sys.exit("%s does not exist!" % pathToPseudoExps)
    
    path1 = pathToPseudoExps
    if not path1.endswith(".root"):
         path1 += "/*"
    data_obs_dic = {}
    scatterplots_dic = None
    scatterplots_bin_dic = None
    if doScatterPlots:
        print "creating scatter plots dic"
        scatterplots_dic = {}
        scatterplots_bin_dic = {}
    if dataObsFile:
        dataObsFile = os.path.abspath(dataObsFile)
    else:
        dataObsFile = ""
    if pdfOutputPath:
        pdfOutputPath = os.path.abspath(pdfOutputPath)
        pdfOutputPath += "/"
    if suffix:
        pdfOutputPath += suffix + "_"

    for path in sorted(glob.glob(path1)):
        if os.path.exists(os.path.abspath(path)):
            path = os.path.abspath(path)

            for infile in filesToCheck:
                base = os.path.basename(path)
                # print "base:", base
                if not ("PseudoExperiment" in base or base.endswith(infile)):
                    continue
                print "checking", path
                
                pathToFitResults = path
                if not pathToFitResults.endswith(infile):
                    pathToFitResults += "/"+infile
                rootFile = ROOT.TFile(pathToFitResults)
                if rootFile.IsOpen() and not rootFile.IsZombie() and not rootFile.TestBit(ROOT.TFile.kRecovered):
                    # print "cd into shapes_prefit"
                    rootFile.cd("shapes_prefit")
                    listOfKeys = ROOT.gDirectory.GetListOfKeys()
                    for key in listOfKeys:
                        if key.IsFolder():
                            cat = key.GetName()
                            if not cat in data_obs_dic:
                                print "creating data_obs_distr for cat", cat
                                data_obs_dic[cat] = {}
                                if scatterplots_dic is not None:
                                    print "creating scatter plot dict for cat", cat
                                    scatterplots_dic[cat] = {}
                                if scatterplots_bin_dic is not None:
                                    print "creating scatter plot dict for cat", cat
                                    scatterplots_bin_dic[cat] = {}

                    getDataObs( data_obs_dic= data_obs_dic, 
                                scatterplots_dic = scatterplots_dic, 
                                rootfile = rootFile,
                                scatterplots_bin_dic = scatterplots_bin_dic)

                    rootFile.Close()
                    # print "done with", path+"/"+infile
                else:
                    print "Could not read {0} in {1}".format(infile, path)

    origFile = ROOT.TFile(dataObsFile)
    ROOT.gStyle.SetOptStat(221112211)
    ROOT.gStyle.SetOptFit(11111);
    print1dDictionary(data_obs_dic = data_obs_dic, pdfOutputPath = pdfOutputPath, doFits = True, origFile = origFile)
    origFile.Close()
    if scatterplots_dic:
        print "printing scatter plots"
        print2dDictionary(data_obs_dic = scatterplots_dic, pdfOutputPath = pdfOutputPath, labelbase = ";r;")
        if scatterplots_bin_dic:
            print "printing bin scatter plots"
            print2dDictionary(data_obs_dic = scatterplots_bin_dic, pdfOutputPath = pdfOutputPath)
    return data_obs_dic

if __name__ == '__main__':
    
    filesToCheck = ["fitDiagnostics.root"]#, "mlfit_MS_mlfit.root"]


    parser = OptionParser()
    
    parser.add_option(  "-m", "--mainPath",
                        help = "first path to get toys and mus from",
                        dest = "main",
                        metavar = "main/path/to/fit/results"
                    )
    parser.add_option(  "-s", "--secondPath",
                        help = "path to toys which are to be compared to main path",
                        dest = "second",
                        metavar = "second/path/to/compare"
                        )
    parser.add_option(  "-r", "--dataObsFile",
                        help = "path to root file with original data_obs",
                        dest = "dataObsFile",
                        metavar = "path/to/data_obs/file")
    parser.add_option(  "-o", "--outputPDFpath",
                        help = "path in which pdf output should be printed (default: here)",
                        dest = "outputPath",
                        default = "./",
                        metavar = "path/for/pdf/output")
    parser.add_option(  "--doScatterPlots",
                        help = "create scatter plots bin content vs. r and integral vs. r",
                        dest = "doScatterPlots",
                        action = "store_true",
                        default = False)
    parser.add_option(  "--comparePostfitMu", "-c",
                        help = "collect postfit signal strengths in histogram (default = False)",
                        dest = "collectMu",
                        action = "store_true",
                        default = False)
    
    (options, args) = parser.parse_args()
    
    path1           = options.main
    path2           = options.second
    pathToRootfile  = options.dataObsFile
    pdfOutputPath   = options.outputPath
    doScatterPlots  = options.doScatterPlots
    collectMu       = options.collectMu
    
    if not path1 or not os.path.exists(os.path.abspath(path1)):
        parser.error('Path "{0}" does not exist!'.format(path1))
    else:
        path1 = os.path.abspath(path1)
    
    if path2 and os.path.exists(os.path.abspath(path2)):
        path2 = os.path.abspath(path2)
        print "start comparison between\npath1: {0}\npath2: {1}".format(path1, path2)
        data_obs_dic_1 = getDataObss( pathToPseudoExps = path1, 
                dataObsFile = pathToRootfile, 
                pdfOutputPath = pdfOutputPath,
                suffix = "path1",
                doScatterPlots = doScatterPlots)
        data_obs_dic_2 = getDataObss( pathToPseudoExps = path2, 
                dataObsFile = pathToRootfile, 
                pdfOutputPath = pdfOutputPath,
                suffix = "path2",
                doScatterPlots = doScatterPlots)
        compareHistos(dic1 = data_obs_dic_1, dic2 = data_obs_dic_2)
        if collectMu:
            compareMus(path1 = path1, path2 = path2, pdfOutputPath = pdfOutputPath)
    else:
        print "could not find second path, only analyse data in path1"
        getDataObss( pathToPseudoExps = path1, 
                dataObsFile = pathToRootfile, 
                pdfOutputPath = pdfOutputPath,
                doScatterPlots = doScatterPlots)
        if collectMu:
            compareMus(path1 = path1, pdfOutputPath = pdfOutputPath)
    
