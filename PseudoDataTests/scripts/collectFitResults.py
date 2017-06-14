import ROOT
import sys
import os
import glob
from array import array
ROOT.gROOT.SetBatch(True)

def readoutMlfitFile(pathToMlFitFile):
    print "Trying to read file '", fileName , "'"
    maxLikelihoodTableEntry = ""

    if os.path.exists(pathToMlFitFile):
        rootFile = ROOT.TFile(pathToMlFitFile, "READ")
        fitTrees = ["tree_fit_sb"]

        for fitTree in fitTrees:
            tree_fit = rootFile.Get(fitTree)

            mu = array( 'd', [ 0.0 ] )
            muLowErr = array( 'd', [ 0.0 ] )
            muHighErr = array( 'd', [ 0.0 ] )
            try:
                tree_fit.SetBranchAddress("mu", mu)
                tree_fit.SetBranchAddress("muLoErr", muLowErr)
                tree_fit.SetBranchAddress("muHiErr", muHighErr)
                tree_fit.GetEntry()
                #print "\tRead Out Values:\n\tmu = ", mu[0], "\tmuLowErr = ", muLowErr[0], "\tmuHighErr = ", muHighErr[0]
                maxLikelihoodTableEntry = maxLikelihoodTableEntry + " & $\\num{" + str(mu[0]) + "}^{\\num{" + str(muHighErr[0]) + "}}_{\\num{-" + str(muLowErr[0]) + "}}$"
                del tree_fit
            except:
                print "\tSomething went wrong while obtaining mlfit data: ", sys.exc_info()[0]
        rootFile.Close()
    else:
        print "File '", pathToMlFitFile , "' does not exist!"
    return maxLikelihoodTableEntry

def fillOutputTableFile(outputTableFileName, slideTitle, columntype, headline, listOfEntries):
    if len(listOfEntries) is not 0:
        listOfEntries.sort()
        outputTableFile = None
        MVA=""
        for entry in listOfEntries:
            currentMVA = entry.split("\\_")[0]
            if currentMVA == MVA:
                outputTableFile.write(entry)
            else:
                if outputTableFile is not None:
                    outputTableFile.write("\\bottomrule\n")
                    outputTableFile.write("\\end{tabular}\n")
                    outputTableFile.write("\\end{table}\n")
                    outputTableFile.write("\\end{frame}")
                    outputTableFile.close()
                MVA = currentMVA
                outputTableFile=open(outputTableFileName + "_" + MVA + ".tex", "w")

                outputTableFile.write("\\begin{frame}{" + slideTitle + " " + MVA +"}\n")
                outputTableFile.write("\\begin{table}\n\\centering\n")
                outputTableFile.write("\\begin{tabular}{" + columntype + "}\n")
                outputTableFile.write("\\toprule\n")
                outputTableFile.write(headline + "\\\\\n")
                outputTableFile.write("\\midrule\n")
        if outputTableFile is not None:
            outputTableFile.write("\\bottomrule\n")
            outputTableFile.write("\\end{tabular}\n")
            outputTableFile.write("\\end{table}\n")
            outputTableFile.write("\\end{frame}")
            outputTableFile.close()
    else:
        print "Did not write file '" + outputTableFileName + ".tex': No entries found!\n"

def getPullPlots(pathToPullPlotsFile, category, signalStrength):
    entryForTexFile = ""
    if os.path.exists(pathToPullPlotsFile):
        rootFile = ROOT.TFile(pathToPullPlotsFile, "READ")
        pullPlotCanvas = rootFile.Get("nuisancs")
        try:
            category = category.replace("\\_", "_")
            #parentPath, fileName = pathToPullPlotsFile.split("/")
            parentPath, fileName = os.path.split(os.path.abspath(pathToPullPlotsFile))
            #parentPath, folderName = os.path.split(os.path.abspath(parentPath))

            canvasName = category+"_pullplot_"+signalStrength+".pdf"
            pullPlotCanvas.SaveAs(parentPath + "/" + canvasName)
            entryForTexFile = canvasName

        except:
            print "\tCould not print pull plot for nuisances!"
    else:
        print "File '", pathToPullPlotsFile , "' does not exist!"
    return entryForTexFile

identifier=sys.argv[1]

limitTableEntryList = []

maxLikelihoodTableEntryList = []

