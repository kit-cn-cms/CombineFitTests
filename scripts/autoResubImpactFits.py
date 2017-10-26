from ROOT import TFile
import os
import sys
import glob
import subprocess
import imp
sys.path.append(os.path.join(os.path.dirname(sys.path[0]),'base'))

from batchConfig import *
batch_config = batchConfig()
#========input variables================================================

resubWildcard 		= sys.argv[1]
datacardWildcard 	= sys.argv[2]
additionalSubmitCmds 	= None
if len(sys.argv) > 3:
    additionalSubmitCmds= sys.argv[3:]



# pathToConfig = scriptDir + "/batch_config.py"
# pathToConfig = os.path.abspath(pathToConfig)
# print "loading config from", pathToConfig

# config = imp.load_source('config', pathToConfig)

#=======================================================================

#========functions======================================================

def check_for_resubmit(folder):
    print "cd into", folder
    os.chdir(folder)
    toResub = []
    scriptList = glob.glob("*.sh")
    if not os.path.exists("commands.txt"):
	print "unable to find commands.txt in", folder
	return True
    if not len(scriptList) == 0:
	rootfiles = glob.glob("higgsCombine_paramFit*.root")
	print "# rootfiles:", len(rootfiles)
	print "# scripts:", len(scriptList)
	if not len(scriptList) == len(rootfiles):
	    with open("commands.txt") as infile:
		lines = infile.readlines()
		cmd = lines[-1]
		print cmd
		subprocess.call([cmd], shell = True)
	else:
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
		if batch_config.arraysubmit is True:
		    batch_config.submitArrayToBatch(scripts, folder + "/logs/arrayJob.sh")
		else:
		    batch_config.submitJobToBatch(scripts)
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
	if foldername in basenames:
	    continue
	else:
	    print "foldername is not in list of impact folders!"
	if not (string.endswith(foldername) for string in listToResub):
	    continue
	else:
	    print "foldername is in list to resub!"
	
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


listOfDatacards = glob.glob(datacardWildcard)

if len(listOfDatacards) is 0:
    sys.exit("Found no datacards to cross check with in %s" % datacardWildcard)

listOfDatacards = [os.path.abspath(datacard) for datacard in listOfDatacards]
impactFolders = [os.path.abspath(path) for path in glob.glob(resubWildcard)]

toResub = []
base = os.getcwd()
for resubFolder in glob.glob(resubWildcard):
    resubFolder = os.path.abspath(resubFolder)
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
