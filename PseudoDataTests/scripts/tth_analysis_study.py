import ROOT
import sys
import os
<<<<<<< HEAD
import subprocess
import time
from array import array
#import glob
ROOT.gROOT.SetBatch(True)

#set up parameters for toy generation here
numberOfToys = 1000
numberOfToysPerJob = 10
toyMode = 1 #controls how many toys per experiment are generated. Should be set to -1 for asimov toys
if toyMode == -1:
    numberOfToys = 1
workdir = "/nfs/dust/cms/user/pkeicher/tth_analysis_study/CombineFitTests/PseudoDataTests/scripts"

POIsuffix = "bgnorm_"
=======
import stat
import subprocess
import time
import shutil
from array import array
from optparse import OptionParser
from optparse import OptionGroup
#import glob
ROOT.gROOT.SetBatch(True)

workdir = "/nfs/dust/cms/user/pkeicher/tth_analysis_study/CombineFitTests/PseudoDataTests/scripts"


parser = OptionParser()
group_required = OptionGroup(parser, "Required Options")
group_globalOptions = OptionGroup(parser, "Options Valid for All Option Groups")
group_scalingOptions = OptionGroup(parser, "Scaling Related Options")
group_required.add_option("-o", "--outputDirectory", dest="outputDirectory", help="save signal strength folders with PseudoExperiments here", metavar = "PATH")
group_globalOptions.add_option("-n", "--numberOfToys", dest="numberOfToys", help="generate this many toys per signal strength (default = 1000)", default = 1000, type="int")
group_globalOptions.add_option("--nToysPerJob", dest = "nPerJob", help="process this many toys at once on bird system (default = 30)", default = 30 , type="int")
group_globalOptions.add_option("--asimov", action="store_true", dest="asimov", default=False, help="only generate asimov toys. If this flag is activated input for '-n' is ignored and is set to 1")
group_globalOptions.add_option("-s", "--injectSignalStrength", dest = "signalStrengths", help="use this signal strength for toy generation", action = "append", type="float")
group_required.add_option("-d", "--datacard", dest="pathToDatacard", help="path to datacard with original MC templates", metavar="path/to/orignal/datacard")
group_required.add_option("-r", "--rootfile", dest="pathToRoofile", help="path to root file specified in the datacard for option '-d'", metavar = "path/to/root/file")
group_globalOptions.add_option("-p", "--additionalPOI", action="append", dest="POIs",
                    help="add an additional POI to the fit.\nSyntax: (PROCESSNAME):POINAME[INIT_VAL, LOWER_RANGE, UPPER_RANGE]\n In order to map multiple process to one POI, use\n(PROCESSNAME1|PROCESSNAME2|...):POINAME[INIT_VAL, LOWER_RANGE, UPPER_RANGE].\nUses combine physics model 'multiSignalModel'. DANGER! This requires an additional datacard of the form 'path/to/original/datacard_POINAME1_POINAME2_....txt'")
group_globalOptions.add_option("--toysFrequentist", dest="toysFrequentist", help = "generate frequentist toys", action="store_true", default = False)
group_scalingOptions.add_option("--scaledDatacard", dest="pathToScaledDatacard", help="use this datacard to throw toys from", metavar="path/to/toy/datacard")
group_scalingOptions.add_option("--scaleProcesses", dest="listOfProcesses", help="comma-separated list of processes to be scaled. Names have to match names in datacard", metavar="PROCESS1,PROCESS2,...")
group_scalingOptions.add_option("--scaleFuncs", dest = "listOfFormulae", help="comma-separated list of functions to scale processes with.\nBased on TF1 functionality. Requires same order as in option '--scaleProcesses'", metavar="FUNC1,FUNC2,...")
group_required.add_option("-c", "--config", dest = "config", default = "config.py", help="path to config.py file specifying lists of signal processes, background processes and keys for templates in root file", metavar="path/to/config")
parser.add_option("-v", "--verbose", dest="verbose", help="increase output", action="store_true", default=False)
parser.add_option_group(group_required)
parser.add_option_group(group_globalOptions)
parser.add_option_group(group_scalingOptions)

