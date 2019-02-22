import os
import sys
import glob
from subprocess import call

datacards = sys.argv[1:]
cmdbase = "text2workspace.py -P HiggsAnalysis.CombinedLimit.PhysicsModel:multiSignalModel --PO verbose -m 125 "

for datacard in datacards:
    for d in glob.glob(datacard):
        newlines = []
        categories = {  "DL" : [],
                        "SL" : [],
                        "FH" : []
                        }
        with open(d, "r") as infile:
            lines = infile.read().splitlines()
            
            for line in lines:
                if line.startswith("shapes"):
                    pair = line.split()[2:4]
                    # print "check pair:\n", pair
                    if "DL" in pair[1] or "dl" in pair[1]:
                        categories["DL"].append(pair[0])
                    elif "FH" in pair[1] or "fh" in pair[1]:
                        categories["FH"].append(pair[0])
                    elif "SL" in pair[1] or "sl" in pair[1]:
                        categories["SL"].append(pair[0])
        print categories
        seperated_mus = cmdbase
        bunchcmd = cmdbase
        for c in categories:
            if len(categories[c])== 0: continue
            bunchcmd += "--PO \'map=("
            prefix = ""
            for n, channel in enumerate(categories[c]):
                bunchcmd += prefix + channel
                if n == 0:
                    prefix = "|"
                seperated_mus += "--PO \'map=(" + channel + ")/(ttH_*):r_"+ channel + "[1,-10,10]\' "
            
            bunchcmd += ")/(ttH_*):r_"+ c + "[1,-10,10]\' "
        parts = d.split(".")
        workspace = ".".join(parts[:len(parts)-1])
        
        bunchcmd += "-o " + workspace + "_packaged_mus.root " + d
        seperated_mus += "-o " + workspace + "_separate_mus.root " + d
        
        print "_______________________________________________________"
        print "command to create workspace with separate signal strenghts:\n"
        print seperated_mus
        print "\n____________________________________________________\n"
        
        print "command to create workspace with signal strength for respective input root file:\n"
        print bunchcmd
        call([bunchcmd], shell= True)