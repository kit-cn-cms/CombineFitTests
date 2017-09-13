import ROOT
import os
import stat
import glob
import sys

def create_script(datacard,iToy,nToys=50,mode="B"):
    print "creating script for toy ",iToy
    cmsswpath=os.getenv("CMSSW_BASE")
    cwd=os.getcwd()
    if cmsswpath=="":
      print "need cmssw with combine!!!!"
      exit(0)
      
    script='#!/bin/bash\n'
    script+='export VO_CMS_SW_DIR=/cvmfs/cms.cern.ch\n'
    script+='source $VO_CMS_SW_DIR/cmsset_default.sh\n'
    script+='cd '+cmsswpath+'/src\neval `scram runtime -sh`\n'
    script+='cd '+cwd+'\n'
    if mode=="SB":
      script+='combine -M MaxLikelihoodFit -t '+str(nToys)+' --expectSignal 1 --minimizerStrategy 0 --minimizerTolerance 0.001 --rMin=-10 --rMax=10 --toysFrequentist --noErrors --minos none -s '+str(iToy)+' --out toysOut -n Toy'+str(iToy)+' '+datacard+'\n'
    else:
      script+='combine -M MaxLikelihoodFit -t '+str(nToys)+' --expectSignal 0 --minimizerStrategy 0 --minimizerTolerance 0.001 --rMin=-10 --rMax=10 --toysFrequentist --noErrors --minos none -s '+str(iToy)+' --out toysOut -n Toy'+str(iToy)+' '+datacard+'\n'
    filename='scripts/toyscipt_'+str(iToy)+'.sh'
    f=open(filename,'w')
    f.write(script)
    f.close()
    st = os.stat(filename)
    os.chmod(filename, st.st_mode | stat.S_IEXEC)
    

if not os.path.exists("scripts"):
      os.makedirs("scripts")
if not os.path.exists("toysOut"):
      os.makedirs("toysOut")
      
datacard=sys.argv[1]
nJobs=int(sys.argv[2])
nToysPerJob=int(sys.argv[3])
mode=sys.argv[4]

for ij in range(nJobs):
  ijob=ij+1
  create_script(datacard,ijob,nToysPerJob,mode)


