import ROOT
import os
import sys

ROOT.gROOT.SetBatch(True)

writeToFile = True

# listOfProcesses = ["ttbarOther", "ttbarPlusB", "ttbarPlus2B", "ttbarPlusBBbar", "ttbarPlusCCbar"]
# listOfProcesses = ["ttbarOther", "ttbarPlusBBbar", "ttbarPlusCCbar"]
# listOfProcesses = "ttbarZ          diboson         ttbarPlusB      ttbarW          singlet         wjets           ttbarPlus2B     ttbarOther      ttbarPlusBBbar  zjets           ttbarPlusCCbar".split()
# listOfProcesses = "ttH_hbb ttbarZ          diboson         ttbarPlusB      ttbarW          singlet         wjets           ttbarPlus2B     ttbarOther      ttbarPlusBBbar  zjets           ttbarPlusCCbar".split()
listOfProcesses = "ttH_hbb ttbarZ diboson ttbarW singlet wjets ttbarOther ttbarPlusBBbar zjets ttbarPlusCCbar".split()
# listOfProcesses = " ttbarZ diboson ttbarW singlet wjets ttbarOther ttbarPlusBBbar zjets ttbarPlusCCbar".split()
# categories = ["ljets_j4_t2", "ljets_j4_t3", "ljets_j4_t4", "ljets_j5_t2", "ljets_j5_t3", "ljets_j5_tge4", "ljets_jge6_t2", "ljets_jge6_t3", "ljets_jge6_tge4"]


suffix = "orig_"
key = "$PROCESS_finaldiscr_$CHANNEL"

def main(args = sys.argv[1:]):
    mode = args[0]
    if not mode == "pseudo":
        listOfProcesses = ["SingleMu", "SingleEl"]

    inpath = os.path.abspath(args[1])

    if os.path.exists(inpath):
        infile = ROOT.TFile(inpath, "UPDATE")
        if infile.IsOpen() and not infile.IsZombie() and not infile.TestBit(ROOT.TFile.kRecovered):
            nomkeys = [x.GetName() for x in infile.GetListOfKeys() if not (x.GetName().endswith("Up") or x.GetName().endswith("Down"))]
            categories = []
            for nomkey in nomkeys:
                cat = nomkey.split("_finaldiscr_")[-1]
                categories = list(set(categories + [cat]))
            print categories
            for cat in categories:
                catKey = key.replace("$CHANNEL", cat)
                orig_data_obs = infile.Get(catKey.replace("$PROCESS", "data_obs"))
                if isinstance(orig_data_obs, ROOT.TH1):
                    orig_data_obs.SetName(suffix+orig_data_obs.GetName())
                    print "saving original data_obs as {0}\t Integral = {1}".format(orig_data_obs.GetName(), orig_data_obs.Integral())
                    if writeToFile:
                        print "saving"
                        orig_data_obs.Write()

                else:
                    print "Could not find data_obs histo for category", cat
                
                new = None

                for proc in listOfProcesses:
                    hname = catKey.replace("$PROCESS", proc)
                    print "loading", hname
                    h = infile.Get(hname)
                    if isinstance(h, ROOT.TH1):
                        if new is None:
                            new = h.Clone(catKey.replace("$PROCESS", "data_obs"))
                        else:
                            new.Add(h)
                    else:
                        print "WARNING! Could not load template for process", proc
                if not new is None:
                    print "saving new data_obs as {0}\t Integral = {1}".format(new.GetName(), new.Integral())
                    if writeToFile:
                        print "saving"
                        new.Write(new.GetName(), ROOT.TObject.kWriteDelete)
                else:
                    print "COULD NOT CONSTRUCT DATA_OBS FOR CAT '%s'" % cat

            infile.Close()
        else:
            sys.exit("Could not properly open file %s" % inpath)

    else:
        sys.exit("Could not find root file in %s" % inpath)


if __name__ == '__main__':
    main()