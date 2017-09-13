#!/usr/bin/env python
# plots profile likelihood scans
# usage: python profile1d.py variable filenames
# example: python profile1d.py kappa_t somefolder/*root

import sys
import ROOT 
import re

# max y-range
ymax=8
 # files that contain the substring 'obs' are assumed to be data
data_identifier=['data','obs']

def tryint(s):
    try:
        return int(s)
    except:
        return s

def alphanum_key(s):
    """ Turn a string into a list of string and number chunks.
        "z23a" -> ["z", 23, "a"]
    """
    return [ tryint(c) for c in re.split('([0-9]+)', s) ]

def sort_nicely(l):
    """ Sort the given list in the way that humans expect.
    """
    l.sort(key=alphanum_key)

ROOT.gROOT.SetBatch(True)

# default format for latex lables
def getLatex(x,y,text):
    tests = ROOT.TLatex(x, y,text)
    tests.SetTextFont(42)
    tests.SetTextSize(0.04)
    tests.SetNDC()
    return tests

# default format of canvas
def getCanvas(name='c',ratiopad=False):
    c=ROOT.TCanvas(name,name,1024,1024 )
    c.SetRightMargin(0.02)
    c.SetTopMargin(0.08)
    c.SetLeftMargin(0.12)
    c.SetBottomMargin(0.12)
    c.SetTicks(1,1)

    return c

# default format for histograms
def setupHisto(histo):
#    histo.SetStats(False)
    histo.SetTitleSize(0.02)
    histo.SetTitleOffset(1.2)
    histo.GetXaxis().SetTitleSize(0.05)
    histo.GetYaxis().SetTitleSize(0.05)
    histo.GetXaxis().SetLabelSize(0.04)
    histo.GetYaxis().SetLabelSize(0.04)
    histo.GetXaxis().SetTitleOffset(0.9)
    histo.GetYaxis().SetTitleOffset(0.9)
    histo.SetLineWidth(2)
    histo.SetLineColor(ROOT.kBlack)
    histo.GetXaxis().SetTitle('#mu_{t#bar{t}H}')
    histo.GetYaxis().SetTitle('-2 #Delta log L')
#    histo.GetZaxis().SetTitle('S/(S+B)')
    histo.GetZaxis().SetTitleSize(0.05)
    histo.GetZaxis().SetLabelSize(0.04)

# default format for legend
def getLegend(): 
    legend=ROOT.TLegend()
    legend.SetX1NDC(0.15)
    legend.SetX2NDC(0.45)
    legend.SetY1NDC(0.73)
    legend.SetY2NDC(0.83)
    legend.SetBorderSize(0);
    legend.SetLineStyle(0);
    legend.SetTextFont(42);
    legend.SetTextSize(0.04);
    legend.SetFillStyle(0);
    return legend


# dict of latex aliases of parameters
alias={}
alias['kappa_t']='#kappa_{t}'
alias['kappa_tilde_t']='#tilde{#kappa}_{t}'
alias['kappa_b']='#kappa_{b}'
alias['kappa_tau']='#kappa_{#tau}'
alias['kappa_W']='#kappa_{W}'
alias['kappa_Z']='#kappa_{Z}'
alias['zeta_t']='#zeta_{t}'

# from the third argument on filenames are given
allfiles=sys.argv[2:]
# files are put in the right order
sort_nicely(allfiles)

# files that contain the substring data_identifier are assumed to be data
obs=[]
exp=[]
for a in allfiles:
    if any(i in a for i in data_identifier):
        obs.append(a)
    else:
        exp.append(a)
    
filenamess=[obs,exp]

# names as displayed in legend
names=['Observed','SM expected']
# name and title of yvar 
yvar='deltaNLL'
ytitle='-2 #Delta log L'

# xvar comes from user input
xvar=sys.argv[1]
outname='scan_'+xvar+'.pdf'
xtitle=alias[xvar]
linestyles=[0,1]
colors=[ROOT.kBlack,ROOT.kRed]
horizontal_lines=[1,4]
xfact=1.
# scale y-var by a factor of two to get 2deltaNlL
yfact=2.
# no title drawn on canvas
title=''