(options, args) = parser.parse_args()


if options.outputDirectory == None:
    parser.error("output directory has to be specified!")
if options.pathToDatacard == None:
    parser.error("Path to original MC template datacard has to be specified!")
if options.pathToRoofile == None:
    parser.error("Path to root file with MC templates has to be specified!")
if options.config == None:
    parser.error("Path to config file has to be specified!")
if options.pathToScaledDatacard and (options.listOfProcesses or options.listOfFormulae):
    parser.error("Cannot specify both path to datacard to throw toys from and list of processes to scale/ scaling functions!")
if (not options.listOfProcesses and options.listOfFormulae) or (options.listOfProcesses and not options.listOfFormulae):
    parser.error("Need to specify both processes to scale and corresponding scaling functions!")
if options.asimov and options.numberOfToys:
    print "WARNGING:\twill ignore input for option '-n' and throw one toy!"


verbose = options.verbose
asimov = options.asimov
listOfMus = options.signalStrengths
if listOfMus == None:
    listOfMus = [0.,1.]
for mu in listOfMus:
    print "generating toys for signal strength", mu
#set up parameters for toy generation here
numberOfToys = options.numberOfToys
numberOfToysPerJob = options.nPerJob
toyMode = 1 #controls how many toys per experiment are generated. Should be set to -1 for asimov toys
if options.asimov:
    toyMode = -1
    if verbose:
        print "Will only generate one asimov toy"
    numberOfToys = 1

pathToConfig = os.path.abspath(options.config)
if os.path.exists(pathToConfig) and os.path.basename(pathToConfig) == "config.py":
    sys.path.append(pathToConfig)
    import config
else:
    sys.exit("Unable to find config.py file in %s" % pathToConfig)

generatorScript = "generateToysAndFits.sh"
if options.toysFrequentist:
    generatorScript = "generateFrequentistToysAndFits.sh"

print "will use {0} to generate toys and perform fits".format(workdir + "/" + generatorScript)
>>>>>>> 81509c8b530b5f11e714f8f281f434aa1387a365

#--------------------------------------------------------------------------------------------------------------------------------------------
#global parameters

<<<<<<< HEAD
outputDirectory = sys.argv[1] #path to store PseudoExperiments in
print "input for outputDirectory:", outputDirectory
pathToDatacard = sys.argv[2] #path to unscaled datacard with data to be fitted to scaled toys
pathToInputRootfile = sys.argv[3] #path to corresponding root file

datacardOrProcessList = None
scaleFuncList = None
if len(sys.argv)>4:
    datacardOrProcessList = sys.argv[4] #either path to datacard with scaled data oder comma-separated list of Processes to be scaled
if len(sys.argv)>5:
    scaleFuncList = sys.argv[5] #if argument 3 is a list of processes this parameter is a comma-separated list of functions to use (TF1)

pathToScaledDatacard = None
=======
outputDirectory = options.outputDirectory #path to store PseudoExperiments in
print "input for outputDirectory:", outputDirectory
pathToDatacard = options.pathToDatacard #path to unscaled datacard with data to be fitted to scaled toys

pathToInputRootfile = options.pathToRoofile #path to corresponding root file

POImap = options.POIs


listOfProcessesString = options.listOfProcesses
scaleFuncList = options.listOfFormulae

pathToScaledDatacard = options.pathToScaledDatacard
>>>>>>> 81509c8b530b5f11e714f8f281f434aa1387a365

scalingDic = [] #2D list of form [(Process, Func to scale with),(...),...]


<<<<<<< HEAD
#check if third argument is a list of process or a datacard and act accordingly
if datacardOrProcessList is not None:
    if datacardOrProcessList.endswith(".txt"):
        pathToScaledDatacard = os.path.abspath(datacardOrProcessList)
        print "using already scaled data from", pathToScaledDatacard
    else:
        listOfProcesses = datacardOrProcessList.split(",")
        listOfFormulae = scaleFuncList.split(",")
        scalingDic = [entry for entry in zip(listOfProcesses, listOfFormulae)]
        print "using scaling dictionary:", scalingDic
