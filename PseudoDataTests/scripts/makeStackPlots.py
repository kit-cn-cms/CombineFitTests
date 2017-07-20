import ROOT
import os
import sys
ROOT.gROOT.SetBatch(True)

inputRootFile = sys.argv[1]
outputSuffix = sys.argv[2]
listOfProcesses = sys.argv[3:]

categoryEndings = ["ljets_j4_t4", "ljets_j5_tge4","ljets_jge6_t3", "ljets_jge6_tge4"]

def makeStackPlots(listOfHistos, outputSuffix = "stackplots"):
    colors = [ROOT.kOrange-3, ROOT.kRed, ROOT.kMagenta+2, ROOT.kBlue, ROOT.kCyan+2, ROOT.kGreen+1, ROOT.kYellow+1, ROOT.kPink-3, ROOT.kViolet-8, ROOT.kAzure+3, ROOT.kTeal-7, ROOT.kSpring-9]
    outfile = ROOT.TFile(outputSuffix+"_stackplots.root", "RECREATE")

    hStack = ROOT.THStack()
    hStack_normed = ROOT.THStack()
    legend = ROOT.TLegend(0.72,0.5,0.95,0.99)
    color = None
    for i in range(len(listOfHistos)):
        color = colors[i%len(colors)]
        listOfHistos[i].SetLineColor(color)
        listOfHistos[i].SetLineWidth(4)
        listOfHistos[i].SetFillColor(color)
        listOfHistos[i].SetMarkerColor(color)
        histoName = listOfHistos[i].GetName()
        if histoName.startswith("ttH"):
            processName = "_".join(histoName.split("_")[:2])
        else:
            processName = histoName.split("_")[0]

        legend.AddEntry(listOfHistos[i], processName, "f")
        hStack.Add(listOfHistos[i].Clone())
        listOfHistos[i].Scale(1./listOfHistos[i].Integral())
        hStack_normed.Add(listOfHistos[i].Clone())
    c = ROOT.TCanvas()
    hStack.Write()
    hStack_normed.Write()
    legend.Write()
    hStack.Draw("HIST")
    legend.Draw("Same")
    c.Write("canvas_hStack")
    c.SaveAs(outputSuffix+"_stackplot.pdf")
    c.Clear()
    hStack_normed.Draw("nostack")
    legend.Draw("Same")
    c.Write("canvas_hStack_normed")
    c.SaveAs(outputSuffix+"_normed_stackplot.pdf")
    outfile.Close()



def loadHisto(key, process, listOfHistos):
    start = ""
    end = ""
    if "*" in process:
        start, end = process.split("*")
    else:
        end = process

    histoName = key.GetName()
    if histoName.startswith(start) and histoName.endswith(end):
        print "adding histo", histoName
        histo = infile.Get(histoName)
        listOfHistos.append(histo)


if len(listOfProcesses) == 0:
    listOfProcesses.append("t*")


listOfHistos = []

if os.path.exists(inputRootFile):
    infile = ROOT.TFile(inputRootFile)
    listOfKeys = infile.GetListOfKeys()
    for catEnding in categoryEndings:
        for process in listOfProcesses:
            for key in listOfKeys:
                loadHisto(key, process+"_finaldiscr_"+catEnding, listOfHistos)

        makeStackPlots(listOfHistos, catEnding+"_"+outputSuffix)
        listOfHistos[:] = []
    infile.Close()
else:
    print "Error! Could not find file", inputRootFile
