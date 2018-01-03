from os import path
import sys
from ROOT import TFile, TKey, TH1, TObject

tonorm = sys.argv[1]
reffile = sys.argv[2]

tonorm_procs = ["ttbarPlusBBbar"]
normto_procs = ["ttbarPlusBBbar", "ttbarPlusB", "ttbarPlus2B"]
histkey = "$PROCESS_finaldiscr_$CHANNEL"


if path.exists(tonorm) and path.exists(reffile):
    tonorm_source = TFile(tonorm,"UPDATE")
    reffile_source = TFile(reffile)
    
    for key in tonorm_source.GetListOfKeys():
	keyname = key.GetName()
	for process in tonorm_procs:
	    prockey = histkey.replace("$PROCESS", process)
	    if "$CHANNEL" in prockey:
		searchpattern = prockey.replace("$CHANNEL", "")
	    
	    if keyname.startswith(searchpattern) and not (keyname.endswith("Up") or keyname.endswith("Down")):
		front, back = keyname.split("_finaldiscr_")
		channelkey = histkey.replace("$CHANNEL", back)
		
		ref = None
		abort = False
		for refproc in normto_procs:
		    refkey = channelkey.replace("$PROCESS", refproc)
		    temp = reffile_source.Get(refkey)
		    if isinstance(temp, TH1):
			if ref:
			    ref.Add(temp)
			else:
			    ref = temp.Clone("hSum")
		    else:
			print "could not find key {0} in reference file, aborting renorm process for {1}".format(refkey, keyname)
			abort = True
		
		if not abort and ref:
		    print "renorming", keyname
		    htonorm = tonorm_source.Get(keyname)
		    htonorm.Scale(ref.Integral() / htonorm.Integral())
		    tonorm_source.cd()
		    htonorm.Write(htonorm.GetName(), TObject.kWriteDelete)
		else:
		    print "could not find histo to norm to! Skipping", keyname
    tonorm_source.Close()
    reffile_source.Close()
