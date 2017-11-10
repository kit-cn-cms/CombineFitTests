from ROOT import TFile, TH1D
from os import path

class processObject:
    
    def __init__(   self, processName, pathToRootfile, nominalname, 
                    systname, sharesDefault = False):
        
        self._name_             = processName
        self._rootfile_         = pathToRootfile
        self._histname_         = nominalname
        self._systname_         = systname
        self._sharesDefault_    = sharesDefault
        
        self._yield_    = self.calculate_yield()
        
    def calculate_yield(self):
        """
        returns yield (TH1D::Integral() ) for this process
        """
        #open rootfile if it exsists
        y = -1
        if path.exists(self._rootfile_):
            infile = TFile(self._rootfile_)
            #check if root file is intact
            if infile.IsOpen() and not infile.IsZombie() and not infile.TestBit(TFile.kRecovered):
                #if yes, try to load histogram
                
                hist = infile.Get(self._histname_)
                if isinstance(hist, TH1D):
                    #if successful, save yield
                    y = hist.Integral()
                else:
                    print "ERROR:\tunable to load histogram! I will let combine calculate it on the fly, but it could crash"
                infile.Close()
            else:
                print "ERROR:\tunable to open root file for process {0}, cannot set yield".format(self._name_)
        else:
            print "ERROR:\troot file does not exist! Cannot set yield for process {0}".format(self._name_)
        return y
        
    def set_yield(self, val):
        """
        set yield for processes to value val
        """
        self._yield_ = val
        
    def get_yield(self):
        """
        get yield for process
        """
        y = self._yield_
        return y
        
    def set_name(self, name):
        """
        set process name
        """
        self._name_ = name
        
    def get_name(self):
        """
        create copy of process name
        """
        s = self._name_
        return s
