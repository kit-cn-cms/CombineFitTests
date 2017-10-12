
#user input begins here
#list of categories used for the fit
categories = [  #"ljets_j4_t2",
                #"ljets_j4_t3",
                "ljets_j4_t4",
                #"ljets_j5_t2",
                #"ljets_j5_t3",
                "ljets_j5_tge4",
                #"ljets_jge6_t2",
                "ljets_jge6_t3",
                "ljets_jge6_tge4"
                ]
histoKey = "$PROCESS_finaldiscr_$CHANNEL" #key for templates in datacard
#list of signal processes
sProcesses = [  "ttH_hbb",
                "ttH_hcc",
                "ttH_hww",
                "ttH_hzz", "ttH_htt",
                "ttH_hgg",
                "ttH_hgluglu",
                "ttH_hzg"
                ]

#list of background processes
bProcesses = [  "ttbarOther",
                "ttbarPlusB",
                "ttbarPlus2B",
                "ttbarPlusBBbar",
                "ttbarPlusCCbar",
                "singlet",
                "wjets",
                "zjets",
                "ttbarW",
                "ttbarZ",
                "diboson"
                ]
ignoreUncertainties = ["bgnorm_*", "QCDscale*"] #list of nuisance parameters to be ignored during toy generation

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
