import os
import sys
path = os.path.join(sys.path[0],'base')
print "appending", path
sys.path.append(path)
from batchConfig import *
config = batchConfig()

scripts	= sys.argv[1:]

basepath = os.getcwd()
if len(scripts)==0:
    sys.exit("received no input!")

scripts = [os.path.abspath(x) for x in scripts if os.path.exists(x)]
config.runtime = 18000 #= 5 hours
config.submitArrayToBatch(scripts, basepath+"/arrayJobs.sh")
