from ROOT import TStopwatch
import os
import subprocess
import stat
import shutil

class batchConfig:

  def __init__(self, hostname=""):
    if not hostname:
      a = subprocess.Popen(["hostname"], stdout=subprocess.PIPE,stderr=subprocess.STDOUT,stdin=subprocess.PIPE)
      output = a.communicate()[0]
      hostname = output
    
    if "lxplus" in hostname:
        print "lxplus system detected!"
        self.jobmode = "lxbatch"
        self.subname = "bsub"
        self.subopts = "-q 8nh"
        self.arraysubmit = False
        self.arraysubopts = None
        
    else:
        print "going to default - desy naf bird system"
        self.jobmode = "SGE"
        self.subname = "qsub"
        self.subopts = "-q default.q -l h=bird* -hard -l os=sld6 -l h_vmem=2000M -l s_vmem=2000M -cwd -S /bin/bash -V".split()
        self.arraysubopts = [self.subname, '-terse','-o', '/dev/null', '-e', '/dev/null']
        self.arraysubopts += self.subopts
        self.arraysubmit = True


  def submitArrayToBatch(scripts, arrayscriptpath):
    """
    generate bash array with scripts from list of scripts and submit it to bird system. Function will create a folder to save log files

    Keyword arguments:

    scripts         -- list of scripts to be submitted
    arrayscriptpath -- path to safe script array in
    """
    submitclock=TStopwatch()
    submitclock.Start()
    logdir = os.getcwd()+"/logs"
    if os.path.exists(logdir):
        print "emptying directory", logdir
        shutil.rmtree(logdir)

    os.makedirs(logdir)

    # get nscripts
    nscripts=len(scripts)
    tasknumberstring='1-'+str(nscripts)

    #create arrayscript to be run on the birds. Depinding on $SGE_TASK_ID the script will call a different plot/run script to actually run

    arrayscriptcode="#!/bin/bash \n"
    arrayscriptcode+="subtasklist=(\n"
    for scr in scripts:
        arrayscriptcode+=scr+" \n"

    arrayscriptcode+=")\n"
    arrayscriptcode+="thescript=${subtasklist[$SGE_TASK_ID-1]}\n"
    arrayscriptcode+="thescriptbasename=`basename ${subtasklist[$SGE_TASK_ID-1]}`\n"
    arrayscriptcode+="echo \"${thescript}\n"
    arrayscriptcode+="echo \"${thescriptbasename}\n"
    arrayscriptcode+=". $thescript 1>>"+logdir+"/$JOB_ID.$SGE_TASK_ID.o 2>>"+logdir+"/$JOB_ID.$SGE_TASK_ID.e\n"
    arrayscriptfile=open(arrayscriptpath,"w")
    arrayscriptfile.write(arrayscriptcode)
    arrayscriptfile.close()
    st = os.stat(arrayscriptpath)
    os.chmod(arrayscriptpath, st.st_mode | stat.S_IEXEC)

    print 'submitting',arrayscriptpath
    #command=['qsub', '-cwd','-terse','-t',tasknumberstring,'-S', '/bin/bash','-l', 'h=bird*', '-hard','-l', 'os=sld6', '-l' ,'h_vmem=2000M', '-l', 's_vmem=2000M' ,'-o', logdir+'/dev/null', '-e', logdir+'/dev/null', arrayscriptpath]
    # command=['qsub', '-cwd','-terse','-t',tasknumberstring,'-S', '/bin/bash','-l', 'h=bird*', '-hard','-l', 'os=sld6', '-l' ,'h_vmem=2000M', '-l', 's_vmem=2000M' ,'-o', '/dev/null', '-e', '/dev/null', arrayscriptpath]
    command = self.arraysubopts
    command.append('-t')
    command.append(tasknumberstring)
    command.append(arrayscriptpath)
    a = subprocess.Popen(command, stdout=subprocess.PIPE,stderr=subprocess.STDOUT,stdin=subprocess.PIPE)
    output = a.communicate()[0]
    jobidstring = output
    if len(jobidstring)<2:
        sys.exit("something did not work with submitting the array job")

    jobidstring=jobidstring.split(".")[0]
    print "the jobID", jobidstring
    jobidint=int(jobidstring)
    submittime=submitclock.RealTime()
    print "submitted job", jobidint, " in ", submittime
    return [jobidint]
