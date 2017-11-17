import subprocess
import os
import sys
import time
import glob

scriptDir = "/nfs/dust/cms/user/pkeicher/tth_analysis_study/CombineFitTests/PseudoDataTests/scripts"

processDic = {
#"ttbarOther": ["0.99", "0.9", "1.01", "1.1"],
# "ttbarPlusBBbar": ["0.5", "0.8", "1.2", "1.5"],
# "ttbarPlusBBbar": ["1.3", "1.8"],

"ttbarPlusBBbar": ["1.3"],
#"ttbarPlusB": ["0.5", "0.8", "1.2", "1.5"],
#"ttbarPlus2B": ["0.5", "0.8", "1.2", "1.5"],
#"ttbarPlusCCbar": ["0.5", "0.8", "1.2", "1.5"],
#"ttbarPlusBBbar,ttbarPlus2B": ["0.5", "0.8", "1.2", "1.5"],
#"ttbarPlusBBbar,ttbarPlusB": ["0.5", "0.8", "1.2", "1.5"],
#"ttbarPlus2B,ttbarPlusB": ["0.5", "0.8", "1.2", "1.5"],
# "ttbarPlusBBbar,ttbarPlus2B,ttbarPlusB": ["1.3", "1.8"]#["0.5", "0.8", "1.2", "1.5"]
}

listOfPoisCombis = [
        #{"r_ttbbPlus2B" : "(ttbarPlusBBbar|ttbarPlus2B):r_ttbbPlus2B[1,-10,10]"},
        #{"r_ttbbPlus2B" : "(ttbarPlusBBbar|ttbarPlus2B):r_ttbbPlus2B[1,-10,10]", "r_ttcc" : "(ttbarPlusCCbar):r_ttcc[1,-10,10]"},
        #{"r_ttbbPlusB" : "(ttbarPlusBBbar|ttbarPlusB):r_ttbbPlusB[1,-10,10]"},
        #{"r_ttbbPlusB" : "(ttbarPlusBBbar|ttbarPlusB):r_ttbbPlusB[1,-10,10]", "r_ttcc" : "(ttbarPlusCCbar):r_ttcc[1,-10,10]"},
        {"r_ttbb" : "(ttbarPlusBBbar):r_ttbb[1,-10,10]"},
        # {"r_ttbb" : "(ttbarPlusBBbar):r_ttbb[1,-10,10]", "r_ttcc" : "(ttbarPlusCCbar):r_ttcc[1,-10,10]"},
        # {"r_ttXB" : "(ttbarPlusBBbar|ttbarPlusB|ttbarPlus2B):r_ttXB[1,-10,10]"},
        # {"r_ttXB" : "(ttbarPlusBBbar|ttbarPlusB|ttbarPlus2B):r_ttXB[1,-10,10]", "r_ttcc" : "(ttbarPlusCCbar):r_ttcc[1,-10,10]"},
        #{"r_ttBPlus2B" : "(ttbarPlusB|ttbarPlus2B):r_ttBPlus2B[1,-10,10]"},
        #{"r_ttBPlus2B" : "(ttbarPlusB|ttbarPlus2B):r_ttBPlus2B[1,-10,10]", "r_ttcc" : "(ttbarPlusCCbar):r_ttcc[1,-10,10]"},
        ]
# listOfPoisCombis = None

def runScript(targetPath, suffix, pathToDatacard, pathToRoofile = None, pois = None, key = None, factor = None, pathToConfig = None, listOfMus = None):
    # construct command string
    commandString = 'python ' + scriptDir+'/tth_analysis_study.py '
    commandString += '-o "'+ targetPath + '/' +suffix + '" '
    commandString += '-d "' + pathToDatacard + '" '
    if pathToRoofile:
        commandString += '-r "' + pathToRoofile +'" '
    if pois is not None:
        for poi in pois:
            commandString = commandString + '-p "' + pois[poi] + '" '

    if key is not None:
        commandString = commandString + key + ' '
    if factor is not None:
        commandString = commandString + factor+ ' '
    if listOfMus is not None:
        for mu in listOfMus:
            commandString += "-s " + mu + " "
    if pathToConfig is not None:
        commandString = commandString + "-c " + pathToConfig + " "
    # execute the command, queue the result
    print commandString
    subprocess.call([commandString], shell=True)

