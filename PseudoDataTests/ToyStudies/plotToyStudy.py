import ROOT
import sys
from subprocess import call
from array import array
ROOT.gROOT.SetBatch(True)
ROOT.gDirectory.cd('PyROOT:/')

params2=['CMS_res_j', 'CMS_scale_j', 'CMS_ttH_CSVCErr1', 'CMS_ttH_CSVCErr2', 'CMS_ttH_CSVHF', 'CMS_ttH_CSVHFStats1', 'CMS_ttH_CSVHFStats2', 'CMS_ttH_CSVLF', 'CMS_ttH_CSVLFStats1', 'CMS_ttH_CSVLFStats2', 'CMS_ttH_PSscale_ttbarOther', 'CMS_ttH_PSscale_ttbarPlus2B', 'CMS_ttH_PSscale_ttbarPlusB', 'CMS_ttH_PSscale_ttbarPlusBBbar', 'CMS_ttH_PSscale_ttbarPlusCCbar', 'CMS_ttH_PU', 'CMS_ttH_Q2scale_ttbarOther', 'CMS_ttH_Q2scale_ttbarPlus2B', 'CMS_ttH_Q2scale_ttbarPlusB', 'CMS_ttH_Q2scale_ttbarPlusBBbar', 'CMS_ttH_Q2scale_ttbarPlusCCbar', 'CMS_ttH_QCDscale_ttbarPlus2B', 'CMS_ttH_QCDscale_ttbarPlusB', 'CMS_ttH_QCDscale_ttbarPlusBBbar', 'CMS_ttH_QCDscale_ttbarPlusCCbar', 'CMS_ttH_dl_Trig', 'CMS_ttH_eff_lepton', 'CMS_ttH_eff_leptonLJ', 'CMS_ttH_ljets_Trig', 'QCDscale_V', 'QCDscale_VV', 'QCDscale_singlet', 'QCDscale_ttH', 'QCDscale_ttbar', 'lumi_13TeV', 'pdf_gg', 'pdf_gg_ttH', 'pdf_qg', 'pdf_qqbar']

toyFile=sys.argv[1]
dataFile=sys.argv[2]

infData=ROOT.TFile(dataFile,"READ")
infToys=ROOT.TFile(toyFile,"READ")

treeSBdata=infData.Get("tree_fit_sb")
treeBdata=infData.Get("tree_fit_b")
treeSBtoys=infToys.Get("tree_fit_sb")
treeBtoys=infToys.Get("tree_fit_b")
allTrees=[treeBdata,treeSBdata,treeBtoys,treeSBtoys]

histoSBtoysIn=[]
histoSBtoysOut=[]
histoBtoysIn=[]
histoBtoysOut=[]
histoSBtoysInVsOut=[]
histoBtoysInVsOut=[]

arrayValIn=[]
arrayValOut=[]

