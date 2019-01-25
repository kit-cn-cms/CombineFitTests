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
jobid1 = batch.submitJobToBatch(scripts[0])
jobid2 = batch.submitJobToBatch(scripts[1], jobid1)

print jobid1+jobid2
batch.do_qstat(jobid1+jobid2)

print("done with submitJobToBatch")

# submitArrayToBatch
print("-"*50 + "\ntesting submitArrayToBatch\n")
scripts = [base + "/scripts/test1_array.sh", base + "/scripts/test2_array.sh"]
jobid = batch.submitArrayToBatch(scripts = scripts, arrayPath = "scripts/arrayJob.sh")
jobid2 = batch.submitArrayToBatch(scripts = scripts, arrayPath = "scripts/arrayJob2.sh", jobid = jobid)

batch.do_qstat(jobid+ jobid2)
print("done with submitArrayToBatch")