=======
#check if 6th argument is a list of processes or a datacard and act accordingly
if listOfProcessesString and scaleFuncList:
    listOfProcesses = listOfProcessesString.split(",")
    listOfFormulae = scaleFuncList.split(",")
    scalingDic = [entry for entry in zip(listOfProcesses, listOfFormulae)]
    print "using scaling dictionary:", scalingDic
>>>>>>> 81509c8b530b5f11e714f8f281f434aa1387a365

#----------------------------------------------------------------------------------------------------------------------------------------------


<<<<<<< HEAD



def submitToNAF(pathToDatacard, datacardToUse, outputDirectory, numberOfToys, numberOfToysPerJob, toyMode, listOfPOIs):
    jobids=[]
    command=[workdir+"/submitCombineToyCommand.sh", pathToDatacard, datacardToUse, outputDirectory, str(numberOfToys), str(numberOfToysPerJob), str(toyMode), listOfPOIs]
=======
def submitArrayToNAF(scripts, arrayscriptpath):
    submitclock=ROOT.TStopwatch()
    submitclock.Start()
    logdir = os.getcwd()+"/logs"
    if os.path.exists(logdir):
        print "emptying directory", logdir
        shutil.rmtree(logdir)

    os.makedirs(logdir)

    # get nscripts
    nscripts=len(scripts)
    tasknumberstring='1-'+str(nscripts)

    #create arrayscript to be run on the birds. Depinding on $SGE_TASK_ID the script will call a different plot/run script to actually run

    arrayscriptcode="#!/bin/bash \n"
    arrayscriptcode+="subtasklist=(\n"
    for scr in scripts:
        arrayscriptcode+=scr+" \n"

    arrayscriptcode+=")\n"
    arrayscriptcode+="thescript=${subtasklist[$SGE_TASK_ID-1]}\n"
    arrayscriptcode+="thescriptbasename=`basename ${subtasklist[$SGE_TASK_ID-1]}`\n"
    arrayscriptcode+="echo \"${thescript}\n"
    arrayscriptcode+="echo \"${thescriptbasename}\n"
    arrayscriptcode+=". $thescript 1>>"+logdir+"/$JOB_ID.$SGE_TASK_ID.o 2>>"+logdir+"/$JOB_ID.$SGE_TASK_ID.e\n"
    arrayscriptfile=open(arrayscriptpath,"w")
    arrayscriptfile.write(arrayscriptcode)
    arrayscriptfile.close()
    st = os.stat(arrayscriptpath)
    os.chmod(arrayscriptpath, st.st_mode | stat.S_IEXEC)

    print 'submitting',arrayscriptpath
    #command=['qsub', '-cwd','-terse','-t',tasknumberstring,'-S', '/bin/bash','-l', 'h=bird*', '-hard','-l', 'os=sld6', '-l' ,'h_vmem=2000M', '-l', 's_vmem=2000M' ,'-o', logdir+'/dev/null', '-e', logdir+'/dev/null', arrayscriptpath]
    command=['qsub', '-cwd','-terse','-t',tasknumberstring,'-S', '/bin/bash','-l', 'h=bird*', '-hard','-l', 'os=sld6', '-l' ,'h_vmem=2000M', '-l', 's_vmem=2000M' ,'-o', '/dev/null', '-e', '/dev/null', arrayscriptpath]
    a = subprocess.Popen(command, stdout=subprocess.PIPE,stderr=subprocess.STDOUT,stdin=subprocess.PIPE)
    output = a.communicate()[0]
    jobidstring = output
    if len(jobidstring)<2:
        sys.exit("something did not work with submitting the array job")

    jobidstring=jobidstring.split(".")[0]
    print "the jobID", jobidstring
    jobidint=int(jobidstring)
    submittime=submitclock.RealTime()
    print "submitted job", jobidint, " in ", submittime
    return [jobidint]


