import os
import sys
import glob
import subprocess
import json
import datetime
sys.path.append(os.path.join(os.path.dirname(sys.path[0]),'base'))


wildcard 	= sys.argv[1]
additionalCmds 	= None
if len(sys.argv) > 2:
	additionalCmds = sys.argv[2:]

from batchConfig import *
config = batchConfig()

#======================================================================

def create_workspace(datacard):
    parts = datacard.split(".")
    if not datacard.endswith(".root"):
		print "calling text2workspace.py to create workspace from", datacard
		datacardOld = datacard
		datacard = ".".join(parts[:len(parts)-1]) + ".root" 
		cmd = "text2workspace.py -m 125 " + datacardOld + " -o " + datacard
		print cmd
		subprocess.call([cmd], shell=True)
	
	
    if os.path.exists(datacard):
		basename = os.path.basename(datacard)
		outputPath = basename.replace(".root", "")
		if not os.path.exists(outputPath):
		    os.makedirs(outputPath)
		outputPath = os.path.abspath(outputPath)
		# print "moving workspace and copying rootfile into directory", outputPath
		# cmd = "mv " + datacard + " " + outputPath
		# print cmd
		# subprocess.call([cmd], shell=True)
		# cmd = "cp " + rootFile + " " + outputPath
		# print cmd
		# subprocess.call([cmd], shell=True)
		
		# return outputPath + "/" + os.path.basename(datacard)
		return datacard

    return ""

def create_impacts(outputPath, datacard, impactList):
	print "submitting impact jobs"
	print "\tin", outputPath
	print "\tfrom", datacard
	if os.path.exists(outputPath) and os.path.exists(datacard):
		impactList.append(outputPath)
		print "cd into", outputPath
		os.chdir(outputPath)	
		outfile = open("commands.txt", "w")
		print "doing initial fit"
		taskname = os.path.basename(datacard).replace(".root", "")
		
		cmd = "combineTool.py -M Impacts -m 125 --doInitialFit --robustFit 1 --rMin -10 --rMax 10 -d " + datacard
		additionalCmd = ""
		if additionalCmds:
		    additionalCmd = " ".join(additionalCmds)
		    additionalCmd = additionalCmd.replace("  ", " ")
		    
		cmd += " " + additionalCmd
		cmd += " -n " + taskname
		cmd = cmd.replace("  ", " ")
		
		print cmd
		outfile.write("initial fit:\n" + cmd + "\n")
		subprocess.call([cmd], shell = True)
		
		print "starting nuisance parameter fits"
		taskname = os.path.basename(datacard).replace(".root", "")
		
		cmd = "combineTool.py -M Impacts -m 125 --robustFit 1 --doFits"
		cmd += " --rMin -10 --rMax 10 -d " + datacard
		cmd += " " + additionalCmd
		cmd += " --job-mode {0} --sub-opts='{1}'".format(config.jobmode, " ".join(config.subopts))
		cmd += " --task-name {0} -n {0}".format(taskname)
		
		# cmd += " --split-points " + nPoints
		
		cmd.replace("  ", " ")
		print cmd
		outfile.write("nuisance parameter fits:\n" + cmd + "\n")
		outfile.close()
		subprocess.call([cmd], shell = True)
		
	else:
		print "Something is wrong! Aborting"

#======================================================================

basepath = os.getcwd()
dic = {}
dic["datacards"] = []
dic["impact_folders"] = []
dic["commands"] = additionalCmds
for datacard in glob.glob(wildcard):
	datacard = os.path.abspath(datacard)
	parts = datacard.split(".")
	outputPath = ".".join(parts[:len(parts)-1])
	outputPath = basepath + "/" + os.path.basename(outputPath)
	workspace = ""
	if not datacard.endswith(".root"):
		
		workspace = ".".join(parts[:len(parts)-1]) + ".root"
		workspace = outputPath + "/" + os.path.basename(workspace)
	else:
		workspace = datacard
	
	if not (os.path.exists(outputPath) and os.path.exists(workspace)):
	    workspace = create_workspace(datacard)
	    
	dic["datacards"].append(workspace)
	create_impacts(outputPath, workspace, impactList = dic["impact_folders"])
	
	os.chdir(basepath)
with open("impact_submit_{:%Y-%b-%d}.json".format(datetime.date.today()), "w") as f:
	json.dump(dic, f, sort_keys=True,
                      separators=(',', ': '))
