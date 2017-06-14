import ROOT
import sys
import os
import subprocess
import time
from array import array
#import glob
ROOT.gROOT.SetBatch(True)

outputDirectory = sys.argv[1] #path to store PseudoExperiments in
print "input for outputDirectory:", outputDirectory
pathToDatacard = sys.argv[2] #path to unscaled datacard with data to be fitted to scaled toys
pathToInputRootfile = sys.argv[3] #path to corresponding root file

if len(sys.argv)>4:
    datacardOrProcessList = sys.argv[4] #either path to datacard with scaled data oder comma-separated list of Processes to be scaled
if len(sys.argv)>5:
    scaleFuncList = sys.argv[5] #if argument 3 is a list of processes this parameter is a comma-separated list of functions to use (TF1)

pathToScaledDatacard = None

scalingDic = [] #2D list of form [(Process, Func to scale with),(...),...]


#check if third argument is a list of process or a datacard and act accordingly
if datacardOrProcessList.endswith(".txt"):
    pathToScaledDatacard = os.path.abspath(datacardOrProcessList)
    print "using already scaled data from", pathToScaledDatacard
else:
    listOfProcesses = datacardOrProcessList.split(",")
    listOfFormulae = scaleFuncList.split(",")
    scalingDic = [entry for entry in zip(listOfProcesses, listOfFormulae)]
    print "using scaling dictionary:", scalingDic

#set up parameters for toy generation here
numberOfToys = 1000
numberOfToysPerJob = 20
toyMode = 1 #controls how many toys per experiment are generated. Should be set to -1 for asimov toys
workdir = "/nfs/dust/cms/user/pkeicher/tth_analysis_study/CombineFitTests/PseudoDataTests/scripts"

POIsuffix = "bgnorm_"

#--------------------------------------------------------------------------------------------------------------------------------------------

def submitToNAF(datacardToUse, listOfPOIs):
    jobids=[]
    command=[workdir+"/submitCombineToyCommand.sh", pathToDatacard, datacardToUse, outputDirectory, str(numberOfToys), str(numberOfToysPerJob), str(toyMode), listOfPOIs]
    a = subprocess.Popen(command, stdout=subprocess.PIPE,stderr=subprocess.STDOUT,stdin=subprocess.PIPE)
    output = a.communicate()[0]
    #print output
    jobidstring = output.split()
    for jid in jobidstring:
        if jid.isdigit():
            jobid=int(jid)
            print "this job's ID is", jobid
            jobids.append(jobid)
            continue

    return jobids

def do_qstat(jobids):
    allfinished=False
    while not allfinished:
        time.sleep(60)
        a = subprocess.Popen(['qstat'], stdout=subprocess.PIPE,stderr=subprocess.STDOUT,stdin=subprocess.PIPE)
        qstat=a.communicate()[0]
        lines=qstat.split('\n')
        nrunning=0
        for line in lines:
            words=line.split()
            for jid in words:
                if jid.isdigit():
                    jobid=int(jid)
                    if jobid in jobids:
                        nrunning+=1
                        break

        if nrunning>0:
            print nrunning,'jobs running'
        else:
            allfinished=True

def checkCopy(original, copy):
    if original.GetNbinsX() is not copy.GetNbinsX():
        print "bins do not match!"
    print "Integrals:\n\toriginal:", original.Integral(), "\tcopy:", copy.Integral()
    for currentBin in range(1, original.GetNbinsX()+1):
        print "values in bin", currentBin, ": original =", original.GetBinContent(currentBin), "\tcopy =", copy.GetBinContent(currentBin)