def submitToNAF(pathToDatacard, datacardToUse, outputDirectory, numberOfToys, numberOfToysPerJob, toyMode, pathToMSworkspace):
    jobids=[]
    command=[workdir+"/submitCombineToyCommand.sh", pathToDatacard, datacardToUse, outputDirectory, str(numberOfToys), str(numberOfToysPerJob), str(toyMode), pathToMSworkspace]
    print command
>>>>>>> 81509c8b530b5f11e714f8f281f434aa1387a365
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

<<<<<<< HEAD
def do_qstat(jobids):
    allfinished=False
    while not allfinished:
        time.sleep(60)
=======

def submitArrayJob(pathToDatacard, datacardToUse, outputDirectory, numberOfToys, numberOfToysPerJob, toyMode, pathToMSworkspace, listOfMus):
    numberOfLoops = numberOfToys//numberOfToysPerJob
    rest = numberOfToys%numberOfToysPerJob
    commands = []
    if pathToMSworkspace is None or pathToMSworkspace == "":
        pathToMSworkspace = "\'\'\"\"\'\'"
    for signalStrength in listOfMus:
        if not outputDirectory.endswith("/"):
            outputDirectory = outputDirectory + "/"
        signalStrengthFolder = outputDirectory + "sig" + str(signalStrength)
        if os.path.exists(signalStrengthFolder):
            print "resetting folder", signalStrengthFolder
            shutil.rmtree(signalStrengthFolder)


        signalStrengthFolder = os.path.abspath(signalStrengthFolder)
        os.makedirs(signalStrengthFolder)

        if not asimov:
            if not os.path.exists(signalStrengthFolder + "/asimov"):
                os.makedirs(signalStrengthFolder + "/asimov")

            commands.append("\'" + workdir + "/"+ generatorScript +" " + pathToDatacard + " " + datacardToUse + " -1 " + str(signalStrength) + " 123456 " + pathToMSworkspace + " " + signalStrengthFolder + "/asimov\'")

        for i in range(numberOfLoops):
            upperBound = (i+1)*numberOfToysPerJob
            lowerBound = i*numberOfToysPerJob
            commands.append("\'" + workdir + "/createFoldersAndDoToyFits.sh " + pathToDatacard + " " + datacardToUse + " " + str(toyMode) + " " + str(signalStrength) + " " + str(lowerBound) + " " + str(upperBound) + " " + pathToMSworkspace + " " + signalStrengthFolder + "\'")

        commands.append("\'" + workdir + "/createFoldersAndDoToyFits.sh " + pathToDatacard + " " + datacardToUse + " " + str(toyMode) + " " + str(signalStrength) + " " + str(int(numberOfToys - rest)) + " " + str(numberOfToys) + " " + pathToMSworkspace + " " + signalStrengthFolder + "\'" )

    for command in commands:
        print command

    return submitArrayToNAF(commands, outputDirectory+"temp/arrayJobs.sh")


def do_qstat(jobids):
    allfinished=False
    while not allfinished:
        time.sleep(10)
>>>>>>> 81509c8b530b5f11e714f8f281f434aa1387a365
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

<<<<<<< HEAD
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
=======
def collectNorms(listOfNorms, histo, category):
    histoName = histo.GetName()

    if histoName in config.signalHistos[category] or histoName in config.backgroundHistos[category]:
        if verbose:
            print "saving norm for histogram", histo.GetName()
        histoCatKey = config.histoKey.replace("$CHANNEL", category)
        processName = histoName.replace(histoCatKey.replace("$PROCESS", ""), "")
        if verbose:
            print "found process name", processName

        indexCategory = -1
        integral = histo.Integral()
        for entry in range(len(listOfNorms)):
            if listOfNorms[entry][0] == category:
>>>>>>> 81509c8b530b5f11e714f8f281f434aa1387a365
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
<<<<<<< HEAD
            listOfNorms.append([categoryName,[[processName, integral]]])
