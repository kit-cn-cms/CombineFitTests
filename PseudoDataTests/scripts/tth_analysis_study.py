import ROOT
import sys
import os
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
pathToCMSSWsetup="/nfs/dust/cms/user/pkeicher/tth_analysis_study/CombineFitTests/PseudoDataTests/scripts/setupCMSSW.txt"


parser = OptionParser()
group_required = OptionGroup(parser, "Required Options")
group_globalOptions = OptionGroup(parser, "Options Valid for All Option Groups")
group_scalingOptions = OptionGroup(parser, "Scaling Related Options")
group_required.add_option("-o", "--outputDirectory",
dest="outputDirectory",
help="save signal strength folders with PseudoExperiments here",
metavar = "PATH"
)
group_globalOptions.add_option(
"-n", "--numberOfToys",
dest="numberOfToys",
help="generate this many toys per signal strength (default = 1000)",
default = 1000,
type="int"
)
group_globalOptions.add_option("--nToysPerJob",
dest = "nPerJob",
help="process this many toys at once on bird system (default = 30)",
default = 30 ,
type="int"
)
group_globalOptions.add_option("--asimov",
action="store_true",
dest="asimov",
default=False,
help="only generate asimov toys. If this flag is activated input for '-n' is ignored and is set to 1"
)
group_globalOptions.add_option("-s", "--injectSignalStrength",
dest = "signalStrengths",
help="use this signal strength for toy generation",
action = "append",
type="float"
)
group_required.add_option("-d", "--datacard",
dest="pathToDatacard",
help="path to datacard with original MC templates",
metavar="path/to/orignal/datacard"
)
group_required.add_option("-r", "--rootfile",
dest="pathToRoofile",
help="path to root file specified in the datacard for option '-d'",
metavar = "path/to/root/file"
)
group_globalOptions.add_option("-p", "--additionalPOI",
action="append",
dest="POIs",
help="add an additional POI to the fit.\nSyntax: (PROCESSNAME):POINAME[INIT_VAL, LOWER_RANGE, UPPER_RANGE]\n In order to map multiple process to one POI, use\n(PROCESSNAME1|PROCESSNAME2|...):POINAME[INIT_VAL, LOWER_RANGE, UPPER_RANGE].\nUses combine physics model 'multiSignalModel'. DANGER! This requires an additional datacard of the form 'path/to/original/datacard_POINAME1_POINAME2_....txt'"
)
group_globalOptions.add_option("--addToyCommand",
dest="additionalToyCmds",
help = "add combine command for toy generation (-M GenerateOnly)(can be used multiple times)",
action="append",
)
group_globalOptions.add_option("--addFitCommand",
dest="additionalFitCmds",
help = "add combine command for fit (-M MaxLikelihoodFit)(can be used multiple times)",
action="append",
)
group_scalingOptions.add_option("--scaledDatacard",
dest="pathToScaledDatacard",
help="use this datacard to throw toys from",
metavar="path/to/toy/datacard"
)
group_scalingOptions.add_option("--scaleProcesses",
dest="listOfProcesses",
help="comma-separated list of processes to be scaled. Names have to match names in datacard",
metavar="PROCESS1,PROCESS2,..."
)
group_scalingOptions.add_option("--scaleFuncs",
dest = "listOfFormulae",
help="comma-separated list of functions to scale processes with.\nBased on TF1 functionality. Requires same order as in option '--scaleProcesses'",
metavar="FUNC1,FUNC2,..."
)
group_required.add_option("-c", "--config",
dest = "config",
#default = "config.py",
help="path to config.py file specifying lists of signal processes, background processes and keys for templates in root file",
metavar="path/to/config")

parser.add_option("-v", "--verbose",
dest="verbose",
help="increase output",
action="store_true",
default=False
)
group_globalOptions.add_option("-f", "--noFolderReset",
dest = "folderReset",
help = "reset only signal strength folders, not entire folder for this pseudo experiment (default = false)",
action = "store_true",
default = False)
parser.add_option_group(group_required)
parser.add_option_group(group_globalOptions)
parser.add_option_group(group_scalingOptions)

