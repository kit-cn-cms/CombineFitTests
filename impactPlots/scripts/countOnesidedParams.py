import os
import sys
import glob

wildcards = sys.argv[1:]

base = os.getcwd()

listOfParams = {}
n = 0
for wildcard in wildcards:
    for path in glob.glob(wildcard):
        path = os.path.abspath(path)
        with open(path) as f:
            lines = f.read().splitlines()
            for line in lines:
                if line in listOfParams:
                    listOfParams[line] += 1
                else:
                    listOfParams[line] = 1
        n += 1
string = "total #files: " + str(n) + "\n"
for param in sorted(listOfParams.items(), key = lambda x: x[1], reverse = True):
    
    string += param[0] + "\t\t" + str(param[1]) + "\n"
        
print string
with open("onesided_params_frequency.txt","w") as out:
    out.write(string)