=======
            listOfNorms.append([category,[[processName, integral]]])
>>>>>>> 81509c8b530b5f11e714f8f281f434aa1387a365
    #else:
        #print "skipping normalisation for histo", histo.GetName()

def saveListAsTree(listOfNormsPrescale, listOfNormsPostscale, outputFileName):
    outfile = ROOT.TFile(outputFileName, "RECREATE")
    print "number of collected categories:", len(listOfNormsPrescale), "\tpostscale:", len(listOfNormsPostscale)
    for category in range(len(listOfNormsPrescale)):
<<<<<<< HEAD
        print "Creating TTree for category", listOfNormsPrescale[category][0]

        tree = ROOT.TTree(listOfNormsPrescale[category][0], listOfNormsPrescale[category][0])
        print "\tsuccess"
=======
        if verbose:
            print "Creating TTree for category", listOfNormsPrescale[category][0]

        tree = ROOT.TTree(listOfNormsPrescale[category][0], listOfNormsPrescale[category][0])
        if verbose:
            print "\tsuccess"
>>>>>>> 81509c8b530b5f11e714f8f281f434aa1387a365
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
<<<<<<< HEAD
            #print "\tcreating branch for process", processName
            tree.Branch(processName+"_prescale", prescaleVals[process], "{0}_prescale[{1}]/D".format(processName, process))
            tree.Branch(processName+"_postscale", postscaleVals[process], "{0}_postscale[{1}]/D".format(processName, process))

            #print "\t\tsuccess"
            prescaleVals[process][0] = prescaleProcesses[1]
            postscaleVals[process][0] = postscaleProcesses[1]
            #print "\tfill branch", processName, "with value", ratios[process][0]
=======
            print "\tcreating branchs for process", processName
            tree.Branch(processName+"_prescale", prescaleVals[process], "{0}_prescale[{1}]/D".format(processName, process))
            tree.Branch(processName+"_postscale", postscaleVals[process], "{0}_postscale[{1}]/D".format(processName, process))

            print "\t\tsuccess"
            prescaleVals[process][0] = prescaleProcesses[1]
            postscaleVals[process][0] = postscaleProcesses[1]
>>>>>>> 81509c8b530b5f11e714f8f281f434aa1387a365

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

<<<<<<< HEAD
def scaleHistogram(key, inputRootFile, funcFormula, currentOutputDir, listOfNormsPrescale, listOfNormsPostscale):
=======
def scaleHistogram(key, category, inputRootFile, funcFormula, currentOutputDir, listOfNormsPrescale, listOfNormsPostscale):
>>>>>>> 81509c8b530b5f11e714f8f281f434aa1387a365
    histo = inputRootFile.Get(key.GetName())
    histo.SetDirectory(currentOutputDir)
    #for usage depending on x values:
    #lowerBound = histo.GetXaxis().GetBinLowerEdge(1)
    #upperBound = histo.GetXaxis().GetBinLowerEdge(histo.GetNbinsX()) + histo.GetXaxis().GetBinWidth(histo.GetNbinsX())/2
<<<<<<< HEAD
    collectNorms(listOfNormsPrescale, histo)
    print "\tScaling", key.GetName(),"with function", funcFormula
=======
    collectNorms(listOfNormsPrescale, histo, category)
    if verbose:
        print "\tScaling", key.GetName(),"with function", funcFormula
>>>>>>> 81509c8b530b5f11e714f8f281f434aa1387a365

    scaleFunc = ROOT.TF1("scaleFunc",funcFormula,0, histo.GetNbinsX() )
    if scaleFunc is not None:
        for currentBin in range(1, histo.GetNbinsX()+1):
            #print "\t\tBefore scaling bin", currentBin,":\t", histo.GetBinContent(currentBin)
            scaleFactor = scaleFunc.Eval(currentBin)
            histo.SetBinContent(currentBin, histo.GetBinContent(currentBin)*scaleFactor)
            #print "\t\tAfter scaling bin", currentBin,":\t", histo.GetBinContent(currentBin)
