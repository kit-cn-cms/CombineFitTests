import os
import sys
from optparse import OptionParser
from subprocess import call

thisdir = os.path.dirname(os.path.realpath(__file__))

basedir = os.path.join(thisdir, "base")
if not basedir in sys.path:
    print "appending path:", basedir
    sys.path.append(basedir)

from batchConfig import batchConfig
from helperClass import helperClass
helper = helperClass()


cmsswsource = '/nfs/dust/cms/user/pkeicher/setup_combine_cmssw.sh'

shell_template = """#!/bin/sh
ulimit -s unlimited
set -e
targetdir="%(TARGETDIR)s"
cmd='%(CMD)s'
cmsswsource="%(CMSSW_SOURCE)s"
if [[ -f $cmsswsource ]]; then
  source $cmsswsource
  if [[ -d $targetdir ]]; then
    cd $targetdir
    echo $cmd
    eval $cmd
    cd -
  else
    echo "could not change into directory $targetdir"
  fi
else
  echo "could not find $cmsswsource"
fi
"""

cmd_template = "combine -M MultiDimFit -m 125"
# cmd_template = 'python /nfs/dust/cms/user/pkeicher/projects/CombineFitTests/PseudoDataTests/scripts/plotting/nllscan.py -d %(INPUT)s -x %(PAR1)s -y %(PAR2)s -a "--cminDefaultMinimizerTolerance 1e-3 --cminDefaultMinimizerStrategy 0 --setParameterRanges %(PAR1)s=-5,5:%(PAR2)s=-5,5" --scan2D --plot2D --points 4000 --nPointsPerJob 20'

def parse_arguments():
    usage = """
    usage: %prog [options] path/to/datacards OR path/to/workspaces
    """

    parser = OptionParser(usage=usage)

    parser.add_option(  "-a", "--addCommands", 
                        help="""additional combine commands (can be used 
                        multiple times)""", 
                        action="append", 
                        dest = "additionalCommand",
                        # type = "str"
                    )
    parser.add_option(  "--runLocally", "-r",
                        help="""run scripts locally instead of submitting them
                        to cluster""",
                        action="store_true",
                        dest = "runLocally",
                        default= False
                    )
    (options, args) = parser.parse_args()

    return options, args

def create_script(finalcmd, foldername, cmsswsource):
    scriptname = "fitcmd.sh"
    scriptlines = shell_template % ({
                    "TARGETDIR": foldername,
                    "CMD"       : finalcmd,
                    "CMSSW_SOURCE" : cmsswsource
                })
    with open(scriptname, "w") as f:
        f.write(scriptlines)
    if os.path.exists(scriptname):
        return os.path.abspath(scriptname)
            

def main(options, inputfiles):
    batch = batchConfig()
    startdir = os.getcwd()
    scripts = []
    for infile in inputfiles:
        if not os.path.isfile(infile):
            print "'%s' is not a file, skipping" % infile
        infile = os.path.abspath(infile)
        fname = os.path.basename(infile)
        foldername = helper.remove_extension(fname)
        foldername = os.path.abspath(foldername)
        helper.create_folder(folder = foldername)
        os.chdir(foldername)
        
        cmdlist = cmd_template.split()
        if not options.additionalCommand is None: 
            for a in options.additionalCommand:
                cmdlist += a.split()

        cmdlist = helper.insert_values(cmds = cmdlist, keyword = "--rMin", toinsert = "-5", joinwith="insert")
        cmdlist = helper.insert_values(cmds = cmdlist, keyword = "--rMax", toinsert = "5", joinwith="insert")
        cmdlist = helper.insert_values(cmds = cmdlist, keyword = "--cminDefaultMinimizerTolerance", toinsert = "1e-1", joinwith="insert")
        cmdlist = helper.insert_values(cmds = cmdlist, keyword = "--cminDefaultMinimizerStrategy", toinsert = "0", joinwith="insert")
        cmdlist = helper.insert_values(cmds = cmdlist, keyword = "--cminPreScan", toinsert = "", joinwith="insert")
        cmdlist.append(infile)

        finalcmd = " ".join(cmdlist)
               
        script = create_script(finalcmd, foldername, cmsswsource)
        if not script is None:
            scripts.append(script)
        os.chdir(startdir)
    if len(scripts) > 0:
        if not options.runLocally:
            batchdir = "job_infos"
            helper.create_folder(batchdir)
            os.chdir(batchdir)
            arrayscriptname = "arrayScript.sh"
            batch.submitArrayToBatch(   scripts = scripts, 
                                    arrayscriptpath = arrayscriptname)
        else:
            for script in scripts:
                cmd = "source " + script
                print cmd
                call([cmd], shell=True)


if __name__ == '__main__':
    options, inputfiles = parse_arguments()
    main(options = options, inputfiles = inputfiles)