(options, args) = parser.parse_args()


if options.outputDirectory == None:
    parser.error("output directory has to be specified!")
if options.pathToDatacard == None:
    parser.error("Path to original MC template datacard has to be specified!")
if options.pathToScaledDatacard and (options.listOfProcesses or options.listOfFormulae):
    parser.error("Cannot specify both path to datacard to throw toys from and list of processes to scale/ scaling functions!")
if (not options.listOfProcesses and options.listOfFormulae) or (options.listOfProcesses and not options.listOfFormulae):
    parser.error("Need to specify both processes to scale and corresponding scaling functions!")
elif options.listOfProcesses and options.listOfFormulae:
    if options.pathToRoofile == None:
        parser.error("Path to root file with MC templates has to be specified - cannot scale without source file!")
    elif not os.path.exists(os.path.abspath(options.pathToRoofile)):
        parser.error("Could not find file {0}, aborting".format(os.path.abspath(options.pathToRoofile)))
    elif options.config == None:
        parser.error("Path to config file needs to be specified!")
    elif not os.path.exists(os.path.abspath(options.config)):
        parser.error("Unable to find config.py file in {0}".format(os.path.abspath(options.config)))

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

if options.config is not None:
    pathToConfig = os.path.abspath(options.config)
    print "checking config file in", pathToConfig
    if os.path.exists(pathToConfig):
        sys.path.append(pathToConfig)
        import config
    else:
        parser.error("Unable to find config.py file in {0}".format(pathToConfig))


additionalToyCmds = options.additionalToyCmds
additionalFitCmds = options.additionalFitCmds

resetFolders = not options.folderReset
#--------------------------------------------------------------------------------------------------------------------------------------------
#global parameters

outputDirectory = options.outputDirectory #path to store PseudoExperiments in
print "input for outputDirectory:", outputDirectory
pathToDatacard = options.pathToDatacard #path to unscaled datacard with data to be fitted to scaled toys

pathToInputRootfile = options.pathToRoofile #path to corresponding root file

POImap = options.POIs


listOfProcessesString = options.listOfProcesses
scaleFuncList = options.listOfFormulae

pathToScaledDatacard = options.pathToScaledDatacard
if pathToScaledDatacard is not None:
    pathToScaledDatacard = os.path.abspath(pathToScaledDatacard)

scalingDic = [] #2D list of form [(Process, Func to scale with),(...),...]


#check if 6th argument is a list of processes or a datacard and act accordingly
if listOfProcessesString and scaleFuncList:
    listOfProcesses = listOfProcessesString.split(",")
    listOfFormulae = scaleFuncList.split(",")
    assert len(listOfProcesses) == len(listOfFormulae), "# of processes does not match # of formulae!"
    scalingDic = [entry for entry in zip(listOfProcesses, listOfFormulae)]
    print "using scaling dictionary:", scalingDic

#----------------------------------------------------------------------------------------------------------------------------------------------

