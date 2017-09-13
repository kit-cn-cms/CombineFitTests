import sys
import ROOT
ROOT.gDirectory.cd('PyROOT:/')
ROOT.gROOT.SetBatch(True)

infn=sys.argv[1]

scats=["ljets_j4_t2",  "ljets_j4_t3", "ljets_j4_t4","ljets_j5_t2","ljets_j5_t3", "ljets_j5_tge4","ljets_jge6_t2","ljets_jge6_t3","ljets_jge6_tge4"]
cats=[]
#for c in scats[:4]:
#  cats.append(c)
#  cats.append(c)
cats = scats
procs=['ttbarOther', 'ttbarPlusB', 'ttbarPlus2B', 'ttbarPlusBBbar', 'ttbarPlusCCbar']

disc="finaldiscr"

results=[]

f=ROOT.TFile(infn,"UPDATE")

for c in cats:
  OldData=f.Get("data_obs"+"_"+disc+"_"+c)
  print OldData
  OldData.SetTitle("OLDDATA"+"_"+disc+"_"+c)
  OldData.SetName("OLDDATA"+"_"+disc+"_"+c)
  OldData.Write()

  stack=f.Get(procs[0]+"_"+disc+"_"+c).Clone()
  stack.SetTitle("data_obs"+"_"+disc+"_"+c)
  stack.SetName("data_obs"+"_"+disc+"_"+c)
  for p in procs[1:]:
    hh=f.Get(p+"_"+disc+"_"+c)
    stack.Add(hh)
  results.append([c,stack.Integral()])
  stack.Write()

f.Close()

for r in results:
  print r[0], r[1]