graphs=[]
real=True
# for data and expecation
for filenames in filenamess:
    g=ROOT.TGraph()
    i=0    
    pairs=[]
    # for all files from fit of data/asimov
    for filename in filenames:
        f=ROOT.TFile(filename)
        # get tree
        t=f.Get('limit')
        # check if t is really a tree
        if not isinstance(t,ROOT.TTree):
            print filename,"does not contain limit tree"
            continue
        ROOT.gDirectory.cd('PyROOT:/')
        firstpoint=True
        # get all x and y values
        for e in t:
            # first point in tree corresponds to best-fit value
            if firstpoint:
                firstpoint=False
                if real:
                    bestfit=xfact*getattr(e,xvar)
                else:
                    sm=xfact*getattr(e,xvar)
                continue
            if yfact*getattr(e,yvar)< ymax:
                pairs.append((xfact*getattr(e,xvar),yfact*getattr(e,yvar)))
            i+=1
        f.Close()
    # sort by increasing x-value
    pairs.sort(key=lambda tup: tup[0])
    for i,p in enumerate(pairs):
        g.SetPoint(i,p[0],p[1])
    firstx=pairs[0][0]
    lastx=pairs[-1][0]
    graphs.append(g)
    real=False

c=getCanvas()
graphs[0].SetLineColor(colors[0])
graphs[0].SetLineStyle(linestyles[0])
graphs[0].Draw('AL')
setupHisto(graphs[0].GetHistogram())

g=graphs[0]
g.SetTitle(title)
g.GetHistogram().GetXaxis().SetTitle(xtitle)
g.GetHistogram().GetYaxis().SetTitle(ytitle)

objects=[]
xmax=g.GetHistogram().GetXaxis().GetXmax()
xmin=g.GetHistogram().GetXaxis().GetXmin()
for y in horizontal_lines:
    objects.append(ROOT.TLine(xmin,y,xmax,y))
    objects[-1].Draw()




# get best-fit and 1-sigma intervals from graph
k_obs=''
k_exp=''
for ig,graph in enumerate(graphs):
    low=None
    high=None
    best=None
    lowest=999.
    # dumb algo to find crossing of graph and 1
    for i in range(10000):
        x=firstx+i*(lastx-firstx)/10000.
        y=graph.Eval(x)#,0,'S')    
        if low==None and y<1:
            low=x
        if low!=None and y<1:
            high=x
        if y<lowest:
            lowest=y
            best=x

#            break
#    sm=1.
    if ig==0:
        k_obs+=str(round(bestfit,2))+'^{+'
        if high!=None:
            k_obs+=str(round(high-bestfit,2))
        k_obs+='}'
        if low!=None:
            k_obs+='_{'+str(round(low-bestfit,2))+'}'
    else:
        k_exp+=str(round(sm,2))+'^{+'
        if high!=None:
            k_exp+=str(round(high-sm,2))
        k_exp+='}'
        if low!=None:
            k_exp+='_{'+str(round(low-sm,2))+'}'


la3=getLatex(0.15,0.85,'Higgs combination')
ks=['#kappa_{t}', '#kappa_{W}', '#kappa_{Z}', '#kappa_{b}', '#kappa_{#tau}']
prfld=[]
for k in ks:
    if not xtitle == k:
        prfld.append(k)
la4=getLatex(0.5,0.78,xtitle+' = '+k_obs+' ('+k_exp+' exp.)')
la5=getLatex(0.5,0.85, ', '.join(prfld)+' profiled')

la1=getLatex(0.12,0.95,'Run 1')
la2=getLatex(0.44,0.95,'5.1 fb^{-1} (7 TeV) + 19.7 fb^{-1} (8 TeV)')
la1.Draw()
la2.Draw()
la3.Draw()
la4.Draw()
la5.Draw()

gs=[]
l=getLegend()
for g,cl,n,st in reversed(zip(graphs,colors,names,linestyles)):    
    g.SetLineColor(cl)
    g.SetLineWidth(2)
    graphs[0].SetLineWidth(3)
    g.SetLineStyle(st)
    g.GetHistogram().SetMaximum(7)
    g.Draw('sameL')
    g.GetHistogram().GetYaxis().SetRangeUser(0,ymax*1.25)
    gs.append(g)
for g,cl,n,st in zip(graphs,colors,names,linestyles):    
    l.AddEntry(g,n,'l')
l.Draw('same')
c.SaveAs(outname)

