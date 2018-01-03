import os
import sys
import glob
import subprocess
from shutil import rmtree

scriptDir = os.path.dirname(sys.argv[0])
pathToTxt = sys.argv[1]
pathToDatacards = sys.argv[2]
cmdList = None
if len(sys.argv) > 3:
    cmdList = sys.argv[3:]

print "input:"
print "\tpathToTxt:", pathToTxt
print "\tpathToDatacards:", pathToDatacards
print "\tcmdList:\n", cmdList

basepath = os.getcwd()

def reset_directory(outputDirectory):
    if os.path.exists(outputDirectory):
        print "resetting ", outputDirectory
        rmtree(outputDirectory)
    os.makedirs(outputDirectory)

if os.path.exists(pathToTxt):
    
    with open(pathToTxt) as f:
        lines = f.read().splitlines()

    for datacard in glob.glob(pathToDatacards):
        datacard = os.path.abspath(datacard)
        parts = os.path.basename(datacard).split(".")
        outputDirectory = ".".join(parts[:len(parts)-1])
        reset_directory(outputDirectory = outputDirectory)
        os.chdir(outputDirectory)
        for param in lines:
            print param
            if param.startswith("#"):
                continue
            if cmdList:
                if "--runLocally" in cmdList:
                    reset_directory(param)
                    os.chdir(param)
            cmd = "python {0}/nllscan.py".format(scriptDir)
            cmd += " -d " + datacard
            cmd += ' -x {0}'.format(param)
            cmd += " -n _" + os.path.basename(outputDirectory)
            if cmdList:
                cmd += " " + " ".join(cmdList)
            print cmd
            subprocess.call([cmd], shell=True)
            if cmdList:
                if "--runLocally" in cmdList:
                    os.chdir("../")
        os.chdir(basepath)
    
