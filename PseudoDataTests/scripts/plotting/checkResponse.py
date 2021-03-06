import ROOT
import os
import sys
import glob
import subprocess
import numpy as np

ROOT.gROOT.SetBatch(True)

sys.path.append(os.path.join(os.path.dirname(sys.path[0]),'base'))

from helpfulFuncs import getLegend

datacards = sys.argv[1]
stepSize = float(sys.argv[2])
upperBound = 2
lowerBound = 0

colors  = [ROOT.kRed    ,ROOT.kBlue     ,ROOT.kOrange   ,ROOT.kMagenta  ]
markers = [20           ,21             ,22             ,23          ]
#dic with commands to check. Form: {label: command}

cmds = {
"minimizerStrategy1, minimizerTolerance 0.0001" :
# "combine -M MaxLikelihoodFit -m 125 --minimizerStrategy 1 --minimizerTolerance 0.0001 --rMin=-10.00 --rMax=10.00 -t -1 --minos all",
"combine -M MaxLikelihoodFit -m 125 --minimizerStrategy 1 --minimizerTolerance 0.0001 --rMin=-10.00 --rMax=10.00 -t -1",
"minimizerStrategy1, minimizerTolerance 0.001" :
# "combine -M MaxLikelihoodFit -m 125 --minimizerStrategy 1 --minimizerTolerance 0.001 --rMin=-10.00 --rMax=10.00 -t -1 --minos all",
"combine -M MaxLikelihoodFit -m 125 --minimizerStrategy 1 --minimizerTolerance 0.001 --rMin=-10.00 --rMax=10.00 -t -1",
"minimizerStrategy0, minimizerTolerance 0.0001" :
# "combine -M MaxLikelihoodFit -m 125 --minimizerStrategy 0 --minimizerTolerance 0.0001 --rMin=-10.00 --rMax=10.00 -t -1 --minos all",
"combine -M MaxLikelihoodFit -m 125 --minimizerStrategy 0 --minimizerTolerance 0.0001 --rMin=-10.00 --rMax=10.00 -t -1",
"minimizerStrategy0, minimizerTolerance 0.001" :
# "combine -M MaxLikelihoodFit -m 125 --minimizerStrategy 0 --minimizerTolerance 0.001 --rMin=-10.00 --rMax=10.00 -t -1 --minos all",
"combine -M MaxLikelihoodFit -m 125 --minimizerStrategy 0 --minimizerTolerance 0.001 --rMin=-10.00 --rMax=10.00 -t -1",

}
combineOutputName = "mlfit.root"
#==============================================================================
def frange(x, y, stepSize):
  while x <= y+stepSize:
    yield x
    x += stepSize

def didConverge(fitObject):
    quality=-2
    quality=fitObject.covQual()
    #print "quality of covariance matrix is", quality
    if quality == -1:
        print "\tUnknown, matrix was externally provided"
    if quality == 0:
        print "\tNot calculated at all"
    if quality == 1:
        print "Approximation only, not accurate"
    if quality == 2:
        print "Full matrix, but forced positive-definite"
    if quality == 3:
        print "Full, accurate covariance matrix"
        return True
    return False

def getNLLvals(datacard, commandString, up, low):
    allVals = []
    for signal in frange(low,up,stepSize):
        command = commandString + " --expectSignal {0} {1}".format(signal, datacard)
        print command
        subprocess.call([command], shell = True)

        if os.path.exists(combineOutputName):
            infile = ROOT.TFile(combineOutputName)
            if infile.IsOpen() and not infile.IsZombie() and not infile.TestBit(ROOT.TFile.kRecovered):
                fit_s = infile.Get("fit_s")
                if isinstance(fit_s, ROOT.RooFitResult):
                    if fit_s.status() == 0 and didConverge(fit_s):
                        mu = fit_s.floatParsFinal().find("r")
                        if isinstance(mu, ROOT.RooRealVar):
                            print "saving {0} {1} +{2}".format(mu.getVal(), mu.getErrorLo(), mu.getErrorHi())
                            vals = [signal, mu.getVal(), mu.getErrorLo(), mu.getErrorHi()]
                            allVals.append(vals)
                        else:
                            print "could not load r from fit_s!"
                    else:
                        print "Fit did not converge! Skipping signal strength", signal
                else:
                    print "Root file does not contain RooFitResult object 'fit_s'!"

                infile.Close()
            else:
                print "could not open", combineOutputName

            os.remove(combineOutputName)
            if os.path.exists("higgsCombineTest.MaxLikelihoodFit.mH125.root"):
                os.remove("higgsCombineTest.MaxLikelihoodFit.mH125.root")

        else:
            print "could not find {0} for injected signal strength {1}!".format(combineOutputName, signal)
    return allVals

