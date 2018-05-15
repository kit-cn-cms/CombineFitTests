from ROOT import TStopwatch
import os
import sys
import subprocess
import stat
import shutil
import time

class batchConfig:

    def __init__(self, hostname="", queue = ""):
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

        elif "naf-cms" in hostname:
            print "NAF HTCondor system detected!"
            self.jobmode = "HTC"
            self.subname = "condor_submit"
            self.memory = "2000M"
            self.arraysumbmit = True

        else:
            print "going to default - desy naf bird system"
            self.jobmode = "SGE"
            self.subname = "qsub"
            self.subopts = "-l h=bird* -hard -l os=sld6 -l h_vmem=2000M -l s_vmem=2000M -cwd -S /bin/bash -V".split()
            if queue:
                self.subopts += ("-q " + queue + ".q ").split()
            self.arraysubmit = True

    def writeSubmitCode(self, script, logdir, hold = False, isArray = False, nscripts = 0):
        '''
        write the code for condor_submit file
        script: path to .sh-script that should be executed
        logdir: path to directory of logs
        isArray: set True if script is an array script
        nscripts: number of scripts in the array script. Only needed if isArray=True
        
        returns path to generated condor_submit file
        '''
        submitPath = script[:-3]+".sub"
        submitScript = script.split("/")[-1][:-3]
        
        submitCode="universe = vanilla\n"
        submitCode+="should_transfer_files = IF_NEEDED\n"
        submitCode+="executable = /bin/bash\n"
        submitCode+="arguments = " + script + "\n"
        submitCode+="initialdir = "+os.getcwd()+"\n"
        submitCode+="notification = Never\n"
        submitCode+="priority = 0\n"
        submitCode+="request_memory = "+self.memory+"\n"
        #submitCode+="request_dist = 5800M\n"
        if hold:
            submitCode+="hold = True\n"

        if isArray:
            submitCode+="error = "+logdir+"/"+submitScript+".$(Cluster)_$(ProcId).err\n"
            submitCode+="output = "+logdir+"/"+submitScript+".$(Cluster)_$(ProcId).out\n"
            #submitCode+="log = "+logdir+"/"+submitScript+".$(Cluster)_$(ProcId).log\n"
            submitCode+="Queue Environment From (\n"
            for taskID in range(nscripts):
                submitCode+="\"SGE_TASK_ID="+str(taskID)+"\"\n"
            submitCode+=")"
        else:
            submitCode+="error = "+logdir+"/"+submitScript+".$(Cluster).err\n"
            submitCode+="output = "+logdir+"/"+submitScript+".$(Cluster).out\n"
            #submitCode+="log = "+logdir+"/"+submitScript+".$(Cluster).log\n"
            submitCode+="queue"

        submitFile = open(submitPath, "w")
        submitFile.write(submitCode)
        submitFile.close()
        return submitPath


    def writeArrayCode(self, scripts, arrayPath):

        arrayCode="#!/bin/bash \n"
        arrayCode+="subtasklist=(\n"
        for scr in scripts:
            arrayCode+=scr+" \n"

        arrayCode+=")\n"
        if self.jobmode == "HTC":
            arrayCode+="thescript=${subtasklist[$SGE_TASK_ID]}\n"
            arrayCode+="echo \"${thescript}\"\n"
            arrayCode+=". $thescript"
        else:
            arrayCode+="thescript=${subtasklist[$SGE_TASK_ID-1]}\n"
            arrayCode+="thescriptbasename=`basename ${subtasklist[$SGE_TASK_ID-1]}`\n"
            arrayCode+="echo \"${thescript}\n"
            arrayCode+="echo \"${thescriptbasename}\n"
            arrayCode+=". $thescript 1>>"+logdir+"/$JOB_ID.$SGE_TASK_ID.o 2>>"+logdir+"/$JOB_ID.$SGE_TASK_ID.e\n"
        arrayFile=open(arrayPath,"w")
        arrayFile.write(arrayCode)
        arrayFile.close()
        st = os.stat(arrayPath)
        os.chmod(arrayPath, st.st_mode | stat.S_IEXEC)
        return arrayPath

    def construct_array_submit(self):
        command = None
        command = [self.subname, '-terse','-o', '/dev/null', '-e', '/dev/null']
        command += self.subopts
        return command
    
    def setupRelease(self, oldJIDs, newJID):
        releaseCode = "from batchConfig import *\n"
        releaseCode += "import os\n"
        releaseCode += "q = batchConfig()\n"
        releaseCode += "q.do_qstat("+str(oldJIDs)+")\n"
        releaseCode += "os.system('condor_release "+str(newJID)+"')"
        
        releasePath = "release_"+str(newJID)+".py"
        with open(releasePath, "w") as releaseFile:
            releaseFile.write(releaseCode)
        os.system("python "+releasePath+" > /dev/null &")
        os.system("rm "+releasePath)

    def submitArrayToBatch(self, scripts, arrayPath, jobid = None):
        """
        generate bash array with scripts from list of scripts and submit it to bird system. Function will create a folder to save log files
        
        Keyword arguments:
        
        scripts         -- list of scripts to be submitted
        arrayPath -- path to safe script array in
        jobid           -- hold this job until job with jobid is done
        """
        submitclock=TStopwatch()
        submitclock.Start()
        arrayPath = os.path.abspath(arrayPath)

        logdir = os.path.dirname(arrayPath)+"/logs"
        print "will save logs in", logdir
        if os.path.exists(logdir):
            print "emptying directory", logdir
            shutil.rmtree(logdir)
        os.makedirs(logdir)
        
        # write array script
        nscripts=len(scripts)
        tasknumberstring='1-'+str(nscripts)

        arrayPath = self.writeArrayCode(scripts, arrayPath)
        
        # prepate submit
        if self.jobmode == "HTC":
            print 'writing code for condor_submit-script'
            hold = True if jobid else False
            submitPath = self.writeSubmitCode(arrayPath, logdir, hold = hold, isArray = True, nscripts = nscripts)
            
            print 'submitting',submitPath
            command = self.subname + " -terse " + submitPath
            command = command.split()
        else:
            print 'submitting',arrayPath
            command = self.construct_array_submit()
            if not command:            
                print "could not generate array submit command"
                return 
            command.append('-t')
            command.append(tasknumberstring)
            if jobid:
                command.append("-hold_jid")
                command.append(str(jobid))
            command.append(arrayPath)
        
        # submitting
        print "command:", command
        a = subprocess.Popen(command, stdout=subprocess.PIPE,stderr=subprocess.STDOUT,stdin=subprocess.PIPE)
        output = a.communicate()[0]
        jobidstring = output
        if len(jobidstring)<2:
            sys.exit("something did not work with submitting the array job")
        
        # extracting jobid
        try:
            jobidint = int(output.split(".")[0])
        except:
            sys.exit("something went wrong with calling condor_submit command, submission of jobs was not succesfull")
        submittime=submitclock.RealTime()
        print "submitted job", jobidint, " in ", submittime
        if hold:
            self.setupRelease(jobid, jobidint)
        return [jobidint]
    
    def submitJobToBatch(self, script, jobid = None):
        script = os.path.abspath(script)
        st = os.stat(script)
        dirname = os.path.dirname(script)
        os.chmod(script, st.st_mode | stat.S_IEXEC)
        cmdlist = [self.subname]
        if self.jobmode == "HTC":
            hold = True if jobid else False
            submitPath = self.writeSubmitCode(script, dirname+"/logs", hold = hold)
            cmdlist.append("-terse")
            cmdlist.append(submitPath)
        else:
            cmdlist += self.subopts
            temp = "-o {0}/log.out -e {0}/log.err".format(dirname)
            cmdlist += temp.split()
            if jobid:
                cmdlist.append("-hold_jid")
                cmdlist.append(str(jobid))
            cmdlist.append(script)
        jobids = []
        #command = " ".join(cmdlist)
        print "command:", cmdlist
        a = subprocess.Popen(cmdlist, stdout=subprocess.PIPE,stderr=subprocess.STDOUT,stdin=subprocess.PIPE)
        output = a.communicate()[0]
        #print output
        if self.jobmode == "HTC":
            try:
                jobidint = int(output.split(".")[0])
            except:
                sys.exit("something went wrong with calling condor_submit command, submission of jobs was not succesfull")
        else:
            jobidstring = output.split()
            for jid in jobidstring:
                if jid.isdigit():
                    jobidint=int(jid)
                    continue

        print "this job's ID is", jobidint
        jobids.append(jobidint)
        if hold:
            self.setupRelease(jobid, jobidint)
        return jobids
        
    def do_qstat(self, jobids):
        allfinished=False
        while not allfinished:
            time.sleep(10)
            statname = ['condor_q'] if self.jobmode == "HTC" else ['qstat']
            if self.jobmode == "HTC":
                statname += jobids
                statname = [str(stat) for stat in statname]
            a = subprocess.Popen(statname, stdout=subprocess.PIPE,stderr=subprocess.STDOUT,stdin=subprocess.PIPE)
            qstat=a.communicate()[0]
            lines=qstat.split('\n')
            nrunning=0
            if self.jobmode == "HTC":
                for line in lines:
                    if "Total for query" in line:
                        joblist = line.split(";")[1]
                        states = joblist.split(",")
                        jobs_running = int(states[3].split()[0])
                        jobs_idle =  int(states[2].split()[0])
                        jobs_held = int(states[4].split()[0])
                        nrunning = jobs_running + jobs_idle + jobs_held
            else:
                for line in lines:
                    words=line.split()
                    for jid in words:
                        if jid.isdigit():
                            jobid=int(jid)
                            if jobid in jobids:
                                nrunning+=1
                                break
    
            if nrunning>0:
                print nrunning,'jobs running'
            else:
                allfinished=True
    
