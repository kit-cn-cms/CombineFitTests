import os
import sys
import ROOT

ROOT.gROOT.SetBatch(True)
paths = sys.argv[1:]

procsToCompare = ["ttbarPlusBBbar"]
histkey = "$PROCESS_finaldiscr_$CHANNEL"
channels = "ljets_j5_t3  ljets_jge6_tge4 ljets_jge6_t3 ljets_j4_t3 ljets_j5_tge4 ljets_j4_t4".split()
colors = [ROOT.kBlue, ROOT.kRed, ROOT.kOrange, ROOT.kMagenta]
ROOT.gStyle.SetOptStat(0)

if all(os.path.exists(inpath) for inpath in paths):
	
	rootfiles = [ROOT.TFile(path) for path in paths]
	if all(x.IsOpen() and not x.IsZombie() and not x.TestBit(ROOT.TFile.kRecovered) for x in rootfiles):
		can = ROOT.TCanvas()
		leg = ROOT.TLegend(0.7,0.1, 0.9, 0.3)
		for proc in procsToCompare:
			procKey = histkey.replace("$PROCESS", proc)
			for ch in channels:
				keyname = procKey.replace("$CHANNEL", ch)
				
				first = True
				lhists = []
				n = 0
				for f, col in zip(rootfiles, colors):
					print "loading from", f.GetName()
					print "color:", col
					hist = f.Get(keyname)
					if isinstance(hist, ROOT.TH1):
						print "hist output:\n", hist.Print()
						hist.SetLineColor(col)
						hist.SetMarkerColor(col)
						lhists.append(hist.Clone(hist.GetName() + "_" + f.GetName()))
						print "#histos:", lhists
						if first:
							lhists[-1].Draw()
							
							first = False
						else:
							print "adding histo to canvas"
							lhists[-1].Draw("Same")
						print "color in list:",lhists[-1].GetLineColor()
						leg.AddEntry(lhists[-1],os.path.basename(paths[n]), "l")
					else:
						print "could not find {0} in {1}".format(keyname, paths[n])
					n+=1
					
				if len(lhists)>1:
					leg.Draw("Same")
					can.SaveAs("comparison_" + ch + ".pdf")
					can.SaveAs("comparison_" + ch + ".root")
					
				else:
					print "could only find {0} histograms, skipping key {1}".format(len(lhists), keyname)
				can.Clear()
				leg.Clear()
