import ROOT
import os
import sys

ROOT.gROOT.SetBatch(True)

file1 = sys.argv[1]
file2 = sys.argv[2]

file1 = os.path.abspath(file1)
file2 = os.path.abspath(file2)

if os.path.exists(file1):
    if os.path.exists(file2):
        infile1 = ROOT.TFile(file1)
        infile2 = ROOT.TFile(file2)

        listOfKeys = infile1.GetListOfKeys()

        for key in listOfKeys:
            histo1 = infile1.Get(key.GetName())
            histo2 = infile2.Get(key.GetName())

            if isinstance(histo1, ROOT.TH1) and isinstance(histo2, ROOT.TH1):
                int1 = histo1.Integral()
                int2 = histo2.Integral()

                if not int1 == int2:
                    print "Detected different histos!"
                    print "\tName: {0}\tInt1 = {1}\tInt2 = {2}".format(key.GetName(), int1, int2)

    else:
        sys.exit("Could not find file %s" % file2)
else:
    sys.exit("Could not find file %s" % file1)
