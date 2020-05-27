import os
import sys
from glob import glob
import json
import ROOT
import fnmatch
from optparse import OptionParser
import imp
from copy import deepcopy

thisdir = os.path.dirname(os.path.realpath(__file__))
basedir = os.path.join(thisdir, "base")
if not basedir in sys.path:
	sys.path.append(basedir)

from helperClass import helperClass
helper = helperClass()

ROOT.PyConfig.IgnoreCommandLineOptions = True
ROOT.gROOT.SetBatch(True)


scenario_colors={
	"ECFA S1": ROOT.kRed,
	"ECFA_S1": ROOT.kRed,
	"ECFA S2": ROOT.kGreen+2,
	"ECFA_S2": ROOT.kGreen+2,
	"YR2018 S1": ROOT.kRed,
	"YR2018_S1": ROOT.kRed,
	"YR2018 S2": ROOT.kGreen+2,
	"YR2018_S2": ROOT.kGreen+2,
}

scenario_labels={
	"YR2018 S1": "#splitline{w/ Run 2}{syst. uncert. (S1)}",
	"YR2018_S1": "#splitline{w/ Run 2}{syst. uncert. (S1)}",
	"YR2018 S2": "#splitline{w/ YR18}{syst. uncert. (S2)}",
	"YR2018_S2": "#splitline{w/ YR18}{syst. uncert. (S2)}"
}


def run_sanity_checks(yvals):
	for group in yvals:
		if "valup" in yvals[group]:
			words = ["valup", "valdown"]
			for w in words:
				if len(yvals[group][w]) != len(yvals[group]["xvals"]):
					print yvals[group]["xvals"]
					print yvals[group][w]
					sys.exit("number of xvals does not match number of values for group %s" % group)
		else:
			if len(yvals[group]["values"]) != len(yvals[group]["xvals"]):
				print yvals[group]["xvals"]
				print yvals[group]["values"]
				sys.exit("number of xvals does not match number of values for group %s" % group)
def find_scenario(filename):
	temp = "_".join(filename.split(".")[:-1])
	parts = temp.split("_")
	index = -1
	if "ECFA" in parts:
		index = parts.index("ECFA")
	elif "YR2018" in parts:
		index = parts.index("YR2018")
	if index != -1:
		return " ".join(parts[index:index+2])
	return None


def fill_dict_with_values(allvals, groupnames, config, xval, postfit, xtitle, par, scenario=None):
	print 
	if not par in allvals:
		allvals[par] = {}

	for groupname in groupnames:
		if not groupname in allvals[par]:
			allvals[par][groupname] = {
			"color"		: config.groupcolors[groupname],
			"marker"	: config.groupmarkers[groupname],
			# "values"	: [],
			"xtitle"	: xtitle,
			"xvals"		: []}
			if isinstance(postfit, tuple):
				allvals[par][groupname]["valup"] = []
				allvals[par][groupname]["valdown"] = []
			else:
				allvals[par][groupname]["values"] = []
			if scenario:
				allvals[par][groupname]["scenario"] = scenario_labels[scenario]
		if xval in allvals[par][groupname]["xvals"]:
			print "DANGERZONE! xval already exists for group %s! Skipping values" % groupname
			print "new values: xval = {0},\tyval = {1}".format(xval, postfit)

			print "keeping values: xval = {0},\tyval = {1}".format(xval, allvals[par][groupname]["values"][allvals[par][groupname]["xvals"].index(xval)])
		else:
			allvals[par][groupname]["xvals"].append(xval)
			if isinstance(postfit, tuple):
				allvals[par][groupname]["valup"].append(postfit[1])
				allvals[par][groupname]["valdown"].append(-postfit[2])
			else:
				allvals[par][groupname]["values"].append(postfit)

def collect_uncertainty_scans(wildcards, pars, filterstring, config, asymmetric = False):
	allvals = {}
	for wildcard in wildcards:
		for f in filterstring.split(":"):
			files = fnmatch.filter(glob(wildcard), f)
			print files
			for file in files:
				filename = os.path.basename(file)
				for par in pars:
					args = config.find_group(filename = filename, filterstring = f)
					if args == None: 
						print "error identifying group name! Skipping par %s in %s" % (par, file)
						continue
					if asymmetric:
						postfit = helper.load_asym_postfit(file, par)
					else:
						postfit = helper.load_postfit_uncert_from_variable(file, par)
					if not postfit:
						print "unable to load values for", par
						continue
					xval = args[0]
					groupnames = args[1]
					xtitle = args[2]
					# scenario = find_scenario(filename)
					fill_dict_with_values(	allvals=allvals,par = par,groupnames=groupnames,
										config = config,xval=xval,postfit=postfit,xtitle=xtitle)

	for par in allvals:
		print "checking values for parameter", par
		run_sanity_checks(allvals[par])
	return allvals

