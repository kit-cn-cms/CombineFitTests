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
            print "\t\tBefore scaling bin", currentBin,":\t", histo.GetBinContent(currentBin)
            scaleFactor = scaleFunc.Eval(currentBin)
            histo.SetBinContent(currentBin, histo.GetBinContent(currentBin)*scaleFactor)
            print "\t\tAfter scaling bin", currentBin,":\t", histo.GetBinContent(currentBin)
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
            index = next((entryNum for entryNum, sublist in enumerate(processScalingDic) if (key.GetName().startswith(sublist[entryNum]))),-1)
            if index is not -1:
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
        datacardToUse = writeDatacard(pathToDatacard, newRootFileName, listOfProcesses)
    elif pathToScaledDatacard is not None and os.path.exists(pathToScaledDatacard):
        datacardToUse = pathToScaledDatacard
    elif pathToScaledDatacard is not None:
        raise sys.exit("Could not find scaled datacard")
    else:
        raise sys.exit("Neither process list nor scaled datacard were found! Please check input variables")

    if os.path.exists(datacardToUse):
        print "creating toy data from datacard", datacardToUse
        subprocess.check_call([workdir+"/generateToys.sh", datacardToUse, str(numberOfToys), str(signalStrength)])
    else:
        print "Couldn't find datacard", datacardToUse
    os.chdir("../")
    return "temp/higgsCombine_{0}toys_sig{1}.GenerateOnly.mH125.123456.root".format(numberOfToys, signalStrength)

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

pathToDatacard = sys.argv[1] #path to unscaled datacard with data to be fitted to scaled toys
pathToInputRootfile = sys.argv[2] #path to corresponding root file

datacardOrProcessList = sys.argv[3] #either path to datacard with scaled data oder comma-separated list of Processes to be scaled
if len(sys.argv)>4:
    scaleFuncList = sys.argv[4] #if argument 3 is a list of processes this parameter is a comma-separated list of functions to use (TF1)

pathToScaledDatacard = None

scalingDic = [] #2D list of form [(Process, Func to scale with),(...),...]

#check if
if datacardOrProcessList.endswith(".txt"):
    pathToScaledDatacard = os.path.abspath(datacardOrProcessList)
    print "using already scaled data from", pathToScaledDatacard
else:
    listOfProcesses = datacardOrProcessList.split(",")
    listOfFormulae = scaleFuncList.split(",")
    scalingDic = [entry for entry in zip(listOfProcesses, listOfFormulae)]

numberOfToys = 1000
signalStrength = 1

workdir = "/nfs/dust/cms/user/pkeicher/tth_analysis_study/CombineFitTests/PseudoDataTests"

if os.path.exists(pathToDatacard) and os.path.exists(pathToInputRootfile):
    pathToDatacard=os.path.abspath(pathToDatacard)
    pathToInputRootfile = os.path.abspath(pathToInputRootfile)
    inputRootFile = ROOT.TFile(pathToInputRootfile, "READ")

    toyfile = generateToysAndFit(inputRootFile, scalingDic, pathToScaledDatacard)

    if os.path.exists(toyfile):
        print "submitting combine fit"
        subprocess.check_call([workdir+"/fitToyData.sh", pathToDatacard, toyfile, str(signalStrength), str(numberOfToys)])
    else:
        print "Cannot find toy file!"
else:
    print "Incorrect paths to input data. Please make sure path to datacard is correct!"