<<<<<<< HEAD
    collectNorms(listOfNormsPostscale, histo)
    return histo

def copyOrScaleElements(inputRootFile, outputFile, processScalingDic, listOfKeys, listOfNormsPrescale, listOfNormsPostscale):
    tempObject = None
=======
    collectNorms(listOfNormsPostscale, histo, category)
    return histo


def saveHistos(category, key, processScalingDic, inputRootFile, outputFile, path, listOfNormsPrescale, listOfNormsPostscale, data_obs, bkg=False):
    index = -1
    for entry in range(len(processScalingDic)):
        if key.GetName().startswith(processScalingDic[entry][0]+"_"):
            if verbose:
                print "Found match for process #{0}: {1}".format(entry, processScalingDic[entry][0])
            index = entry
    if index is not -1:
        if verbose:
            print "found match at index", index

        tempObject = scaleHistogram(key, category, inputRootFile, processScalingDic[index][1], outputFile.GetDirectory(path), listOfNormsPrescale, listOfNormsPostscale)
    else:
        tempObject = inputRootFile.Get(key.GetName())
        collectNorms(listOfNormsPrescale, tempObject, category)
        collectNorms(listOfNormsPostscale, tempObject, category)

        tempObject.SetDirectory(outputFile.GetDirectory(path))
        if verbose:
            print "\tCopied", key.GetName(),"to new root file"
    if tempObject is not None:
        if verbose:
            checkCopy(inputRootFile.Get(key.GetName()), tempObject)
        if bkg:
            data_obs[category].Add(tempObject)
        tempObject.Write()


def copyOrScaleElements(inputRootFile, outputFile, processScalingDic, listOfKeys, listOfNormsPrescale, listOfNormsPostscale):
    tempObject = None
    data_obs = {}

>>>>>>> 81509c8b530b5f11e714f8f281f434aa1387a365
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
<<<<<<< HEAD
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
    print "\tdone with copying/scaling"

=======
            keyName = key.GetName()
            saved=False
            for cat in config.categories:
                if not saved:
                    if not cat in data_obs:
                        histoCatKey = config.histoKey.replace("$CHANNEL", cat)
                        data_obs_key = histoCatKey.replace("$PROCESS", "data_obs")
                        temp = inputRootFile.Get(data_obs_key)
                        if not isinstance(temp, ROOT.TH1):
                            sys.exit("Unable to load data_obs with key %s! Aborting" % data_obs_key)

                        data_obs[cat] = temp.Clone()
                        data_obs[cat].SetDirectory(outputFile.GetDirectory(path))
                        data_obs[cat].Reset()

                    for signalHistoName in config.signalHistos[cat]:
                        if keyName.startswith(signalHistoName):
                            saveHistos(cat, key, processScalingDic, inputRootFile, outputFile, path, listOfNormsPrescale, listOfNormsPostscale, data_obs)
                            saved = True
                            break
                    if not saved:
                        for backgroundHistoName in config.backgroundHistos[cat]:
                            if keyName.startswith(backgroundHistoName):
                                saveHistos(cat, key, processScalingDic, inputRootFile, outputFile, path, listOfNormsPrescale, listOfNormsPostscale, data_obs, (keyName == backgroundHistoName))
                                saved = True
                                break
                else:
                    break
    for cat in data_obs:
        data_obs[cat].Write()
    print "\tdone with copying/scaling"

