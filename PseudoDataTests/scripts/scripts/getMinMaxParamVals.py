import os
import sys


def check_wildcard(key, paramlist):
    # print "input list:\n", nuisancelist
    for p in paramlist:
        if "*" in p:
            # print "checking", p
            parts = p.split("*")
            start = parts[0]
            body = parts[1:len(parts)-1]
            end = parts[-1]
            # print "start =", start
            # print "body = ", body
            # print "end = ", end
            if  key.startswith(start) and all( x in key for x in body) and key.endswith(end):
                return True
        else:
            if p == key:
                return True
    return False
    

if __name__ == '__main__':
    for key in keylist:
        vals[key] = []
        
    infile = sys.argv[1]
    
    if os.path.exists(infile):
        with open(infile) as f:
            lines = f.read().splitlines()
            for line in lines:
                for key in keylist:
                    if check_wildcard(line, key):
                        values_raw = line.split()[2:]
                        vals[key] = 
                    