def collect_bestfit_scans(wildcards, pars):
	all_results = {}
	for par in pars:
		results = {}
		for wildcard in wildcards:
			for path in glob(wildcard):
				filename = os.path.basename(path)
				if not "lumi_" in filename:
					print "could not find luminosity in filename %s, skipping" % filename
					continue
				postfitvals = helper.load_asym_postfit(filename = path,parname="r")
				if postfitvals == None:
					print "something went wrong when loading values from %s, skipping..." % filename
					continue
				
				keyname = find_scenario(filename)
				keyname = helper.treat_special_chars(keyname)
				if keyname in scenario_colors:
					color = scenario_colors[keyname]
				else:
					color = ROOT.kBlack
				lumi = helper.get_lumi_from_filename(filename)
				
				if lumi != None:
					if not keyname in results: results[keyname] = {}
					results[keyname][float(lumi)] = {
					"central": postfitvals[0],
					"up": postfitvals[1],
					"dn": postfitvals[2],
					"color": color
					}
				else:
					print "could not load lumi from filename", filename
		if len(results) != 0:
			all_results[par] = results
	return all_results

def init_lumit_dict():
	dic = {
		"luminosity" : [],
		"limit": [],
		"up1" : [],
		"up2" : [],
		"down1" : [],
		"down2" : []
	}
	return dic

def collect_limit_scans(wildcards):
	results = {}
	for wildcard in wildcards:
		for path in glob(wildcard):
			rfile = ROOT.TFile.Open(path)
			filename = os.path.basename(path)
			if helper.intact_root_file(rfile):
				limit = rfile.Get("limit")
				if isinstance(limit, ROOT.TTree) and limit.GetEntries() == 5:
					lumi = helper.get_lumi_from_filename(filename)
					if lumi != None:
						keyname = "scenario"
						vallist = ["up2", "up1", "limit", "down1", "down2"]
						keyname = find_scenario(filename)
						if not keyname in results:
							results[keyname] = init_lumit_dict()
							results[keyname]["label"] = keyname
						for i,e in enumerate(limit):
							results[keyname][vallist[i]].append(limit.limit)
						results[keyname]["luminosity"].append(lumi)

					else:
						print "could not load lumi from filename", filename
				else:
					print "encountered faulty TTree in", path
			else:
				print "could not open file", path
	return results

def sign(x):
	return 1 if x >= 0 else -1

def substract_quadrature(val1, val2):
	return sign(val2)*(abs(val1**2 - val2**2))**(1./2)
	

def do_quadratic_substraction(return_dic, keyword, valkeyword = "values"):
	lumis = return_dic[keyword]["xvals"]

	print "valkeyword:", valkeyword
	statvals = return_dic[keyword][valkeyword]
	print statvals
	if not "Stat" in return_dic:
		sys.exit("Could not find group 'Stat'!")
	#add new group for "KEYWORD-Stat"
	for group in return_dic:
		if group == keyword or group == "Stat": continue
		for i, val in enumerate(lumis):
			if not val in return_dic[group]["xvals"]:
				print "Could not find value {0} for group {1}! Cannot substract {2} uncertainties".format(val, group, keyword)
				return None
			index = return_dic[group]["xvals"].index(val)
			if index != i: print "Careful! The ordering of lumis is different -> is something wrong?"
			groupval = return_dic[group][valkeyword][index]
			newval = substract_quadrature(val1 = groupval, val2 = statvals[i])
			print "\tgroup value(%s) =" % group, groupval
			print "\t%s value =" % keyword, statvals[i]
			print "\tnew value =", newval
			return_dic[group][valkeyword][index] = newval

def get_value_identifier(dic):
	if "valup" in dic:
		return ["valup", "valdown"]
	else:
		return ["values"]

def do_substraction(allvals, keyword):
	return_dic = {}
	if keyword in allvals:
		return_dic = deepcopy(allvals)
		print return_dic
		words = get_value_identifier(return_dic[keyword])
		for w in words:	do_quadratic_substraction(return_dic = return_dic,keyword = keyword, valkeyword = w)
	else:
		print "could not find '%s' group" % keyword
	return return_dic

