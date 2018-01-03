from os import path
import sys
import glob
#import json

#jsonfile = sys.argv[1]
datacards = sys.argv[1:]

excludes = ["Combination", "imax", "jmax", "kmax", "shapes", "process",
            "rate", "bin", "observation", "--"
            ]           

def check_wildcard(key, nuisancelist):
    # print "input list:\n", nuisancelist
    for np in nuisancelist:
        if "*" in np:
            # print "checking", np
            parts = np.split("*")
            start = parts[0]
            body = parts[1:len(parts)-1]
            end = parts[-1]
            # print "start =", start
            # print "body = ", body
            # print "end = ", end
            if  key.startswith(start) and all( x in key for x in body) and key.endswith(end):
                return True
        else:
            if np == key:
                return True
    return False

def save_val(dic, group, val):
    if not group in dic:
        dic[group] = []
    if not val in dic[group]:
        # print "\tappending parameter", val
        dic[group].append(val)

# jsonfile = path.abspath(jsonfile)
# if not (path.exists(jsonfile) or jsonfile.endswith(".json")):
    # sys.exit("Input json file not found!")

# groupDict = {}
# with open(jsonfile) as j:
    # groupDict = json.load(j)

groupDict = {

"bgn" : ['bgnorm_*'],
"syst" : "CMS* QCD* bgnorm* lumi* pdf*".split(),
"thy" : "QCD* bgnorm* *HDAMP* *UE* *ISR* *pdf* *scaleMu* *FSR*".split(),
"btag" : ["*btag*"],
"exp" : "lumi* *eff* *_j *btag* *PU* *QCDscaleFactor*".split(),
"jes" : ["*_j"],
"ps" : "*_ISR_* *_FSR_* *_UE_* *_HDAMP_*".split()                
}

for wildcard in datacards:
    for datacard in glob.glob(wildcard):
        finaldict = {}
        newlines = []
        with open(datacard, "r+") as d:
            
            lines = d.read().splitlines()
            for line in lines:
                if not any(line.startswith(x) for x in excludes) and not "autoMCStats" in line:
                    words = line.split()
                    if words:
                        if "group" in words and ("=" in words or "+=" in words):
                            print "found group in datacard! Copying"
                            group = words[0]
                            for word in words[3:]:
                                save_val(   dic = finaldict,
                                            group = group, val = word)       
                            continue
                        key = words[0]
                        if not all(x == "-" for x in words[2:]):
                            for group in groupDict:
                                if check_wildcard(  key = key,
                                                    nuisancelist = groupDict[group]
                                                    ):
                                    print "found match for group", group
                                    save_val(   dic = finaldict,
                                                group = group, val = key)
                newlines.append(line)
        for group in finaldict:
            l = finaldict[group]
            if len(l) != 0:
                s = group + " group = "
                s += " ".join(l)
                newlines.append(s)
        s = "\n".join(newlines)
        print s
        with open(datacard, "w") as d:
            
            d.write(s)
