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
<<<<<<< HEAD
            self.jobmode = "condor"
            self.subname = "condor_qsub"
            self.subopts = "-l h=bird* -l os=sld6 -l h_vmem=2000M -l s_vmem=2000M -cwd -S /bin/bash -V".split()
            self.arraysubmit = True

=======
            self.jobmode = "HTC"
            self.subname = "condor_submit"
            self.memory = "2000M"
            self.arraysumbmit = True    
>>>>>>> fc0cac04a16d2bec389cb8848a79012cf143206b
        else:
            print "going to default - desy naf bird system"
            self.jobmode = "SGE"
            self.subname = "qsub"
            self.subopts = "-l h=bird* -hard -l os=sld6 -l h_vmem=2000M -l s_vmem=2000M -cwd -S /bin/bash -V".split()
            if queue:
                self.subopts += ("-q " + queue + ".q ").split()
            self.arraysubmit = True

    def writeSubmitCode(self, script, logdir, isArray = False, nscripts = 0):
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

    def construct_array_submit(self):
        command = None
        if self.arraysubmit and self.jobmode == "condor":
            command = [self.subname, '-terse', '-o', 'logs/', '-e', 'logs/']
            command += self.subopts
        elif self.arraysubmit:
            command = [self.subname, '-terse','-o', '/dev/null', '-e', '/dev/null']
            command += self.subopts
        return command
    
    
    def submitArrayToBatch(self, scripts, arrayscriptpath, jobid = None):
        """
        generate bash array with scripts from list of scripts and submit it to bird system. Function will create a folder to save log files
        
        Keyword arguments:
        
        scripts         -- list of scripts to be submitted
        arrayscriptpath -- path to safe script array in
        jobid           -- hold this job until job with jobid is done
        """
        submitclock=TStopwatch()
        submitclock.Start()
        arrayscriptpath = os.path.abspath(arrayscriptpath)
        logdir = os.path.dirname(arrayscriptpath)+"/logs"
        print "will save logs in", logdir
        if os.path.exists(logdir):
            print "emptying directory", logdir
            shutil.rmtree(logdir)
        
        os.makedirs(logdir)
        
        # get nscripts
        nscripts=len(scripts)
        if self.jobmode == "condor":
            tasknumberstring='0-'+str(nscripts-1)+':1'
        else:
            tasknumberstring='1-'+str(nscripts)
        
        
        #create arrayscript to be run on the birds. Depending on $SGE_TASK_ID the script will call a different plot/run script to actually run
        
        arrayscriptcode="#!/bin/bash \n"
        arrayscriptcode+="subtasklist=(\n"
        for scr in scripts:
            arrayscriptcode+=scr+" \n"
        
        arrayscriptcode+=")\n"
<<<<<<< HEAD
        if self.jobmode == "condor":
            arrayscriptcode+="thescript=${subtasklist[$SGE_TASK_ID]}\n"
            arrayscriptcode+='echo "${thescript}"\n'
=======
        if self.jobmode == "HTC":
            arrayscriptcode+="thescript=${subtasklist[$SGE_TASK_ID]}\n"
            arrayscriptcode+="echo \"${thescript}\"\n"
>>>>>>> fc0cac04a16d2bec389cb8848a79012cf143206b
            arrayscriptcode+=". $thescript"
        else:
            arrayscriptcode+="thescript=${subtasklist[$SGE_TASK_ID-1]}\n"
            arrayscriptcode+="thescriptbasename=`basename ${subtasklist[$SGE_TASK_ID-1]}`\n"
<<<<<<< HEAD
            arrayscriptcode+="echo \"${thescript}\"\n"
            arrayscriptcode+="echo \"${thescriptbasename}\"\n"
            arrayscriptcode+=". $thescript 1>>"+logdir+"/$JOB_ID.$SGE_TASK_ID.o 2>>"+logdir+"/$JOB_ID.$SGE_TASK_ID.e\n"
=======
            arrayscriptcode+="echo \"${thescript}\n"
            arrayscriptcode+="echo \"${thescriptbasename}\n"
            arrayscriptcode+=". $thescript 1>>"+logdir+"/$JOB_ID.$SGE_TASK_ID.o 2>>"+logdir+"/$JOB_ID.$SGE_TASK_ID.e\n"

>>>>>>> fc0cac04a16d2bec389cb8848a79012cf143206b
        arrayscriptfile=open(arrayscriptpath,"w")
        arrayscriptfile.write(arrayscriptcode)
        arrayscriptfile.close()
        st = os.stat(arrayscriptpath)
        os.chmod(arrayscriptpath, st.st_mode | stat.S_IEXEC)
