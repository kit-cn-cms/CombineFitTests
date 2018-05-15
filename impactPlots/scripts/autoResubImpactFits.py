from ROOT import TFile
import os
import sys
import glob
import subprocess
import json
sys.path.append(os.path.join(os.path.dirname(sys.path[0]),'base'))
scriptDir = os.path.join(os.path.dirname(sys.path[0]),'scripts')
from batchConfig import *
batch_config = batchConfig()
#========input variables================================================

listOfDatacards = None
listOfResubFolders = None
additionalSubmitCmds 	= None
useBatch = True

if len(sys.argv) == 2:
    pathToJson = sys.argv[1]
    pathToJson = os.path.abspath(pathToJson)
    if os.path.exists(pathToJson) and pathToJson.endswith(".json"):
		print "loading impact submit infos from", pathToJson
		with open(pathToJson) as f:
		    dic = json.load(f)
		listOfDatacards = dic["datacards"]
		listOfResubFolders = dic["impact_folders"]
		additionalSubmitCmds = dic["commands"]
    else:
		sys.exit("Input has to be a .json file!")
else:
    resubWildcard 		= sys.argv[1]
    datacardWildcard 	= sys.argv[2]
    additionalSubmitCmds 	= None
    if len(sys.argv) > 3:
		additionalSubmitCmds= sys.argv[3:]
    
    listOfDatacards = glob.glob(datacardWildcard)
    listOfResubFolders = glob.glob(resubWildcard)


# pathToConfig = scriptDir + "/batch_config.py"
# pathToConfig = os.path.abspath(pathToConfig)
# print "loading config from", pathToConfig

# config = imp.load_source('config', pathToConfig)

#=======================================================================

#========functions======================================================

def check_for_resubmit(folder):
    if not os.path.exists(folder):
	return True
    print "cd into", folder
    os.chdir(folder)
    toResub = []
    scriptList = glob.glob("*.sh")
    if not os.path.exists("commands.txt"):
	print "unable to find commands.txt in", folder
	return True
    with open("commands.txt") as infile:
	lines = infile.read().splitlines()
	if not len(scriptList) == 0:
	    rootfiles = glob.glob("higgsCombine_paramFit*.root")
	    print "# rootfiles:", len(rootfiles)
	    print "# scripts:", len(scriptList)
	    initFitList = glob.glob("higgsCombine_initialFit_*.root")
	    redoInitFit = False
	    if initFitList:
			initFitFile = initFitList[0]
			infile = TFile(initFitFile)
			if not (infile.IsOpen() and not infile.IsZombie() and not infile.TestBit(TFile.kRecovered)):
			    redoInitFit = True
			else:
			    print "init file is working"
	    else:
		print "could not find initial fit file!"
		return True
	    
	    if redoInitFit:
			cmd = lines[1]
			print cmd
			subprocess.call([cmd], shell = True)
	    
	    if (not len(scriptList) == len(rootfiles) and not batch_config.jobmode == "condor") or redoInitFit:
		    cmd = lines[-1]
		    print cmd
		    subprocess.call([cmd], shell = True)
	    else:
	    	if not batch_config.jobmode == "conodor":
				scripts = []
				for path in rootfiles:
				    infile = TFile(path)
				    if not (infile.IsOpen() and not infile.IsZombie() and not infile.TestBit(TFile.kRecovered)):
						print "looking for script for file", path
						keywords = path.replace("higgsCombine", "").split(".")
						for script in scriptList:
						    script = os.path.abspath(script)
						    s = open(script)
						    lines = s.read().splitlines()
						    s.close()
						    if keywords[0] in lines[-1]:
								scripts.append(script)
								break
				
		if len(scripts) is not 0:
		    if useBatch:
				if batch_config.arraysubmit is True:
				    batch_config.submitArrayToBatch(scripts, folder + "/logs/arrayJob.sh")
				else:
				    batch_config.submitJobToBatch(scripts)
		    else:
				for script in scripts:
				    subprocess.call([script], shell=True)
		else:
		    print "all root files are intact"
			    
    
	    return False
	else:
	    print "unable to find any .sh files!"
	    return True
    

def submit_missing(	impactFolders, listToCrossCheck, 
			listToResub, cmdList = None):
    folders = []
    basenames = [os.path.basename(i) for i in impactFolders]
    print "list of impact folders to be cross checked with list of datacards/workspaces"
    print basenames
    for path in listToCrossCheck:
		parts = os.path.basename(path).split(".")
		foldername = ".".join(parts[:len(parts)-1])
		print "checking", foldername
		for entry in listToResub:
		    name = os.path.basename(entry)
		    print "\tcomparing", name
		    if foldername == name:
				print "found match!"
				folders.append(path)
				break
		if foldername in basenames:
		    print "\tfound matching folder"
		else:
		    print "\t%s flagged for fresh submit" % foldername
		    folders.append(path)
	    
    fresh_submit(folders, cmdList)
    
def fresh_submit(datacards, cmdList = None):
    for datacard in datacards:
		scriptPath = scriptDir + "/submitImpactFits.py"
		print "calling", scriptPath
		cmd = "python {0} {1}".format(scriptPath, datacard)
		if cmdList:
		    cmd += " " + " ".join(cmdList)
		print cmd
		subprocess.call([cmd], shell = True)

#=======================================================================

#=======script==========================================================

print listOfDatacards
print listOfResubFolders
print additionalSubmitCmds
print " ".join(additionalSubmitCmds)

if len(listOfDatacards) is 0:
    sys.exit("Found no datacards to cross check with in %s" % datacardWildcard)

listOfDatacards = [os.path.abspath(datacard) for datacard in listOfDatacards]
impactFolders = [os.path.abspath(path) for path in listOfResubFolders]

toResub = []
base = os.getcwd()
for resubFolder in impactFolders:
    # resubFolder = os.path.abspath(resubFolder)
    foldername = os.path.basename(resubFolder)
    
    if check_for_resubmit(folder = resubFolder):
		toResub.append(resubFolder)
    os.chdir(base)

print "list to resubmit:"
print toResub

submit_missing(	impactFolders = impactFolders,
		listToCrossCheck = listOfDatacards,
		listToResub = toResub,
		cmdList = additionalSubmitCmds)
