from os import path
import sys
import ROOT
scriptpath = path.abspath(sys.argv[0])
basepath = path.join(path.dirname(scriptpath),"base")
if path.exists(basepath) and basepath not in sys.path:
    print "adding path", basepath
    sys.path.append(basepath)
else:
    sys.exit("Could not load path %s" % basepath)
from helpfulFuncs import getLatex
ROOT.gROOT.SetBatch(True)

ROOT.gStyle.SetOptStat(0)
tonorm = sys.argv[1]
reffile = sys.argv[2]

tonorm_procs = ["ttbarPlusBBbar"]
normto_procs = ["ttbarPlusBBbar", "ttbarPlusB", "ttbarPlus2B"]
histkey = "$PROCESS_finaldiscr_$CHANNEL"


refcolors = {"ttbarPlusB": ROOT.kBlue,	"ttbarPlus2B":	ROOT.kCyan+2, "ttbarPlusBBbar":	ROOT.kGreen+1}
tonormcolors={"ttbarPlusBBbar": ROOT.kRed}

justDraw = True
titleSize = 0.045

def make_stakes(hists, colors):
    sums = {}
    hsum = None
    for tup in sorted(hists.items(), key=lambda x: x[1].Integral()):
        proc = tup[0]
        h = tup[1]
        print "proc =",proc
        print "int =", h.Integral()
        if hsum and isinstance(hsum, ROOT.TH1):
            hsum.Add(h)
        else:
            hsum = h.Clone("hsum")

        sums[proc] = hsum.Clone("hsum_" + proc)
        sums[proc].SetLineColor(colors[proc])
        sums[proc].SetMarkerColor(colors[proc])
        sums[proc].SetFillColor(colors[proc])
        sums[proc].GetXaxis().SetTitle(h.GetXaxis().GetTitle())
        sums[proc].GetXaxis().SetTitleSize(titleSize)
        sums[proc].GetYaxis().SetTitle(h.GetYaxis().GetTitle())
        sums[proc].GetYaxis().SetTitleSize(titleSize)
        sums[proc].SetTitle(h.GetTitle())
    return sums

def norm_bins(basehist, hist):
    # basehist.Sumw2(False)
    # basehist.Sumw2(True)
    # hist.Sumw2(False)
    # hist.Sumw2(True)
    hist.Divide(basehist)

def save_as_txt(hist, outname):
    lines = []
    lines.append("bin\tratio")
    for i in range(1, hist.GetNbinsX()+1):
        lines.append("{0}\t{1}".format(i, hist.GetBinContent(i)))
    with open(outname+".txt", "w") as f:
        f.write("\n".join(lines))

def make_ratio_plot(basehist, hists, labels, outname):
    can = ROOT.TCanvas()
    leg = ROOT.TLegend(0.72,0.66,0.95,0.95)
    ratios = []
    ratios.append(basehist.Clone())
    norm_bins(basehist, ratios[-1])
    print "powheg ratios (should be 1)"
    for i in range(1, ratios[-1].GetNbinsX()+1):
        print "\t",ratios[-1].GetBinError(i)
    ratios[-1].GetXaxis().SetTitle(basehist.GetXaxis().GetTitle())
    ratios[-1].GetXaxis().SetTitleSize(titleSize)
    ratios[-1].GetYaxis().SetTitle("Ratio")
    ratios[-1].GetYaxis().SetRangeUser(0, 2)
    ratios[-1].GetYaxis().SetTitleSize(titleSize)
    ratios[-1].SetLineColor(ROOT.kBlack)
    ratios[-1].SetLineStyle(2)
    ratios[-1].SetLineWidth(3)
    ratios[-1].SetFillStyle(3005)
    ratios[-1].SetFillColor(ROOT.kBlack)
    for h in hists:
        ratios.append(h.Clone())
        norm_bins(basehist, ratios[-1])
        ratios[-1].SetMarkerColor(h.GetLineColor())
        ratios[-1].SetMarkerStyle(20)
        ratios[-1].SetLineColor(h.GetLineColor())
        
    latex = getLatex(0.12,0.91, "CMS private work")
    errorband = ratios[0].Clone("errorband")
    errorband.Draw("E3")
    for i in range(1, ratios[0].GetNbinsX()+1):
        ratios[0].SetBinError(i, 0)
    ratios[0].SetFillStyle(0)
    
    drawopt = "HISTsame"
    for h, label in zip(ratios, labels):
        h.Draw(drawopt)
        drawopt = "PE1X0same"
        leg.AddEntry(h, label, "lp")
    save_as_txt(ratios[-1], outname)
    leg.Draw("same")
    latex.Draw("Same")
    can.SaveAs(outname + ".pdf")
    can.SaveAs(outname + ".root")

