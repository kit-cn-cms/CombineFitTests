import os
import sys
import glob
import json

wildcards = sys.argv[1:]

basepath = os.getcwd()
for wildcard in wildcards:
    for infile in glob.glob(wildcard):
	infile = os.path.abspath(infile)
	
	with open(infile) as f:
	    print "cd into",os.path.dirname(infile)
	    os.chdir(os.path.dirname(infile))
	    onesidedImpacts = {}
	    data = json.load(f)
	    listOfNP = data["params"]
	    for param in listOfNP:
		rVals = param["r"]
		# print rVals
		if (rVals[0] > rVals[1] and rVals[-1] > rVals[1]) or (rVals[0] < rVals[1] and rVals[-1] < rVals[1]):
		    onesidedImpacts[param["name"]] = rVals[0]
		    
	    outfile = open("onesidedNPs.txt","w")
	    # print onesidedImpacts
	    # print sorted(onesidedImpacts.items(), key = lambda x: x[1], reverse = True)
	    for paramSet in sorted(onesidedImpacts.items(), key = lambda x: x[1], reverse = True):
		name = paramSet[0]
		print name
		outfile.write(name + "\n")
	    outfile.close()
	    os.chdir(basepath)
    
