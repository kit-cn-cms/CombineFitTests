import os
import sys
import glob
import shutil
from batchConfig import *
batch = batchConfig()

print( "removing all data from previous tests...")
os.system("rm -r scripts/logs/*")
os.system("rm scripts/*.txt")
os.system("rm scripts/*.err")
os.system("rm scripts/*.out")
os.system("rm scripts/*.sub")

base = os.path.dirname(os.path.abspath(sys.argv[0]))
print(base)

# submitJobToBatch
print("-"*50 + "\ntesting submitJobToBatch\n")
scripts = [base + "/scripts/test1_job.sh", base + "/scripts/test2_job.sh"]
nentries = [1, 2]
jobid1 = batch.submitJobToBatch(scripts[0])
jobid2 = batch.submitJobToBatch(scripts[1])

print jobid1+jobid2
batch.do_qstat(jobid1+jobid2)

print("done with submitJobToBatch")

# submitArrayToBatch
print("-"*50 + "\ntesting submitArrayToBatch\n")
scripts = [base + "/scripts/test1_array.sh", base + "/scripts/test2_array.sh"]
nentries = [3,4]
jobid = batch.submitArrayToBatch(scripts = scripts, arrayscriptpath = "scripts/arrayJob.sh")

batch.do_qstat(jobid)
print("done with submitArrayToBatch")
