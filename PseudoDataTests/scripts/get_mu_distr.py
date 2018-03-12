from ROOT import TFile, TCanvas, TH1, gStyle, TTree, gROOT
from os import path
from sys import argv
from glob import glob

gROOT.SetBatch(True)

files = argv[1:]

for word in files:
    for f in glob(word):
        if not path.exists(f):
            print "could not file", f
            continue
        if not f.endswith(".root"):
            print "input must be .root file!"
            continue
        dirname = path.dirname(path.abspath(f))
        print "saving pdf in", dirname
        infile = TFile(f)
        if infile.IsOpen() and not infile.IsZombie() and not infile.TestBit(TFile.kRecovered):
            tree_fit_sb = infile.Get("tree_fit_sb")
            gStyle.SetOptStat(221112211)
            c = TCanvas()
            tree_fit_sb.Draw("r")
            outname = dirname +"/r_" + path.basename(f)
            outname = outname.replace(".root", ".pdf")
            c.SaveAs(outname)
            infile.Close()
        else:
            print "file is broken: ", f
