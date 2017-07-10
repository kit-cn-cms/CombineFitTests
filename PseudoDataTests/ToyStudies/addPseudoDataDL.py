import sys
import ROOT
#ROOT.gDirectory.cd('PyROOT:/')
ROOT.gROOT.SetBatch(True)

infn=sys.argv[1]

cats=["dl_j3_t3_BDT","dl_gej4_t3_high_BDT","dl_gej4_t3_low_BDT","dl_gej4_get4_high_BDT","dl_gej4_get4_low_BDT"]

procs=['ttbarOther', 'ttbarPlusB', 'ttbarPlus2B', 'ttbarPlusBBbar', 'ttbarPlusCCbar', 'singlet', 'wjets', 'zjets', 'ttbarW', 'ttbarZ', 'diboson']

disc="finaldiscr"

results=[]

f=ROOT.TFile(infn,"UPDATE")

for c in cats:
  f.ls()
  #f.cd(0)
  f.cd(c)
  f.ls()
  OldData=f.Get(c+"/data_obs")
  print OldData, OldData.Integral()
  OldData.SetTitle("OLDDATA")
  OldData.SetName("OLDDATA")
  OldData.Write()
  
  stack=f.Get(c+"/"+procs[0]).Clone()
  stack.SetTitle("data_obs")
  stack.SetName("data_obs")
  for p in procs[1:]:
    hh=f.Get(c+"/"+p)
    stack.Add(hh)
  results.append([c,stack.Integral()])
  stack.Write()
  
f.Close()

for r in results:
  print r[0], r[1]