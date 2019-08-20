import ROOT
import fnmatch
from os import path as ospath
from sys import path as syspath
thisdir = ospath.dirname(ospath.realpath(__file__))
basedir = ospath.join(thisdir, "../base")
if not basedir in syspath:
	syspath.append(basedir)

# from helperClass import helperClass
# helper = helperClass()

clearnames = {
	"all"					: "Stat",
	"thy"				: "Theory",
	"bgn"					: "  Background normalisation",
	"exp"					: "Exp",
	"syst"			: "Systematic",
	"btag"				: "  B tagging",
	"jes"				: "  JES",
	"ps"				: "  PS",
	"QCD"				: "QCD",
	"autoMCStats"		: "MC uncertainty",
	"sig_thy"			: "Theory for Signal",
	"tthf_bgn"			: "  add. t#bar{t}+HF XS",
	"tthf_model"		: "  add. t#bar{t}+HF XS + PS",
}

groupcolors = {
	"Total"					: 1,
	"Stat"					: 2,
	"Theory"					: 28,
	"  add. t#bar{t}+HF XS"	: ROOT.kViolet-1,
	"Exp"					: 8,
	"  B tagging"			: ROOT.kGreen+2,
	# "    stat. b tagging"		: ROOT.kOrange+4
	# "    syst. b tagging"		: ROOT.kYellow-3,
	"  JES"					: ROOT.kOrange,
	# "  Luminosity"			: ROOT.kCyan + 1,
	"Systematic"					: 4,
	"  PS"					: ROOT.kCyan + 1,
	"QCD"					: ROOT.kSpring,
	"MC uncertainty"		: ROOT.kOrange-2,
	"Theory for Signal"		: ROOT.kRed+4,
	"  Background normalisation" : ROOT.kViolet-4,
	"  add. t#bar{t}+HF XS + PS": ROOT.kSpring +3,
}

groupmarkers = {
"Total"                 :   20,
"Stat"                  :   33,
"Systematic"                 :   22,
"Theory"                 :   21,
"  add. t#bar{t}+HF XS" :   25,
"Exp"                   :   34,
"  PS"          :   2,
"  B tagging"           :   3,
"  JES"                 :   5,
"QCD"					:	5,
"MC uncertainty"		:	5,
"Theory for Signal"		:	5,
"  Background normalisation": 5,
"  add. t#bar{t}+HF XS + PS" : 5

}

def translate_names(breakdownname, filterstring):
	if "bestfit" in filterstring:
		return "Total"
	elif breakdownname in clearnames:
		breakdownname = clearnames[breakdownname]
	if breakdownname in groupcolors:
		return breakdownname


def find_group(filename, filterstring):
	temp = filterstring
	if temp.startswith("*"):
		temp = temp[1:]
	if temp.endswith("*"):
		temp = temp[:-1]
	name = ".".join(filename.split(".")[:-1])
	parts = name.split(temp)
	xtitle = 'Postfit Uncertainty'
	print parts
	
	numstring = ""

	sub = parts[-1]
	groupname = translate_names(breakdownname = sub, filterstring= filterstring)
	if groupname:
		lumi = None
		print groupname, "\t", filename
		# lumi = helper.get_lumi_from_filename(filename = filename)
		lumi = 0
		if not lumi is None:
			return [round(lumi,2), [groupname], xtitle]
	else:
		print("could not find number with filter string %s" % filterstring)

