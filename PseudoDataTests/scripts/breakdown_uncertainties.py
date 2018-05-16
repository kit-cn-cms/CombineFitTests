import os
import sys
import glob
import shutil
import ROOT
ROOT.PyConfig.IgnoreCommandLineOptions = True
ROOT.gROOT.SetBatch(True)
ROOT.gDirectory.cd('PyROOT:/')
from optparse import OptionParser

directory = os.path.dirname(os.path.abspath(sys.argv[0]))
basefolder = os.path.abspath(os.path.join(directory, "base"))

if not basefolder in sys.path:
    sys.path.append(basefolder)

from batchConfig import *
batch_fits = batchConfig()
import helpfulFuncs
# batch_collect = batchConfig()

pathToCMSSWsetup="/nfs/dust/cms/user/pkeicher/tth_analysis_study/CombineFitTests/PseudoDataTests/scripts/setupCMSSW_8_1_0.txt"


#=======================================================================

def add_basic_commands(cmd, mu, murange, suffix = ""):
	fitrangeUp = murange
	fitrangeDown = -1*murange
	if mu is not None:
		fitrangeUp += mu
		fitrangeDown += mu
	helpfulFuncs.insert_values(cmds = cmd, keyword = "-n", toinsert = suffix, joinwith = "_")
	# cmd += "--cminDefaultMinimizerStrategy 0".split()
	helpfulFuncs.insert_values(cmds = cmd, keyword = "--cminDefaultMinimizerStrategy", toinsert = "0", joinwith = "insert")
	# cmd += "--cminDefaultMinimizerTolerance 1e-3".split()
	helpfulFuncs.insert_values(cmds = cmd, keyword = "--cminDefaultMinimizerTolerance", toinsert = "1e-3", joinwith = "insert")
	# cmd += ("--rMin {0} --rMax {1} -t -1 --expectSignal {2}".format(mu-murange, mu+murange, mu)).split()
	helpfulFuncs.insert_values(cmds = cmd, keyword = "--rMin", toinsert = str(fitrangeDown), joinwith = "insert")
	helpfulFuncs.insert_values(cmds = cmd, keyword = "--rMax", toinsert = str(fitrangeUp), joinwith = "insert")
	if mu is not None:
		helpfulFuncs.insert_values(cmds = cmd, keyword = "-t", toinsert = "-1", joinwith = "insert")
		helpfulFuncs.insert_values(cmds = cmd, keyword = "--expectSignal", toinsert = str(mu), joinwith = "insert")
	helpfulFuncs.insert_values(cmds = cmd, keyword = "--saveFitResult", toinsert = "", joinwith = "insert")


def create_fit_cmd(	mdfout, paramgroup, outfolder, suffix,
			mu = None, murange = 5., cmdbase = None):
	script = ["if [ -f " + pathToCMSSWsetup + " ]; then"]
	script.append("  source " + pathToCMSSWsetup)
	script.append("  if [ -d " + outfolder + " ]; then")
	script.append("    cd " + outfolder)
	script.append("    if [ -f " + mdfout + " ]; then")
	# cmd = "combine -M FitDiagnostics".split()
	cmd = "combine -M MultiDimFit".split()
	cmd += "--algo grid --points 50 -m 125".split()
	if cmdbase:
		cmd += cmdbase
	if paramgroup:
		cmd += '-w w --snapshotName MultiDimFit'.split()
	add_basic_commands(cmd = cmd, mu = mu, murange = murange, suffix = suffix)
	if paramgroup:
		helpfulFuncs.insert_values(cmds = cmd, keyword = "--freezeNuisanceGroups", toinsert = paramgroup, joinwith = ",")
	# cmd += "--minos all".split()
	cmd.append(mdfout)
	script.append('      cmd="' + " ".join(cmd) + '"')
	script.append('      echo "$cmd"')
	script.append('      eval "$cmd"')
	script.append("    else")
	script.append('      echo "could not find multidimfit output!"')
	script.append("    fi")
	script.append("  else")
	script.append('    echo "folder {0} does not exist!"'.format(outfolder))
	script.append("  fi")
	script.append("else")
	script.append('  echo "Could not find CMSSW setup file!"')
	script.append("fi")
	
	if paramgroup:
	    outscript = "script_"+paramgroup + ".sh"
	else:
	    outscript = "script_fit_all.sh"
	with open(outscript, "w") as out:
		out.write("\n".join(script))
	if os.path.exists(outscript):
		return outscript
	else:
		return ""

def create_folders( foldername, combineInput, paramgroup, suffix,
                    mu, scripts, cmdbase, murange):
    if paramgroup:
        outfolder = "group_" + paramgroup
    else:
        outfolder = "all_errors"
    
    suffix += outfolder
    print "resetting folder", outfolder
    if os.path.exists(outfolder):
        shutil.rmtree(outfolder)
    os.makedirs(outfolder)
    # outfolder = os.path.join(foldername, outfolder)
            
    path = create_fit_cmd( 	mdfout = combineInput,
            paramgroup = paramgroup,
            suffix = suffix,
            mu = mu,
            outfolder = outfolder,
            cmdbase = cmdbase,
            murange = murange)		
    if path:
        scripts.append(path)