allparams=params2
for ip, p in enumerate(allparams):
  print "doing ", p
  minVal=0.0
  maxVal=0.0
  
  for tree in allTrees:
    if tree.GetMinimum(p)<minVal:
      minVal=tree.GetMinimum(p)
    if tree.GetMaximum(p)>maxVal:
      maxVal=tree.GetMaximum(p)
    if tree.GetMinimum(p+"_In")<minVal:
      minVal=tree.GetMinimum(p+"_In")
    if tree.GetMaximum(p+"_In")>maxVal:
      maxVal=tree.GetMaximum(p+"_In")
    print minVal, maxVal, tree.GetMaximum(p),tree.GetMinimum(p),tree.GetMaximum(p+"_In"),tree.GetMinimum(p+"_In")
  
  if minVal<-5:
    minVal=-5.0
  if maxVal>5:
    maxVal=5.0  
  
  histoSBtoysIn.append(ROOT.TH1D("toy_sb_In_"+p,"toy_sb_In_"+p,100,minVal,maxVal))
  histoSBtoysIn[-1].SetLineColor(ROOT.kBlue)
  histoBtoysIn.append(ROOT.TH1D("toy_b_In_"+p,"toy_b_In_"+p,100,minVal,maxVal))
  histoBtoysIn[-1].SetLineColor(ROOT.kRed)
  histoSBtoysOut.append(ROOT.TH1D("toy_sb_Out_"+p,"toy_sb_Out_"+p,100,minVal,maxVal))
  histoSBtoysOut[-1].SetLineColor(ROOT.kCyan)
  histoBtoysOut.append(ROOT.TH1D("toy_b_Out_"+p,"toy_b_Out_"+p,100,minVal,maxVal))
  histoBtoysOut[-1].SetLineColor(ROOT.kRed-5)
  
  histoSBtoysInVsOut.append(ROOT.TH2D("toy_sb_InVsOut_"+p,"toy_sb_InVsOut_"+p,100,minVal,maxVal,100,minVal,maxVal))
  histoSBtoysInVsOut[-1].GetXaxis().SetTitle("toy_sb_In_"+p)
  histoSBtoysInVsOut[-1].GetYaxis().SetTitle("toy_sb_Out_"+p)
  
  histoBtoysInVsOut.append(ROOT.TH2D("toy_b_InVsOut_"+p,"toy_b_InVsOut_"+p,100,minVal,maxVal,100,minVal,maxVal))
  histoBtoysInVsOut[-1].GetXaxis().SetTitle("toy_b_In_"+p)
  histoBtoysInVsOut[-1].GetYaxis().SetTitle("toy_b_Out_"+p)


  dataSBValIn=treeSBdata.GetMinimum(p+"_In")
  dataBValIn=treeBdata.GetMinimum(p+"_In")
  dataSBValOut=treeSBdata.GetMinimum(p)
  dataBValOut=treeBdata.GetMinimum(p)

  #get Fit sb from toys
  nEvts=treeSBtoys.GetEntries()
  arrayValIn.append(array("d",[0.0]))
  arrayValOut.append(array("d",[0.0]))
  treeSBtoys.SetBranchAddress(p,arrayValOut[-1])
  treeSBtoys.SetBranchAddress(p+"_In",arrayValIn[-1])
  for ievt in range(nEvts):
    treeSBtoys.GetEntry(ievt)
    #print arrayValIn[-1][0],arrayValOut[-1][0]
    histoSBtoysIn[-1].Fill(arrayValIn[-1][0])
    histoSBtoysOut[-1].Fill(arrayValOut[-1][0])
    histoSBtoysInVsOut[-1].Fill(arrayValIn[-1][0],arrayValOut[-1][0])
  
  #get Fit b from toys
  nEvts=treeBtoys.GetEntries()
  arrayValIn.append(array("d",[0.0]))
  arrayValOut.append(array("d",[0.0]))
  treeBtoys.SetBranchAddress(p,arrayValOut[-1])
  treeBtoys.SetBranchAddress(p+"_In",arrayValIn[-1])
  for ievt in range(nEvts):
    treeBtoys.GetEntry(ievt)
    histoBtoysIn[-1].Fill(arrayValIn[-1][0])
    histoBtoysOut[-1].Fill(arrayValOut[-1][0])
    histoBtoysInVsOut[-1].Fill(arrayValIn[-1][0],arrayValOut[-1][0])
  
  cDistributions=ROOT.TCanvas("c_"+p,"c_"+p,800,600)
  cDistributions.cd()
  
  maxYval=0
  thisallhistos=[histoSBtoysIn[-1],histoBtoysIn[-1],histoSBtoysOut[-1],histoBtoysOut[-1]]
  for h in thisallhistos:
    if maxYval<h.GetMaximum():
      maxYval=h.GetMaximum()
  
  print "ok"
  for h in thisallhistos:
    print h

  histoSBtoysIn[-1].GetYaxis().SetRangeUser(0,maxYval*1.5)
  histoSBtoysIn[-1].Draw()
  histoBtoysIn[-1].Draw("SAME")
  histoSBtoysOut[-1].Draw("SAME")
  histoBtoysOut[-1].Draw("SAME")
  
  cDistributions.BuildLegend()
  cDistributions.Update()
  
  ldataSBValIn=ROOT.TLine(dataSBValIn,0,dataSBValIn,maxYval*1)
  ldataSBValIn.SetLineColor(ROOT.kBlue)
  ldataBValIn=ROOT.TLine(dataBValIn,0,dataBValIn,maxYval*0.9)
  ldataBValIn.SetLineColor(ROOT.kRed)
  ldataSBValOut=ROOT.TLine(dataSBValOut,0,dataSBValOut,maxYval*0.7)
  ldataSBValOut.SetLineColor(ROOT.kCyan)
  ldataBValOut=ROOT.TLine(dataBValOut,0,dataBValOut,maxYval*0.6)
  ldataBValOut.SetLineColor(ROOT.kRed-5)

  ldataSBValIn.Draw()
  ldataBValIn.Draw()
  ldataSBValOut.Draw()
  ldataBValOut.Draw()
  
  if ip==0:
    cDistributions.SaveAs("toyPlots/toyPlots.pdf[")
  cDistributions.SaveAs("toyPlots/toyPlots.pdf")

  cTwoDimSB=ROOT.TCanvas("cTwoDimSB_"+p,"cTwoDimSB_"+p,800,600)
  cTwoDimSB.cd()
  histoSBtoysInVsOut[-1].Draw("COLZ")
  cTwoDimSB.SaveAs("toyPlots/toyPlots.pdf")
  
  cTwoDimB=ROOT.TCanvas("cTwoDimB_"+p,"cTwoDimB_"+p,800,600)
  cTwoDimB.cd()
  histoBtoysInVsOut[-1].Draw("COLZ")
  cTwoDimB.SaveAs("toyPlots/toyPlots.pdf")
  
  if ip==len(allparams)-1:
    cDistributions.SaveAs("toyPlots/toyPlots.pdf]")
  
  
  
  
infData.Close()
infToys.Close()
  
  
  
  
  
  