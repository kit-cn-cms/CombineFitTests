import ROOT
from os import path
import sys
from math import exp, log, isnan
import imp
import numpy as np
from scipy.stats import distributions

    
def loadHisto(infile, key):
    hist = infile.Get(key)
    if not isinstance(hist, ROOT.TH1): 
        sys.exit("Could not load histogram %s" % key)
    return hist.Clone()
    
def poisson(k, l):
    # print "calculating poisson for k={0}, lambda={1}".format(k, l)
    k = int(round(k))
    
    poisson = distributions.poisson
    # fac = 1
    # for num in range(1, k+1):
        # fac *= num
    # result = float(l)**k/fac*exp(-l)
    result = poisson.pmf(k, l)
    # print result
    return result

def getNLL(r, listHsignals, listHbkgs, data_obss):
    print "calling getNLL with"
    print "r =", r
    # print "listHsignals =", listHsignals
    # print "listHbkgs =", listHbkgs,
    # print "data_obss =", data_obss
    loglikelihood = 0
    for cat in listHsignals:
        nbins = listHsignals[cat][0].GetNbinsX()
        if not all(h.GetNbinsX() == nbins for h in listHsignals[cat]):
            print "Not all signal histos have same binning!"
            print "Skipping category", cat
            continue
        if not all(h.GetNbinsX() == nbins for h in listHbkgs[cat]):
            print "Binning in bkg histos!"
            print "Skipping category", cat
            continue
        if not data_obss[cat].GetNbinsX() == nbins:
            print "Binning in data_obs is different!"
            print "Skipping category", cat
            continue
        for i in range(1, nbins+1):
            s = 0
            for hist in listHsignals[cat]:
                s += r*hist.GetBinContent(i)
            for hist in listHbkgs[cat]:
                s+= hist.GetBinContent(i)
            loglikelihood += log(poisson(  k = data_obss[cat].GetBinContent(i),
                                    l = s))
    return -1*loglikelihood

if __name__ == '__main__':
    rootfilepath = sys.argv[1]
    pathToConfig = sys.argv[2]
    asimov = False
    if len(sys.argv) == 4 and (sys.argv[3] == "asimov" or sys.argv[3] == "Asimov"):
        print "will generate asimov data set"
        asimov = True
    
    if path.exists(pathToConfig):
        config = imp.load_source('config', pathToConfig)
    else:
        sys.exit("could not load config from %s" % pathToConfig)
    
    #dictionary with categories as keys, list of histos
    listHsignals = {}
    listHbkgs = {}
    data_obss = {}
    infile = ROOT.TFile(rootfilepath)

    if not (infile.IsOpen() and not infile.IsZombie() and not infile.TestBit(ROOT.TFile.kRecovered)):
        sys.exit("could not load root file in %s" % rootfilepath)
    for cat in config.signalHistos:
        catHistoKey = config.histoKey.replace("$CHANNEL", cat)
        #load observed data from root file
        data_obss[cat] = loadHisto( infile = infile,
                                    key = catHistoKey.replace("$PROCESS", "data_obs"))
        #generate asimov data set
        if asimov: data_obss[cat].Reset()
        
        if not cat in listHsignals:
            listHsignals[cat] = []
            listHbkgs[cat] = []
        for h in config.signalHistos[cat]:
            listHsignals[cat].append(loadHisto(infile = infile, key = h))
            #for asimov data set with sig = 1
            if asimov: 
                print "adding histo {0} to pseudo observed data".format(h)
                data_obss[cat].Add(listHsignals[cat][-1])
        for h in config.backgroundHistos[cat]:
            listHbkgs[cat].append(loadHisto(infile = infile, key = h))
            #for asimov data set containing background
            if asimov: 
                print "adding histo {0} to pseudo observed data".format(h)
                data_obss[cat].Add(listHbkgs[cat][-1])
    
    scanrange = np.linspace(-5, 5, num=1000)
    nlls = []
    rs = []
    for r in scanrange:
        nll = getNLL(   r = r, 
                            listHsignals = listHsignals, 
                            listHbkgs = listHbkgs, 
                            data_obss = data_obss)
        print "NLL({0}) = {1}".format(r, nll)
        if not isnan(nll):
            print "\tsaving", nll
            nlls.append(nll)
            rs.append(r)

    bestfit_nll = min(nlls)
    deltanlls = [2*(ll - bestfit_nll) for ll in nlls]
    
    x = []
    for i in range(len(deltanlls)):
        if deltanlls[i]<= 10:
            x.append(rs[i])
    
    deltanlls = [d for d in deltanlls if d <=10 ]
    bestfit_nll = min(deltanlls)
    bestfit_r   = x[deltanlls.index(bestfit_nll)]
    print "drawing"
    print "x=", x
    print "y=", deltanlls
    print "found best fit val at {0}, {1}".format(bestfit_r, bestfit_nll)
    
    outname = "simpleMLL"
    outfile = ROOT.TFile(outname+".root", "RECREATE")
    c = ROOT.TCanvas()
    graph = ROOT.TGraph(len(deltanlls))
    for i in range(len(deltanlls)):
        graph.SetPoint(i, x[i], deltanlls[i])
    graph.GetHistogram().GetXaxis().SetTitle("#mu_{t#bar{t}H}")
    graph.GetHistogram().GetYaxis().SetTitle("2#Delta NLL")
    graph.GetHistogram().SetTitle("")
    graph.Sort()
    graph.Draw()
    graph.Write("nllscan")
    
    bestfit = ROOT.TGraph(1)
    bestfit.SetPoint(0, bestfit_r, bestfit_nll)
    bestfit.SetMarkerStyle(34)
    bestfit.SetMarkerSize(1.5)
    bestfit.Sort()
    bestfit.Draw("P") 
    bestfit.Write("bestfit")   
    c.SaveAs(outname + ".pdf")
    c.Write("canvas")
    outfile.Close()
    
        
            
    
