#user input begins here
#list of categories used for the fit
# categories = "ljets_j5_t3  ljets_jge6_tge4_high ljets_jge6_t3_high ljets_j4_t3 ljets_j4_t4_low ljets_jge6_tge4_low ljets_j5_tge4_high ljets_j4_t4_high ljets_jge6_t3_low ljets_j5_tge4_low".split()
categories = [
                "e_eq4j_ge3b_cc_v6b_ttH125_bb",
                "e_eq5j_ge3b_cc_v6b_ttJets_bb",
                "e_eq5j_ge3b_cc_v6b_ttJets_cc",
                "e_eq5j_ge3b_cc_v6b_ttJets_lf",
                "e_ge6j_ge2b_cc_v6b_ttH125_bb",
                "e_ge6j_ge2b_cc_v6b_ttJets_2b",
                "e_ge6j_ge2b_cc_v6b_ttJets_b",
                "e_ge6j_ge2b_cc_v6b_ttJets_bb",
                "e_ge6j_ge2b_cc_v6b_ttJets_cc",
                "e_ge6j_ge2b_cc_v6b_ttJets_lf",
                "mu_eq4j_ge3b_cc_v6b_ttH125_bb",
                "e_eq4j_ge3b_cc_v6b_ttJets_2b",
                "mu_eq4j_ge3b_cc_v6b_ttJets_2b",
                "mu_eq4j_ge3b_cc_v6b_ttJets_b",
                "mu_eq4j_ge3b_cc_v6b_ttJets_bb",
                "mu_eq4j_ge3b_cc_v6b_ttJets_cc",
                "mu_eq4j_ge3b_cc_v6b_ttJets_lf",
                "mu_eq5j_ge3b_cc_v6b_ttH125_bb",
                "mu_eq5j_ge3b_cc_v6b_ttJets_2b",
                "mu_eq5j_ge3b_cc_v6b_ttJets_b",
                "mu_eq5j_ge3b_cc_v6b_ttJets_bb",
                "mu_eq5j_ge3b_cc_v6b_ttJets_cc",
                "e_eq4j_ge3b_cc_v6b_ttJets_b",
                "mu_eq5j_ge3b_cc_v6b_ttJets_lf",
                "mu_ge6j_ge2b_cc_v6b_ttH125_bb",
                "mu_ge6j_ge2b_cc_v6b_ttJets_2b",
                "mu_ge6j_ge2b_cc_v6b_ttJets_b",
                "mu_ge6j_ge2b_cc_v6b_ttJets_bb",
                "mu_ge6j_ge2b_cc_v6b_ttJets_cc",
                "mu_ge6j_ge2b_cc_v6b_ttJets_lf",
                "dl_ge4jge4t_low_BDT",
                "dl_ge4jge4t_high_BDT",
                "dl_ge4j3t_low_BDT",
                "e_eq4j_ge3b_cc_v6b_ttJets_bb",
                "dl_ge4j3t_high_BDT",
                "dl_3j3t_BDT",
                "e_eq4j_ge3b_cc_v6b_ttJets_cc",
                "e_eq4j_ge3b_cc_v6b_ttJets_lf",
                "e_eq5j_ge3b_cc_v6b_ttH125_bb",
                "e_eq5j_ge3b_cc_v6b_ttJets_2b",
                "e_eq5j_ge3b_cc_v6b_ttJets_b"
                ]
histoKey = "$CHANNEL/$PROCESS" #key for templates in datacard
#sProcesses = ["ttH_hbb"] #list of signal processes
sProcesses = "ttH_hcc         ttH_hbb         ttH_htt         ttH_hgg         ttH_hgluglu     ttH_hww         ttH_hzz         ttH_hzg".split()
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