pullPlotNames = []


for folders in glob.glob(identifier):
    rest, category = folders.split("limits_")
    category = category.replace("_datacard", "")
    category = category.replace("_hdecay", "")
    print "category name: ", category

    category = category.replace("_", "\\_")

    fileName = folders + "/higgsCombineTest.Asymptotic.mH125.root"
    print "Trying to read file '", fileName , "'"
    if os.path.exists(fileName):
        rootFile = ROOT.TFile(fileName, "READ")
        limitTree = rootFile.Get("limit")
        try:
            readOutVal = array( 'd', [ 0.0 ] )
            limitTree.SetBranchAddress("limit", readOutVal)
            limitTree.GetEntry(1)
            limit_16percent = readOutVal[0]
            limitTree.GetEntry(2)
            limit_50percent = readOutVal[0]
            limitTree.GetEntry(3)
            limit_84percent = readOutVal[0]
            limitTableEntryList.append(category + " & $\\num{" + str(limit_50percent) + "}^{\\num{" + str(limit_84percent - limit_50percent) + "}}_{\\num{" + str(limit_16percent - limit_50percent) + "}}$\\\\\n")
        except:
            print "\tSomething went wrong while reading limit tree: ", sys.exc_info()[0]
        rootFile.Close()
    else:
        print "file '", fileName, "' does not exist!"

    fileName = folders + "/mlfit_sig0.root"

    maxLikelihoodTableEntry = category
    maxLikelihoodTableEntry = maxLikelihoodTableEntry + readoutMlfitFile(fileName)
    fileName = folders + "/pullplots_sig0.root"
    pullPlotNames.append((category, [getPullPlots(fileName, category, "sig0")]))

    maxLikelihoodTableEntry = maxLikelihoodTableEntry
    fileName = folders + "/mlfit_sig1.root"
    maxLikelihoodTableEntry = maxLikelihoodTableEntry + readoutMlfitFile(fileName)
    fileName = folders + "/pullplots_sig1.root"
    pullPlotNames[len(pullPlotNames)-1][1].append(getPullPlots(fileName, category, "sig1"))

    maxLikelihoodTableEntry = maxLikelihoodTableEntry + "\\\\\n"
    maxLikelihoodTableEntryList.append(maxLikelihoodTableEntry)

fillOutputTableFile("limitTable", "Limit Fit Results" ,"cc", "MVA\\_Categories & limit (median)", limitTableEntryList)

multiColumnHeader = "MVA\\_Categories & Expected Signal Strength = 0 & Expected Signal Strength = 1"
fillOutputTableFile("maxLikelihoodTable", "MaxLikelihood Fit Results " ,"ccc", multiColumnHeader, maxLikelihoodTableEntryList)

if len(pullPlotNames) is not 0:
    pullPlotNames.sort()
    MVA = ""
    pullPlotsTexFile = None
    for categories in pullPlotNames:
        currentMVA = categories[0].split("\\_")[0]
        if currentMVA == MVA:
            pullPlotsTexFile.write("\n\\begin{frame}{Pull Plots " + categories[0] + "}\n")
            pullPlotsTexFile.write("\\begin{figure}\n")

            for canvasName in categories[1]:
                category, signalStrength = canvasName.split("_pullplot_")
                signalStrength = signalStrength.replace(".pdf", "")
                signalStrength = signalStrength.replace("sig", "")
                pullPlotsTexFile.write("\\subfloat[Expected Signal Strength = " + signalStrength + "]{\\includegraphics[width=0.48\\textwidth]{pullplots/" + canvasName + "}}\n")
                if canvasName is not categories[1][-1]:
                    pullPlotsTexFile.write("$\\quad$\n")
            pullPlotsTexFile.write("\\caption{Pull Plots for category " + categories[0] +"}\n")
            pullPlotsTexFile.write("\\end{figure}\n")
            pullPlotsTexFile.write("\\end{frame}\n")
        else:
            if pullPlotsTexFile is not None:
                pullPlotsTexFile.close()
            MVA = currentMVA
            pullPlotsTexFile=open("pullPlots_" + MVA + ".tex", "w")

    if pullPlotsTexFile is not None:
        pullPlotsTexFile.close()
else:
    print "could not write pull plot tex file: No pull plot names collected!"
