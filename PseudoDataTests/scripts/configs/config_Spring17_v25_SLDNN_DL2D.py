#user input begins here
#list of categories used for the fit
# categories = "ljets_j5_t3  ljets_jge6_tge4_high ljets_jge6_t3_high ljets_j4_t3 ljets_j4_t4_low ljets_jge6_tge4_low ljets_j5_tge4_high ljets_j4_t4_high ljets_jge6_t3_low ljets_j5_tge4_low".split()
categories = [
                "DL_eq3j_eq3b_cc_v3a_ttH125_bb",
                "DL_eq3j_eq3b_cc_v3a_ttJets_2b",
                "DL_eq3j_eq3b_cc_v3a_ttJets_b",
                "DL_eq3j_eq3b_cc_v3a_ttJets_bb",
                "DL_eq3j_eq3b_cc_v3a_ttJets_cc",
                "DL_eq3j_eq3b_cc_v3a_ttJets_lf",
                "DL_ge4j_ge3b_cc_v3a_ttH125_bb",
                "DL_ge4j_ge3b_cc_v3a_ttJets_2b",
                "DL_ge4j_ge3b_cc_v3a_ttJets_b",
                "DL_ge4j_ge3b_cc_v3a_ttJets_bb",
                "DL_ge4j_ge3b_cc_v3a_ttJets_cc",
                "DL_ge4j_ge3b_cc_v3a_ttJets_lf",
                "SL_eq4j_ge3b_cc_v6b_ttH125_bb",
                "SL_eq4j_ge3b_cc_v6b_ttJets_2b",
                "SL_eq4j_ge3b_cc_v6b_ttJets_b",
                "SL_eq4j_ge3b_cc_v6b_ttJets_bb",
                "SL_eq4j_ge3b_cc_v6b_ttJets_cc",
                "SL_eq4j_ge3b_cc_v6b_ttJets_lf",
                "SL_eq5j_ge3b_cc_v6b_ttH125_bb",
                "SL_eq5j_ge3b_cc_v6b_ttJets_2b",
                "SL_eq5j_ge3b_cc_v6b_ttJets_b",
                "SL_eq5j_ge3b_cc_v6b_ttJets_bb",
                "SL_eq5j_ge3b_cc_v6b_ttJets_cc",
                "SL_eq5j_ge3b_cc_v6b_ttJets_lf",
                "SL_ge6j_ge3b_cc_v6b_ttH125_bb",
                "SL_ge6j_ge3b_cc_v6b_ttJets_2b",
                "SL_ge6j_ge3b_cc_v6b_ttJets_b",
                "SL_ge6j_ge3b_cc_v6b_ttJets_bb",
                "SL_ge6j_ge3b_cc_v6b_ttJets_cc",
                "SL_ge6j_ge3b_cc_v6b_ttJets_lf",
                "dl_ge4jge4t_high_BDT",
                "dl_ge4jge4t_low_BDT",
                "dl_ge4j3t_high_BDT",
                "dl_ge4j3t_low_BDT",
                "dl_3j2t_BDT",
                "dl_3j3t_BDT",
                "dl_ge4j2t_BDT",
                "dl_ge4j3t_BDT",
                "dl_ge4jge4t_BDT"
]
histoKey = "$CHANNEL/$PROCESS" #key for templates in datacard
#sProcesses = ["ttH_hbb"] #list of signal processes
sProcesses = "ttH_hcc         ttH_hbb         ttH_htt         ttH_hww         ttH_hgluglu     ttH_hgg         ttH_hzz         ttH_hzg".split()
#bProcesses = ["ttbarOther", "ttbarPlusB", "ttbarPlus2B", "ttbarPlusBBbar", "ttbarPlusCCbar"] #list of background processes
bProcesses = "ttbarZ          diboson         ttbarPlusB      ttbarW          singlet         wjets           ttbarPlus2B     ttbarOther      ttbarPlusBBbar  zjets           ttbarPlusCCbar".split()
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
