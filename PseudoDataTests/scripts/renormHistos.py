from os import path
import sys
from ROOT import TFile, TKey, TH1, TObject

tonorm = sys.argv[1]
reffile = sys.argv[2]

processes = ["ttbarPlusBBbar"]
histkey = "$PROCESS_finaldiscr_$CHANNEL"


if path.exists(tonorm) and path.exists(reffile):
    tonorm_source = TFile(tonorm)
    reffile_source = TFile(reffile,"UPDATE")
    safety_copy = reffile_source.Clone(reffile.replace(".root", "_orig.root"))
    safety_copy.Close()
    
    for key in tonorm_source.GetListOfKeys():
	keyname = key.GetName()
	for process in processes:
	    prockey = histkey.replace("$PROCESS", process)
	    if "$CHANNEL" in prockey:
		prockey = prockey.replace("$CHANNEL", "")
	    
	    if keyname.startswith(prockey) and not (keyname.endswith("Up") or keyname.endswith("Down")):
		
		ref = reffile_source.Get(keyname)
		if isinstance(ref, TH1):
		    print "renorming", keyname
		    htonorm = tonorm_source.Get(keyname)
		    htonorm.Scale(ref.Integral() / htonorm.Integral())
		    reffile_source.cd()
		    htonorm.Write(htonorm.GetName(), TObject.kWriteDelete)
		else:
		    print "could not find histo to norm to! Skipping", keyname
    tonorm_source.Close()
    reffile_source.Close()
