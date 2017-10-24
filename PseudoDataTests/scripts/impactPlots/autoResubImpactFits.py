import os
import sys
import glob
import subprocess

#========input variables================================================

resubWildcard 		= sys.argv[1]
datacardWildcard 	= sys.argv[2]
additionalSubmitCmds 	= None
if len(sys.argv) > 3:
    additionalSubmitCmds= sys.argv[3:]

scriptDir		= os.path.dirname(sys.argv[0])

#=======================================================================

#========functions======================================================

def submitArrayJob(scriptList):
    pass

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
	    # submitArrayJob(scriptList)
	    with open("commands.txt") as infile:
		lines = infile.readlines()
		cmd = lines[-1]
		print cmd
		subprocess.call([cmd], shell = True)
	    
	return False
    else:
	print "unable to find any .sh files!"
	return True
    

def submit_missing(	impactFolders, listToCrossCheck, 
			listToResub, cmdList = None):
    folders = []
    basenames = [os.path.basename(i) for i in impactFolders]
    print basenames
    for path in listToCrossCheck:
	parts = os.path.basename(path).split(".")
	foldername = ".".join(parts[:len(parts)-1])
	print "checking", foldername
	if foldername in basenames:
	    continue
	else:
	    print "foldername is not in basenames!"
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
for resubFolder in glob.glob(resubWildcard):
    resubFolder = os.path.abspath(resubFolder)
    foldername = os.path.basename(resubFolder)
    base = os.path.dirname(resubFolder)
    if check_for_resubmit(	folder = resubFolder):
	toResub.append(resubFolder)
    os.chdir(base)

print "list to resubmit:"
print toResub

submit_missing(	impactFolders = impactFolders,
		listToCrossCheck = listOfDatacards,
		listToResub = toResub,
		cmdList = additionalSubmitCmds)
