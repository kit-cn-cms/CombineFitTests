import sys
from os import path

directory = path.abspath(path.join(path.dirname("./"), "."))
if not directory in sys.path:
    sys.path.append(directory)

from processObject import *
from systematicObject import *
from categoryObject import *

class datacardMaker:
    
    def __init__(self):
        self._header_       = []
        self._bins_         = ""
        self._observation_  = ""
        self._processes_    = []
        
        
    def __init__(   self, pathToDatacard, 
                    processIdentifier = "$PROCESS",
                    channelIdentifier = "$CHANNEL",
                    systIdentifier = "$SYSTEMATIC"):
        if os.path.exists(pathToDatacard):
            
            
            
            print "loading datacard from", pathToDatacard
            with open(pathToDatacard) as datacard:
                lines = datacard.read().splitlines()
                self._header_ = []
                self._shapelines_ = []
                for n, line in enumerate(lines):
                    if line.startswith("-"):
                            continue
                    elif    line.startswith("Combination") or
                            line.startswith("imax") or
                            line.startswith("kmax") or
                            line.startswith("jmax"):
                        self._header_.append(line)
                    elif    line.startswith("bin") and
                            n != len(lines) and
                            lines[n+1].startswith("observation"):
                        self._bins_ = line
                        self._observation_ = lines[n+1]
                    elif line.startswith("shapes"):
                        self._shapelines_.append(line)
                    elif    line.startswith("process") and
                            n != 0 and
                            lines[n-1].startswith("bin")
                    else:
                        
                        
                        
        else:
            print "could not load %s: no such file" % pathToDatacard
        
