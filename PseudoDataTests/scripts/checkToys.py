import ROOT
import os
import sys
import glob
import math
from optparse import OptionParser

ROOT.gROOT.SetBatch(True)

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

(options, args) = parser.parse_args()

path1           = options.main
path2           = options.second
pathToRootfile  = options.dataObsFile
pdfOutputPath   = options.outputPath

if not path1 or not os.path.exists(os.path.abspath(path1)):
    parser.error('Path "{0}" does not exist!'.format(path1))
else:
    path1 = os.path.abspath(path1)


def getDataObs(data_obs_dic):
    for cat in data_obs_dic:
        if not cat in data_obs_dic:
            print "creating data_obs_distr for cat", cat
            data_obs_dic[cat] = {}
            
        data_obs = ROOT.gDirectory.Get(cat+"/data")
        if isinstance(data_obs, ROOT.TGraphAsymmErrors):
            #print "looping over entries"
            s = 0.
            for i in range(data_obs.GetN()):
                val = data_obs.GetY()[i]
                if not i in data_obs_dic[cat]:
                    
                    uid = ROOT.TUUID()
                    data_obs_dic[cat][i] = ROOT.TH1D("data_obs_distr_"+cat+"_"+str(i)+"_"+uid.AsString(), ";bin content;frequency", 400, val-10*(val)**(1./2.),val+30*(val)**(1./2.))
                    data_obs_dic[cat][i].SetDirectory(0)
                    
                data_obs_dic[cat][i].Fill(val)
                s += val
            
            if not "integral" in data_obs_dic[cat]:
                uidint = ROOT.TUUID()
                data_obs_dic[cat]["integral"] = ROOT.TH1D("data_obs_int_distr_"+cat+"_"+str(i)+"_"+uidint.AsString(), ";integral;frequency", 400, s-10*(s)**(1./2.),s+30*(s)**(1./2.))
                data_obs_dic[cat]["integral"].SetDirectory(0)
            data_obs_dic[cat]["integral"].Fill(s)
            

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
                    val = entry.r
                    error = entry.rErr
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
        if os.path.exists(os.path.abspath(path)):
                        
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
                    
                    r1, error1 = loadVariable(pathToLoad)
                    tree_r1, tree_error1 = loadVariable(pathToLoad, True)
                    
                    if r1 == -9999 or error1 == -9999:
                        print "Could not load variable r1 or error1 from {0}/{1}".format(path, infile)
                        continue
    
                    if tree_r1 == -9999 or tree_error1 == -9999:
                        print "Could not load variable tree_r1 or tree_error1 from {0}/{1}".format(path, infile)
                        continue
                    if path2:
                        if os.path.exists(siblingPath):
                            r2, error2 = loadVariable(siblingPath)
                            tree_r2, tree_error2 = loadVariable(siblingPath, True)
                            
                            if r2 == -9999 or error2 == -9999:
                                print "Could not load variable r2 or error2 from {0}/{1}".format(siblingPath, infile)
                                continue
                                
                            if tree_r2 == -9999 or tree_error2 == -9999:
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
        print "computed signal strength for {5} in path {0}: mu = {1} +- {2} +- {3} +- {4}".format(path1, mu1[infile].GetMean(), mu1[infile].GetMeanError(), mu1[infile].GetRMS(), muError1[infile].GetMean(), infile)
        #print "computed signal strength for {5} in path {0}: mu = {1} +- {2} +- {3} +- {4}".format(path2, mu2[infile].GetMean(), mu2[infile].GetMeanError(), mu2[infile].GetRMS(), muError2[infile].GetMean(), infile)
        c = ROOT.TCanvas()
        mu1[infile].Draw()
        c.SaveAs(pdfOutputPath + "mu1_" + infile.replace(".root","") + ".pdf" )
        c.Clear()
        if path2:
            mu2[infile].Draw()
            c.SaveAs(pdfOutputPath + "mu2_" + infile.replace(".root","") + ".pdf" )
     

