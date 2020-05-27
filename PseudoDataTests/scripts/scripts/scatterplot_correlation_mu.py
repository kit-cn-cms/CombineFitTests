import ROOT
import os
import sys
ROOT.gROOT.SetBatch(True)
ROOT.gDirectory.cd('PyROOT:/')

var1 = sys.argv[1]
var2 = sys.argv[2]
rootfiles = sys.argv[3:]

def happy_file(f):
    if not f is None and isinstance(f, ROOT.TFile):
        if f.IsOpen() and not f.IsZombie() and not f.TestBit(ROOT.TFile.kRecovered):
            return True
    return False

def fit_converged(fit_s):
    if fit_s.status() == 0 and fit_s.covQual() == 3:
        return True
    return False

def load_postfit_val(val, results):
    var = results.floatParsFinal().find("r")
    if not var is None and isinstance(var, ROOT.RooRealVar):
        return var.getVal()
    return None

label = ";{0};corr({1},{2})".format("r", var1, var2)
outputname = "scatterplot_r_vs_corr_{0}-{1}.root".format(var1, var2)
outfile = ROOT.TFile.Open(outputname, "RECREATE")
hist = ROOT.TH2D("hist", label, 500, -5, 5, 100, -1, 1)
for i,path in enumerate(rootfiles, 1):
    if i%50 == 0: print "checking file #{0}/{1}".format(i,len(rootfiles))
    if os.path.exists(path):
        path = os.path.abspath(path)
        f = ROOT.TFile(path)
        if happy_file(f):
            fit_s = f.Get("fit_s")
            if not fit_s is None and isinstance(fit_s, ROOT.RooFitResult):
                if not fit_converged(fit_s): continue
                
                corr = fit_s.correlation(var1, var2)
                r = load_postfit_val("r", fit_s)
                hist.Fill(r, corr)

                fit_s.Delete()
            f.Close()

can = ROOT.TCanvas()
hist.Draw("colz")
can.Write("can")
hist.Write("hist")
can.SaveAs(outputname.replace(".root",".pdf"))
outfile.Close()
