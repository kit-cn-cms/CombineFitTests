from os import path
import sys
directory = path.abspath(path.join(path.dirname("./"), "."))
if not directory in sys.path:
    sys.path.append(directory)

from processObject import *

class categoryObject:
    
    def __init__(   self, categoryName, defaultRootFile, 
                    defaultnominalkey,
                    systkey = None, 
                    listOfSignals = None, 
                    listOfBkg = None, 
                    processIdentifier = "$PROCESS",
                    channelIdentifier = "$CHANNEL",
                    systIdentifier = "$SYSTEMATIC" ):
        """
        init category. A category has a name, a root file containing
        the process histograms and a key to find the nominal histograms
        in the root file. You can also give it a key to find the
        systematic histograms and a list of signal or background 
        processes. You can also set the identifier which will be used
        in the datacard.
        
        Variables:
        categoryName        --  name for this category
        defaultRootFile     --  use this root file path as default for all processes
        defaultnominalkey   --  key to find the nominal histograms
        systkey             --  key to find the histograms corresponding to a nuisance parameter shape variation
        processIdentifier   --  string that is to be replaced with the process name in the keys
        channelIdentifier   --  string that is to be replaced with the channel name in the keys
        systIdentifier      --  string that is to be replaced with the nuisance parameter name in the keys
        """
        
        self._name_     = categoryName
        self._nomkey_   = defaultnominalkey
        self._systkey_  = systkey
        self._procIden_ = processIdentifier
        self._chIden_   = channelIdentifier
        self._systIden_ = systIdentifier
        self._data_obs_ = None
        self._signalprocs_ = []
        self._bkgprocs_    = []
        
        #check if process/channel identifiers are in nominal histo key
        if not self._procIden_ in self._nomkey_:
            print "WARNING:\tProcess identifier is still part of nominal histo key!"
        if self._chIden_ in self._nomkey_:
            print "WARNING:\tChannel identifier is still part of nominal histo key! Will replace it"
            self._nomkey_ = self._nomkey_.replace(self._chIden_, self._name_)
        
        #check if systematics/process/channel identifiers are in systematics histo key
        if not self._systIden_ in self._systkey_:
            print "WARNING:\tSystematic identifier still not part of nominal histo key!"
        if self._chIden_ in self._systkey__:
            print "WARNING:\tChannel identifier is still part of systematic histo key! Will replace it"
            self._systkey__ = self._systkey__.replace(self._chIden_, self._name_)
        if not self._systIden_ in self._systkey_:
            print "WARNING:\tSystematic identifier still not part of nominal histo key!"
        
        #if a list of signal processes is given, add them with default vals
        if listOfSignals:
            for proc in listOfSignals:
                self.add_signal_process(name = proc,
                                        rootfile = defaultRootFile)
        
        #if a list of bkg processes is given, add them with default vals
        if listOfBkg:
            for proc in listOfBkg:
                self.add_background_process(name = proc,
                                        rootfile = defaultRootFile)
        
        
    def add_signal_process( self, name, rootfile, 
                            histoname = self._nomkey_, 
                            systkey = self._systkey_):
        """
        add a signal process. Calls function add_process with 
        list of signal processes
        """
        self.add_process(   dic = self._signalprocs_, name = name,
                            rootfile = rootfile, histoname = histoname,
                            systkey = systkey)      
    
    def add_background_process( self, name, rootfile, 
                                histoname = self._nomkey_, 
                                systkey = self._systkey_):
        """
        add a background process. Calls function add_process with 
        list of background processes
        """
                                    
        self.add_process(   dic = self._bkgprocs_, name = name,
                            rootfile = rootfile, histoname = histoname,
                            systkey = systkey)
                            
    def add_process(self, dic, name, rootfile, 
                    histoname = self._nomkey_, systkey = self._systkey_
                    ):
        changedKey = False
        if self._procIden_ in histoname:
            print "WARNING:\tProcess identifier is still part of nominal histo key! Will replace it"
            histoname = histoname.replace(self._procIden_, name)
        if self._chIden_ in histoname:
            print "WARNING:\tChannel identifier is still part of nominal histo key! Will replace it"
            histoname = histoname.replace(self._chIden_, name)
        if self._procIden_ in systkey:
            print "WARNING:\tProcess identifier is still part of nominal histo key! Will replace it"
            systkey = systkey.replace(self._procIden_, name)
        if self._chIden_ in systkey:
            print "WARNING:\tChannel identifier is still part of nominal histo key! Will replace it"
            systkey = systkey.replace(self._chIden_, name)
            
        controlNomKey = self._nomkey_.replace(self._procIden_, name)
        controlSysKey = self._systkey_.replace(self._procIden_, name)
        if not (histoname = self._nomkey_ and systkey = self._systkey_):
            changedKey = True
        
        if name in dic:
            print ""
