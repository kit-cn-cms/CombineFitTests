import ROOT
import sys
import os
import subprocess
#import glob
#from array import array
ROOT.gROOT.SetBatch(True)

def checkCopy(original, copy):
    if original.GetNbinsX() is not copy.GetNbinsX():
        print "bins do not match!"
    print "Integrals:\n\toriginal:", original.Integral(), "\tcopy:", copy.Integral()
    for currentBin in range(1, original.GetNbinsX()+1):
        print "values in bin", currentBin, ": original =", original.GetBinContent(currentBin), "\tcopy =", copy.GetBinContent(currentBin)

def scaleHistogram(key, inputRootFile, funcFormula, currentOutputDir):
    histo = inputRootFile.Get(key.GetName())
    histo.SetDirectory(currentOutputDir)
    #for usage depending on x values:
    #lowerBound = histo.GetXaxis().GetBinLowerEdge(1)
    #upperBound = histo.GetXaxis().GetBinLowerEdge(histo.GetNbinsX()) + histo.GetXaxis().GetBinWidth(histo.GetNbinsX())
    print "\tScaling", key.GetName(),"with function", funcFormula

    scaleFunc = ROOT.TF1("scaleFunc",funcFormula,0, histo.GetNbinsX() )
    if scaleFunc is not None:
        for currentBin in range(1, histo.GetNbinsX()+1):
            #print "\t\tBefore scaling bin", currentBin,":\t", histo.GetBinContent(currentBin)
            scaleFactor = scaleFunc.Eval(currentBin)
            histo.SetBinContent(currentBin, histo.GetBinContent(currentBin)*scaleFactor)
            #print "\t\tAfter scaling bin", currentBin,":\t", histo.GetBinContent(currentBin)
    return histo

def copyOrScaleElements(inputRootFile, outputFile, processScalingDic, listOfKeys):
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
            copyOrScaleElements(inputRootFile, outputFile, processScalingDic, ROOT.gDirectory.GetListOfKeys())
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

                tempObject = scaleHistogram(key,inputRootFile, processScalingDic[index][1], outputFile.GetDirectory(path))
            else:
                tempObject = inputRootFile.Get(key.GetName())
                tempObject.SetDirectory(outputFile.GetDirectory(path))
                #print "\tCopied", key.GetName(),"to new root file"
            if tempObject is not None:
                #checkCopy(inputRootFile.Get(key.GetName()), tempObject)
                tempObject.Write()
                del tempObject

def generateToysAndFit(inputRootFile, processScalingDic, pathToScaledDatacard):
    if not os.path.exists("temp"):
        os.makedirs("temp")
    os.chdir("temp")
    if len(processScalingDic) is not 0:
        newRootFileName = "temp_" + os.path.basename(inputRootFile.GetName())
        outputFile = ROOT.TFile(newRootFileName,"RECREATE")


        copyOrScaleElements(inputRootFile, outputFile, processScalingDic, inputRootFile.GetListOfKeys())
        outputFile.Close()
        datacardToUse = os.path.abspath(writeDatacard(pathToDatacard, newRootFileName, listOfProcesses))
        os.chdir("../")

    elif pathToScaledDatacard is not None and os.path.exists(pathToScaledDatacard):
        datacardToUse = pathToScaledDatacard
    elif pathToScaledDatacard is not None:
        raise sys.exit("Could not find scaled datacard")
    else:
        raise sys.exit("Neither process list nor scaled datacard were found! Please check input variables")

    if os.path.exists(datacardToUse):
        print "creating toy data from datacard", datacardToUse
        subprocess.check_call([workdir+"/submitCombineToyCommand.sh", pathToDatacard, datacardToUse, "./", str(numberOfToys), str(numberOfToysPerJob), str(toyMode)])
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

outputDirectory = sys.argv[1] #path to store PseudoExperiments in

pathToDatacard = sys.argv[2] #path to unscaled datacard with data to be fitted to scaled toys
pathToInputRootfile = sys.argv[3] #path to corresponding root file

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
    print "using scaling dictionary: ", scalingDic

#set up parameters for toy generation here
numberOfToys = 1000
numberOfToysPerJob = 20
toyMode = 1 #controls how many toys per experiment are generated. Should be set to -1 for asimov toys
#signalStrength = 1

workdir = "/nfs/dust/cms/user/pkeicher/tth_analysis_study/CombineFitTests/PseudoDataTests"

if os.path.exists(pathToDatacard) and os.path.exists(pathToInputRootfile):
    pathToDatacard=os.path.abspath(pathToDatacard)
    pathToInputRootfile = os.path.abspath(pathToInputRootfile)
    inputRootFile = ROOT.TFile(pathToInputRootfile, "READ")
    if not os.path.exists(outputDirectory):
        os.makedirs(outputDirectory)
    os.chdir(outputDirectory)

    generateToysAndFit(inputRootFile, scalingDic, pathToScaledDatacard)

else:
    print "Incorrect paths to input data. Please make sure path to datacard is correct!"
