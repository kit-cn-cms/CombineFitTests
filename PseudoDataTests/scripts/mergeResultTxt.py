import os
import sys
import glob



if __name__ == '__main__':
    outputname = sys.argv[1]
    if not outputname:
        outputname = "merged_param_list.txt"
    wildcards = sys.argv[2:]
    results = {}
    print "input:\n"
    print wildcards
    for wildcard in wildcards:
        for f in glob.glob(wildcard):
            print "opening file", f
            f = os.path.abspath(f)
            if os.path.exists(f) and f.endswith(".txt"):
                with open(f) as infile:
                    basename = os.path.basename(f)
                    
                    lines = infile.read().splitlines()
                    subdict = {}
                    for line in lines:
                        if '&' in line:
                            parts = line.split('&')
                            
                            subdict[parts[0]] = parts[1]
                    
                    
                    
                    results[basename.replace(".txt", "")] = subdict
    
    head = "input file & Experiment Labels\n"
    body = [" "]
    labellist = []
    for i, fkey in enumerate(results):
        body.append(fkey + "\n")
        for label in results[fkey]:
            if not label in labellist:
                labellist.append(label)
        for label in labellist:
            if label.startswith("r_"):
                body.append(label)
                if label in results[fkey]:
                    
                    vals = results[fkey][label]
                    
                    body[-1] +=  " & {0}\n".format(vals.replace(",", " +- "))
                else:
                    body[-1] += " & -\n"
    # body[0] += "& " + " & ".join(labellist)
            
    if len(body) > 1:
        with open(outputname, "w") as out:
            out.write(head)
            out.write("\n".join(body))
    else:
        print "could not collect any results!"
    
