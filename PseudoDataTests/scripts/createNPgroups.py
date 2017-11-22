import os
import sys
import json

readme = sys.argv[1]

def save_val(dic, group, val):
    if not group in dic:
        dic[group] = []
    if not val in dic[group]:
        print "\tappending parameter", val
        dic[group].append(val)

def get_groups(dic, line):
    if not "  Name  " in line and not "---" in line: 
        cols = line.split("|")
        if len(cols)>1:
            for col in cols:
                col.replace(" ", "")
                col.replace('`', "")
            groups = cols[3].split(",")
            var = cols[1]
            for group in groups:
                save_val(dic = dic, group = group, val = var)

if os.path.exists(readme):
    dic = {}
    with open(readme) as r:
        lines = r.read().splitlines()
        second = False
        for line in lines:
            if second:
                get_groups(dic = dic, line= line)
            elif line.startswith('###') and second:
                break
            elif line.startswith('###') and not second:
                second = True
            
    print dic
            
