import os
import sys
import ROOT

tocopy = sys.argv[1]

ignoreProcesses = ["ttbarPlusBBbar", "ttbarPlusB", "ttbarPlus2B"]

if os.path.exists(tocopy) and tocopy.endswith(".root"):
    tocopy = os.path.abspath(tocopy)
    infile = ROOT.TFile(tocopy)
    removed = "_{0}_removed.root".format("_".join(ignoreProcesses))
    outfile = ROOT.TFile(tocopy.replace(".root", removed), "RECREATE")
    if isinstance(infile, ROOT.TFile) and infile.IsOpen() and not infile.IsZombie() and not infile.TestBit(ROOT.TFile.kRecovered):
        for key in infile.GetListOfKeys():
            keyname = key.GetName()
            # print "checking", keyname
            if not any(keyname.startswith(x) for x in ignoreProcesses) or (keyname.endswith("Up") or keyname.endswith("Down")):
                temp = infile.Get(keyname)
                if isinstance(temp, ROOT.TH1):
                    outfile.cd()
                    temp.Write()
                else:
                    print "%s is not a histogram!" % keyname
            else:
                print "skipping %s" % keyname
        outfile.Close()
        infile.Close()
