import os
import sys
import glob
import subprocess

scriptDir = os.path.dirname(sys.argv[0])
wildcard = sys.argv[1]
pathToDatacards = sys.argv[2]
cmdList = None
if len(sys.argv) > 3:
    cmdList = sys.argv[3:]

print "input:"
print "\twildcard:", wildcard
print "\tpathToDatacards:", pathToDatacards
print "\tcmdList:\n", cmdList

basepath = os.getcwd()
if os.path.exists(pathToDatacards):
    pathToDatacards = os.path.abspath(pathToDatacards)
    for txtfilepath in glob.glob(wildcard):
	txtfilepath = os.path.abspath(txtfilepath)
	parentDir = os.path.dirname(txtfilepath)
	datacardname = os.path.basename(parentDir)
	os.chdir(parentDir)
	with open(txtfilepath) as f:
	    lines = f.read().splitlines()
	    for param in lines:
		print param
		cmd = "python {0}/nllscan.py".format(scriptDir)
		cmd += " -d {0}/{1}.root".format(pathToDatacards, datacardname)
		cmd += ' -x {0}'.format(param)
		cmd += ' --floatR'
		cmd += " -n _" + datacardname
		if cmdList:
		    cmd += " " + " ".join(cmdList)
		print cmd
		subprocess.call([cmd], shell=True)
	os.chdir(basepath)
