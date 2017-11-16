class systematicObject:
    def __init__(self, name, nature, dic = None):
        
        self._name_     = name
        self._nature_   = nature
        self._dic_      = {}
        
        if dic and isinstance(dic, dict):
            self._dic_ = dic
        
    def add_process(self, name, correlation):
        self._dic_[name] = correlation
        
    def add_process(self, dic):
        if dic and isinstance(dic, dict):
            self._dic_ += dic
        else:
            print "Could not add process: input must be dictionary!"
    
    def get_correlation(self, procName):
        if procName in self._dic_:
            return str(self._dic_[procName])
        else:
            return "-"
    
            