def getDataObss(pathToPseudoExps, dataObsFile, pdfOutputPath = "./", suffix = None):
    if os.path.exists(os.path.abspath(pathToPseudoExps)):
        pathToPseudoExps = os.path.abspath(pathToPseudoExps)
    else:
        sys.exit("%s does not exist!" % pathToPseudoExps)
    
    path1 = pathToPseudoExps
    if not path1.endswith(".root"):
         path1 += "/*"
    data_obs_dic = {}
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
                #~ print "base:", base
                if not ("PseudoExperiment" in base or base.endswith(infile)):
                    continue
                print "checking", path
                
                pathToFitResults = path
                if not pathToFitResults.endswith(infile):
                    pathToFitResults += "/"+infile
                rootFile = ROOT.TFile(pathToFitResults)
                if rootFile.IsOpen() and not rootFile.IsZombie() and not rootFile.TestBit(ROOT.TFile.kRecovered):
                    print "cd into shapes_prefit"
                    rootFile.cd("shapes_prefit")
                    listOfKeys = ROOT.gDirectory.GetListOfKeys()
                    for key in listOfKeys:
                        if key.IsFolder():
                            cat = key.GetName()
                            if not cat in data_obs_dic:
                                print "creating data_obs_distr for cat", cat
                                data_obs_dic[cat] = {}

                    getDataObs(data_obs_dic)

                    rootFile.Close()
                    print "done with", path+"/"+infile
                else:
                    print "Could not read {0} in {1}".format(infile, path)

    origFile = ROOT.TFile(dataObsFile)
    ROOT.gStyle.SetOptStat(221112211)
    ROOT.gStyle.SetOptFit(11111);
    for cat in data_obs_dic:
        orig_data_obs = None
        if origFile.IsOpen() and not origFile.IsZombie() and not origFile.TestBit(ROOT.TFile.kRecovered):
            orig_data_obs = origFile.Get("data_obs_finaldiscr_" + cat)
        else: print "could not open file", dataObsFile
        canvasname = ""
        for n, i in enumerate(sorted(data_obs_dic[cat])):
            outFile = ROOT.TFile(pdfOutputPath+data_obs_dic[cat][i].GetName()+".root", "RECREATE")
            canvas = ROOT.TCanvas()

            data_obs_dic[cat][i].Draw()
            fit = ROOT.TF1("fit", "[0]*TMath::Poisson(x, [1]) + [2]", data_obs_dic[cat][i].GetBinCenter(1), data_obs_dic[cat][i].GetBinCenter(data_obs_dic[cat][i].GetNbinsX()))
            fit.SetParNames("Amp", "Mean", "Offset")
            fit.SetParameters(data_obs_dic[cat][i].GetEntries(), data_obs_dic[cat][i].GetMean(), 0)
            fit.SetNpx(500)
            
            data_obs_dic[cat][i].Fit(fit,"R")
            fit.Draw("Same")
            name = data_obs_dic[cat][i].GetName()
            parts = name.split("_")
            name = "_".join(parts[0:len(parts)-2])
            if not canvasname:
                canvasname = pdfOutputPath+name.replace("_{0}".format(i), "")+".pdf"
            
            if isinstance(orig_data_obs, ROOT.TH1) and not isinstance(i, str):
                val = orig_data_obs.GetBinContent(i+1)
                leg = ROOT.TLegend(0.1,0.7,0.3,0.9)
                line = ROOT.TLine(val, 0, val, data_obs_dic[cat][i].GetMaximum())
                line.SetLineWidth(3)
                leg.AddEntry(line,"orig_val = {0}".format(val),"l")
                line.Draw("Same")
                leg.Draw("Same")
                
                
            elif isinstance(orig_data_obs, ROOT.TH1) and isinstance(i, str):
                print "saving distribution for toy yields"
                val = orig_data_obs.Integral()
                leg = ROOT.TLegend(0.1,0.7,0.3,0.9)
                line = ROOT.TLine(val, 0, val, data_obs_dic[cat][i].GetMaximum())
                line.SetLineWidth(3)
                leg.AddEntry(line,"orig_val = {0}".format(val),"l")
                line.Draw("Same")
                leg.Draw("Same")
            else:
                print "could not load data_obs object \'{0}\' from file \'{1}\'".format("data_obs_finaldiscr_" + cat, dataObsFile)
            if n == 0:
                canvas.Print(canvasname + "(","pdf")
            elif n == len(data_obs_dic[cat])-1:
                canvas.Print(canvasname + ")","pdf")
            else:
                canvas.Print(canvasname,"pdf")
            canvas.Write()
            #data_obs_dic[cat].SetDirectory(outFile)
            data_obs_dic[cat][i].Write()
            outFile.Close()

    origFile.Close()
    return data_obs_dic


if path2 and os.path.exists(os.path.abspath(path2)):
    path2 = os.path.abspath(path2)
    print "start comparison between\npath1: {0}\npath2: {1}".format(path1, path2)
    data_obs_dic_1 = getDataObss( pathToPseudoExps = path1, 
            dataObsFile = pathToRootfile, 
            pdfOutputPath = pdfOutputPath,
            suffix = "path1")
    data_obs_dic_2 = getDataObss( pathToPseudoExps = path2, 
            dataObsFile = pathToRootfile, 
            pdfOutputPath = pdfOutputPath,
            suffix = "path2")
    compareHistos(dic1 = data_obs_dic_1, dic2 = data_obs_dic_2)
    compareMus(path1 = path1, path2 = path2, pdfOutputPath = pdfOutputPath)
else:
    print "could not find second path, only analyse data in path1"
    getDataObss( pathToPseudoExps = path1, 
            dataObsFile = pathToRootfile, 
            pdfOutputPath = pdfOutputPath)
    compareMus(path1 = path1, pdfOutputPath = pdfOutputPath)