<<<<<<< HEAD
        
        print 'submitting',arrayscriptpath
        #command=['qsub', '-cwd','-terse','-t',tasknumberstring,'-S', '/bin/bash','-l', 'h=bird*', '-hard','-l', 'os=sld6', '-l' ,'h_vmem=2000M', '-l', 's_vmem=2000M' ,'-o', logdir+'/dev/null', '-e', logdir+'/dev/null', arrayscriptpath]
        # command=['qsub', '-cwd','-terse','-t',tasknumberstring,'-S', '/bin/bash','-l', 'h=bird*', '-hard','-l', 'os=sld6', '-l' ,'h_vmem=2000M', '-l', 's_vmem=2000M' ,'-o', '/dev/null', '-e', '/dev/null', arrayscriptpath]
        command = self.construct_array_submit()
        if command:
            command.append('-t')
            command.append(tasknumberstring)
            if jobid:
                command.append("-hold_jid")
                command.append(str(jobid))
            
            command.append(arrayscriptpath)
            print command
            print " ".join(command)
            a = subprocess.Popen(command, stdout=subprocess.PIPE,stderr=subprocess.STDOUT,stdin=subprocess.PIPE)
            output = a.communicate()[0]
            jobidstring = output
            if len(jobidstring)<2:
                sys.exit("something did not work with submitting the array job")
            
            if self.jobmode == "condor":
                jobidstring=jobidstring.split(" ")[2]
            else:
                jobidstring=jobidstring.split(".")[0]
            jobidint=int(jobidstring)
            submittime=submitclock.RealTime()
            print "submitted job", jobidint, " in ", submittime
            return jobidint
        else:
            print "could not generate array submit command"
    
    def submitJobToBatch(self, script, jobid = None):
        script = os.path.abspath(script)
        st = os.stat(script)
        dirname = os.path.dirname(script)
        os.chmod(script, st.st_mode | stat.S_IEXEC)
=======

        if self.jobmode == "HTC":
            print 'writing code for condor_submit-script'
            submitPath = self.writeSubmitCode(arrayscriptpath, logdir, isArray = True, nscripts = nscripts)
            
            print 'submitting',submitPath
            command = self.subname + " -terse " + submitPath
            command = command.split()
        else:        
            print 'submitting',arrayscriptpath
            command = self.construct_array_submit()
            if not command:
                print "could not generate array submit command"
                return

            command.append('-t')
            command.append(tasknumberstring)
            command.append(arrayscriptpath)

        print command
        print " ".join(command)
        a = subprocess.Popen(command, stdout=subprocess.PIPE,stderr=subprocess.STDOUT,stdin=subprocess.PIPE)
        output = a.communicate()[0]
        jobidstring = output
        if len(jobidstring)<2:
            sys.exit("something did not work with submitting the array job")
               
        # extractiong jobid
        try:
            jobidint=int(jobidstring.split(".")[0])
        except:
            sys.exit("womething went wrong with calling condor_submit comand, submission of jobs was not successfull")
        submittime=submitclock.RealTime()
        print "submitted job", jobidint, " in ", submittime
        return [jobidint]
    
    # is this function even used? doesnt seem like it
    def submitJobToBatch(self, scripts):
>>>>>>> fc0cac04a16d2bec389cb8848a79012cf143206b
        cmdlist = [self.subname]
        cmdlist += self.subopts
        temp = "-o {0}/log.out -e {0}/log.err".format(dirname)
        cmdlist += temp.split()
        if jobid:
            cmdlist.append("-hold_jid")
            cmdlist.append(str(jobid))
        cmdlist.append(script)
        jobids = []
        #command = " ".join(cmdlist)
        print cmdlist
        print " ".join(cmdlist)
        a = subprocess.Popen(cmdlist, stdout=subprocess.PIPE,stderr=subprocess.STDOUT,stdin=subprocess.PIPE)
        output = a.communicate()[0]
        #print output
        jobidstring = output.split()
        for jid in jobidstring:
            if jid.isdigit():
                jobid=int(jid)
                print "this job's ID is", jobid
                jobids.append(jobid)
                continue
        
        return jobids
        
    def do_qstat(self, jobids):
        allfinished=False
        while not allfinished:
            time.sleep(5)
            statname = 'condor_q' if self.jobmode == "condor" else 'qstat'
            a = subprocess.Popen([statname], stdout=subprocess.PIPE,stderr=subprocess.STDOUT,stdin=subprocess.PIPE)
            qstat=a.communicate()[0]
            lines=qstat.split('\n')
            nrunning=0
            if self.jobmode == "condor":
                for line in lines:
                    if "Total for query" in line:
                        joblist = line.split(";")[1]
                        states = joblist.split(",")
                        jobs_running = int(states[2].split()[0])
                        jobs_idle =  int(states[3].split()[0])
                        print(str(jobs_running) + " jobs running, " + str(jobs_idle) + " jobs idling")
                        nrunning = jobs_running + jobs_idle
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
    
