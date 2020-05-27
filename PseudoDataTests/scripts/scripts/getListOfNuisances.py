import sys

incard=sys.argv[1]
skipBinByBin=False
onlyLnN=False
onlyShapes=False
with open(incard) as infile:
  inlist=infile.read().splitlines()

nlist=[]

for l in inlist:
  splitl=l.split()
  #print splitl
  #raw_input()
  if len(splitl) == 0: continue
  if splitl[0]=="shapes":
    continue
  if "lnN" in l or "shape" in l:
    if skipBinByBin and "BDTbin" in l:
      continue
    #print l
    if "shape" in l and onlyLnN:
      continue
    if "lnN" in l and onlyShapes:
      continue
    if "#" in l:
        continue
    name=l.split(" ")[0]
    nlist.append(name)

print nlist
print ""
print "r "+" ".join(nlist)
print ""
print "r,"+",".join(nlist)

print ""
for n in nlist:
  print n
