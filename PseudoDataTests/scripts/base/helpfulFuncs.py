import ROOT
import os
import subprocess
import stat
def getLatex(x,y,text):
    tests = ROOT.TLatex(x, y,text)
    tests.SetTextFont(42)
    tests.SetTextSize(0.04)
    tests.SetNDC()
    return tests


def getCanvas(name='c',ratiopad=False):
    c=ROOT.TCanvas(name,name,1024,1024)
    c.SetRightMargin(0.14)
    c.SetTopMargin(0.12)
    c.SetLeftMargin(0.12)
    c.SetBottomMargin(0.12)
    c.SetTicks(1,1)

    return c
def getLegend():
    legend=ROOT.TLegend()
    legend.SetX1NDC(0.15)
    legend.SetX2NDC(0.45)
    legend.SetY1NDC(0.68)
    legend.SetY2NDC(0.86)
    legend.SetBorderSize(0);
    legend.SetLineStyle(0);
    legend.SetTextFont(42);
    legend.SetTextSize(0.04);
    legend.SetFillStyle(1001); #1001 -> solid, 0 -> hollow
    return legend


def setupPad(p):
    p.SetRightMargin(0.14)
    p.SetTopMargin(0.12)
    p.SetLeftMargin(0.12)
    p.SetBottomMargin(0.12)
    p.SetTicks(1,1)


def lnn(beta,err):
    return pow(1.+err,beta)


def submitArrayToNAF(scripts, arrayscriptpath):
    """
    generate bash array with scripts from list of scripts and submit it to bird system. Function will create a folder to save log files

    Keyword arguments:

    scripts         -- list of scripts to be submitted
    arrayscriptpath -- path to safe script array in
    """
    submitclock=ROOT.TStopwatch()
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
    command=['qsub', '-cwd','-terse','-t',tasknumberstring,'-S', '/bin/bash','-l', 'h=bird*', '-hard','-l', 'os=sld6', '-l' ,'h_vmem=2000M', '-l', 's_vmem=2000M' ,'-o', '/dev/null', '-e', '/dev/null', arrayscriptpath]
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
