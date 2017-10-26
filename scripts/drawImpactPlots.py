import os
import sys
import glob
import subprocess

wildcard = sys.argv[1]
datacardDir = sys.argv[2]
datacardDir = os.path.abspath(datacardDir)
if os.path.exists(datacardDir):
    basepath = os.getcwd()
    for directory in glob.glob(wildcard):
	directory = os.path.abspath(directory)
	os.chdir(directory)
	print "changing into", directory
    
	workspace = datacardDir + "/" + os.path.basename(directory) + ".root"
	print "checking for file", workspace
	if os.path.exists(workspace):
	    print "creating impact plots for directory", directory
	    taskname = os.path.basename(workspace).replace(".root", "")
	    impactName = os.path.basename(directory) + "_impacts"
	    cmd = "combineTool.py -M Impacts -m 125"
	    cmd += " -o " + impactName +".json"
	    cmd += " -d " + workspace
	    cmd += " -n " + taskname
	    print cmd
	    subprocess.call([cmd], shell=True)
	    
	    if os.path.exists(impactName +".json"):
		cmd = "plotImpacts.py -i " + impactName +".json"
		cmd += " -o " + impactName + " --transparent"
		print cmd
		subprocess.call([cmd], shell=True)
	    else:
		print "could not find .json file"
	else:
	    print "could not find workspace in", workspace
	os.chdir(basepath)
else:
    sys.exit("path to datacards does not exist!")