def checkForMSworkspace(pathToDatacard, POImap):
    returnString = ""
    PathToMSDatacard = pathToDatacard
    msworkspacePath = ""
    if not( POImap is None or POImap == ""):
        for mapping in POImap:
            containsPOIname = mapping.split(":")[-1]
            POIname = containsPOIname.split("[")[0]
            PathToMSDatacard = PathToMSDatacard.replace(".txt", "_"+POIname+".txt")
        msworkspacePath = PathToMSDatacard.replace(".txt","_multisig.root")
        if os.path.exists(PathToMSDatacard):
            if not os.path.exists(msworkspacePath):
                bashCmd = "source {0}/setupCMSSW.txt ;".format(workdir)
                bashCmd = bashCmd + "text2workspace.py -P HiggsAnalysis.CombinedLimit.PhysicsModel:multiSignalModel --PO verbose  --PO \'map=.*/(ttH_*):r[1,-10,10]\'"
                for mapping in POImap:
                    bashCmd = bashCmd + " --PO \'map=.*/" + mapping + "\'"
                bashCmd = bashCmd + " {0} -o {1}".format(PathToMSDatacard, msworkspacePath)
                print bashCmd
                os.system(bashCmd)

        else:
            print "could not find datacard for multisig workspace in", PathToMSDatacard

    if os.path.exists(msworkspacePath):
        returnString = msworkspacePath

    return returnString

>>>>>>> 81509c8b530b5f11e714f8f281f434aa1387a365
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
<<<<<<< HEAD

        listOfPOIs = "r"
        for processPair in processScalingDic:
            listOfPOIs = listOfPOIs+","+POIsuffix+processPair[0]
        jobids = submitToNAF(pathToDatacard, datacardToUse, outputDirectory, numberOfToys, numberOfToysPerJob, toyMode, listOfPOIs)
        print "waiting for toy generation to finish"
        do_qstat(jobids)
        os.chdir(workdir)
        print "calling plotResults with arguments:"
        print "\toutputPath =", outputPath
        print "\tshapeExpectation =", shapeExpectation
        subprocess.check_call([workdir+"/submitScript.sh", outputPath, shapeExpectation])
=======
        pathToMSworkspace = checkForMSworkspace(pathToDatacard, POImap)
        jobids = submitArrayJob(pathToDatacard, datacardToUse, outputDirectory, numberOfToys, numberOfToysPerJob, toyMode, pathToMSworkspace, listOfMus = listOfMus)
        print "waiting for toy generation to finish"
        # do_qstat(jobids)
        # os.chdir(workdir)
        # print "calling plotResults with arguments:"
        # print "\toutputPath =", outputPath
        # print "\tshapeExpectation =", shapeExpectation
        # subprocess.check_call([workdir+"/submitScript.sh", outputPath, shapeExpectation])
>>>>>>> 81509c8b530b5f11e714f8f281f434aa1387a365

    else:
        print "Couldn't find datacard", datacardToUse

<<<<<<< HEAD
=======
def skipParameter(param):
    if not config.ignoreUncertainties == None:
        for np in config.ignoreUncertainties:
            if np.startswith("*") and np.endswith("*"):
                if np.replace("*", "") in entries[0]:
                    return True
            elif np.startswith("*"):
                if param.endswith(np.replace("*","")):
                    return True
            elif np.endswith("*"):
                if param.startswith(np.replace("*","")):
                    return True
            else:
                if np == param:
                    return True
    return False


>>>>>>> 81509c8b530b5f11e714f8f281f434aa1387a365
def writeDatacard(pathToDatacard, newRootFileName, listOfProcesses):
    print "creating new datacard from input", pathToDatacard
    datacard = open(pathToDatacard)
    newDatacardName = "temp_datacard.txt"
    newDatacard = open(newDatacardName, "w")
    listOfEntriesToChange = []
    print listOfProcesses
    for line in datacard:
        entries = line.split()
<<<<<<< HEAD
        for i, entry in enumerate(entries):
            #print "\t", i, "\t", entry

=======
        if entries[0].startswith("#"): #skip lines that start with '#', as those are not considered in combine anyway
            continue
        if skipParameter(entries[0]): #skip line if parameter is to be ignored (as per definition in config file)
            print "skipping line that starts with", entries[0]
            continue

        for i, entry in enumerate(entries):
            #print "\t", i, "\t", entry
            if entries[0] == "observation" and not i == 0:
                entry = "-1"
>>>>>>> 81509c8b530b5f11e714f8f281f434aa1387a365
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
    print "\tdone"
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
