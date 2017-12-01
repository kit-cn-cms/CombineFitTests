import os
import sys
import glob
import ROOT

ROOT.gROOT.SetBatch(True)

outputDirectory = sys.argv[1]
wildcards = sys.argv[2:]

if not os.path.exists(outputDirectory):
    sys.exit("Output directory does not exist")
else:
    outputDirectory = os.path.abspath(outputDirectory)

scatterplots = {
    "fit_s" : {},
    "fit_b" : {}
}

prefix = "scatterplot_"

print "# input arguments:", len(wildcards)

counter = 0
for wildcard in wildcards:
    for filepath in glob.glob(wildcard):
        infile = ROOT.TFile(filepath)
        if counter % 10 == 0:
            print "Analyzing toy #{0}".format(counter)
        if infile.IsOpen() and not infile.IsZombie() and not infile.TestBit(ROOT.TFile.kRecovered):
            for name in scatterplots:
                results = infile.Get(name)
                if isinstance(results, ROOT.RooFitResult):
                    params = results.floatParsFinal().contentsString().split(",")
                    params.sort()
                    for i, param1 in enumerate(params):
                        p1 = results.floatParsFinal().find(param1)
                        if not isinstance(p1, ROOT.RooRealVar): continue
                        for param2 in params[i:]:
                            histname = prefix + param1 + "_v_" + param2
                            p2 = results.floatParsFinal().find(param2)
                            if not isinstance(p2, ROOT.RooRealVar): continue
                            val1 = p1.getVal()
                            val2 = p2.getVal()
                            if not histname in scatterplots[name]:
                                scatterplots[name][histname] = [[],[]]
                            scatterplots[name][histname][0].append(val1)
                            scatterplots[name][histname][1].append(val2)
        counter += 1


print "done collecting"


print "creating scatter plots in", outputDirectory

c = ROOT.TCanvas()
for fit in scatterplots:
    for name in scatterplots[fit]:
        histname = fit + "_" + name
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
        c.SaveAs(outputDirectory + "/" + histname + ".pdf")
        c.SaveAs(outputDirectory + "/" + histname + ".root")
