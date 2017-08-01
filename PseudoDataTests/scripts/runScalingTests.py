from threading import Thread
import Queue
import commands
import os
import time
import glob

scriptDir = "/nfs/dust/cms/user/pkeicher/tth_analysis_study/CombineFitTests/PseudoDataTests/scripts"

def runScript(targetPath, suffix, pathToDatacard, pathToRoofile, pois = None, key = None, factor = None):
    # construct command string
    commandString = 'python tth_analysis_study.py "'+ targetPath + '/' +suffix + '" "' + pathToDatacard + '" "' + pathToRoofile
    if pois is not None:
        commandString = commandString + '" "' + ";".join("".join(mapping) for key, mapping in sorted(pois.items()))
            
    if key is not None:
        commandString = commandString + '" "' + key
    if factor is not None:
        commandString = commandString + '" "' + factor
    commandString = commandString +'"'
    # execute the command, queue the result
    print commandString
    (status, output) = commands.getstatusoutput(commandString)
    return status, output

def tth_fit_stability():
    targetPath = "/nfs/dust/cms/user/pkeicher/tth_analysis_study/CombineFitTests/PseudoDataTests/test/officialHiggsCombine/JTBDT_Spring17v10/test_msfit/"
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
    threads = list()
    que = Queue.Queue()

    pois = {"r_ttBPlus2B" : "(ttbarPlusB|ttbarPlus2B):r_ttBPlus2B[1,-10,10]",
            "r_ttcc" : "(ttbarPlusCCbar):r_ttcc[1,-10,10]"
            }

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
            base_suffix = base_suffix + "_".join(pois)+"_"+process+"_"+temp_factor

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

                t = Thread(target=lambda q, arg1, arg2, arg3, arg4, arg5, arg6, arg7: q.put(runScript(arg1, arg2, arg3, arg4, arg5, arg6, arg7)), args=(que, targetPath, suffix, datacard, pathToRoofile, pois, key, factor), name=suffix)
                threads.append(t)
                t.start()
                time.sleep(40)
            if datacardTable is not None:
                datacardTable.close()
                datacardTable = None
    return threads, que


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


# Create empty thread list and queue
threads = list()
que = Queue.Queue()

# Create list to save thread output
threadOutput = list()

#(threads, que) = JES_uncertainty_study()
(threads, que) = tth_fit_stability()


while threads:
    print "\n___________________________________\n"
    for thread in threads:
        if thread.isAlive():
            print('Thread for processing scaling {} is alive.'.format(thread.getName()))
            time.sleep(0.1)
        else:
            print('Thread for processing scaling {} is dead / finished.'.format(thread.getName()))
            #Get return value for thread
            thread.join()
            print('Joined array for tree {}.'.format(thread.getName()))
            # Remove thread from thread list
            threads.remove(thread)

    time.sleep(15)

while not que.empty():
    tmp_array = que.get()
    threadOutput.append(tmp_array)

print threadOutput