def collectNorms(listOfNorms, histo):
    histoName = histo.GetName()
    processName, trunk = histoName.split("_finaldiscr_")
    groups = trunk.split("_")
    if not (groups[len(groups)-1].endswith("Up") or groups[len(groups)-1].endswith("Down")):
        print "saving norm for histogram", histo.GetName()

        categoryName = "_".join(groups[:3])
        indexCategory = -1
        integral = histo.Integral()
        for entry in range(len(listOfNorms)):
            if listOfNorms[entry][0] == categoryName:
                indexCategory = entry
        indexProcess = -1
        if not indexCategory == -1:
            for entry in range(len(listOfNorms[indexCategory][1])):
                if listOfNorms[indexCategory][1][entry][0] == processName:
                    indexProcess = entry
            if not indexProcess == -1:
                raise sys.exit("Trying to fill one process twice for one category! Aborting...")
            else:
                listOfNorms[indexCategory][1].append([processName, integral])
        else:
            listOfNorms.append([categoryName,[[processName, integral]]])
    #else:
        #print "skipping normalisation for histo", histo.GetName()

def saveListAsTree(listOfNormsPrescale, listOfNormsPostscale, outputFileName):
    outfile = ROOT.TFile(outputFileName, "RECREATE")
    print "number of collected categories:", len(listOfNormsPrescale), "\tpostscale:", len(listOfNormsPostscale)
    for category in range(len(listOfNormsPrescale)):
        print "Creating TTree for category", listOfNormsPrescale[category][0]

        tree = ROOT.TTree(listOfNormsPrescale[category][0], listOfNormsPrescale[category][0])
        print "\tsuccess"
        prescaleVals = []
        postscaleVals = []
        total_background_prescale = array("d", [0.])
        total_background_postscale = array("d", [0.])
        total_signal_prescale = array("d",[0.])
        total_signal_postscale = array("d",[0.])

        for process in range(len(listOfNormsPrescale[category][1])):
            prescaleVals.append(array("d", [0.]))
            postscaleVals.append(array("d", [0.]))
            prescaleProcesses = listOfNormsPrescale[category][1][process]
            postscaleProcesses = listOfNormsPostscale[category][1][process]

            processName = prescaleProcesses[0]
            #print "\tcreating branch for process", processName
            tree.Branch(processName+"_prescale", prescaleVals[process], "{0}_prescale[{1}]/D".format(processName, process))
            tree.Branch(processName+"_postscale", postscaleVals[process], "{0}_postscale[{1}]/D".format(processName, process))

            #print "\t\tsuccess"
            prescaleVals[process][0] = prescaleProcesses[1]
            postscaleVals[process][0] = postscaleProcesses[1]
            #print "\tfill branch", processName, "with value", ratios[process][0]

            print "adding process", processName, "\n\tprescale:", prescaleProcesses[1], "\tpostscale", postscaleProcesses[1]
            if processName.startswith("ttH_"):
                #print "current status:\n\ttotal_signal_prescale:", total_signal_prescale[0], "\ttotal_signal_postscale:", total_signal_postscale[0]

                total_signal_prescale[0] = total_signal_prescale[0]+prescaleProcesses[1]
                total_signal_postscale[0] = total_signal_postscale[0]+postscaleProcesses[1]
            elif not processName.startswith("ttH") and processName.startswith("t"):
                #print "current status:\n\ttotal_background_prescale:", total_background_prescale[0], "\ttotal_background_postscale:", total_background_postscale[0]
                total_background_prescale[0] = total_background_prescale[0]+prescaleProcesses[1]
                total_background_postscale[0] = total_background_postscale[0]+postscaleProcesses[1]

        tree.Branch("total_signal_prescale", total_signal_prescale, "total_signal/D")
        tree.Branch("total_signal_postscale", total_signal_postscale, "total_signal/D")

        tree.Branch("total_background_prescale", total_background_prescale, "total_background/D")
        tree.Branch("total_background_postscale", total_background_postscale, "total_background/D")

        #print "\tfill branch total_signal with value", total_signal[0]
        #print "\tfill branch total_background with value", total_background[0]

        tree.Fill()
        tree.Write()
        del tree
    outfile.Close()