def generateShellScript(targetDatacard, toyDatacard, numberOfToysPerExperiment,
pathToMSworkspace, additionalToyCmds, additionalFitCmds):
    """
    generate bash script to generate toys and perform maximum likelihood fits.

    Command for toy generation:
    combine -M GenerateOnly -m 125 --saveToys -t $numberOfToysPerExperiment -n _$((numberOfToysPerExperiment))toys_sig$signalStrength --expectSignal $signalStrength -s $((randomseed)) $toyDatacard

    Command for MaxLikelihoodFit (both for simple fit and multi signal model):
    combine -M MaxLikelihoodFit -m 125 --minimizerStrategy 0 --minimizerTolerance 0.001 --saveNormalizations --saveShapes --rMin=-10.00 --rMax=10.00 -t $numberOfToysPerExperiment --toysFile $toyFile --minos all $targetDatacard

    Keyword arguments:

    targetDatacard              --  path to datacard to use for performing MaxLikelihoodFit
    toyDatacard                 --  path to datacard to use for toy generation
    numberOfToysPerExperiment   --  number of toys to throw per pseudo experiment
    pathToMSworkspace           --  path to multi signal work space
    additionalToyCmds           --  list of additional combine commands to use for toy generation
    additionalFitCmds           --  list of additional combine command to use for MaxLikelihoodFit
    """
    #create combine command for toy generation
    generateToysCmd = "combine -M GenerateOnly -m 125 "
    generateToysCmd += "--saveToys -t $numberOfToysPerExperiment "
    generateToysCmd += "-n _$((numberOfToysPerExperiment))toys_sig$signalStrength "
    generateToysCmd += "--expectSignal $signalStrength -s $((randomseed)) "
    if additionalToyCmds is not None:
        for cmd in additionalToyCmds:
            generateToysCmd += cmd + " "

    print "will use this command for toy generation:\n", generateToysCmd
    #create combine MaxLikelihoodFit command

    mlfitCmd = "combine -M MaxLikelihoodFit "
    mlfitCmd += "-m 125 --minimizerStrategy 0 --minimizerTolerance 0.001 "
    mlfitCmd += "--saveNormalizations --saveShapes --rMin=-10.00 --rMax=10.00 "
    mlfitCmd += "-t $numberOfToysPerExperiment --toysFile $toyFile --minos all "
    if additionalFitCmds is not None:
        for cmd in additionalFitCmds:
            mlfitCmd += cmd + " "

    print "will use this command for fit:\n", mlfitCmd

    mswExists = pathToMSworkspace is not None and not pathToMSworkspace == ""
    shellscript = []
    shellscript.append('#!/bin/bash')
    shellscript.append('pathToCMSSWsetup='+pathToCMSSWsetup)
    shellscript.append('if [[ -f "$pathToCMSSWsetup" ]]; then\n')

    shellscript.append('\teval "source $pathToCMSSWsetup"')
    shellscript.append('\ttargetDatacard='+targetDatacard)
    shellscript.append('\ttoyDatacard='+toyDatacard)
    if mswExists:
        shellscript.append('\tpathToMSworkspace=' + pathToMSworkspace)

    shellscript.append('\tsignalStrength=$1')
    shellscript.append('\trandomseed=$2')
    shellscript.append('\toutputPath=$3')
    shellscript.append('\tnumberOfToysPerExperiment=$4\n')

    shellscript.append('#___________________________________________________')
    shellscript.append('\techo "input variables:"')
    shellscript.append('\techo "targetDatacard = $targetDatacard"')
    shellscript.append('\techo "toyDatacard = $toyDatacard"')
    shellscript.append('\techo "#Toys/Experiment = $numberOfToysPerExperiment"')
    shellscript.append('\techo "mu = $signalStrength"')
    shellscript.append('\techo "randomseed = $randomseed"')
    shellscript.append('\techo "pathToMSworkspace = $pathToMSworkspace"')
    shellscript.append('\techo "outputPath = $outputPath"\n')

    shellscript.append('\techo "changing directory to $outputPath"')
    shellscript.append('\tif [[ -d "$outputPath" ]]; then')
    shellscript.append('\t\tcd $outputPath\n')

    shellscript.append('\t\tif [[ -f $toyDatacard ]]; then')

    shellscript.append('\t\t\tcombineCmd="' + generateToysCmd + '$toyDatacard"')
    shellscript.append('\t\t\techo "$combineCmd"')
    shellscript.append('\t\t\teval $combineCmd\n')
    shellscript.append('\t\t\tif [[ -f *.root.dot ]]; then')
    shellscript.append('\t\t\t\trm *.root.dot')
    shellscript.append('\t\t\tfi\n')

    shellscript.append('\t\t\ttoyFile="higgsCombine_$((numberOfToysPerExperiment))toys_sig$signalStrength.GenerateOnly.mH125.$((randomseed)).root"')
    shellscript.append('\t\t\techo "$toyFile"')

    shellscript.append('\t\t\tif [[ -f $toyFile ]]; then')

    shellscript.append('\t\t\t\tcombineCmd="' + mlfitCmd + '$targetDatacard"')
    shellscript.append('\t\t\t\techo "$combineCmd"')
    shellscript.append('\t\t\t\teval $combineCmd\n')

    shellscript.append('\t\t\t\tif [[ -f "higgsCombineTest.MaxLikelihoodFit.mH125.123456.root" ]]; then')
    shellscript.append('\t\t\t\t\trm "higgsCombineTest.MaxLikelihoodFit.mH125.123456.root"')
    shellscript.append('\t\t\t\tfi\n')

    shellscript.append('\t\t\t\tif ! [[ -f "mlfit.root" ]]; then')
    shellscript.append('\t\t\t\t\techo "could not produce mlfit.root file!"')
    shellscript.append('\t\t\t\tfi')

    if mswExists:
        shellscript.append('\t\t\t\tif [[ -f $pathToMSworkspace ]]; then')
        shellscript.append('\t\t\t\t\techo "starting multiSignal analysis"')

        shellscript.append('\t\t\t\t\tcombineCmd="' + mlfitCmd + '-n _MS_mlfit $pathToMSworkspace"')
        shellscript.append('\t\t\t\t\techo "$combineCmd"')
        shellscript.append('\t\t\t\t\teval $combineCmd\n')

        shellscript.append('\t\t\t\t\tif [[ -f "higgsCombine_MS_mlfit.MaxLikelihoodFit.mH125.123456.root" ]]; then')
        shellscript.append('\t\t\t\t\t\trm "higgsCombine_MS_mlfit.MaxLikelihoodFit.mH125.123456.root"')
        shellscript.append('\t\t\t\t\tfi\n')

        shellscript.append('\t\t\t\t\tif ! [[ -f "mlfit_MS_mlfit.root" ]]; then')
        shellscript.append('\t\t\t\t\t\techo "could not produce mlfit_MS_mlfit.root file!"')
        shellscript.append('\t\t\t\t\tfi')

        shellscript.append('\t\t\t\tfi\n')

    shellscript.append('\t\t\telse')
    shellscript.append('\t\t\t\techo "Could not find toyFile, skipping the fit"')
    shellscript.append('\t\t\tfi\n')

    shellscript.append('\t\telse')
    shellscript.append('\t\t\techo "Could not find datacard for toy generation! Aborting..."')
    shellscript.append('\t\tfi\n')
    shellscript.append('\telse')
    shellscript.append('\t\techo "$outputPath is not a directory! Aborting"')
    shellscript.append('\tfi\n')
    shellscript.append('else')
    shellscript.append('\techo "Could not find file to setup CMSSW! Aborting"')
    shellscript.append('fi')

    return shellscript

