#user input begins here
#list of categories used for the fit
# categories = "ljets_j5_t3  ljets_jge6_tge4_high ljets_jge6_t3_high ljets_j4_t3 ljets_j4_t4_low ljets_jge6_tge4_low ljets_j5_tge4_high ljets_j4_t4_high ljets_jge6_t3_low ljets_j5_tge4_low".split()
# categoriesstring = "limits_All_v24_datacard_ljets_j4_tge3_tt2bnode_hdecay.txt  limits_All_v24_datacard_ljets_j4_tge3_ttHnode_hdecay.txt  limits_All_v24_datacard_ljets_j4_tge3_ttbbnode_hdecay.txt  limits_All_v24_datacard_ljets_j4_tge3_ttbnode_hdecay.txt  limits_All_v24_datacard_ljets_j4_tge3_ttccnode_hdecay.txt  limits_All_v24_datacard_ljets_j4_tge3_ttlfnode_hdecay.txt  limits_All_v24_datacard_ljets_j5_tge3_tt2bnode_hdecay.txt  limits_All_v24_datacard_ljets_j5_tge3_ttHnode_hdecay.txt  limits_All_v24_datacard_ljets_j5_tge3_ttbbnode_hdecay.txt  limits_All_v24_datacard_ljets_j5_tge3_ttbnode_hdecay.txt  limits_All_v24_datacard_ljets_j5_tge3_ttccnode_hdecay.txt  limits_All_v24_datacard_ljets_j5_tge3_ttlfnode_hdecay.txt  limits_All_v24_datacard_ljets_jge6_tge3_tt2bnode_hdecay.txt  limits_All_v24_datacard_ljets_jge6_tge3_ttHnode_hdecay.txt  limits_All_v24_datacard_ljets_jge6_tge3_ttbbnode_hdecay.txt  limits_All_v24_datacard_ljets_jge6_tge3_ttbnode_hdecay.txt  limits_All_v24_datacard_ljets_jge6_tge3_ttccnode_hdecay.txt  limits_All_v24_datacard_ljets_jge6_tge3_ttlfnode_hdecay.txt"
categoriesstring = "limits_All_v24_datacard_ljets_j4_tge3_tt2bnode_hdecay.txt  limits_All_v24_datacard_ljets_j4_tge3_ttHnode_hdecay.txt  limits_All_v24_datacard_ljets_j4_tge3_ttbbnode_hdecay.txt  limits_All_v24_datacard_ljets_j4_tge3_ttbnode_hdecay.txt  limits_All_v24_datacard_ljets_j4_tge3_ttccnode_hdecay.txt  limits_All_v24_datacard_ljets_j5_tge3_tt2bnode_hdecay.txt  limits_All_v24_datacard_ljets_j5_tge3_ttHnode_hdecay.txt  limits_All_v24_datacard_ljets_j5_tge3_ttbbnode_hdecay.txt  limits_All_v24_datacard_ljets_j5_tge3_ttbnode_hdecay.txt  limits_All_v24_datacard_ljets_j5_tge3_ttccnode_hdecay.txt  limits_All_v24_datacard_ljets_jge6_tge3_tt2bnode_hdecay.txt  limits_All_v24_datacard_ljets_jge6_tge3_ttHnode_hdecay.txt  limits_All_v24_datacard_ljets_jge6_tge3_ttbbnode_hdecay.txt  limits_All_v24_datacard_ljets_jge6_tge3_ttbnode_hdecay.txt  limits_All_v24_datacard_ljets_jge6_tge3_ttccnode_hdecay.txt  "

categoriesstring = categoriesstring.replace("limits_All_v24_datacard_", "")
categoriesstring = categoriesstring.replace("_hdecay.txt", "")
categories = categoriesstring.split()
histoKey = "$PROCESS_finaldiscr_$CHANNEL" #key for templates in datacard
#sProcesses = ["ttH_hbb"] #list of signal processes
sProcesses = "ttH_hcc         ttH_hbb         ttH_htt         ttH_hgg         ttH_hgluglu     ttH_hww         ttH_hzz         ttH_hzg".split()
#bProcesses = ["ttbarOther", "ttbarPlusB", "ttbarPlus2B", "ttbarPlusBBbar", "ttbarPlusCCbar"] #list of background processes
bProcesses = "ttbarZ          diboson         ttbarPlusB      ttbarW          singlet         wjets           ttbarPlus2B     ttbarOther      ttbarPlusBBbar  zjets           ttbarPlusCCbar  ".split()
ignoreUncertainties = []#["bgnorm_*", "QCDscale*"]#, "pdf_*"] #list of nuisance parameters to be ignored during toy generation

#user input ends here
#_________________________________________________________________________________________________
signalHistos     = {}
backgroundHistos = {}

for category in categories:
    if not category in signalHistos:
        signalHistos[category] = []
        backgroundHistos[category] = []
    catHistoKey = histoKey.replace("$CHANNEL", category)
    for proc in sProcesses:
        signalHistos[category].append(catHistoKey.replace("$PROCESS", proc))
    for proc in bProcesses:
        backgroundHistos[category].append(catHistoKey.replace("$PROCESS", proc))
