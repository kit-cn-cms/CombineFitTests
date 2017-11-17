import os
import sys
import glob
import shutil
import ROOT
ROOT.gROOT.SetBatch(True)
ROOT.gDirectory.cd('PyROOT:/')

directory = os.path.dirname(os.path.abspath(sys.argv[0]))
basefolder = os.path.abspath(os.path.join(directory, "base"))

if not basefolder in sys.path:
    sys.path.append(basefolder)

from batchConfig import *
batch_fits = batchConfig(queue="short")
batch_collect = batchConfig()

pathToCMSSWsetup="/nfs/dust/cms/user/pkeicher/tth_analysis_study/CombineFitTests/PseudoDataTests/scripts/setupCMSSW_8_1_0.txt"

mu = sys.argv[1]
wildcards = sys.argv[2:]
paramgroups = ["all", "exp", "thy", "syst", "btag", "jes"]

#=======================================================================
def check_workspace(pathToDatacard):
    workspacePath = ""
    parts = pathToDatacard.split(".")
    outputPath = ".".join(parts[:len(parts)-1]) + ".root"
    if not os.path.exists(outputPath) or doWorkspaces:
        print "generating workspace for", pathToDatacard
        
        bashCmd = ["source {0} ;".format(pathToCMSSWsetup)]
        bashCmd.append("text2workspace.py -m 125 " + pathToDatacard)
        bashCmd.append("-o " + outputPath)
        print bashCmd
        subprocess.call([" ".join(bashCmd)], shell = True)
   
    workspacePath = outputPath
   
    if os.path.exists(workspacePath):
        f = ROOT.TFile(workspacePath)
        if not (f.IsOpen() and not f.IsZombie() and not f.TestBit(ROOT.TFile.kRecovered)):
            workspacePath = ""
        else:
            test = f.Get("w")
            if not isinstance(test, ROOT.RooWorkspace):
                print "could not find workspace in", workspacePath
                workspacePath = ""
    else:
        print "could not find", workspacePath
        workspacePath = ""
    return workspacePath

def create_fit_cmd(mdfout, paramgroup, outfolder, mu = mu):
	script = ["if [ -f " + pathToCMSSWsetup + " ]; then"]
	script.append("  source " + pathToCMSSWsetup)
	script.append("  if [ -d " + outfolder + " ]; then")
	script.append("    cd " + outfolder)
	script.append("  else")
	script.append('    echo "statOnly folder does not exist!"')
	script.append("  fi")
	script.append("    if [ -f " + mdfout + " ]; then")
	cmd = ["combine -M FitDiagnostics -w w"]
	cmd.append('--snapshotName "MultiDimFit"')
	cmd.append("-n _" + suffix)
	cmd.append("--cminDefaultMinimizerStrategy 0")
	cmd.append("--cminDefaultMinimizerTolerance 1e-5")
	cmd.append("--rMin -10 --rMax 10 -t -1 --expectSignal " + str(mu))
	cmd.append("--freezeParameters " + paramgroup)
	cmd.append("--minos all")
	cmd.append(mdfout)
	script.append('      echo "' + " ".join(cmd) + '"')
	script.append("      " + " ".join(cmd))	
	script.append("    else")
	script.append('      echo "could not find multidimfit output!')
	script.append("    fi")
	script.append("else")
	script.append('  echo "Could not find CMSSW setup file!')
	script.append("fi")
	
	outscript = "script_"+paramgroup + ".sh"
	with open(outscript, "w") as out:
		out.write("\n".join(script))
	if os.path.exists(outscript):
		return outscript
	else:
		return ""

def submit_fit_cmds(ws, paramgroups = ["all"], mu = mu):
    if os.path.exists(ws):
	parts = ws.split(".")
	foldername = ".".join(parts[:len(parts)-1])
	if not os.path.exists(foldername):
	    os.makedirs(foldername)
	os.chdir(foldername)
	outfolder = "splitted_errors"
	suffix = os.path.basename(foldername)
	if os.path.exists(outfolder):
	    shutil.rmtree(outfolder)
	os.makedirs(outfolder)
	outfolder = os.path.join(foldername, outfolder)
	scripts = []
	script = ["if [ -f " + pathToCMSSWsetup + " ]; then"]
	script.append("  source " + pathToCMSSWsetup)
	script.append("  if [ -d " + outfolder + " ]; then")
	script.append("    cd " + outfolder)
	cmd = ["combine -M MultiDimFit --saveWorkspace -n"]
	cmd.append("-n _prefit_" + suffix)
	cmd.append("--cminDefaultMinimizerStrategy 0")
	cmd.append("--cminDefaultMinimizerTolerance 1e-5"
	cmd.append("--rMin -10 --rMax 10 -t -1 --expectSignal"
	cmd.append(str(mu))
	cmd.append("--saveFitResult")
	
	script.append('    echo "' + " ".join(cmd) + '"')
	script.append("    " + " ".join(cmd))
			
	script.append("  else")
	script.append('    echo "statOnly folder does not exist!"')
	script.append("  fi")
	
	script.append("else")
	script.append('  echo "Could not find CMSSW setup file!')
	script.append("fi")
	
	prefit_script = "script_prefit.sh"
	with open(prefit_script, "w") as s:
	    s.write("\n".join(script))
	
	if os.path.exists(prefit_script):
	    jobid = batch_fits.submitJobToBatch(prefit_script)
	    scripts = []
	    mdfout = "higgsCombine_prefit_" + suffix + ".MultiDimFit.mH120.root"
	    for group in paramgroups:
		path = create_fit_cmd(	mdfout = mdfout,
					paramgroup = group,
					mu = mu,
					outfolder = outfolder)
		if path:
		    scripts.append(path)
	    if(len(scripts) > 0):
		return batch_fits.submitArrayToBatch(	scripts = scripts,
						arrayscriptpath = "arrayJob.sh",
						jobid = jobid )
        return -1


#=======================================================================
base = os.getcwd()
for wildcard in wildcards:
    for d in glob.glob(wildcard):
	d = os.path.abspath(d)
	if os.path.exists(d):
	    d = check_workspace(d)
	    arrayid = submit_fit_cmds(	ws = d,
					paramgroups = paramgroups,
					mu = mu)
		
			
			
			
