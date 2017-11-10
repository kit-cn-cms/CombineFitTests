import os
import sys
path = os.path.join(sys.path[0],'base')
print "appending", path
sys.path.append(path)
from batchConfig import *
config = batchConfig()

scripts	= sys.argv[1:]

basepath = ""
if len(scripts)>0:
    basepath = os.path.dirname(os.path.abspath(scripts[0]))
else:
    sys.exit("received no input!")
config.submitArrayToBatch(scripts, basepath+"/arrayJobs.sh")
