import subprocess
import os
import sys
import glob

wildcard = sys.argv[1]

minNumberMLfit = 800

def submitJobs(arrayscriptpath):
    print "open file", arrayscriptpath
    infile = open(arrayscriptpath, "r")
    inArray = False
    jobs = 0
    for line in infile:
        if ")" in line:
            inArray = False
        if inArray:
            jobs += 1
        if "(" in line:
            print "entering array"
            inArray = True

    tasknumberstring = '1-'+str(jobs)
    os.chdir(os.path.dirname(arrayscriptpath))
    if not os.path.exists(os.path.dirname(arrayscriptpath) + "/logs"):
        os.makedirs(os.path.dirname(arrayscriptpath) + "/logs")
    command=['qsub', '-cwd','-terse','-t',tasknumberstring,'-S', '/bin/bash','-l', 'h=bird*', '-hard','-l', 'os=sld6', '-l' ,'h_vmem=2000M', '-l', 's_vmem=2000M' ,'-o', '/dev/null', '-e', '/dev/null', arrayscriptpath]
    a = subprocess.Popen(command, stdout=subprocess.PIPE,stderr=subprocess.STDOUT,stdin=subprocess.PIPE)
    output = a.communicate()[0]
    jobidstring = output
    if len(jobidstring)<2:
        sys.exit("something did not work with submitting the array job")

    jobidstring=jobidstring.split(".")[0]
    print "the jobID", jobidstring
    jobidint=int(jobidstring)

basepath = os.getcwd()
for arrayscriptpath in glob.glob(wildcard):
    arrayscriptpath = os.path.abspath(arrayscriptpath)
    os.chdir(os.path.dirname(arrayscriptpath))
    print os.path.dirname(arrayscriptpath)+"/../sig*"
    for signalFolder in glob.glob(os.path.dirname(arrayscriptpath)+"/../sig*"):
        print "found", len(glob.glob(signalFolder + "/PseudoExperiment*/mlfit.root")), "mlfit.root files in path", signalFolder

        if len(glob.glob(signalFolder + "/PseudoExperiment*/mlfit.root")) < minNumberMLfit:
            submitJobs(arrayscriptpath)
            break
    os.chdir(basepath)
