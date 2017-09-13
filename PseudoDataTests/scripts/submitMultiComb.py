import ROOT
import sys
import os
import glob
import subprocess

ROOT.gROOT.SetBatch(True)

workdir = "/nfs/dust/cms/user/pkeicher/tth_analysis_study/CombineFitTests/PseudoDataTests/scripts"

wildcard = sys.argv[1]
for inputFile in glob.glob(wildcard):
    inputFile = os.path.abspath(inputFile)
    print "submitting data in path", inputFile
    command=[workdir+"/submitScript.sh", inputFile, inputFile+"/temp/temp_shape_expectation.root"]
    subprocess.check_call(command)
print "done"
