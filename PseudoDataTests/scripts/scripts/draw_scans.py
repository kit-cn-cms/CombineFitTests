import os
import sys
import glob
from ROOT import TFile, TTree
keyword = sys.argv[1]
wildcards = sys.argv[2:]

def is_good_file(file):
	test = TFile.Open(file)
	if test.IsOpen() and not test.IsZombie() and not test.TestBit(TFile.kRecovered):
		tree = test.Get("limit")
		if isinstance(tree, TTree):
			return True

	return False

def get_breakdowns(folders, keyword):
	startfolder = os.getcwd()
	dic = {}
	if keyword.endswith("*"):
		keyword = keyword[:-1]
	for folder in folders:
		if os.path.isdir(folder):
			os.chdir(folder)
			empty, groupname = folder.split(keyword)
			# if groupname == "all":
			# 	groupname = "syst"
			l = glob.glob("higgsCombine*MultiDimFit*.root")
			if len(l) != 0:
				file = l[0]
			else: continue
			file = os.path.abspath(file)
			dic[groupname] = file
			os.chdir(startfolder)
	return dic


def draw_breakdown(prefitfile, npgroups, outname):
	cmd = "python /nfs/dust/cms/user/pkeicher/CMSSW_8_1_0/src/CombineHarvester/CombineTools/scripts/plot1DScan.py".split()
	cmd.append(prefitfile)
	cmd.append("--others")
	breakdownnames = []
	for i, group in enumerate(npgroups, 2):
		s = "\'" + npgroups[group]
		s+= ":Freeze " + group + ":"+str(i)
		s += "\'"
		cmd.append(s)
		breakdownnames.append(group)
	breakdownnames.append("rest")
	cmd.append("--breakdown")
	cmd.append(",".join(breakdownnames))
	cmd += ["-o", outname]
	print " ".join(cmd)
	os.system(" ".join(cmd))


basedir = os.getcwd()
failed = []
for wildcard in wildcards:
	os.chdir(basedir)
	for folder in glob.glob(wildcard):
		print "checking", folder
		os.chdir(basedir)
		if os.path.isdir(folder):
			print "cd into", folder
			os.chdir(folder)
			print "looking for prefit"
			prefitkey = "higgsCombine*nominal*MultiDimFit*.root"
			l = glob.glob(prefitkey)
			if len(l) != 0:
				prefitfile = l[0]
			else: 
				print "could not find prefit file! Skipping..."
				failed.append(folder + "/" + prefitkey)
				continue
			if not is_good_file(prefitfile):
				print "Error: something is wrong with prefit file", prefitfile
				continue
			#get fit results with frozen np groups
			print "found prefit file"
			print "looking for", keyword
			l = glob.glob(keyword)
			npgroups = get_breakdowns(folders = l, keyword = keyword)
			print "found {0} break downs for drawing".format(len(npgroups))
			for group in npgroups:
				draw_breakdown(prefitfile = prefitfile, npgroups = {group : npgroups[group]}, outname = folder.split("/")[-1]+"_breakdown_" + group)
os.chdir(basedir)
if len(failed) > 0:
	with open("failed_breakdowns.txt", "w") as f:
		f.write("\n".join(failed))
