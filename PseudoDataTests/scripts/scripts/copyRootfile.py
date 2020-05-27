import os
import sys
import ROOT

tocopy = sys.argv[1]

ignoreProcesses = ["ttbarPlusBBbar", "ttbarPlusB", "ttbarPlus2B"]
copyOnly = ["ttbarPlusBBbar"]

if os.path.exists(tocopy) and tocopy.endswith(".root"):
    tocopy = os.path.abspath(tocopy)
    infile = ROOT.TFile(tocopy)
    if len(copyOnly) == 0:
        removed = "_{0}_removed.root".format("_".join(ignoreProcesses))
    else:
        removed = "_only_{0}.root".format("_".join(copyOnly))
    outfile = ROOT.TFile(tocopy.replace(".root", removed), "RECREATE")
    if isinstance(infile, ROOT.TFile) and infile.IsOpen() and not infile.IsZombie() and not infile.TestBit(ROOT.TFile.kRecovered):
        
        for key in infile.GetListOfKeys():
            keyname = key.GetName()
            temp = None
            # print "checking", keyname
            if len(copyOnly) == 0:
                if not any(keyname.startswith(x) for x in ignoreProcesses):
                    temp = infile.Get(keyname)
            else:
                if any(keyname.startswith(x) for x in copyOnly) and not (keyname.endswith("Up") or keyname.endswith("Down") or keyname.endswith("up") or keyname.endswith("down")):
                    temp = infile.Get(keyname)
            if temp and isinstance(temp, ROOT.TH1):
                outfile.cd()
                temp.Write()
            else:
                print "skipping %s" % keyname
            # else:
                # 
                    
        outfile.Close()
        infile.Close()