def do_scaling( targetPath, pathToDatacard, pathToRoofile = None,
                pois = None, pathToConfig = None, additionalCmds = None,
                pathToAMC = None, pathToSherpa = None):
    print pois
    base_suffix = os.path.basename(targetPath) + "_" 
    print "base_suffix:", base_suffix
    if pois:
        for poi in sorted(pois):
            print poi
            if not targetPath.endswith('/'):
                targetPath = targetPath + "_"
            targetPath = targetPath + poi
        base_suffix += "_".join(sorted(pois))+"_"

    string = ""
    if additionalCmds:
        string = additionalCmds
    runScript(targetPath = targetPath, suffix = base_suffix+"noScaling", pathToDatacard = pathToDatacard, pois = pois, key = string)
    if pathToSherpa:
        runScript(targetPath, base_suffix+"sherpa_ol_wo_ttbarPlusB_ttbarPlus2B", pathToDatacard, pathToRoofile, pois, key= "--scaledDatacard " + pathToSherpa + " " + string)
    # if pathToAMC:
        # runScript(targetPath, base_suffix+"amc", pathToDatacard, pathToRoofile, pois, key= "--scaledDatacard " + pathToAMC)
    for key in processDic:
        for factor in processDic[key]:
            process = key
            temp_factor = factor

            if "," in key:
                process = key.split(",")
                for i in range(len(key.split(","))-1):
                    temp_factor = temp_factor + "_" + factor
                process = "_".join(process)

            suffix_noscale = base_suffix

            suffix = base_suffix+process+"_"+temp_factor

            if "_" in temp_factor:
                factor = temp_factor.replace("_",",")

            string = "--scaleProcesses " + key
            if additionalCmds:
                string += " " + additionalCmds


            runScript(  targetPath  = targetPath,
                        suffix      = suffix,
                        pathToDatacard = pathToDatacard,
                        pathToRoofile = pathToRoofile,
                        pois = pois,
                        key = string,
                        factor = "--scaleFuncs " + factor,
                        pathToConfig = pathToConfig)


def tth_fit_stability(pois, additionalCmds = None):
    targetPath = "/nfs/dust/cms/user/pkeicher/tth_analysis_study/test/new_powheg/freezeNP/PseudoData/noFrequentist/"
    # pathToDatacard = "/nfs/dust/cms/user/pkeicher/ttbb/pyroot-plotscripts/limits_BDT_powheg/limits_BDT_powheg_ttHbb_63445464.txt"
    pathToDatacard = "/nfs/dust/cms/user/pkeicher/ttbb/pyroot-plotscripts/limits_BDT_powheg/limits_BDT_powheg_ttHbb_allCats_allNP.txt"
    #pathToDatacard = "/nfs/dust/cms/user/pkeicher/ttbb/pyroot-plotscripts/limits_BDT_powheg/limits_BDT_powheg_ttHbb_63445464.txt"
    pathToRoofile = "/nfs/dust/cms/user/pkeicher/ttbb/pyroot-plotscripts/workdir/limits_BDT_powheg/output_limitInput_majorBkgDataObs.root"
    #pathToConfig = "config_ttHbb_allCats_include_nps.py"

    #pathToDatacard = "/nfs/dust/cms/user/pkeicher/tth_analysis_study/PseudoDataTests/datacards/limits_JTBDT_Spring17v10_63445464_ttHbb.txt"
    #pathToRoofile = "/nfs/dust/cms/user/pkeicher/tth_analysis_study/PseudoDataTests/datacards/limits_JTBDT_Spring17v10/limits_JTBDT_Spring17v10_limitInput.root"

    #pathToDatacard = "/nfs/dust/cms/user/pkeicher/ttbb/pyroot-plotscripts/limits_BDT_powheg/limits_BDT_powheg_allProcs_63445464.txt"
    #pathToRoofile = "/nfs/dust/cms/user/pkeicher/ttbb/pyroot-plotscripts/workdir/limits_BDT_powheg/output_limitInput.root"
    #pathToConfig = "config_allProcs_63445464.py"

    pathToConfig = "config_ttHbb_allCats.py"
    #pathToConfig = "config_allProcs_allCats.py"

    pathToSherpa 	= "/nfs/dust/cms/user/pkeicher/ttbb/pyroot-plotscripts/limits_BDT_sherpa_ol_normedHisto/limits_BDT_sherpa_ol_normedHisto_63445464_ttHbb.txt"
    pathToAMC 		= "/nfs/dust/cms/user/pkeicher/ttbb/pyroot-plotscripts/limits_BDT_amc_normedHisto/limits_BDT_amc_normedHisto_63445464_ttHbb.txt"

    do_scaling( targetPath = targetPath, 
                pathToDatacard = pathToDatacard, 
                pathToRoofile = pathToRoofile, pois = pois, 
                additionalCmds = additionalCmds, 
                pathToAMC = pathToAMC, pathToSherpa = pathToSherpa,
                pathToConfig = pathToConfig
                )


