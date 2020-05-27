import ROOT
import sys
import os
import glob

ROOT.gROOT.SetBatch(True)


identifier = "PseudoExperiment*"
fileName = "mlfit.root"

hNorms_shapeFolder_prefit = []
hNorms_shapeFolder_postfitS = []
hNorms_shapeFolder_postfitB = []
listOfShapeDirs = {"shapes_prefit": hNorms_shapeFolder_prefit, "shapes_fit_b" : hNorms_shapeFolder_postfitB, "shapes_fit_s" : hNorms_shapeFolder_postfitS}


hNorms_rooFit_prefit = []
hNorms_rooFit_postfitS = []
hNorms_rooFit_postfitB = []

listOfRooArgSets = {"norm_prefit": hNorms_rooFit_prefit, "norm_fit_b" : hNorms_rooFit_postfitB, "norm_fit_s" : hNorms_rooFit_postfitS}

debug = True

def fillHistoList(integral, suffix, categoryName, processName, listOfHistos):
    keyLoaded = False

    for histo in listOfHistos:
        if histo.GetName().startswith(categoryName) and processName in histo.GetName():
            histo.Fill(integral)
            keyLoaded = True
    if not keyLoaded:
        if integral == 0:
            lowerBound = -100
            upperBound = 100
        else:
            lowerBound = integral*0.05
            upperBound = integral*2.5
        nBins = int((upperBound - lowerBound)/2)
        if nBins <=0:
            nBins = 100
        histoName = categoryName+"_"+processName+"_"+suffix
        print "\tcreating new histogram", histoName, "from", lowerBound,"to",upperBound,"with",nBins,"bins!"
        tempHisto = ROOT.TH1D(histoName,";norm;Frequency", nBins, lowerBound, upperBound)
        tempHisto.SetDirectory(0)
        tempHisto.Fill(integral)
        listOfHistos.append(tempHisto)

        del tempHisto

def findHistos(currentDir, suffix, listOfHistos):
    listOfKeys = currentDir.GetListOfKeys()
    for key in listOfKeys:
        keyLoaded = False
        keyName = key.GetName()
        if key.IsFolder():
            if debug:
                print "current dir in findHistos:", currentDir.GetName(),"\ttrying to cd to", keyName
            if currentDir.cd(keyName):
                if debug :
                    print "\tsuccess!"
                findHistos(currentDir.GetDirectory(ROOT.gDirectory.GetPath()), suffix, listOfHistos)
        else:
            hProcess = currentDir.Get(keyName)

            if hProcess is not None and hProcess.InheritsFrom("TH1") and not "2" in hProcess.ClassName():
                integral = hProcess.Integral()
                if debug:
                    print "integral for process", keyName, "in category", currentDir.GetName(), "is", integral
                    fillHistoList(integral, suffix, currentDir.GetName(), keyName, listOfHistos)

def drawHistosFromList(histoDic):
    for entry in histoDic:
        for histo in histoDic[entry]:
            c = ROOT.TCanvas()
            histo.Draw()
            c.SaveAs(histo.GetName()+".pdf")
            c.SaveAs(histo.GetName()+".root")
            c.Clear()

print "identifier:", identifier
folderList = glob.glob(identifier)
#print "list of available folders:\n", folderList
for folder in glob.glob(identifier):
    if os.path.exists(folder):
        os.chdir(folder)
        if os.path.exists(fileName):
            if debug:
                print "loading", fileName, "from directory", folder
            infile = ROOT.TFile(fileName)
            for shapeFolder in listOfShapeDirs:
                if debug:
                    print "trying to cd to", shapeFolder
                if infile.cd(shapeFolder):
                    findHistos(infile.GetDirectory(ROOT.gDirectory.GetPath()), shapeFolder, listOfShapeDirs[shapeFolder])
            for rooObject in listOfRooArgSets:
                if debug:
                    print "loading", rooObject
                normObject = infile.Get(rooObject)
                if normObject is not None:
                    listOfContents = normObject.contentsString().split(",")
                    for process in listOfContents:
                        fillHistoList(normObject[process].getVal(), rooObject, process.split("/")[0], process.split("/")[1], listOfRooArgSets[rooObject])

            infile.Close()

        else:
            print "Error! Could not load file", fileName
        os.chdir("../")
    else:
        print "unable to open folder", folder

drawHistosFromList(listOfShapeDirs)
drawHistosFromList(listOfRooArgSets)