#==============================================================================
for datacard in glob.glob(datacards):
    assert datacard.endswith(".txt") or datacard.endswith(".root"), "input file has to be useable for combine! (.txt / .root)"
    if os.path.exists(datacard):
        assert len(cmds) == len(colors) and len(cmds) == len(markers), "lists have different numbers of entries!"
        graphs = []
        legend = getLegend()
        datacard = os.path.abspath(datacard)
        print "creating response plot for datacard", datacard

        values = {}
        for cmd in cmds:
            values[cmd] = getNLLvals(   datacard = datacard,
                                        commandString = cmds[cmd],
                                        up = upperBound,
                                        low = lowerBound)

        c = ROOT.TCanvas()
        canvasName = "responsePlot"
        if datacard.endswith(".txt"):
            canvasName += "_" + os.path.basename(datacard).replace(".txt","")
        elif datacard.endswith(".root"):
            canvasName += "_" + os.path.basename(datacard).replace(".root","")

        graphOutput = ROOT.TFile("graphs_"+canvasName+".root", "RECREATE")
        ideal = ROOT.TF1("ideal", "x", lowerBound, upperBound)
        ideal.SetLineStyle(2)
        ideal.SetLineColor(ROOT.kBlack)
        first = True
        for cmd, color, marker in zip(values, colors, markers):
            if len(values[cmd]) is not 0:
                graph = ROOT.TGraphAsymmErrors(len(values[cmd]))
                for npoint, valSet in enumerate(values[cmd]):
                    graph.SetPoint(npoint, valSet[0], valSet[1])
                    graph.SetPointEYlow(npoint, ROOT.TMath.Abs(valSet[2]))
                    graph.SetPointEYhigh(npoint, ROOT.TMath.Abs(valSet[3]))
                    #print "Set point {0} to x={1}\ty={2}\t-{3}\t+{4}".format(npoint,graph.GetX()[npoint], graph.GetY()[npoint],graph.GetEYlow()[npoint], graph.GetEYhigh()[npoint])

                graph.SetLineColor(color)
                graph.SetMarkerColor(color)
                graph.SetMarkerStyle(marker)

                localLeg = getLegend()
                localLeg.AddEntry(graph, cmd, "lp")
                localLeg.AddEntry(ideal, "Ideal Response Behavior", "l")

                graphName = cmd.replace(",", "")
                graphName = graphName.replace(" ", "_")
                graphName = graphName.replace(".", "p")
                graph.SetName(graphName)
                graphs.append(graph.Clone())
                legend.AddEntry(graphs[-1], cmd, "lp")

                graph.Write(graphName)
                graph.GetHistogram().GetXaxis().SetTitle('injected #mu')
                graph.GetHistogram().GetYaxis().SetTitle('fitted #mu')
                graph.Draw("AP")
                ideal.Draw("Same")
                localLeg.Draw("Same")
                if first:
                    first = False
                    c.Print(canvasName + ".pdf(","pdf")
                else:
                    c.SaveAs(canvasName+".pdf","pdf")

                c.Clear()
        first = True
        for graph in graphs:
            if first:
                first=False
                print "drawing graph", graph.GetName()
                graph.Draw("AP")
            else:
                print "drawing graph {0} in same canvas".format(graph.GetName())
                graph.Draw("PSame")
        legend.AddEntry(ideal, "Ideal Response Behavior", "l")
        ideal.Draw("Same")
        legend.Draw("Same")
        c.Print(canvasName + ".pdf)","pdf")
        graphOutput.Close()
    else:
        sys.exit("could not find file {0}".format(datacard))
