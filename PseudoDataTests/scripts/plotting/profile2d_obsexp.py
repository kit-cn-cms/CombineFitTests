#!/usr/bin/env python
# compares observed and expecxted constraints for anomalous top-higgs coupling
# usage: python profile2d_obsexp.py filenames
# example: python profile2d_obsexp.py somefolder/*root

import sys
import ROOT 
from math import log
from math import sqrt
from math import exp
from math import pi
ROOT.gROOT.SetBatch(True)

def getLatex(x,y,text):
    tests = ROOT.TLatex(x, y,text)
    tests.SetTextFont(42)
    tests.SetTextSize(0.04)
    tests.SetNDC()
    return tests


def getCanvas(name='c',ratiopad=False):
    c=ROOT.TCanvas(name,name,1024,1024)
    c.SetRightMargin(0.14)
    c.SetTopMargin(0.12)
    c.SetLeftMargin(0.12)
    c.SetBottomMargin(0.12)
    c.SetTicks(1,1)

    return c
def getLegend(): 
    legend=ROOT.TLegend()
    legend.SetX1NDC(0.15)
    legend.SetX2NDC(0.45)
    legend.SetY1NDC(0.68)
    legend.SetY2NDC(0.86)
    legend.SetBorderSize(0);
    legend.SetLineStyle(0);
    legend.SetTextFont(42);
    legend.SetTextSize(0.04);
    legend.SetFillStyle(0);
    return legend


def setupPad(p):
    p.SetRightMargin(0.14)
    p.SetTopMargin(0.12)
    p.SetLeftMargin(0.12)
    p.SetBottomMargin(0.12)
    p.SetTicks(1,1)


def lnn(beta,err):
    return pow(1.+err,beta)


##################
allfiles=sys.argv[1:]
obs=[]
exp=[]
for a in allfiles:
    if 'obs' in a:
        obs.append(a)
    else:
        exp.append(a)
    
filenamess=[obs,exp]
colors=[ROOT.kBlack,ROOT.kRed,ROOT.kBlue,ROOT.kGreen+1]
outname='scan2d_obsexp.pdf'

c=getCanvas()
setupPad(c)

iFile=0
objects=[]

histos=[]

for filenames,color in zip(filenamess,colors):
    iFile+=1
    first=(iFile==1)
    g=ROOT.TGraph2D()
    objects.append(g)
    nll_best=999
    x_best=0.
    y_best=0.
    i=0    
    for filename in filenames:
        f=ROOT.TFile(filename)
        t=f.Get('limit')
        if not isinstance(t,ROOT.TTree):
            print filename,"does not contain limit tree"
            continue
        ROOT.gDirectory.cd('PyROOT:/')
        firstpoint=True
        for e in t:
            if firstpoint:
                firstpoint=False
                continue
            if nll_best>e.deltaNLL:
                nll_best=e.deltaNLL
                x_best=e.kappa_t
                y_best=e.kappa_tilde_t

            nll=2*e.deltaNLL
            nll=min(10,nll)
            g.SetPoint(i,e.kappa_t,e.kappa_tilde_t,nll)
            i+=1
            g.SetPoint(i,e.kappa_t,-e.kappa_tilde_t,nll)
            i+=1                
        f.Close()


    g.Draw('COLZ')
    g.GetHistogram().SetTitle('')
    g.GetHistogram().GetXaxis().SetTitle("#kappa_{t}")
    g.GetHistogram().GetYaxis().SetTitle("#tilde#kappa_{t}")
    g.GetHistogram().GetXaxis().SetTitleSize(0.06)
    g.GetHistogram().GetYaxis().SetTitleSize(0.06)
    g.GetHistogram().GetXaxis().SetLabelSize(0.04)
    g.GetHistogram().GetYaxis().SetLabelSize(0.04)
    g.GetHistogram().GetXaxis().SetTitleOffset(0.8)
    g.GetHistogram().GetYaxis().SetTitleOffset(0.8)
    g.GetHistogram().GetXaxis().SetRangeUser(-1.2,1.6)
    g.GetHistogram().GetYaxis().SetRangeUser(-1,1.25)
    h1=g.GetHistogram().Clone('h1'+str(iFile))
    objects.append(h1)
    h1.SetContour(1)
    h1.SetContourLevel(0,2.297)
    h1.SetLineColor(color)
    h1.SetLineWidth(2)
    if first:
        h1.SetLineWidth(3)
    h1.Draw('cont3')
    histos.append(h1.Clone())
    h2=g.GetHistogram().Clone('h2'+str(iFile))
    h2.SetContour(1)
    h2.SetContourLevel(0,5.991)
    h2.SetLineColor(color)
    h2.SetLineStyle(2)
    h2.SetLineWidth(2)
    if first:
        h2.SetLineWidth(3)
    h2.Draw('cont3 same')
    histos.append(h2.Clone())
    h3=g.GetHistogram().Clone('h3'+str(iFile))
    h3.SetContour(1)
    h3.SetLineColor(color)
    h3.SetLineStyle(3)
    h3.SetContourLevel(0,9.2104)
    h3.SetLineWidth(2)
    if first:
        h3.SetLineWidth(3)
    h3.Draw('cont3 same')
#    histos.append(h3.Clone())
    g.GetXaxis().SetTitle("#kappa_{t}")
    g.GetYaxis().SetTitle("#tilde#kappa_{t}")
    if first:
        bf=ROOT.TGraph()
        if y_best<0.04:
            y_best=0.
        bf.SetPoint(0,x_best,y_best)
        bf.SetPoint(1,x_best,-y_best)
        bf.SetMarkerStyle(34)
        bf.SetMarkerSize(3)
        objects.append(bf)






#    break
#print histos
histos[0].Draw('cont3')
for h in histos[1:]:
    h.Draw('cont3 same')


la1=getLatex(0.12,0.95,'Run 1')
la2=getLatex(0.12,0.91,'Higgs combination')
la3=getLatex(0.52,0.91,'#kappa_{W}, #kappa_{Z}, #kappa_{b}, #kappa_{#tau} profiled')
la4=getLatex(0.32,0.95,'5.1 fb^{-1} (7 TeV) + 19.7 fb^{-1} (8 TeV)')
    
    
la1.Draw()
la2.Draw()
la3.Draw()
la4.Draw()

l=getLegend()
dummy1=ROOT.TLine()
dummy1.SetLineStyle(1)
dummy1.SetLineWidth(3)
dummy2=ROOT.TLine()
dummy2.SetLineWidth(3)
dummy2.SetLineStyle(2)
dummy3=ROOT.TLine()
dummy3.SetLineStyle(3)

dummyR=ROOT.TLine()
dummyR.SetLineWidth(2)
dummyB=ROOT.TLine()
dummyB.SetLineWidth(2)
dummyG=ROOT.TLine()
dummyG.SetLineWidth(2)
dummyR.SetLineColor(ROOT.kRed)
dummyB.SetLineColor(ROOT.kBlue)
dummyG.SetLineColor(ROOT.kGreen+1)

sm=ROOT.TGraph()
sm.SetPoint(0,1,0)
sm.SetMarkerStyle(29)
sm.SetMarkerSize(3)
sm.SetMarkerColor(ROOT.kRed)
bf.Draw('p')
sm.Draw('p')


l.AddEntry(dummy1,'Obs. 68% CL',"L")
l.AddEntry(dummy2,'Obs. 95% CL',"L")
l.AddEntry(bf,'Best fit',"P")
l.AddEntry(dummyR,'Expected',"L")
l.Draw()

c.SaveAs( outname)

