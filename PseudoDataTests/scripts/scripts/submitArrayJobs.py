import subprocess
import os
import sys
import glob

wildcard = sys.argv[1]

minNumberMLfit = 800

def writeSubmitCode(script, logdir, nscripts):
    submitPath = script[:-3]+".sub"
    submitScript = script.split("/")[-1][:-3]

    submitCode="universe = vanilla\n"
    submitCode+="should_transfer_files = IF_NEEDED\n"
    submitCode+="executable = /bin/bash\n"
    submitCode+="arguments = " + script + "\n"
    submitCode+="initialdir = "+os.getcwd()+"\n"
    submitCode+="notification = Never\n"
    submitCode+="priority = 0\n"
    submitCode+="request_memory = 2000M\n"
    submitCode+="error = "+logdir+"/"+submitScript+".$(Cluster)_$(ProcId).err\n"
    submitCode+="output = "+logdir+"/"+submitScript+".$(Cluster)_$(ProcId).out\n"
    #submitCode+="log = "+logdir+"/"+submitScript+".$(Cluster)_$(ProcId).log\n"
    submitCode+="Queue Environment From (\n"
    for taskID in range(nscripts):
        submitCode+="\"SGE_TASK_ID="+str(taskID)+"\"\n"
    submitCode+=")"

      submitFile = open(submitPath, "w")
    submitFile.write(submitCode)
    submitFile.close()

    return submitPath

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

    os.chdir(os.path.dirname(arrayscriptpath))
    if not os.path.exists(os.path.dirname(arrayscriptpath) + "/logs"):
        os.makedirs(os.path.dirname(arrayscriptpath) + "/logs")
    
    submitPath = writeSubmitCode( arrayscriptpath, arrayscriptpath+"/logs", jobs)
    command="condor_submit -terse "+submitPath
    a = subprocess.Popen(command.split(), stdout=subprocess.PIPE,stderr=subprocess.STDOUT,stdin=subprocess.PIPE)
    output = a.communicate()[0]
    jobidstring = output
    try:
        jobidint=int(jobidstring.split(".")[0])
    except:
        sys.exit("something did not work with submitting the array job")
    print "the jobID:", jobidstring

    return jobidint

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
