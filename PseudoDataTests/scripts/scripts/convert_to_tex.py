import os
import sys

source = sys.argv[1]

separator = " & "
if os.path.exists(source):
    source = os.path.abspath(source)
    dirname = os.path.dirname(source)
    filename = os.path.basename(source)
    
    if not source.endswith(".txt"):
        sys.exit("Need .txt file as input!")
    
    with open(source) as infile:
        lines = infile.read().splitlines()
    
    texlines = []
    for line in lines:
        words = line.split()
        texlines.append(separator.join(words))
    
    with open(source.replace(".txt", ".tex"),"w") as out:
        out.write("\\\\\n".join(texlines))
