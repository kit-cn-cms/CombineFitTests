import subprocess
import os
import sys
import time
import glob

scriptDir = "/nfs/dust/cms/user/pkeicher/tth_analysis_study/CombineFitTests/PseudoDataTests/scripts"

def runScript(targetPath, suffix, pathToDatacard, pathToRoofile, pois = None, key = None, factor = None):
    # construct command string
    commandString = 'python ' + scriptDir+'/tth_analysis_study.py "'+ targetPath + '/' +suffix + '" "' + pathToDatacard + '" "' + pathToRoofile +'" "'
    if pois is not None:
        commandString = commandString + ";".join("".join(mapping) for key, mapping in sorted(pois.items()))

    if key is not None:
        commandString = commandString + '" "' + key
    if factor is not None:
        commandString = commandString + '" "' + factor
    commandString = commandString +'"'
    # execute the command, queue the result
    print commandString
    subprocess.call([commandString], shell=True)

def tth_fit_stability(pois):
    targetPath = "/nfs/dust/cms/user/pkeicher/tth_analysis_study/test/JTBDT_Spring17v10/wo_NP/asimov/"
    pathToDatacards = "/nfs/dust/cms/user/pkeicher/tth_analysis_study/PseudoDataTests/datacards/limits_JTBDT_Spring17v10_63445464_ttHbb.txt"
    pathToRoofile = "/nfs/dust/cms/user/pkeicher/tth_analysis_study/PseudoDataTests/datacards/limits_JTBDT_Spring17v10/limits_JTBDT_Spring17v10_limitInput.root"

    processDic = {
    "ttbarOther": ["0.99", "0.9", "1.01", "1.1"],
    "ttbarPlusBBbar": ["0.5", "0.8", "1.2", "1.5"],
    "ttbarPlusB": ["0.5", "0.8", "1.2", "1.5"],
    "ttbarPlus2B": ["0.5", "0.8", "1.2", "1.5"],
    "ttbarPlusCCbar": ["0.5", "0.8", "1.2", "1.5"],
    "ttbarPlusBBbar,ttbarPlus2B": ["0.5", "0.8", "1.2", "1.5"],
    "ttbarPlusBBbar,ttbarPlusB": ["0.5", "0.8", "1.2", "1.5"],
    "ttbarPlus2B,ttbarPlusB": ["0.5", "0.8", "1.2", "1.5"],
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

    datacardTable = None
    for key in processDic:
        for factor in processDic[key]:
            process = key
            temp_factor = factor

            if "," in key:
                process = key.split(",")
                for i in range(len(key.split(","))-1):
                    temp_factor = temp_factor + "_" + factor
                process = "_".join(process)

            base_suffix = "63445464_ttHbb_N1000_"
            base_suffix = base_suffix + "_".join(sorted(pois))+"_"+process+"_"+temp_factor

            if "_" in temp_factor:
                factor = temp_factor.replace("_",",")

            if len(glob.glob(pathToDatacards))>1 and datacardTable == None and not os.path.exists(targetPath+"datacardTable.txt"):
                datacardTable = open(targetPath+"datacardTable.txt","w")
                string = "Number\t\t Path to Datacard\n"
                string.write()

            for i, datacard in enumerate(glob.glob(pathToDatacards)):
                if datacardTable is not None:
                    string = str(i) + "\t\t" + datacard
                    string.write()

                suffix = base_suffix
                if len(glob.glob(pathToDatacards))>1:
                    suffix = suffix + "_" + str(i)

                runScript(targetPath, suffix, datacard, pathToRoofile, pois, key, factor)

            if datacardTable is not None:
                datacardTable.close()
                datacardTable = None


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

def throwToys(wildcard, inputRootFile):
    #wildcard = sys.argv[1]
    #inputRootFile = sys.argv[2]

    if os.path.exists(os.path.abspath(inputRootFile)):
        inputRootFile = os.path.abspath(inputRootFile)
        for datacard in glob.glob(wildcard):
            if os.path.exists(os.path.abspath(datacard)):
                datacard = os.path.abspath(datacard)
                outputDirectory = os.path.dirname(datacard) + "/noScaling/" + os.path.basename(datacard).replace(".txt", "")
                if not os.path.exists(outputDirectory):
                    os.makedirs(outputDirectory)
                os.chdir(outputDirectory)

                runScript(os.path.dirname(datacard) + "/noScaling", os.path.basename(datacard).replace(".txt", ""), datacard, inputRootFile)

            else:
                print "Could not find datacard", datacard
    else:
        sys.exit("Could not find root file in %s" % inputRootFile)

#throwToys(sys.argv[1], sys.argv[2])
listOfPoisCombis = [
        #{"r_ttbbPlus2B" : "(ttbarPlusBBbar|ttbarPlus2B):r_ttbbPlus2B[1,-10,10]"},
        #{"r_ttbbPlus2B" : "(ttbarPlusBBbar|ttbarPlus2B):r_ttbbPlus2B[1,-10,10]", "r_ttcc" : "(ttbarPlusCCbar):r_ttcc[1,-10,10]"},
        #{"r_ttbbPlusB" : "(ttbarPlusBBbar|ttbarPlusB):r_ttbbPlusB[1,-10,10]"},
        #{"r_ttbbPlusB" : "(ttbarPlusBBbar|ttbarPlusB):r_ttbbPlusB[1,-10,10]", "r_ttcc" : "(ttbarPlusCCbar):r_ttcc[1,-10,10]"},
        {"r_ttbb" : "(ttbarPlusBBbar):r_ttbb[1,-10,10]"},
        {"r_ttbb" : "(ttbarPlusBBbar):r_ttbb[1,-10,10]", "r_ttcc" : "(ttbarPlusCCbar):r_ttcc[1,-10,10]"},
        {"r_ttXB" : "(ttbarPlusBBbar|ttbarPlusB|ttbarPlus2B):r_ttXB[1,-10,10]"},
        {"r_ttXB" : "(ttbarPlusBBbar|ttbarPlusB|ttbarPlus2B):r_ttXB[1,-10,10]", "r_ttcc" : "(ttbarPlusCCbar):r_ttcc[1,-10,10]"},
        #{"r_ttBPlus2B" : "(ttbarPlusB|ttbarPlus2B):r_ttBPlus2B[1,-10,10]"},
        #{"r_ttBPlus2B" : "(ttbarPlusB|ttbarPlus2B):r_ttBPlus2B[1,-10,10]", "r_ttcc" : "(ttbarPlusCCbar):r_ttcc[1,-10,10]"},
        ]

for pois in listOfPoisCombis:
   tth_fit_stability(pois)
