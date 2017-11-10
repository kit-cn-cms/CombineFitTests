import ROOT
import os
import sys

ROOT.gROOT.SetBatch(True)

inpath = os.path.abspath(sys.argv[1])

listOfProcesses = ["ttbarOther", "ttbarPlusB", "ttbarPlus2B", "ttbarPlusBBbar", "ttbarPlusCCbar"]
# listOfProcesses = "ttbarOther ttbarPlusB ttbarPlus2B ttbarPlusBBbar ttbarPlusCCbar singlet wjets zjets ttbarW ttbarZ diboson".split()
# categories = ["ljets_j4_t2", "ljets_j4_t3", "ljets_j4_t4", "ljets_j5_t2", "ljets_j5_t3", "ljets_j5_tge4", "ljets_jge6_t2", "ljets_jge6_t3", "ljets_jge6_tge4"]

categories = [  "ljets_j4_t4", 
                "ljets_j5_tge4", 
                "ljets_jge6_t3", 
                "ljets_jge6_tge4",
                "ljets_j5_t3",
                "ljets_j4_t3",
                "ljets_jge6_t2",
                
                "ljets_j4_t4_high", 
                "ljets_j5_tge4_high", 
                "ljets_jge6_t3_high", 
                "ljets_jge6_tge4_high",
                # "ljets_j5_t3_high",
                # "ljets_j4_t3_high",
                
                "ljets_j4_t4_low", 
                "ljets_j5_tge4_low", 
                "ljets_jge6_t3_low", 
                "ljets_jge6_tge4_low",
                # "ljets_j5_t3_low",
                # "ljets_j4_t3_low",
                
                "ljets_j4_t4_MEMONLY", 
                "ljets_j5_tge4_MEMONLY", 
                "ljets_jge6_t3_MEMONLY", 
                "ljets_jge6_tge4_MEMONLY",
                # "ljets_j5_t3_MEMONLY",
                # "ljets_j4_t3_MEMONLY"
                
                "ljets_j4_t4_ttbbOpt",
                "ljets_j5_tge4_ttbbOpt",
                "ljets_jge6_t3_ttbbOpt",
                "ljets_jge6_tge4_ttbbOpt",
                
                "ljets_j4_t4_ttbbOpt_high",
                "ljets_j5_tge4_ttbbOpt_high", 
                "ljets_jge6_t3_ttbbOpt_high",
                "ljets_jge6_tge4_ttbbOpt_high",
                
                "ljets_j4_t4_ttbbOpt_low",
                "ljets_j5_tge4_ttbbOpt_low", 
                "ljets_jge6_t3_ttbbOpt_low",
                "ljets_jge6_tge4_ttbbOpt_low",
                
                "ljets_j4_t3_BLR",
                "ljets_j5_t3_BLR",
                "ljets_jge6_t3_BLR",
                "ljets_jge6_t2_BLR",
                
                "ljets_j4_tge3_ttHnode",
                "ljets_j5_tge3_ttHnode",
                "ljets_jge6_tge3_ttHnode",
                
                "ljets_j4_tge3_ttbbnode",
                "ljets_j5_tge3_ttbbnode",
                "ljets_jge6_tge3_ttbbnode",
                
                "ljets_j4_tge3_ttbnode",
                "ljets_j5_tge3_ttbnode",
                "ljets_jge6_tge3_ttbnode",
                
                "ljets_j4_tge3_tt2bnode",
                "ljets_j5_tge3_tt2bnode",
                "ljets_jge6_tge3_tt2bnode",
                
                "ljets_j4_tge3_ttccnode",
                "ljets_j5_tge3_ttccnode",
                "ljets_jge6_tge3_ttccnode",
                
                "ljets_j4_tge3_ttlfnode",
                "ljets_j5_tge3_ttlfnode",
                "ljets_jge6_tge3_ttlfnode"
                
                ] #list of categories used for the fit

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
