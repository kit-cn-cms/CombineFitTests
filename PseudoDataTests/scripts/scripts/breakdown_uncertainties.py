import os
import sys
import glob
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

pathToCMSSWsetup = '/nfs/dust/cms/user/pkeicher/setup_combine_cmssw.sh'

# shell_template = """#!/bin/sh
# ulimit -s unlimited
# set -e
# targetdir="%(TARGETDIR)s"
# cmd="%(CMD)s"
# cmsswsource="%(CMSSW_SOURCE)s"
# if [[ -f $cmsswsource ]]; then
#   source $cmsswsource
#   if [[ -d $targetdir ]]; then
#     cd $targetdir
#     echo $cmd
#     eval $cmd
#     cd -
#   else
#     echo "could not change into directory $targetdir"
#   fi
# else
#   echo "could not find $cmsswsource"
# fi
# """

#=======================================================================

def add_basic_commands(cmd, mu, murange, suffix = ""):
    fitrangeUp = murange
    fitrangeDown = -1*murange
    if mu is not None:
        fitrangeUp += mu
        fitrangeDown += mu
    cmd = helpfulFuncs.insert_values(cmds = cmd, keyword = "-n", toinsert = suffix, joinwith = "_")
    # cmd += "--cminDefaultMinimizerStrategy 0".split()
    cmd = helpfulFuncs.insert_values(cmds = cmd, keyword = "--cminDefaultMinimizerStrategy", toinsert = "0", joinwith = "insert")
    # cmd += "--cminDefaultMinimizerTolerance 1e-3".split()
    cmd = helpfulFuncs.insert_values(cmds = cmd, keyword = "--cminDefaultMinimizerTolerance", toinsert = "1e-3", joinwith = "insert")
    # cmd += ("--rMin {0} --rMax {1} -t -1 --expectSignal {2}".format(mu-murange, mu+murange, mu)).split()
    cmd = helpfulFuncs.insert_values(cmds = cmd, keyword = "--rMin", toinsert = str(fitrangeDown), joinwith = "insert")
    cmd = helpfulFuncs.insert_values(cmds = cmd, keyword = "--rMax", toinsert = str(fitrangeUp), joinwith = "insert")
    
    if mu is not None:
        cmd = helpfulFuncs.insert_values(cmds = cmd, keyword = "-t", toinsert = "-1", joinwith = "insert")
        cmd = helpfulFuncs.insert_values(cmds = cmd, keyword = "--expectSignal", toinsert = str(mu), joinwith = "insert")
    

def create_script(pathToCMSSWsetup, cmd, scriptname, outfolder = None, wsfile = None):
    script = ["ulimit -s unlimited"]
    script.append("if [ -f " + pathToCMSSWsetup + " ]; then")
    script.append("  source " + pathToCMSSWsetup)
    if outfolder is not None:
        script.append("  if [ -d " + outfolder + " ]; then")
        script.append("    cd " + outfolder)

    script.append("    if [ -f " + wsfile + " ]; then")

    for c in cmd:
        script.append('    cmdnominal="' + " ".join(c) + '"')
        script.append('    echo "$cmdnominal"')
        script.append('    eval "$cmdnominal"\n')

    script.append("    else")
    script.append('      echo "could not find input for combine here: ' + wsfile +'!"')
    script.append("    fi")

    if outfolder is not None:
        script.append("  else")
        script.append('    echo "folder {0} does not exist!"'.format(outfolder))
        script.append("  fi")

    script.append("else")
    script.append('  echo "Could not find CMSSW setup file!"')
    script.append("fi")

    with open(scriptname, "w") as s:
        s.write("\n".join(script))

def finish_cmds(cmd, mu, murange, suffix, paramgroup, param = None, pois = None):
    add_basic_commands(cmd = cmd, mu = mu, murange = murange, suffix = suffix)
    
    if paramgroup and paramgroup != "all":
        cmd = helpfulFuncs.insert_values(cmds = cmd, keyword = "--freezeNuisanceGroups", toinsert = paramgroup, joinwith = ",")
    elif paramgroup and paramgroup == "all":
        cmd = helpfulFuncs.insert_values(cmds = cmd, keyword = "-S", toinsert = "0", joinwith="insert")
        
        if (pois and param) and len(pois) > 1:
            tofreeze = pois[:]
            if param in tofreeze:
                tofreeze.pop(tofreeze.index(param))
            helpfulFuncs.insert_values(cmds = cmd, keyword = "--freezeParameters", toinsert = ",".join(tofreeze), joinwith=",")

    cmd = [x for x in cmd if x != ""]