def generateFolderGeneratorScript(generatorScriptPath, toyMode):
    shellscript = []
    shellscript.append('#!/bin/bash\n')

    shellscript.append('signalStrength=$1')
    shellscript.append('lowerBound=$2')
    shellscript.append('upperBound=$3')
    shellscript.append('outputPath=$4')
    shellscript.append('numberOfToysPerExperiment='+str(toyMode)+'\n')

    shellscript.append('if [[ -d $outputPath ]]; then')
    shellscript.append('\tcd $outputPath\n')

    shellscript.append('\techo "starting PseudoExperiment generation"')
    shellscript.append('\tfor (( i = $((lowerBound+1)); i <= $upperBound; i++ )); do')
    shellscript.append('\t\tmkdir -p PseudoExperiment$i')
    shellscript.append('\t\tif [[ -d PseudoExperiment$i ]]; then')
    shellscript.append('\t\t\tcd PseudoExperiment$i\n')

    shellscript.append('\t\t\teval "' + generatorScriptPath + ' $signalStrength $i $outputPath/PseudoExperiment$i $numberOfToysPerExperiment"\n')

    shellscript.append('\t\t\tcd ../\n')
    shellscript.append('\t\telse')
    shellscript.append('\t\t\t echo "Could not generate folder PseudoExperiment$i in $outputPath"')
    shellscript.append('\t\tfi\n')
    shellscript.append('\tdone')
    shellscript.append('else')
    shellscript.append('\techo "$outputPath is not a directory! Aborting"')
    shellscript.append('fi')
    return shellscript

def submitArrayToNAF(scripts, arrayscriptpath):
    """
    generate bash array with scripts from list of scripts and submit it to bird system. Function will create a folder to save log files

    Keyword arguments:

    scripts         -- list of scripts to be submitted
    arrayscriptpath -- path to safe script array in
    """
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

