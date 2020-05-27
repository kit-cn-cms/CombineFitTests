import ROOT
import os
import sys
ROOT.gROOT.SetBatch(True)


correlationspath = sys.argv[1]
cut = float(sys.argv[2])

def filter_values(hIn, cut = 0.10):
    nx = hIn.GetNbinsX()
    ny = hIn.GetNbinsY()
    print "checking {0}x{1} histogram".format(nx, ny)
    params = []
    lines = []
    for x in range(1,nx+1):
        for y in range(x+1, ny+1):
            val = hIn.GetBinContent(x,y)
            error = hIn.GetBinError(x,y)
            if abs(val) >= cut:
                l = "corr({0},{1}) = {2} +- {3}".format(hIn.GetXaxis().GetBinLabel(x), hIn.GetYaxis().GetBinLabel(y), val, error)
                print l
                if not x in params:
                    params.append(x)
                if not y in params:
                    params.append(y)
                
    params.sort()
    print params
    
    nfiltered = len(params)
    if nfiltered != 0:
        print "initializing filtered histogram with {0} bins".format(nfiltered)
        filtered = ROOT.TH2D("filtered", ";;",nfiltered, 0, nfiltered, nfiltered, 0, nfiltered)
        for i,nparx in enumerate(params, 1):
            parname = hIn.GetXaxis().GetBinLabel(nparx)
            filtered.GetXaxis().SetBinLabel(i, parname)
            filtered.GetYaxis().SetBinLabel(i, parname)
            for j,npary in enumerate(params, 1):
                val = hIn.GetBinContent(nparx, npary)
                error = hIn.GetBinError(nparx, npary)
                l = "corr({0},{1}) = {2} +- {3}".format(parname, hIn.GetYaxis().GetBinLabel(npary), val, error)
                lines.append(l)
                filtered.SetBinContent(i,j,val)
                filtered.SetBinError(i,j,error)       
        filtered.GetZaxis().SetRangeUser(-1,1)
        return filtered, lines
    else:
        print "found no parameters!"
        return None
                
if os.path.exists(correlationspath):
    correlationspath = os.path.abspath(correlationspath)
    dirname = os.path.dirname(correlationspath)
    basename = os.path.basename(correlationspath)
    corfile = ROOT.TFile.Open(correlationspath)
    if corfile.IsOpen() and not corfile.IsZombie() and not corfile.TestBit(ROOT.TFile.kRecovered):
        keylist = corfile.GetListOfKeys()
        for key in keylist:
            clname = key.GetClassName()
            if clname.startswith("TH2"):
                keyname = key.GetName()
                h = corfile.Get(keyname)
                print h
                print cut
                new, lines = filter_values(hIn = h, cut = cut)
    if not new is None:
        ROOT.gStyle.SetOptStat(0)
        outname = dirname+"/filtered_"+str(cut).replace(".", "p")+"_"+basename
        outfile = ROOT.TFile(outname, "RECREATE")
        can = ROOT.TCanvas()
        can.SetTopMargin(can.GetTopMargin()*0.2)
        can.SetLeftMargin(can.GetLeftMargin()*2.6)
        can.SetBottomMargin(can.GetBottomMargin()*1.8)
        can.SetRightMargin(can.GetRightMargin()*1.3)
        new.Draw("colz")
        new.Write()
        can.SaveAs(outname.replace(".root", ".pdf"))
        can.Write()
        outfile.Close()
        fname = outname.replace(".root", ".txt")
        print "opening", fname
        with open(fname, "w") as f:
            f.write("\n".join(lines))
    corfile.Close()
