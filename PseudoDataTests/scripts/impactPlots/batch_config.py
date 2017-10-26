import subprocess
#=======================================================================

a = subprocess.Popen(["hostname"], stdout=subprocess.PIPE,stderr=subprocess.STDOUT,stdin=subprocess.PIPE)
output = a.communicate()[0]
hostname = output

if "lxplus" in hostname:
	print "lxplus system detected!"
	jobmode = "lxbatch"
	subname = "bsub"
	subopts = "-q 8nh"
else:
	print "going to default - desy naf bird system"
	jobmode = "SGE"
	subname = "qsub"
	subopts = "-q default.q -l h=bird* -hard -l os=sld6 -l h_vmem=2000M -l s_vmem=2000M -cwd -S /bin/bash -V"

#======================================================================
