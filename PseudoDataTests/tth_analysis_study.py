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
    for currentBin in range(original.GetNbinsX()):
        print "values in bin", currentBin, ": original =", original.GetBinContent(currentBin), "\tcopy =", copy.GetBinContent(currentBin)

def scaleHistogram(key, inputRootFile, funcFormula):
    histo = inputRootFile.Get(key.GetName())
    #for usage depending on x values:
    #lowerBound = histo.GetXaxis().GetBinLowerEdge(1)
    #upperBound = histo.GetXaxis().GetBinLowerEdge(histo.GetNbinsX()) + histo.GetXaxis().GetBinWidth(histo.GetNbinsX())
    print "\tScaling", key.GetName(),"with function", funcFormula

    scaleFunc = ROOT.TF1("scaleFunc",funcFormula,0, histo.GetNbinsX() )
    for currentBin in range(histo.GetNbinsX()):
        print "\t\tBefore scaling bin", currentBin,":\t", histo.GetBinContent(currentBin)
        scaleFactor = scaleFunc.Eval(currentBin)
        histo.SetBinContent(currentBin, histo.GetBinContent(currentBin)*scaleFactor)
        print "\t\tAfter scaling bin", currentBin,":\t", histo.GetBinContent(currentBin)

    return histo

def loopOverElements(inputRootFile, processScalingDic, listOfKeys):
    tempObject = None
    for key in listOfKeys:
        if key.IsFolder():
            path=ROOT.gDirectory.GetPathStatic()
            if not path.endswith("/"):
                path = path + "/"
            pathIntoKeyDir = path + key.GetName()
            inputRootFile.cd(pathIntoKeyDir)
            loopOverElements(inputRootFile, processScalingDic, ROOT.gDirectory.GetListOfKeys())
        else:
            index = next((entryNum for entryNum, sublist in enumerate(processScalingDic) if key.GetName() in sublist),-1)
            if index is not -1:
                tempObject = scaleHistogram(key,inputRootFile, processScalingDic[index][1])
            else:
                print "\tCopied", key.GetName(),"to new root file"
                tempObject = inputRootFile.Get(key.GetName())
            if tempObject is not None:
                checkCopy(inputRootFile.Get(key.GetName()), tempObject)
                tempObject.Write()

#createScaledInput(inputRootFile, )

def createToys(inputRootFile, processScalingDic):
    if not os.path.exists("temp"):
        os.makedirs("temp")
    os.chdir("temp")
    newRootFileName = "temp_" + os.path.basename(inputRootFile.GetName())
    outputFile = ROOT.TFile(newRootFileName,"RECREATE")

    loopOverElements(inputRootFile, processScalingDic, inputRootFile.GetListOfKeys())
    outputFile.Close()
    #datacardToUse = writeDatacard(pathToDatacard, newRootFileName, listOfProcesses)
    datacardToUse = pathToDatacard
    if os.path.exists(datacardToUse):
        subprocess.check_call(["../generateToys.sh", datacardToUse, str(numberOfToys), str(signalStrength)])
    else:
        print "Couldn't find datacard!"
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
            if entries[0] is "process":
                if entry in listOfProcesses:
                    print "found match! Saving index of process", entry
                    listOfEntriesToChange.append(i)
            if entries[0] is "rate":
                if i in listOfEntriesToChange:
                    print "found match!"
                    entry = "-1"
            if entry.endswith(".root"):
                entry = newRootFileName
            newDatacard.write(entry+" ")
        newDatacard.write("\n")

            #print "\t", i, "\t", entry
    newDatacard.close()
    datacard.close()
    return newDatacardName

pathToDatacard = sys.argv[1]
pathToInputRootfile = sys.argv[2]
processesToAlter = sys.argv[3]
scalingFormulae = sys.argv[4]

listOfProcesses = processesToAlter.split(",")
listOfFormulae = scalingFormulae.split(",")

numberOfToys = 1
signalStrength = 0

scalingDic = [entry for entry in zip(listOfProcesses, listOfFormulae)]


if os.path.exists(pathToInputRootfile) and os.path.exists(pathToDatacard):
    pathToDatacard=os.path.abspath(pathToDatacard)
    pathToInputRootfile = os.path.abspath(pathToInputRootfile)
    inputRootFile = ROOT.TFile(pathToInputRootfile, "READ")

    toyfile = createToys(inputRootFile, scalingDic)
    if os.path.exists(toyfile):
        print "starting combine fit"
        #subprocess.check_call(["./fitToyData.sh", pathToDatacard, toyfile, str(signalStrength)])
    else:
        print "Cannot find toy file!"
else:
    print "Incorrect paths to input data. Please make sure paths to .root file and datacard are correct!"