def JES_uncertainty_study(pathToDatacards, folderSuffix, additionalCmds):
    targetPath = "/nfs/dust/cms/user/pkeicher/JES_CSV_impact_study/tests/CMS_nominal_CSV/"
    #pathToDatacards = "/nfs/dust/cms/user/pkeicher/JES_CSV_impact_study/input/nuisanceImpact/datacard_63445463_CMS_scale_*j_*.txt"
    pathToRoofile = "/nfs/dust/cms/user/pkeicher/JES_CSV_impact_study/input/nuisanceImpact/nuisanceImpact/nuisanceImpact_limitInput.root"

    for datacard in glob.glob(pathToDatacards):
        # if "reduced" in datacard:
        #     continue
        suffix = datacard.split("/")[-1]
        suffix = suffix.replace(".txt","")
        suffix = suffix.replace("datacard_","")

        runScript(targetPath = targetPath, suffix = suffix + folderSuffix , pathToDatacard = datacard, key = additionalCmds)

def throwToys(wildcard, rootfile = None, pathToConfig = None, additionalCmds = None, pois = None):
    sherpa = "/nfs/dust/cms/user/pkeicher/tth_analysis_study/Spring17_v22/finalComb_v22_fresh/SL_BDTonly_645444635343_sherpa.txt"
    for datacard in glob.glob(wildcard):
        if os.path.exists(os.path.abspath(datacard)):
            datacard = os.path.abspath(datacard)
            targetPath = os.path.dirname(datacard)
            rootfile = os.path.abspath(rootfile)
            parts = os.path.basename(datacard).split(".")
            
            outputDirectory = targetPath
            if additionalCmds and "asimov" in additionalCmds:
                outputDirectory += "/asimov/"
            else:
                outputDirectory += "/toys/" 
            outputDirectory += ".".join(parts[:len(parts)-1])
            outputDirectory = os.path.abspath(outputDirectory)
            
            string = ""
            if additionalCmds:
                string = additionalCmds
            if pois:
                print pois
                for poi in pois:
                    print poi
                    do_scaling( targetPath = outputDirectory, 
                        pathToDatacard = datacard, 
                        pathToRoofile = rootfile,
                        pathToConfig = pathToConfig, 
                        additionalCmds = additionalCmds,
                        pois = poi,
                        pathToSherpa = sherpa)
            else:
                do_scaling( targetPath = outputDirectory, 
                        pathToDatacard = datacard, 
                        pathToRoofile = rootfile,
                        pathToConfig = pathToConfig, 
                        additionalCmds = additionalCmds,
                        pathToSherpa = sherpa)
        else:
            print "Could not find datacard", datacard

throwToys(  wildcard = sys.argv[1], 
            rootfile = sys.argv[2], 
            pathToConfig = sys.argv[3],
            additionalCmds = sys.argv[4],
            pois = listOfPoisCombis)


# for pois in listOfPoisCombis:
    # tth_fit_stability(pois, sys.argv[1])

#JES_uncertainty_study(pathToDatacards = sys.argv[1], folderSuffix = sys.argv[2], additionalCmds = sys.argv[3])
