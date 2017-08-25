import sys
import os
import ROOT
import glob
ROOT.gDirectory.cd('PyROOT:/')
ROOT.gROOT.SetBatch(True)

infiles=sys.argv[1]

toRemove=['singlet', 'wjets', 'zjets', 'ttbarW', 'ttbarZ', 'diboson', "ttH_hcc", "ttH_hww", "ttH_hzz", "ttH_htt", "ttH_hgg", "ttH_hgluglu", "ttH_hzg"]

for inf in glob.glob(infiles):
    inf = os.path.abspath(inf)
    if os.path.exists(inf):
        infl=open(inf,"r")
        outfname = os.path.dirname(inf) + "/removed_"+os.path.basename(inf)
        print "creating", outfname
        outf=open(outfname,"w")
        if os.path.exists(outfname):
            inlist=list(infl)
            infprocs=[]
            for line in inlist:
                if "process " in line and infprocs==[]:
                    print "found process line"
                    sl=line.replace("\n","").replace("\t","").replace("  "," ").split(" ")
                    print sl
                    for pl in sl[1:]:
                        infprocs.append(pl)

                    print infprocs

            for line in inlist:
                sl=line.replace("\n","").replace("\t","").replace("  "," ").split(" ")
                print sl
                if len(sl)<len(infprocs):
                    outf.write(line)
                else:
                    if ("bin" in line or "process" in line or "rate" in line) and not ( "shape" in line or "Combin" in line):
                        newline=sl[0]
                        for im,m in enumerate(sl[1:]):
                            print im, m
                            if infprocs[im] in toRemove:
                                print "remove ", infprocs[im], " in ", sl[0]
                                newline+=""
                            else:
                                newline+=" "+m

                        newline+="\n"

                    elif "lnN" in line or "shape " in line:
                        newline=sl[0]+" "+sl[1]
                        for im,m in enumerate(sl[2:]):
                            if infprocs[im] in toRemove:
                                print "remove ", infprocs[im], " in ", sl[0]
                                newline+=""
                            else:
                                newline+=" "+m

                        newline+="\n"

                    else:
                        newline=line

                    outf.write(newline)

            outf.close()
        else:
            sys.exit("Could not create %s" % outfname)
    else:
        sys.exit("Could not find input file %s" % inf)