def scaleHistogram(key, inputRootFile, funcFormula, currentOutputDir, listOfNormsPrescale, listOfNormsPostscale):
    histo = inputRootFile.Get(key.GetName())
    histo.SetDirectory(currentOutputDir)
    #for usage depending on x values:
    #lowerBound = histo.GetXaxis().GetBinLowerEdge(1)
    #upperBound = histo.GetXaxis().GetBinLowerEdge(histo.GetNbinsX()) + histo.GetXaxis().GetBinWidth(histo.GetNbinsX())/2
    collectNorms(listOfNormsPrescale, histo)
    print "\tScaling", key.GetName(),"with function", funcFormula

    scaleFunc = ROOT.TF1("scaleFunc",funcFormula,0, histo.GetNbinsX() )
    if scaleFunc is not None:
        for currentBin in range(1, histo.GetNbinsX()+1):
            #print "\t\tBefore scaling bin", currentBin,":\t", histo.GetBinContent(currentBin)
            scaleFactor = scaleFunc.Eval(currentBin)
            histo.SetBinContent(currentBin, histo.GetBinContent(currentBin)*scaleFactor)
            #print "\t\tAfter scaling bin", currentBin,":\t", histo.GetBinContent(currentBin)
    collectNorms(listOfNormsPostscale, histo)
    return histo

def copyOrScaleElements(inputRootFile, outputFile, processScalingDic, listOfKeys, listOfNormsPrescale, listOfNormsPostscale):
    tempObject = None
    for key in listOfKeys:
        path=ROOT.gDirectory.GetPathStatic()
        if key.IsFolder():
            if not path.endswith("/"):
                path = path + "/"
            pathIntoKeyDir = path + key.GetName()
            outputFile.mkdir(pathIntoKeyDir)
            outputFile.cd(pathIntoKeyDir)
            inputRootFile.cd(pathIntoKeyDir)
            copyOrScaleElements(inputRootFile, outputFile, processScalingDic, ROOT.gDirectory.GetListOfKeys(), listOfNormRatios)
            outputFile.cd(path)
            inputRootFile.cd(path)
        else:
            #index = next((entryNum for entryNum, sublist in enumerate(processScalingDic) if (key.GetName().startswith(sublist[entryNum]) and not (key.GetName().endswith("Up") or key.GetName().endswith("Down")) )),-1)
            #print "current key name:", key.GetName()
            index = -1
            for entry in range(len(processScalingDic)):
                if key.GetName().startswith(processScalingDic[entry][0]+"_"):
                    #print "Found match for process #{0}: {1}".format(entry, processScalingDic[entry][0])
                    index = entry
            if index is not -1:
                #print "found match at index", index

                tempObject = scaleHistogram(key,inputRootFile, processScalingDic[index][1], outputFile.GetDirectory(path), listOfNormsPrescale, listOfNormsPostscale)
            else:
                tempObject = inputRootFile.Get(key.GetName())
                collectNorms(listOfNormsPrescale, tempObject)
                collectNorms(listOfNormsPostscale, tempObject)

                tempObject.SetDirectory(outputFile.GetDirectory(path))
                #print "\tCopied", key.GetName(),"to new root file"
            if tempObject is not None:
                #checkCopy(inputRootFile.Get(key.GetName()), tempObject)
                tempObject.Write()
                del tempObject