def substract_stat_uncert(allvals):
	return_dic = {}
	for par in allvals:
		print "performing substraction for parameter", par
		temp = do_substraction(allvals = allvals[par], keyword = "Stat")
		if temp:
			return_dic[par] = temp
	return return_dic

def substract_from_total(allvals):
	return_dic = {}
	for par in allvals:
		print "performing substraction for parameter", par
		temp = do_substraction(allvals = allvals[par], keyword = "Total")
		if temp:
			newname = "Total-Stat"
			print "constructing", newname
			temp[newname] = deepcopy(temp["Total"])
			print temp[newname]
			identifyer = get_value_identifier(temp[newname])
			for valkey in identifyer:
				for i, lumi in enumerate(temp[newname]["xvals"]):
					temp[newname][valkey][i] = substract_quadrature(val1 = temp[newname][valkey][i], val2 = temp["Stat"][valkey][i])
			return_dic[par] = temp
	return return_dic

def main(sysargs = sys.argv):

	usage = "usage: %prog [options] path/to/rootfiles\n"
	usage += "current modes:\n"
	usage += "\tbestfits (b): collect post-fit uncertainties as a function of luminosity. Both ECFA scenarios are drawn on one canvas\n"
	usage += "\tparamScan (p): collect post-fit uncertainties. Requires config file!\n"
	usage += "\tlimits (l): draw expected limit as function of lumi\n"
	usage += "\tsignificances (s): draw expected significances as function of lumi\n"
	usage += "\tcorrelations (c): collect correlations between multiple parameters as function of lumi (WIP)\n"

	parser = OptionParser(usage=usage)
	parser.add_option(	"-m", "--mode", 
						help="select mode to run in", 
						dest="mode",
						choices = ["bestfits", "b", 
									"limits", "l",
									"paramScan", "p",
									"correlations", "c"
									]
					)
	parser.add_option(	"-o", "--outname", 
						help="name of output json file", 
						dest = "outname",
						default = "result_file.json"
					)
	parser.add_option(	"-p", "--parameters", "--params",
						help="collect values for these parameters. Can be either comma-separated list or used multiple times", 
						action="append", 
						dest = "params"
					)
	parser.add_option(	"-f", "--filter",
						help="use this filter to find correct values. Use ':' to separate multiple filters, e.g. '*bestfit*:*breakdown_*",
						dest = "filter",
					)
	parser.add_option(	"-c", "--config",
						help = "config with the function 'find_group' to find the group name and the color scheme",
						dest= "pathToConfig"
					)
	parser.add_option( "-a", "--asymmetric",
						help = "collect the asymmetric postfit uncertainties",
						dest = "asymmetric",
						action = "store_true",
						default = False
					)
	
	(options, args) = parser.parse_args()


	mode		= options.mode
	outname 	= options.outname
	if not outname.endswith(".json"):
		outname += ".json"
	wildcards 	= args

	pars = []
	temp_pars = options.params
	if temp_pars is None:
		pars = ["r"]
	else:
		for p in temp_pars:
			pars += p.split(",")

	filterstring = options.filter
	config = None

	pathToConfig = options.pathToConfig
	if not pathToConfig and ( mode == "paramScan" or mode == "p"):
		parser.error("You have to specify a config file for this mode!")
		
	
	if pathToConfig and os.path.exists(pathToConfig):
		config = imp.load_source('config', pathToConfig)
		print "loaded config from", pathToConfig

	if mode == "paramScan" and filterstring == None:
		parser.error("Please specify filter to collect param scan results!")

	allvals = {}
	substract_stat = {}
	if mode == "bestfits" or mode == "b":
		allvals = collect_bestfit_scans(wildcards = wildcards, pars = pars)
	elif mode == "limits" or mode == "l":
		allvals = collect_limit_scans(wildcards)
	elif mode == "significance" or mode == "s":
		allvals = collect_significances(wildcards)
	elif mode == "paramScan" or mode == "p":
		allvals = collect_uncertainty_scans(wildcards = wildcards, pars = pars, 
											filterstring = filterstring, 
											config = config, 
											asymmetric = options.asymmetric)
		substract_stat = substract_from_total(allvals)
	else:
		sys.exit("Did not recognize mode!")
	
	print "creating file", outname
	helper.dump_json(outname = outname, allvals = allvals)
	if substract_stat:
		print "creating file", "substracted_"+outname
		helper.dump_json(outname = "substracted_"+outname, allvals = substract_stat)


if __name__ == '__main__':
	main()
