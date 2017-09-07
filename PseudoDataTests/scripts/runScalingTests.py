import subprocess
import os
import sys
import time
import glob

scriptDir = "/nfs/dust/cms/user/pkeicher/tth_analysis_study/CombineFitTests/PseudoDataTests/scripts"

def runScript(targetPath, suffix, pathToDatacard, pathToRoofile, pois = None, key = None, factor = None, pathToConfig = None, listOfMus = None):
    # construct command string
    commandString = 'python ' + scriptDir+'/tth_analysis_study.py -o "'+ targetPath + '/' +suffix + '" -d "' + pathToDatacard + '" -r "' + pathToRoofile +'" '
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

def tth_fit_stability(pois):
    targetPath = "/nfs/dust/cms/user/pkeicher/tth_analysis_study/test/JTBDT_Spring17v10/wo_NP/PseudoData_newPowheg/"
    pathToDatacard = "/nfs/dust/cms/user/pkeicher/tth_analysis_study/PseudoDataTests/datacards/limits_BDT_powheg/limits_BDT_powheg_datacard_63445464_ttHbb.txt"
    pathToRoofile = "/nfs/dust/cms/user/pkeicher/tth_analysis_study/PseudoDataTests/datacards/limits_BDT_powheg/limits_BDT_powheg/limits_BDT_powheg_limitInput.root"

    processDic = {
    #"ttbarOther": ["0.99", "0.9", "1.01", "1.1"],
    "ttbarPlusBBbar": ["0.5", "0.8", "1.2", "1.5"],
    #"ttbarPlusB": ["0.5", "0.8", "1.2", "1.5"],
    #"ttbarPlus2B": ["0.5", "0.8", "1.2", "1.5"],
    #"ttbarPlusCCbar": ["0.5", "0.8", "1.2", "1.5"],
    #"ttbarPlusBBbar,ttbarPlus2B": ["0.5", "0.8", "1.2", "1.5"],
    #"ttbarPlusBBbar,ttbarPlusB": ["0.5", "0.8", "1.2", "1.5"],
    #"ttbarPlus2B,ttbarPlusB": ["0.5", "0.8", "1.2", "1.5"],
    "ttbarPlusBBbar,ttbarPlus2B,ttbarPlusB": ["0.5", "0.8", "1.2", "1.5"]
    }

    # pois = dict()
    # for arg in sys.argv[1:]:
    #     arglist = arg.split(";")
    #     pois[arglist[0]] = arglist[1]

    print pois
    for poi in sorted(pois):
        if not targetPath.endswith('/'):
            targetPath = targetPath + "_"
        targetPath = targetPath + poi

    base_suffix = "63445464_ttHbb_N1000_" + "_".join(sorted(pois))+"_"
    runScript(targetPath, base_suffix+"noScaling", pathToDatacard, pathToRoofile, pois)

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
            #produce unscaled toys

            suffix = base_suffix+process+"_"+temp_factor

            if "_" in temp_factor:
                factor = temp_factor.replace("_",",")


            runScript(targetPath, suffix, pathToDatacard, pathToRoofile, pois, "--scaleProcesses " + key, "--scaleFuncs " + factor)


def JES_uncertainty_study():
    targetPath = "/nfs/dust/cms/user/pkeicher/JES_CSV_impact_study/tests/CMS_nominal_CSV/"
    pathToDatacards = "/nfs/dust/cms/user/pkeicher/JES_CSV_impact_study/input/nuisanceImpact/datacard_63445463_CMS_scale_*j_*.txt"
    pathToRoofile = "/nfs/dust/cms/user/pkeicher/JES_CSV_impact_study/input/nuisanceImpact/nuisanceImpact/nuisanceImpact_limitInput.root"

    threads = list()
    que = Queue.Queue()
    for datacard in glob.glob(pathToDatacards):

        suffix = datacard.split("/")[-1]
        suffix = suffix.replace(".txt","")
        suffix = suffix.replace("datacard_","")

        t = Thread(target=lambda q, arg1, arg2, arg3, arg4: q.put(runScript(arg1, arg2, arg3, arg4)), args=(que, targetPath, suffix, datacard, pathToRoofile), name=suffix)
        threads.append(t)
        t.start()
    return threads, que

def throwToys(wildcard, inputRootFile, pathToConfig):
    #wildcard = sys.argv[1]
    #inputRootFile = sys.argv[2]
    listOfSignalStrenghts = [0.,1.,2.]
    if os.path.exists(os.path.abspath(inputRootFile)):
        inputRootFile = os.path.abspath(inputRootFile)
        pathToConfig = os.path.abspath(pathToConfig)
        for datacard in glob.glob(wildcard):
            if os.path.exists(os.path.abspath(datacard)):
                datacard = os.path.abspath(datacard)
                outputDirectory = os.path.dirname(datacard) + "/noScaling/" + os.path.basename(datacard).replace(".txt", "")
                if not os.path.exists(outputDirectory):
                    os.makedirs(outputDirectory)
                os.chdir(outputDirectory)

                runScript(os.path.dirname(datacard) + "/noScaling", os.path.basename(datacard).replace(".txt", ""), datacard, inputRootFile, pathToConfig = pathToConfig, listOfMus = listOfSignalStrenghts)

            else:
                print "Could not find datacard", datacard
    else:
        sys.exit("Could not find root file in %s" % inputRootFile)

throwToys(sys.argv[1], sys.argv[2], sys.argv[3])
listOfPoisCombis = [
        #{"r_ttbbPlus2B" : "(ttbarPlusBBbar|ttbarPlus2B):r_ttbbPlus2B[1,-10,10]"},
        #{"r_ttbbPlus2B" : "(ttbarPlusBBbar|ttbarPlus2B):r_ttbbPlus2B[1,-10,10]", "r_ttcc" : "(ttbarPlusCCbar):r_ttcc[1,-10,10]"},
        #{"r_ttbbPlusB" : "(ttbarPlusBBbar|ttbarPlusB):r_ttbbPlusB[1,-10,10]"},
        #{"r_ttbbPlusB" : "(ttbarPlusBBbar|ttbarPlusB):r_ttbbPlusB[1,-10,10]", "r_ttcc" : "(ttbarPlusCCbar):r_ttcc[1,-10,10]"},
        #{"r_ttbb" : "(ttbarPlusBBbar):r_ttbb[1,-10,10]"},
        #{"r_ttbb" : "(ttbarPlusBBbar):r_ttbb[1,-10,10]", "r_ttcc" : "(ttbarPlusCCbar):r_ttcc[1,-10,10]"},
        {"r_ttXB" : "(ttbarPlusBBbar|ttbarPlusB|ttbarPlus2B):r_ttXB[1,-10,10]"},
        {"r_ttXB" : "(ttbarPlusBBbar|ttbarPlusB|ttbarPlus2B):r_ttXB[1,-10,10]", "r_ttcc" : "(ttbarPlusCCbar):r_ttcc[1,-10,10]"},
        #{"r_ttBPlus2B" : "(ttbarPlusB|ttbarPlus2B):r_ttBPlus2B[1,-10,10]"},
        #{"r_ttBPlus2B" : "(ttbarPlusB|ttbarPlus2B):r_ttBPlus2B[1,-10,10]", "r_ttcc" : "(ttbarPlusCCbar):r_ttcc[1,-10,10]"},
        ]

# for pois in listOfPoisCombis:
#    tth_fit_stability(pois)
