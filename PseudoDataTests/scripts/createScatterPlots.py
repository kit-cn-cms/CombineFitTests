import os
import sys
import glob
import ROOT
from shutil import rmtree

ROOT.gROOT.SetBatch(True)

outputDirectory = sys.argv[1] # output directory for plots
wildcards = sys.argv[2:] #combine output files

if not os.path.exists(outputDirectory):
    sys.exit("Output directory does not exist")
else:
    outputDirectory = os.path.abspath(outputDirectory)

scatterplots = {
    "fit_s" : {},
    "fit_b" : {}
}

def converged_fit(results):
    if results.status() == 0 and results.covQual() == 3:
        return True
    return False

def check_for_reset(foldername):
    if os.path.exists(foldername):
        print "resetting", foldername
        rmtree(foldername)
    os.makedirs(foldername)

def load_vals(scatterplots, infile, prefix):
    for name in scatterplots:
        results = infile.Get(name)
        if not results == None and isinstance(results, ROOT.RooFitResult):
            if not converged_fit(results): 
                print "fit did not converge, skipping"
                continue
            params = results.floatParsFinal().contentsString().split(",")
            params.sort()
            for i, param1 in enumerate(params):
                # print "p1: {0}/{1}".format(i, len(params))
                p1 = results.floatParsFinal().find(param1)
                if p1 == None or not isinstance(p1, ROOT.RooRealVar): 
                    continue
                val1 = p1.getVal()
                for param2 in params[i:]:
                    histname = prefix + param1 + "_v_" + param2
                    p2 = results.floatParsFinal().find(param2)
                    if p2 == None or not isinstance(p2, ROOT.RooRealVar): 
                        continue
                    
                    val2 = p2.getVal()
                    if not histname in scatterplots[name]:
                        scatterplots[name][histname] = [[],[]]
                    scatterplots[name][histname][0].append(val1)
                    scatterplots[name][histname][1].append(val2)
                    # print "deleting p2"
                    # p2.Delete()
                # print "deleting p1"
                # p1.Delete()
            # print "deleteing results"
            results.Delete()

prefix = "scatterplot_"

print "# input arguments:", len(wildcards)

counter = 0
for wildcard in wildcards:
    for filepath in glob.glob(wildcard):
        infile = ROOT.TFile(filepath)
        if counter % 10 == 0:
            print "Analyzing toy #{0}".format(counter)
        if infile.IsOpen() and not infile.IsZombie() and not infile.TestBit(ROOT.TFile.kRecovered):
            load_vals(scatterplots, infile, prefix)
            infile.Close()
        counter += 1

print "done collecting"


print "creating scatter plots in", outputDirectory

check_for_reset(outputDirectory + "/pngs")
check_for_reset(outputDirectory + "/pdfs")
check_for_reset(outputDirectory + "/rootfiles")


for fit in scatterplots:
    lines = []
    for name in scatterplots[fit]:
        histname = fit + "_" + name
        outfile = ROOT.TFile.Open(outputDirectory + "/rootfiles/"+histname + ".root", "RECREATE")
        c = ROOT.TCanvas()
        
        containsparams = name.replace(prefix, "")
        param1, param2 = containsparams.split("_v_")
        labels = ";" + param1 + ";" + param2
        xmin = min(scatterplots[fit][name][0])
        xmax = max(scatterplots[fit][name][0])
        ymin = min(scatterplots[fit][name][1])
        ymax = max(scatterplots[fit][name][1])
        hist = ROOT.TH2D(histname, labels, 100, xmin, xmax, 100, ymin, ymax)
        
        for i in range(len(scatterplots[fit][name][0])):
            x = scatterplots[fit][name][0][i]
            y = scatterplots[fit][name][1][i]
            hist.Fill(x,y)
        hist.Draw()
        hist.Write()
        lines.append(",".join([param1, param2]) + "\t" + str(hist.GetCorrelationFactor()))
        c.SaveAs(outputDirectory + "/pdfs/" + histname + ".pdf")
        c.SaveAs(outputDirectory + "/pngs/" + histname + ".png")
        c.Write("canvas")
        outfile.Close()
    lines.sort()
    with open(outputDirectory+"/"+fit + "_correlations.txt","w") as f:
        f.write("\n".join(lines))
