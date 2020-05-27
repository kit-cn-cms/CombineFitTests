import csv
import sys
import os

csvfilepath = sys.argv[1]

info = ["name", "generator", {"N_gen" : ["nGen", "Npos-Nneg/Ntotal"]}, "XS"]
mainseparator = " & "
if os.path.exists(csvfilepath):
    csvfilepath = os.path.abspath(csvfilepath)
    lines = []
    #create header
    line = ""
    for ninfo, i in enumerate(info):
        separator = mainseparator
        if ninfo == 0:
            separator = ""
        if isinstance(i, dict):
            for key in i:
                line += (separator + key)
        else:
            line += separator + i
    lines.append(line)
    with open(csvfilepath, "rb") as f:
        data = csv.DictReader(f, delimiter = ",")
        print data
        for row in data:
            line = ""
            if all(entry == "" for entry in row.items()): continue
            for ninfo, i in enumerate(info):
                if ninfo == 0:
                    sep = ""
                else:
                    sep = mainseparator
                if isinstance(i, dict):
                    
                    for key in i:
                        print "calculating", key
                        fak = 1.
                        for element in i[key]:
                            print "loading", element
                            val = row[element]
                            print "loaded", val
                            if isinstance(val, str):
                                print "string detected, converting"
                                if val != "":
                                    val = float(val)
                            if isinstance(val, int) or isinstance(val, float):
                                print "value in {0} to multiply: {1}".format(element, val)
                                fak *= val
                        if fak != 1.:
                            line += sep + str(fak)
                else:
                    if not row[i] == "":
                        val = row[i]
                        if i == "name":
                            parts = val.split("_")
                            val = "\\_".join(parts[:len(parts)-1])
                            val = "\\" + val
                        line += "{0}{1}".format(sep, val)
                        if i == "XS":
                            line += "e-12"
            if not line == "":
                lines.append(line)
            else:
                lines[-1] += ('\\\\\\midrule\n')
    
    with open("generator_table.tex", "w") as out:
        s = "\\\\\n".join(lines)
        print s
        out.write(s)
           
