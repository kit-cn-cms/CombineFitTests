
#user input begins here
categories = ["ljets_j4_t4", "ljets_j5_tge4", "ljets_jge6_t3", "ljets_jge6_tge4"] #list of categories used for the fit
histoKey = "$PROCESS_finaldiscr_$CHANNEL" #key for templates in datacard
sProcesses = ["ttH_hbb"] #list of signal processes
bProcesses = ["ttbarOther", "ttbarPlusB", "ttbarPlus2B", "ttbarPlusBBbar", "ttbarPlusCCbar"] #list of background processes
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