def submitArrayJob(pathToDatacard, datacardToUse, outputDirectory, numberOfToys, numberOfToysPerJob, toyMode, pathToMSworkspace, listOfMus):

    generatorScript = os.path.abspath(outputDirectory + "/temp/generateToysAndFits.sh")
    folderGeneratorScript = os.path.abspath(outputDirectory + "/temp/generateFolders.sh")

    genScript = open(generatorScript, "w")
    genScript.write("\n".join(generateShellScript(
    targetDatacard = pathToDatacard,
    toyDatacard = datacardToUse,
    numberOfToysPerExperiment = toyMode,
    pathToMSworkspace = pathToMSworkspace,
    additionalFitCmds = additionalFitCmds,
    additionalToyCmds = additionalToyCmds ) ) )
    genScript.close()

    if os.path.exists(generatorScript):

        subprocess.call("chmod 755 "+generatorScript, shell = True)
        genFolderScript = open(folderGeneratorScript,"w")
        genFolderScript.write("\n".join(generateFolderGeneratorScript(generatorScript, toyMode = toyMode)))
        genFolderScript.close()

        if os.path.exists(folderGeneratorScript):

            subprocess.call("chmod 755 "+folderGeneratorScript, shell = True)

            numberOfLoops = numberOfToys//numberOfToysPerJob
            rest = numberOfToys%numberOfToysPerJob
            commands = []

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

                    commands.append("\'" + generatorScript +" " + str(signalStrength) + " 123456 " + signalStrengthFolder + "/asimov -1\'")

                for i in range(numberOfLoops):
                    upperBound = (i+1)*numberOfToysPerJob
                    lowerBound = i*numberOfToysPerJob
                    commands.append("\'" + folderGeneratorScript + " " + str(signalStrength) + " " + str(lowerBound) + " " + str(upperBound) + " " + signalStrengthFolder + "\'")

                commands.append("\'" + folderGeneratorScript + " " + str(signalStrength) + " " + str(int(numberOfToys - rest)) + " " + str(numberOfToys) + " " + signalStrengthFolder + "\'")

            for command in commands:
                print command

            return submitArrayToNAF(commands, outputDirectory+"temp/arrayJobs.sh")
        else:
            print "Could not write folder generator!"
    else:
        print "Could not write toy and fit generator!"

def do_qstat(jobids):
    allfinished=False
    while not allfinished:
        time.sleep(10)
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
            listOfNorms.append([category,[[processName, integral]]])
    #else:
        #print "skipping normalisation for histo", histo.GetName()

def saveListAsTree(listOfNormsPrescale, listOfNormsPostscale, outputFileName):
    outfile = ROOT.TFile(outputFileName, "RECREATE")
    print "number of collected categories:", len(listOfNormsPrescale), "\tpostscale:", len(listOfNormsPostscale)
    for category in range(len(listOfNormsPrescale)):
        if verbose:
            print "Creating TTree for category", listOfNormsPrescale[category][0]

        tree = ROOT.TTree(listOfNormsPrescale[category][0], listOfNormsPrescale[category][0])
        if verbose:
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
            print "\tcreating branchs for process", processName
            #tree.Branch(processName+"_prescale", prescaleVals[process], "{0}_prescale[{1}]/D".format(processName, process))
            tree.Branch(processName+"_prescale", prescaleVals[process], "{0}_prescale[1]/D".format(processName))
            #tree.Branch(processName+"_postscale", postscaleVals[process], "{0}_postscale[{1}]/D".format(processName, process))
            tree.Branch(processName+"_postscale", postscaleVals[process], "{0}_postscale[1]/D".format(processName))

            print "\t\tsuccess"
            prescaleVals[process][0] = prescaleProcesses[1]
            postscaleVals[process][0] = postscaleProcesses[1]

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

