import ROOT
import os
import sys
ROOT.gROOT.SetBatch(True)

ROOT.gStyle.SetOptStat(0)
inputRootFile = sys.argv[1]
outputSuffix = sys.argv[2]
listOfProcesses = [	"ttH_hbb", 
					"ttbarOther", 
					"ttbarPlusCCbar", 
					"ttbarPlusB", 
					"ttbarPlus2B", 
					"ttbarPlusBBbar"
					]

categoryEndings = [	"ljets_j4_t4", 
					"ljets_j5_tge4",
					"ljets_jge6_t3", 
					"ljets_jge6_tge4"
					]

colors = [	ROOT.kBlack,	ROOT.kRed,		ROOT.kMagenta+2, 
			ROOT.kBlue,		ROOT.kCyan+2,	ROOT.kGreen+1, 
			ROOT.kYellow+1,	ROOT.kPink-3,	ROOT.kViolet-8, 
			ROOT.kAzure+3,	ROOT.kTeal-7,	ROOT.kSpring-9
			]

histoKey = "$PROCESS_finaldiscr_$CHANNEL"

titleSize = 0.065
titleX = "final discriminator"
titleOffsetX = 0.65
titleYstack = "#Events"
titleYnormed = "Normalized Events"
titleOffsetY = 0.75

def setupHistoStyle(hist, color, titleY):
    hist.GetXaxis().SetTitle(titleX)
    hist.GetXaxis().SetTitleOffset(titleOffsetX)
    hist.GetXaxis().SetTitleSize(titleSize)
    #print "xlabel:", hist.GetXaxis().GetTitle()

    hist.GetYaxis().SetTitle(titleY)
    hist.GetYaxis().SetTitleOffset(titleOffsetY)
    hist.GetYaxis().SetTitleSize(titleSize)
    #print "ylabel:", hist.GetYaxis().GetTitle()
    hist.SetTitle("")

    hist.SetLineColor(color)
    hist.SetMarkerColor(color)
    hist.SetFillColor(color)
    hist.SetLineWidth(4)



def makeStackPlots(listOfHistos, outputSuffix = "stackplots"):
    outfile = ROOT.TFile(outputSuffix+"_stackplots.root", "RECREATE")
    if len(listOfHistos) > 0:
        hStack = listOfHistos[0].Clone()
        hStack.Reset()
        stackStages = []
        #hStack_normed = ROOT.THStack()

        legend = ROOT.TLegend(0.72,0.5,0.95,0.99)
        color = None
        for i in range(len(listOfHistos)):
            color = colors[i%len(colors)]

            hStack.Add(listOfHistos[i].Clone())
            hStack.SetName(listOfHistos[i].GetName() + "_{0}".format(i))
            setupHistoStyle(hStack, color, titleY = titleYstack)

            stackStages.append(hStack.Clone())
            setupHistoStyle(listOfHistos[i], color, titleY = titleYnormed)

            listOfHistos[i].Scale(1./listOfHistos[i].Integral())

        c = ROOT.TCanvas()

        for i, hist in enumerate(reversed(stackStages)):
            hist.SetFillStyle(1001)
            if i == 0:
                hist.GetYaxis().SetRangeUser(0, 1.1*hist.GetBinContent(hist.GetMaximumBin()))

                hist.Draw("HIST")
            else:
                hist.Draw("SameHIST")
            hist.Write()
            histoName = hist.GetName()

            if histoName.startswith("ttH"):
                processName = "_".join(histoName.split("_")[:2])
            else:
                processName = histoName.split("_")[0]

            legend.AddEntry(hist, processName, "f")


        legend.Draw("Same")

        c.Write("canvas_hStack")
        c.SaveAs(outputSuffix+"_stackplot.pdf")
        hStack.Write("stackplot")
        #print listOfHistos
        listOfHistos.sort(key = lambda h: h.GetBinContent(h.GetMaximumBin()),reverse=True)
        #print listOfHistos
        first = True

        for hist in listOfHistos:

            hist.Write(hist.GetName()+"_normed")
            if first:
                first = False
                hist.GetYaxis().SetRangeUser(0, 1.1*hist.GetBinContent(hist.GetMaximumBin()))

                hist.Draw()
            else:
                hist.Draw("Same")

        legend.Draw("Same")
        c.Write("canvas_hStack_normed")
        c.SaveAs(outputSuffix+"_normed_stackplot.pdf")

        legend.Write("legend")
        hStack.Draw("HIST")
        c.Clear()



        outfile.Close()
    else:
        print "did not find any histos!"



def loadHisto(key, process, listOfHistos):
    start = ""
    end = ""
    if "*" in process:
        start, end = process.split("*")
    else:
        end = process

    histoName = key.GetName()
    #print "comparing {0} with process input {1}".format(histoName, process)
    if histoName.startswith(start) and histoName.endswith(end):
        print "adding histo", histoName
        histo = infile.Get(histoName)
        listOfHistos.append(histo)


listOfHistos = []

if os.path.exists(inputRootFile):
    infile = ROOT.TFile(inputRootFile)
    listOfKeys = infile.GetListOfKeys()
    for catEnding in categoryEndings:
        for process in listOfProcesses:
            processKey = histoKey.replace("$CHANNEL", catEnding)
            processKey = processKey.replace("$PROCESS", process)
            if processKey in listOfKeys:
                print "adding histo", processKey
                histo = infile.Get(processKey)
                listOfHistos.append(histo)

        makeStackPlots(listOfHistos, catEnding+"_"+outputSuffix)
        listOfHistos[:] = []
    infile.Close()
else:
    print "Error! Could not find file", inputRootFile
