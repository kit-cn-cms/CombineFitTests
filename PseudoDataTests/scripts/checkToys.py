import ROOT
import os
import sys
import glob
import math

ROOT.gROOT.SetBatch(True)

filesToCheck = ["mlfit.root"]#, "mlfit_MS_mlfit.root"]

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

def compareMus(path1, path2, pdfOutputPath):
    # path1 = sys.argv[1]
    # path2 = None
    # if len(sys.argv) == 1:
    #     path2 = sys.argv[2]
    # else:
    #     path2 = path1



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
        c = ROOT.TCanvas()
        mu1[infile].Draw()
        c.SaveAs(pdfOutputPath + "/mu1_" + infile.replace(".root","") + ".pdf" )

def getDataObs(pathToPseudoExps, rootfile, pdfOutputPath = "./"):
    if os.path.exists(os.path.abspath(pathToPseudoExps)):
        pathToPseudoExps = os.path.abspath(pathToPseudoExps)
    else:
        sys.exit("%s does not exist!" % pathToPseudoExps)

    path1 = pathToPseudoExps + "/*"

    data_obs_dic = {}
    rootfile = os.path.abspath(rootfile)
    pdfOutputPath = os.path.abspath(pdfOutputPath)

    for path in sorted(glob.glob(path1)):
        if os.path.exists(os.path.abspath(path)):
            if not "PseudoExperiment" in os.path.basename(path):
                continue
            path = os.path.abspath(path)
            print "checking", path

            for infile in filesToCheck:
                rootFile = ROOT.TFile(path+"/"+infile)
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

                            data_obs = ROOT.gDirectory.Get(cat+"/data")
                            if isinstance(data_obs, ROOT.TGraphAsymmErrors):
                                print "looping over entries"
                                s = 0.
                                for i in range(data_obs.GetN()):
                                    val = data_obs.GetY()[i]
                                    if not i in data_obs_dic[cat]:
                                        data_obs_dic[cat][i] = ROOT.TH1D("data_obs_distr_"+cat+"_"+str(i), ";integral;frequency", 400, val-10*(val)**(1./2.),val+30*(val)**(1./2.))
                                        data_obs_dic[cat][i].SetDirectory(0)

                                    data_obs_dic[cat][i].Fill(val)
                                    # s += data_obs.GetY()[i]
                                # print "filling value =", s
                                # data_obs_dic[cat].Fill(s)

                    rootFile.Close()
                    print "done with", path+"/"+infile
                else:
                    print "Could not read {0} in {1}".format(infile, path)

    origFile = ROOT.TFile(rootfile)
    ROOT.gStyle.SetOptStat(221112211)
    for cat in data_obs_dic:
        orig_data_obs = None
        if origFile.IsOpen() and not origFile.IsZombie() and not origFile.TestBit(ROOT.TFile.kRecovered):
            orig_data_obs = origFile.Get("data_obs_finaldiscr_" + cat)
        else: print "could not open file", rootfile
        for i in sorted(data_obs_dic[cat]):
            outFile = ROOT.TFile(pdfOutputPath+"/"+data_obs_dic[cat][i].GetName()+".root", "RECREATE")
            canvas = ROOT.TCanvas()

            data_obs_dic[cat][i].Draw()
            name = data_obs_dic[cat][i].GetName()

            if isinstance(orig_data_obs, ROOT.TH1):
                val = orig_data_obs.GetBinContent(i+1)
                leg = ROOT.TLegend(0.1,0.7,0.3,0.9)
                line = ROOT.TLine(val, 0, val, data_obs_dic[cat][i].GetMaximum())
                line.SetLineWidth(3)
                leg.AddEntry(line,"orig_val = {0}".format(val),"l")
                line.Draw("Same")
                leg.Draw("Same")
            else:
                print "could not load data_obs object \'{0}\' from file \'{1}\'".format("data_obs_finaldiscr_" + cat, rootfile)
            if i == 0:
                canvas.Print(pdfOutputPath+"/"+name.replace("_{0}".format(i), "")+".pdf(","pdf")
            elif i == len(data_obs_dic[cat])-1:
                canvas.Print(pdfOutputPath+"/"+name.replace("_{0}".format(i), "")+".pdf)","pdf")
            else:
                canvas.Print(pdfOutputPath+"/"+name.replace("_{0}".format(i), "")+".pdf","pdf")
            canvas.Write()
            #data_obs_dic[cat].SetDirectory(outFile)
            data_obs_dic[cat][i].Write()
            outFile.Close()

    origFile.Close()

getDataObs(sys.argv[1], sys.argv[2], sys.argv[3])
compareMus(sys.argv[1], sys.argv[1], sys.argv[3])