if path.exists(tonorm) and path.exists(reffile):
    tonorm_source = ROOT.TFile(tonorm,"UPDATE")
    reffile_source = ROOT.TFile(reffile)
    
    for key in tonorm_source.GetListOfKeys():
        keyname = key.GetName()
        for process in tonorm_procs:
            prockey = histkey.replace("$PROCESS", process)
            if "$CHANNEL" in prockey:
                searchpattern = prockey.replace("$CHANNEL", "")
            
            if keyname.startswith(searchpattern) and not (keyname.endswith("Up") or keyname.endswith("Down") or keyname.endswith("up") or keyname.endswith("down")):
                front, back = keyname.split("_finaldiscr_")
                channelkey = histkey.replace("$CHANNEL", back)
                c = ROOT.TCanvas()
                legend = ROOT.TLegend(0.72,0.5,0.95,0.99)
                ref = None
                abort = False
                hists = {}
                for refproc in normto_procs:
                    refkey = channelkey.replace("$PROCESS", refproc)
                    temp = reffile_source.Get(refkey)
                    if isinstance(temp, ROOT.TH1):
                        if ref:
                            ref.Add(temp)
                        else:
                            ref = temp.Clone("hSum")
                        hists[refproc] = temp.Clone("_".join([refproc, back]))
                        hists[refproc].GetXaxis().SetTitle("final discriminator "+ back.replace("ljets_", ""))
                        hists[refproc].GetYaxis().SetTitle("#Events")
                        hists[refproc].SetTitle("")
                    else:
                        print "could not find key {0} in reference file, aborting renorm process for {1}".format(refkey, keyname)
                        abort = True
                
                if not abort and ref:
                    print "renorming", keyname
                    htonorm = tonorm_source.Get(keyname)
                    integral = htonorm.Integral()
                    if integral == 0: integral = 1
                    htonorm.Scale(ref.Integral() / integral)
                    tonorm_source.cd()
                    
                    if not justDraw:
                        htonorm.Write(htonorm.GetName(), ROOT.TObject.kWriteDelete)
                    htonorm.SetLineColor(tonormcolors[process])
                    htonorm.SetLineWidth(3)
                    htonorm.SetMarkerColor(tonormcolors[process])
                    drawopt = "HIST"
                    stacked = make_stakes(hists, refcolors)
                    largest = None
                    latex = getLatex(0.12,0.91, "CMS private work")
                    for tup in sorted(stacked.items(), key = lambda x: abs(x[1].Integral()), reverse = True):
                        proc = tup[0]
                        if not largest:
                            largest = proc
                        h = tup[1]
                        h.SetFillStyle(1001)
                        if drawopt == "HIST":
                            h.GetYaxis().SetRangeUser(0, 30+h.GetBinContent(h.GetMaximumBin()))
                        h.Draw(drawopt)
                        drawopt = "HISTsame"
                        legend.AddEntry(h, "{0} Powheg".format(proc), "f")
                    htonorm.SetFillStyle(0)
                    htonorm.SetTitle("")
                    htonorm.Draw("HISTsame")
                    legend.AddEntry(htonorm, "{0} Sherpa".format(process), "l")
                    legend.Draw("same")
                    latex.Draw("same")
                    c.SaveAs("powheg_vs_sherpa_" + back + ".pdf")
                    c.SaveAs("powheg_vs_sherpa_" + back + ".root")
                    make_ratio_plot(basehist = stacked[largest], hists = [htonorm], labels = ["Powheg", "Sherpa"], outname = "powheg_vs_sherpa_" + back + "_ratios")
                else:
                    print "could not find histo to norm to! Skipping", keyname
    tonorm_source.Close()
    reffile_source.Close()
