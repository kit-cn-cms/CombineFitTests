import subprocess
import time

processDic = {
#"ttbarOther": ["0.99", "1.01"],
#"ttbarPlusBBbar": ["0.8", "1.2"],
#"ttbarPlusB": ["0.8", "1.2"],
#"ttbarPlus2B": ["0.8", "1.2"],
"ttbarPlusCCbar": ["0.8", "1.2"],
#"ttbarPlusBBbar,ttbarPlusB": ["0.8", "1.2"]
}

pois = ["ttbarPlusXBar","ttbarPlusCCbar"]
scriptDir = "/nfs/dust/cms/user/pkeicher/tth_analysis_study/CombineFitTests/PseudoDataTests/scripts"
targetPath = "/nfs/dust/cms/user/pkeicher/tth_analysis_study/CombineFitTests/PseudoDataTests/test/officialHiggsCombine/JTBDT_Spring17v10"
pathToDatacard = "/nfs/dust/cms/user/pkeicher/tth_analysis_study/PseudoDataTests/datacards/limits_JTBDT_Spring17v10_63445464_ttHbb_expanded.txt"
pathToRoofile = "/nfs/dust/cms/user/pkeicher/tth_analysis_study/PseudoDataTests/datacards/limits_JTBDT_Spring17v10_limitInput.root"

for key in processDic:
    for factor in processDic[key]:
        process = key
        if "," in key:
            process = key.split(",")
            process = "_".join(process)
        suffix = "_".join(pois)+"_"+process+"_"+factor
        screenCmd = "screen -dmS " + suffix + " bash -c \'source "+ scriptDir +"/setupCMSSW.txt;" #start screen in detached mode and setup CMSSW
        screenCmd = screenCmd + " python tth_analysis_study.py \""+ targetPath + "/"+suffix +"\" \"" + pathToDatacard + "\" \"" + pathToRoofile +"\""
        screenCmd = screenCmd + " \"" + ",".join(pois)+"\""
        screenCmd = screenCmd + "\"" + process + "\" \"" + factor +"\""
        screenCmd = screenCmd + "\'"
        print "calling screen:\n",screenCmd.split()
        #subprocess.check_call(screenCmd.split())
        #time.sleep(30)
