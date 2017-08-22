categories = ["ljets_j4_t4", "ljets_j5_tge4", "ljets_jge6_t3", "ljets_jge6_tge4"]
histoKey = "$PROCESS_finaldiscr_$CHANNEL"
sProcesses = ["ttH_hbb"]
bProcesses = ["ttbarOther", "ttbarPlusB", "ttbarPlus2B", "ttbarPlusBBbar", "ttbarPlusCCbar"]


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