def scaleHistogram(key, category, inputRootFile, funcFormula, currentOutputDir, listOfNormsPrescale, listOfNormsPostscale):
    histo = inputRootFile.Get(key.GetName())
    histo.SetDirectory(currentOutputDir)
    #for usage depending on x values:
    #lowerBound = histo.GetXaxis().GetBinLowerEdge(1)
    #upperBound = histo.GetXaxis().GetBinLowerEdge(histo.GetNbinsX()) + histo.GetXaxis().GetBinWidth(histo.GetNbinsX())/2
    collectNorms(listOfNormsPrescale, histo, category)
    if verbose:
        print "\tScaling", key.GetName(),"with function", funcFormula

    scaleFunc = ROOT.TF1("scaleFunc",funcFormula,0, histo.GetNbinsX() )
    if scaleFunc is not None:
        for currentBin in range(1, histo.GetNbinsX()+1):
            #print "\t\tBefore scaling bin", currentBin,":\t", histo.GetBinContent(currentBin)
            scaleFactor = scaleFunc.Eval(currentBin)
            histo.SetBinContent(currentBin, histo.GetBinContent(currentBin)*scaleFactor)
            #print "\t\tAfter scaling bin", currentBin,":\t", histo.GetBinContent(currentBin)
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

def generateToysAndFit(inputRootFile, processScalingDic, pathToScaledDatacard, outputPath):
    print "outputPath in generateToysAndFit:", outputPath
    if not os.path.exists("temp"):
        os.makedirs("temp")
    os.chdir("temp")
    listOfNormsPrescale = []
    listOfNormsPostscale = []
    shapeExpectation = ""
    if len(processScalingDic) is not 0:
        if inputRootFile == None:
            sys.exit("Tried to scale processes but no input root file is open! Aborting...")
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
        pathToMSworkspace = checkForMSworkspace(pathToDatacard, POImap)
        jobids = submitArrayJob(pathToDatacard, datacardToUse, outputDirectory, numberOfToys, numberOfToysPerJob, toyMode, pathToMSworkspace, listOfMus = listOfMus)
        print "waiting for toy generation to finish"
        # do_qstat(jobids)
        # os.chdir(workdir)
        # print "calling plotResults with arguments:"
        # print "\toutputPath =", outputPath
        # print "\tshapeExpectation =", shapeExpectation
        # subprocess.check_call([workdir+"/submitScript.sh", outputPath, shapeExpectation])

    else:
        print "Couldn't find datacard", datacardToUse

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

def writeDatacard(pathToDatacard, newRootFileName, listOfProcesses):
    print "creating new datacard from input", pathToDatacard
    datacard = open(pathToDatacard)
    newDatacardName = "temp_datacard.txt"
    newDatacard = open(newDatacardName, "w")
    listOfEntriesToChange = []
    print listOfProcesses
    for line in datacard:
        entries = line.split()
        if entries[0].startswith("#"): #skip lines that start with '#', as those are not considered in combine anyway
            continue
        if skipParameter(entries[0]): #skip line if parameter is to be ignored (as per definition in config file)
            print "skipping line that starts with", entries[0]
            continue

        for i, entry in enumerate(entries):
            #print "\t", i, "\t", entry
            if entries[0] == "observation" and not i == 0:
                entry = "-1"
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


if os.path.exists(pathToDatacard):
    pathToDatacard=os.path.abspath(pathToDatacard)
    inputRootFile = None
    if pathToInputRootfile is not None:
        pathToInputRootfile = os.path.abspath(pathToInputRootfile)
        inputRootFile = ROOT.TFile(pathToInputRootfile, "READ")
    outputDirectory = os.path.abspath(outputDirectory)
    if os.path.exists(outputDirectory) and resetFolders:
        print "resetting folder", outputDirectory
        shutil.rmtree(outputDirectory)

    if not os.path.exists(outputDirectory):
        os.makedirs(outputDirectory)
    #print "outputDirectory after directory was created:", outputDirectory
    #print "outputDir after calling abspath:", outputDirectory
    os.chdir(outputDirectory)
    #print "outputDirectory after changing dirs:", outputDirectory
    generateToysAndFit(inputRootFile, scalingDic, pathToScaledDatacard, outputDirectory)
else:
    print "Incorrect paths to input datacard. Please make sure path to datacard is correct!"
