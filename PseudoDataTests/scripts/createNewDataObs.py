import ROOT
import os
import sys

ROOT.gROOT.SetBatch(True)

inpath = os.path.abspath(sys.argv[1])

listOfProcesses = ["ttbarOther", "ttbarPlusB", "ttbarPlus2B", "ttbarPlusBBbar", "ttbarPlusCCbar"]
categories = ["ljets_j4_t2", "ljets_j4_t3", "ljets_j4_t4", "ljets_j5_t2", "ljets_j5_t3", "ljets_j5_tge4", "ljets_jge6_t2", "ljets_jge6_t3", "ljets_jge6_tge4"]
suffix = "all_Bkg_"
key = "$PROCESS_finaldiscr_$CHANNEL"

if os.path.exists(inpath):
    infile = ROOT.TFile(inpath, "UPDATE")
    if infile.IsOpen() and not infile.IsZombie() and not infile.TestBit(ROOT.TFile.kRecovered):
        for cat in categories:
            catKey = key.replace("$CHANNEL", cat)
            orig_data_obs = infile.Get(catKey.replace("$PROCESS", "data_obs"))
            if isinstance(orig_data_obs, ROOT.TH1):
                new = orig_data_obs.Clone()
                orig_data_obs.SetName(suffix+orig_data_obs.GetName())
                print "saving original data_obs as {0}\t Integral = {1}".format(orig_data_obs.GetName(), orig_data_obs.Integral())
                orig_data_obs.Write()

                new.Reset()
                for proc in listOfProcesses:
                    h = infile.Get(catKey.replace("$PROCESS", proc))
                    if isinstance(h, ROOT.TH1):
                        new.Add(h)
                    else:
                        print "WARNING! Could not load template for process", proc
                print "saving new data_obs as {0}\t Integral = {1}".format(new.GetName(), new.Integral())
                new.Write(new.GetName(), ROOT.TObject.kWriteDelete)
            else:
                print "Could not find data_obs histo for category", cat

        infile.Close()
    else:
        sys.exit("Could not properly open file %s" % inpath)

else:
    sys.exit("Could not find root file in %s" % inpath)
