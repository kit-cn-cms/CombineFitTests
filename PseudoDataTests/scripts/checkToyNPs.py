import ROOT
import sys
import os
import glob

ROOT.gROOT.SetBatch(True)

wildcard = sys.argv[1]
numberOfToysPerFile = sys.argv[2]

suffix = ""
if len(sys.argv) > 3:
    suffix = sys.argv[3]


histos = {}
errors = {}

for inputFile in glob.glob(wildcard):
    if os.path.exists(inputFile):
        infile = ROOT.TFile(inputFile)
        print "loaded file", inputFile

        infile.cd("toys")
        for toy in range(0,int(numberOfToysPerFile)):
            print "loading toy number", toy+1
            npSet = infile.Get("toys/toy_{0}_snapshot".format(toy+1))
            it = npSet.createIterator()
            var = it.Next()
            while var:
                print var.GetName()
                if not var.GetName() in histos:
                    histos[var.GetName()] = ROOT.TH1F(var.GetName(),"{0}; {0}; Frequency".format(var.GetName()), 100, -4,4)
                    histos[var.GetName()].SetDirectory(0)
                    errors[var.GetName()] = ROOT.TH1F("error_"+var.GetName(),"error_{0}; error_{0}; Frequency".format(var.GetName()), 100, -4,4)
                    errors[var.GetName()].SetDirectory(0)

                histos[var.GetName()].Fill(var.getVal())
                errors[var.GetName()].Fill(var.getError())
                var = it.Next()
        infile.Close()

print "\n__________________________________", histos, "\n_____________________ "

c = ROOT.TCanvas()
for var in histos:
    print "saving histos for variable", var
    histos[var].Draw()
    c.SaveAs(suffix+"_"+var+".pdf")
    c.SaveAs(suffix+"_"+var+".root")

    c.Clear()
    errors[var].Draw()

    c.SaveAs(suffix+"_"+var+"_errors.pdf")
    c.SaveAs(suffix+"_"+var+"_errors.root")
