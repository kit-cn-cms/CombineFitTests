import ROOT
import sys
import os
from array import array 
ROOT.gROOT.SetBatch(True)
ROOT.gStyle.SetOptStat(0)
import imp
configpath = sys.argv[1]
infname=sys.argv[2]
print "init"
if os.path.exists(configpath):
	config = imp.load_source("config", configpath)
	from config import *
else:
	sys.exit("ERROR: COULD NOT FIND CONFIG IN '%s'" % configpath)

linewidth = 3
inf=ROOT.TFile(infname,"READ")

name="allshapes"
if not os.path.exists(name+"SystShapes"):
    os.makedirs(name+"SystShapes")
    
buff=ROOT.TCanvas("buff","buff",1200,1000)
buff.Print(name+'.pdf[')
outname_template = "{syst}__{proc}__{chan}"
exts = ["pdf", "png"]
def getCanvas(name):
	c = ROOT.TCanvas(name, name, 1024, 1024)
	c.Divide(1, 2)
	c.cd(1).SetPad(0., 0.3, 1.0, 1.0)
	c.cd(1).SetBottomMargin(0.0)
	c.cd(2).SetPad(0., 0.0, 1.0, 0.3)
	c.cd(2).SetTopMargin(0.0)
	c.cd(1).SetTopMargin(0.07)
	c.cd(2).SetBottomMargin(0.4)
	c.cd(1).SetRightMargin(0.05)
	c.cd(1).SetLeftMargin(0.15)
	c.cd(2).SetRightMargin(0.05)
	c.cd(2).SetLeftMargin(0.15)
	c.cd(2).SetTicks(1, 1)
	c.cd(1).SetTicks(1, 1)
	return c

counter=0
for c in cats:
	print c
	prockey = key.replace("$CHANNEL", c)
	systprockey = systkey.replace("$CHANNEL", c)
	for p in procs:
		nomhistkey = prockey.replace("$PROCESS", p)
		systhistkey = systprockey.replace("$PROCESS", p)
		firstnom=inf.Get(nomhistkey)
		if not isinstance(firstnom, ROOT.TH1):
			print "DANGERZONE! Could not find nominal histo %s" % nomhistkey
			continue
		for s in systs:
			nom=firstnom.Clone()
			finalsysthistkey = systhistkey.replace("$SYSTEMATIC", s)
			print finalsysthistkey
			up=inf.Get(finalsysthistkey+"Up")
			down=inf.Get(finalsysthistkey+"Down")
			print down, up
			if up!=None and down!=None:
				up.SetLineColor(ROOT.kRed)
				up.SetTitle("up")
				down.SetTitle("down")
				down.SetLineColor(ROOT.kBlue)
				nom.SetLineColor(ROOT.kBlack)
				up.SetLineWidth(linewidth)
				down.SetLineWidth(linewidth)
				nom.SetLineWidth(linewidth)
				finalsysthistkey = finalsysthistkey.replace("/", "_")
				canvas=getCanvas(finalsysthistkey)
				# canvas=ROOT.TCanvas(finalsysthistkey,finalsysthistkey,800,600)
				canvas.cd(1)
				maxval=max(nom.GetMaximum(), max(up.GetMaximum(),down.GetMaximum()))
				# nom.SetMaximum(1.5*maxval)
				nom.GetYaxis().SetRangeUser(1e-5, 1.5*maxval)
				nom.Draw("histoE")
				nom.SetTitle(c+"_"+p+" "+s)
				up.Draw("histoSameE")
				down.Draw("histoSameE")
				integNom=nom.Integral()
				integDown=down.Integral()
				integUp=up.Integral()
				fracUp=0
				fracDown=0
				if integNom!=0:
					fracDown=integDown/integNom
					fracUp=integUp/integNom
					
				canvas.SetTitle(c+"_"+p+" "+s)
				#canvas.BuildLegend()
				legend=ROOT.TLegend(0.15,0.7,0.95, 0.88)
				#legend.SetX1NDC(0.15)
				#legend.SetX2NDC(0.95)
				#legend.SetY1NDC(0.7)
				#legend.SetY2NDC(0.88)
				legend.SetBorderSize(0);
				legend.SetLineStyle(0);
				legend.SetTextFont(42);
				legend.SetTextSize(0.03);
				legend.SetFillStyle(0);
				legend.AddEntry(nom,c+"_"+p+" (%.2f)" % integNom,"l")
				legend.AddEntry(up,c+"_"+p+" Up"+" (%.2f, %.2f" % (integUp,fracUp) +")","l")
				legend.AddEntry(down,c+"_"+p+" Down"+" (%.2f, %.2f" % (integDown,fracDown) +")","l")
				legend.Draw("same")

				print "saving canvases"
				# canvas.Print(name+'.pdf')
				# canvas.SaveAs(name+"SystShapes/"+finalsysthistkey+".png")
				# canvas.SaveAs(name+"SystShapes/"+finalsysthistkey+".pdf")
				# ratioc=ROOT.TCanvas("ratio_" + finalsysthistkey,"ratio_" + finalsysthistkey,800,300)
				canvas.cd(2)
				nomr=nom.Clone()
				y_nomr = nomr.GetYaxis()
				y_nomr.SetTitle("#frac{variation}{nominal}")
				y_nomr.SetTitleSize(0.06)
				y_nomr.CenterTitle()
				nomr.Divide(nom)
				
				upr=up.Clone()
				upr.Divide(nom)
				downr=down.Clone()
				downr.Divide(nom)
				rmax = max(upr.GetBinContent(upr.GetMaximumBin()), downr.GetBinContent(downr.GetMaximumBin()))
				rmin = min(upr.GetBinContent(upr.GetMinimumBin()), downr.GetBinContent(downr.GetMinimumBin()))
				nomr.GetYaxis().SetRangeUser(rmin*0.8,rmax*1.2)
				nomr.Draw("histoE")
				upr.Draw("samehistoE")
				downr.Draw("samehistoE")
				# legend.Draw("same")
				# ratioc.SetTitle(nom.GetTitle())
				# ratioc.SaveAs(name+"SystShapes/Ratio_"+finalsysthistkey+".png")
				# ratioc.SaveAs(name+"SystShapes/Ratio_"+finalsysthistkey+".pdf")
				canvas.Print(name+'.pdf')
				outname = outname_template.format(	syst = s,
													proc = p,
													chan = c	
												)
				for ext in exts:
					canvas.SaveAs("{}SystShapes/{}.{}".format(name,outname,ext))
				counter+=1
buff.Print(name+'.pdf]')
	
	#exit(0)
