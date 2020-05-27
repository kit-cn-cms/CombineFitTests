import os
import sys

asimovpath = sys.argv[1]
poissonpath = sys.argv[2]
if len(sys.argv) > 2:
    prefix = sys.argv[3]
else:
    prefix = ""

def load_vals(line):
    # print line
    if not line.startswith("corr") and "," in line:
        line = (" ".join(line.split(",")))
        # print line
    
    parts = line.split()
    # print parts
    pos = 1
    errorpos = 2
    if parts[pos] == "=":
        pos += 1
        errorpos = pos + 2
    if parts[pos] == "&":
        pos += 1
        errorpos = pos +1
    if parts[errorpos] == "+-":
        errorpos += 1
    if errorpos < len(parts):
        return parts[0], float(parts[pos]), float(parts[errorpos])
    else:
        return parts[0], float(parts[pos])

def calc_difference(baseval, val, error = None):
    if error:
        return (val - baseval)/error
    else:    
        if baseval != 0:
            return (val - baseval)/abs(baseval)
        else:
            return str(val-baseval)

with open(asimovpath) as asimov:
    with open(poissonpath) as poisson:
        diffs = {}
        lasimov = asimov.read().splitlines()
        # print lasimov
        lpoisson = poisson.read().splitlines()
        
        for asi in lasimov:
            # print asi
            if asi == "" or asi.startswith("#"): continue
            parname, asival, asierror = load_vals(asi)
            for pois in lpoisson:
                if pois == ""or pois.startswith("#"): continue
                poiscorr, poisval, poiserror = load_vals(pois)
                if poiscorr == parname or (poiscorr == parname.replace("bgnorm_ttbarPlusBBbar", "r_ttbb")) or (parname == poiscorr.replace("bgnorm_ttbarPlusBBbar", "r_ttbb")):
                    
                    dif = calc_difference(baseval = asival, val = poisval, error=poiserror)
                    if not isinstance(dif, str):
                        diffs[parname] = [dif, poisval - asival, poiserror]
                    else:
                        print "absolute difference for {0} = {1}".format(parname, dif)

lines = ["differences relativ to asimov value\t absolute difference\t mean error of poisson values:"]
for pairs in sorted(diffs.items(), key = lambda x: abs(x[1][1])):
    l="{0}\t{1}\t{2}\t+-{3}".format(pairs[0], pairs[1][0], pairs[1][1],pairs[1][2])
    print l
    lines.append(l)
print "opening .txt"
with open(prefix+"correlation_comparison.txt","w") as out:
    out.write("\n".join(lines))