def create_fit_cmd( mdfout, paramgroup, outfolder, suffix,
            mu = None, murange = 5., cmdbase = None, pois = None, fast = False, param = None):
    all_cmds = []
    if not fast:
        cmd = "combine -M MultiDimFit".split()
        cmd.append(mdfout)
        cmd += "--algo grid --points 50 -m 125".split()
        cmd = helpfulFuncs.insert_values(cmds= cmd, keyword = "--floatOtherPOIs", toinsert = str(1), joinwith = "insert")
        if cmdbase:
            cmd += cmdbase
        if paramgroup:
            cmd += '-w w --snapshotName MultiDimFit'.split()

        cmd = helpfulFuncs.insert_values(cmds = cmd, keyword = "--saveFitResult", toinsert = "", joinwith = "insert")

        if paramgroup and paramgroup == "all":
            cmd = helpfulFuncs.insert_values(cmds = cmd, keyword = "--fastScan", toinsert = "", joinwith = "insert")

        finish_cmds(cmd=cmd,mu=mu,murange=murange,suffix="_"+suffix,paramgroup=paramgroup,pois=pois, param = param)
        
        
        all_cmds.append(cmd)

    cmd = "combine -M FitDiagnostics".split()
    cmd += mdfout.split()
    if cmdbase:
        cmd += cmdbase
    if paramgroup:
        cmd += '-w w --snapshotName MultiDimFit'.split()
    # cmd += "--minos all".split()
    temp = paramgroup
    
    finish_cmds(cmd = cmd,mu=mu,murange=murange,suffix= "_"+suffix,paramgroup=temp,pois=pois, param = param)
    all_cmds.append(cmd)

    outscript = "script_"+paramgroup + ".sh"
    if param:
        outscript = outscript.replace(".sh", "_%s.sh" % param)
    
    create_script(pathToCMSSWsetup = pathToCMSSWsetup, cmd = all_cmds, scriptname = outscript, outfolder = outfolder, wsfile = mdfout)

    if os.path.exists(outscript):
        return outscript
    
    return ""

def loadPOIs(workspace):
    print "loading POIs from workspace in", workspace
    infile = ROOT.TFile(workspace)
    w = infile.Get("w")
    npois = 1
    if isinstance(w, ROOT.RooWorkspace):
        mc = w.obj("ModelConfig")
        if isinstance(mc, ROOT.RooStats.ModelConfig):
            return mc.GetParametersOfInterest().contentsString().split(",")

def create_folders( outfolder, combineInput, paramgroup, suffix,
                    mu, scripts, cmdbase, murange, pois = None, fast = False, param = None):

    # outfolder = "breakdown_" + paramgroup
    
    # suffix += "_"+outfolder
    helpfulFuncs.check_for_reset(outfolder)
    # outfolder = os.path.join(foldername, outfolder)

    # pois = loadPOIs(workspace)
    # pois = [x for x in pois if x != "r"]
    # if len(pois) > 0:
    #  helpfulFuncs.insert_values(cmds = cmdbase, keyword = "--freezeParameters", toinsert = ",".join(pois), joinwith=",")
            
    path = create_fit_cmd(  mdfout = combineInput,
            paramgroup = paramgroup,
            suffix = suffix,
            mu = mu,
            outfolder = outfolder,
            cmdbase = cmdbase,
            murange = murange,
            pois = pois,
            fast = fast,
            param = param)        
    if path:
        scripts.append(path)

def create_folders_and_scripts(foldername, combineInput, paramgroup, suffix,
                    mu, scripts, cmdbase, murange, pois = None, fast = False):
    outfolder = "breakdown_" + paramgroup
    
    suffix += "_"+outfolder
 
    if paramgroup == "all":
        if pois:
            for param in pois:
                final_outfolder = "%s_%s" % (outfolder, param)
                create_folders(outfolder = final_outfolder, combineInput = combineInput,
                                paramgroup = paramgroup, suffix = suffix,
                                mu = mu, scripts = scripts, cmdbase = cmdbase, murange = murange, 
                                pois = pois, fast = fast, param = param)
    else:
        create_folders(outfolder = outfolder, combineInput = combineInput,
                        paramgroup = paramgroup, suffix = suffix,
                        mu = mu, scripts = scripts, cmdbase = cmdbase, murange = murange, 
                        pois = pois, fast = fast)



