import os
import sys

inputfilepath = sys.argv[1]
threshold = float(sys.argv[2])

if os.path.exists(inputfilepath):
    with open(inputfilepath, "r") as f:
        lines = f.read().splitlines()
    counter = 0
    errorcounter = 0
    lines = lines[1:]
    for line in lines:
        words = line.split()
        val = abs(float(words[2]))
        error = words[3]
        error = error.replace("+-", "")
        error = float(error)
        if error < val:
            errorcounter += 1
        # print val
        if val >= threshold:
            counter += 1
    frac = float(counter)/len(lines) * 100
    errorfrac = float(errorcounter)/len(lines) * 100
    print "{0} % of the lines ({3}/{4}) in {1} are above the threshold {2}".format(frac, inputfilepath, threshold, counter, len(lines))
    print "{0} % of the lines ({2}/{3}) in {1} show differences that are greater than the mean error".format(errorfrac, inputfilepath, errorcounter, len(lines))