def generateToysAndFit(inputRootFile, processScalingDic, pathToScaledDatacard, outputPath):
    print "outputPath in generateToysAndFit:", outputPath
    if not os.path.exists("temp"):
        os.makedirs("temp")
    os.chdir("temp")
    listOfNormsPrescale = []
    listOfNormsPostscale = []
    shapeExpectation = ""
    if len(processScalingDic) is not 0:
        suffix = ""
        for processPair in processScalingDic:
            suffix = suffix+processPair[0].replace("*","")+"_"+processPair[1].replace("*","")+"_"
        newRootFileName = "temp_" + suffix + os.path.basename(inputRootFile.GetName())
        outputFile = ROOT.TFile(newRootFileName,"RECREATE")


        copyOrScaleElements(inputRootFile, outputFile, processScalingDic, inputRootFile.GetListOfKeys(), listOfNormsPrescale, listOfNormsPostscale)
        outputFile.Close()

        datacardToUse = os.path.abspath(writeDatacard(pathToDatacard, newRootFileName, listOfProcesses))
        newRootFileName = "temp_shape_expectation.root"#_" + suffix + os.path.basename(inputRootFile.GetName())
        saveListAsTree(listOfNormsPrescale, listOfNormsPostscale, newRootFileName)
        shapeExpectation = "{0}/temp/{1}".format(outputPath, newRootFileName)
        os.chdir("../")
    elif pathToScaledDatacard is None:
        datacardToUse = pathToDatacard
    elif pathToScaledDatacard is not None and os.path.exists(pathToScaledDatacard):
        datacardToUse = pathToScaledDatacard
    elif pathToScaledDatacard is not None:
        raise sys.exit("Could not find scaled datacard")
    else:
        raise sys.exit("Neither process list nor scaled datacard were found! Please check input variables")

    if os.path.exists(datacardToUse):
        print "creating toy data from datacard", datacardToUse
        listOfPOIs = "r"
        for processPair in processScalingDic:
            listOfPOIs = listOfPOIs+","+POIsuffix+processPair[0]
        jobids = submitToNAF(datacardToUse, listOfPOIs)
        print "waiting for toy generation to finish"
        do_qstat(jobids)
        os.chdir(workdir)
        print "calling plotResults with arguments:"
        print "\toutputPath =", outputPath
        print "\tshapeExpectation =", shapeExpectation
        subprocess.check_call([workdir+"/submitScript.sh", outputPath, shapeExpectation])

    else:
        print "Couldn't find datacard", datacardToUse

def writeDatacard(pathToDatacard, newRootFileName, listOfProcesses):
    datacard = open(pathToDatacard)
    newDatacardName = "temp_datacard.txt"
    newDatacard = open(newDatacardName, "w")
    listOfEntriesToChange = []
    print listOfProcesses
    for line in datacard:
        entries = line.split()
        for i, entry in enumerate(entries):
            #print "\t", i, "\t", entry

            if entries[0]=="process":
                #print "first entry of line is process!"
                if entry in listOfProcesses:
                    #print "found match! Saving index of process", entry
                    listOfEntriesToChange.append(i)
            if entries[0]=="rate":
                #print "first entry of line is rate!"

                if i in listOfEntriesToChange:
                    #print "found match!"
                    entry = "-1"
            if entry.endswith(".root"):
                #print "\tfound entry for root file! Replacing", entry, "with", newRootFileName
                entry = newRootFileName
            newDatacard.write(entry+" ")
        newDatacard.write("\n")

    newDatacard.close()
    datacard.close()
    return newDatacardName


if os.path.exists(pathToDatacard) and os.path.exists(pathToInputRootfile):
    pathToDatacard=os.path.abspath(pathToDatacard)
    pathToInputRootfile = os.path.abspath(pathToInputRootfile)
    inputRootFile = ROOT.TFile(pathToInputRootfile, "READ")
    if not os.path.exists(outputDirectory):
        os.makedirs(outputDirectory)
    #print "outputDirectory after directory was created:", outputDirectory
    outputDirectory = os.path.abspath(outputDirectory)
    #print "outputDir after calling abspath:", outputDirectory
    os.chdir(outputDirectory)
    #print "outputDirectory after changing dirs:", outputDirectory
    generateToysAndFit(inputRootFile, scalingDic, pathToScaledDatacard, outputDirectory)
else:
    print "Incorrect paths to input data. Please make sure path to datacard is correct!"