def submit_fit_cmds(ws, paramgroups = ["all"], mu = None, cmdbase = None, murange = 5., suffix = "", pois = None, fast = False):
    print "entering submit_fit_cmds"
    if not os.path.exists(ws):
        raise sys.exit("workspace file %s does not exist!" % ws)
    parts = os.path.basename(ws).split(".")
    foldername = ".".join(parts[:len(parts)-1])
    if suffix:
        foldername = suffix + "_" + foldername
    helpfulFuncs.check_for_reset(foldername)
    os.chdir(foldername)
    print os.getcwd()

    
    #do nominal scan
    if not fast:
        cmd = "combine -M MultiDimFit --algo grid --points 50".split()
        if cmdbase:
            cmd += cmdbase
        add_basic_commands(cmd = cmd, mu = mu, murange = murange, suffix = "_nominal_" + foldername)

        cmd = helpfulFuncs.insert_values(cmds = cmd, keyword = "--saveFitResult", toinsert = "", joinwith = "insert")
        cmd.append(ws)
        create_script(pathToCMSSWsetup = pathToCMSSWsetup, cmd = [cmd], scriptname = "nominal_scan.sh", wsfile = ws)
        if os.path.exists("nominal_scan.sh"):
            batch_fits.submitJobToBatch("nominal_scan.sh")
            # pass
        else:
            sys.exit("could not create script for nominal scan! Aborting")

    #do bestfit
    cmd = "combine -M MultiDimFit --saveWorkspace --algo none".split()
    if cmdbase:
        cmd += cmdbase
    add_basic_commands(cmd = cmd, mu = mu, murange = murange, suffix = "_bestfit_" + foldername)

    cmd = helpfulFuncs.insert_values(cmds = cmd, keyword = "--saveFitResult", toinsert = "", joinwith = "insert")
    cmd.append(ws)


    prefit_script = "bestfit.sh"
    create_script(pathToCMSSWsetup=pathToCMSSWsetup, cmd=[cmd], scriptname = prefit_script, wsfile = ws)
    if os.path.exists(prefit_script):
        print "successfully created prefit script, submitting"
        jobid = batch_fits.submitJobToBatch(prefit_script)
        scripts = []
        mdfout = "higgsCombine"
        mdfout += cmd[cmd.index("-n")+1]
        mdfout += ".MultiDimFit.mH120.root"
        mdfout = os.path.abspath(mdfout)
        #start scancs with frozen np groups
        for group in paramgroups:
            create_folders_and_scripts( foldername = foldername,
                            combineInput = mdfout,
                            paramgroup = group,
                            suffix = foldername,
                            mu = mu, scripts = scripts,
                            cmdbase = cmdbase, murange = murange,
                            pois = pois, fast = fast
                            )
        # if not "all" in paramgroups:
        #   create_folders( foldername = foldername,
        #               combineInput = mdfout,
        #               paramgroup = "all",
        #               suffix = foldername,
        #               mu = mu, scripts = scripts,
        #               cmdbase = cmdbase, murange = murange,
        #               pois = pois, fast = fast)

        if(len(scripts) > 0):
            print "submitting {0} jobs".format(len(scripts))
            return batch_fits.submitArrayToBatch(   scripts = scripts,
                        arrayscriptpath = "arrayJob.sh",
                        jobid = jobid )
        return -1


#=======================================================================

if __name__ == '__main__':

    parser = OptionParser()
    parser.add_option(  "-a", "--addCommands", 
                        help="additional combine commands (can be used multiple times)", 
                        action="append", 
                        dest = "addCommands"
                    )
    parser.add_option(  "-r", "--mu",
                        help = "signal strength for asimov toys",
                        dest = "mu",
                        type= "float")
    parser.add_option(  "--murange",
                        help= "+/- range around injected value to be scanned",
                        type = "float",
                        default = 5.0,
                        dest = "murange")
    parser.add_option(  "-b", "--breakdown",
                        help = "list of parameter groups to break down. Either comma-separated or can be used multiple times",
                        action = "append",
                        dest = "paramgroups",
                        )
    parser.add_option(  "-s", "--suffix",
                        help = "add this suffix to output files",
                        dest = "suffix")
    parser.add_option(  "-f", "--fast",
                        help = "skip NLL scans, just perform fits",
                        dest = "fast",
                        action = "store_true",
                        default = False
                    )

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
                pois = loadPOIs(d)
                # pois = [x for x in pois if x != "r"]
                arrayid = submit_fit_cmds(  ws = d,
                                            paramgroups = paramgroups,
                                            mu = mu,
                                            murange= murange,
                                            cmdbase = combineoptions,
                                            suffix = options.suffix,
                                            pois = pois,
                                            fast = options.fast)
                if arrayid != -1:
                    print "all fits submitted to batch"
                os.chdir(base)