def submit_fit_cmds(ws, paramgroups = ["all"], mu = None, cmdbase = None, murange = 5., suffix = ""):
	print "entering submit_fit_cmds"
	if not os.path.exists(ws):
		raise sys.exit("workspace file %s does not exist!" % ws)
	parts = os.path.basename(ws).split(".")
	foldername = ".".join(parts[:len(parts)-1])
	if suffix:
		foldername = suffix + "_" + foldername
	print "creating", foldername
	if not os.path.exists(foldername):
		os.makedirs(foldername)
	os.chdir(foldername)
	print os.getcwd()
	scripts = []
	script = ["if [ -f " + pathToCMSSWsetup + " ]; then"]
	script.append("  source " + pathToCMSSWsetup)
	cmd = "combine -M MultiDimFit --saveWorkspace --algo none".split()
	if cmdbase:
		cmd += cmdbase

	add_basic_commands(cmd = cmd, mu = mu, murange = murange, suffix = "_prefit_" + foldername)

	helpfulFuncs.insert_values(cmds = cmd, keyword = "--saveFitResult", toinsert = "", joinwith = "insert")
	cmd.append(ws)

	script.append('    cmd="' + " ".join(cmd) + '"')
	script.append('    echo "$cmd"')
	script.append('    eval "$cmd"')

	script.append("else")
	script.append('  echo "Could not find CMSSW setup file!"')
	script.append("fi")

	prefit_script = "script_prefit.sh"
	with open(prefit_script, "w") as s:
		s.write("\n".join(script))

	if os.path.exists(prefit_script):
		print "successfully created prefit script, submitting"
		jobid = batch_fits.submitJobToBatch(prefit_script)
		scripts = []
		mdfout = "higgsCombine"
		mdfout += cmd[cmd.index("-n")+1]
		mdfout += ".MultiDimFit.mH120.root"
		mdfout = os.path.abspath(mdfout)
		for group in paramgroups:
			create_folders( foldername = foldername,
							combineInput = mdfout,
							paramgroup = group,
							suffix = foldername,
							mu = mu, scripts = scripts,
							cmdbase = cmdbase, murange = murange)

		create_folders( foldername = foldername,
						combineInput = ws,
						paramgroup = "",
						suffix = foldername,
						mu = mu, scripts = scripts,
						cmdbase = cmdbase, murange = murange)

        if(len(scripts) > 0):
        	print "submitting {0} jobs".format(len(scripts))
        	return batch_fits.submitArrayToBatch(	scripts = scripts,
						arrayscriptpath = "arrayJob.sh",
						jobid = jobid )
    	return -1


#=======================================================================

if __name__ == '__main__':

	parser = OptionParser()
	parser.add_option(	"-a", "--addCommands", 
						help="additional combine commands (can be used multiple times)", 
						action="append", 
						dest = "addCommands"
					)
	parser.add_option(	"-r", "--mu",
						help = "signal strength for asimov toys",
						dest = "mu",
						type= "float")
	parser.add_option(	"--murange",
						help= "+/- range around injected value to be scanned",
						type = "float",
						default = 5.0,
						dest = "murange")
	parser.add_option(	"-b", "--breakdown",
						help = "list of parameter groups to break down. Either comma-separated or can be used multiple times",
						action = "append",
						dest = "paramgroups",
						)
	parser.add_option(	"-s", "--suffix",
						help = "add this suffix to output files",
						dest = "suffix")

	(options, args) = parser.parse_args()
	mu = options.mu
	murange = options.murange
	wildcards = args
	print wildcards
	# paramgroups = ["all", "exp", "thy", "syst", "btag", "jes", "bgn", "ps"]
	paramgroups = []
	if options.paramgroups == None:
		# paramgroups = ["all", "scales_with_lumi", "scaling_theory", "bgn"]
		paramgroups = ["all"]
	else:
		for group in options.paramgroups:
			paramgroups += group.split(",")
	combineoptions = []
	if options.addCommands != None:
		for cmd in options.addCommands:
			combineoptions += cmd.split()

	base = os.getcwd()
	for wildcard in wildcards:
		for d in glob.glob(wildcard):
			d = os.path.abspath(d)
			if os.path.exists(d):
				print "checking %s for workspace" % d
				d = helpfulFuncs.check_workspace(d)
				arrayid = submit_fit_cmds(	ws = d,
											paramgroups = paramgroups,
											mu = mu,
											cmdbase = combineoptions,
											suffix = options.suffix)
				if arrayid != -1:
					print "all fits submitted to batch"
				os.chdir(base)
			
			
			
