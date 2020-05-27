import ROOT
import os
import sys

scriptpath = os.path.abspath(sys.argv[0])
basepath = os.path.join(os.path.dirname(scriptpath),"base")
if os.path.exists(basepath) and basepath not in sys.path:
    print "adding path", basepath
    sys.path.append(basepath)
else:
    sys.exit("Could not load path %s" % basepath)
from helpfulFuncs import getLatex

ROOT.gROOT.SetBatch(True)

ROOT.gStyle.SetOptStat(0)
inputRootFile = sys.argv[1]
outputSuffix = sys.argv[2]
# listOfProcesses = [	"ttH_hbb", 
					# "ttbarOther", 
					# "ttbarPlusCCbar", 
					# "ttbarPlusB", 
					# "ttbarPlus2B", 
					# "ttbarPlusBBbar"
					# ]
procString = "ttH_hcc         ttH_hbb         ttH_htt         ttH_hww         ttH_hgluglu     ttH_hgg         ttH_hzz         ttH_hzg "
procString += "ttbarZ          diboson         ttbarPlusB      ttbarW          singlet         wjets           ttbarPlus2B     ttbarOther      ttbarPlusBBbar  zjets           ttbarPlusCCbar"
listOfProcesses = procString.split()


categoryEndings = [	"ljets_j4_t4_low",
                    "ljets_j4_t4_high",
                    "ljets_j4_t3",
					"ljets_j5_tge4_low",
                    "ljets_j5_tge4_high",
                    "ljets_j5_t3",
					"ljets_jge6_t3_low",
                    "ljets_jge6_t3_high",
					"ljets_jge6_tge4_low",
                    "ljets_jge6_tge4_high"
					]

colors = [	ROOT.kBlack,	ROOT.kRed,		ROOT.kMagenta+2, 
			ROOT.kBlue,		ROOT.kCyan+2,	ROOT.kGreen+1, 
			ROOT.kYellow+1,	ROOT.kPink-3,	ROOT.kViolet-8, 
			ROOT.kAzure+3,	ROOT.kTeal-7,	ROOT.kOrange-3,
            ROOT.kSpring-9, ROOT.kRed-4,    ROOT.kMagenta-8,
            ROOT.kBlue-7,   ROOT.kCyan-3,   ROOT.kGreen-6,
            ROOT.kSpring-6
			]
        
ttbar = "t#bar{t}"

names = {   "ttH_hcc"       :   ttbar + "H(c#bar{c})",
            "ttH_hbb"       :   ttbar + "H(b#bar{b})",
            # "ttH_hcc"       :   ttbar + "H(c#bar{c})",
            "ttH"           :   ttbar + "H",
            "ttbarOther"    :   ttbar + "+lf",
            "ttbarPlusBBbar":   ttbar + "+b#bar{b}",
            "ttbarPlusCCbar":   ttbar + "+c#bar{c}",
            "ttbarPlusB"    :   ttbar + "+b",
            "ttbarPlus2B"   :   ttbar + "+2b",
            "ttbarW"        :   ttbar + "+W",
            "ttbarZ"        :   ttbar + "+Z",
            "diboson"       :   "Diboson",
            "singlet"       :   "Single Top",
            "wjets"         :   "W+jets",
            "zjets"         :   "Z+jets",
            
        }   

histoKey = "$PROCESS_finaldiscr_$CHANNEL"

titleSize = 0.045
titleX = "final discriminator"
titleOffsetX = 1
titleYstack = "#Events"
titleYnormed = "Normalized Events"
titleOffsetY = 1

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
    if len(listOfHistos) > 0:
        outfile = ROOT.TFile(outputSuffix+"_stackplots.root", "RECREATE")
        latex = getLatex(0.12,0.91, "CMS private work")
        hStack = listOfHistos[0].Clone()
        hStack.Reset()
        stackStages = []
        #hStack_normed = ROOT.THStack()

        legend = ROOT.TLegend(0.72,0.5,0.95,0.99)
        color = None
        listOfHistos.sort(key = lambda h: h.Integral(),reverse=False)
        for i in range(len(listOfHistos)):
            color = colors[i%len(colors)]
            
            
            hStack.Add(listOfHistos[i].Clone())
            hStack.SetName(listOfHistos[i].GetName() + "_{0}".format(i))
            setupHistoStyle(hStack, color, titleY = titleYstack)

            stackStages.append(hStack.Clone())
            setupHistoStyle(listOfHistos[i], color, titleY = titleYnormed)
            
            if listOfHistos[i].Integral() != 0:
                listOfHistos[i].Scale(1./listOfHistos[i].Integral())
                listOfHistos[i].SetFillStyle(0)

        c = ROOT.TCanvas()
        # c.SetLogy(1)
        for i, hist in enumerate(reversed(stackStages)):
            hist.SetFillStyle(1001)
            if i == 0:
                hist.GetYaxis().SetRangeUser(0, 1.1*hist.GetBinContent(hist.GetMaximumBin()))

                hist.Draw("HIST")
            else:
                hist.Draw("SameHIST")
            hist.Write()
            histoName = hist.GetName()

            if histoName.startswith("ttH_h"):
                processName = "_".join(histoName.split("_")[:2])
            else:
                processName = histoName.split("_")[0]

            legend.AddEntry(hist, names[processName], "f")


        legend.Draw("Same")
        
        latex.Draw("Same")
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

                hist.Draw("HIST")
            else:
                hist.Draw("HISTsame")

        legend.Draw("Same")
        latex.Draw("Same")
        # c.SetLogy(0)
        c.Write("canvas_hStack_normed")
        c.SaveAs(outputSuffix+"_normed_stackplot.pdf")

        legend.Write("legend")
        hStack.Draw("HIST")
        c.Clear()



        outfile.Close()
    else:
        print "did not find any histos!"

listOfHistos = []

if os.path.exists(inputRootFile):
    infile = ROOT.TFile(inputRootFile)
    listOfKeys = infile.GetListOfKeys()
    for catEnding in categoryEndings:
        ttH = None
        for process in listOfProcesses:
            processKey = histoKey.replace("$CHANNEL", catEnding)
            processKey = processKey.replace("$PROCESS", process)
            if processKey in listOfKeys:
                print "adding histo", processKey
                histo = infile.Get(processKey)
                if isinstance(histo, ROOT.TH1):
                    if not processKey.startswith("ttH_"):
                        listOfHistos.append(histo.Clone())
                    else:
                        if ttH == None:
                            ttH = histo.Clone("ttH_finaldiscr_" + catEnding)
                        else:
                            ttH.Add(histo)
        listOfHistos.append(ttH)

        makeStackPlots(listOfHistos, catEnding+"_"+outputSuffix)
        listOfHistos[:] = []
    infile.Close()
else:
    print "Error! Could not find file", inputRootFile
