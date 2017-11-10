import sys
from os import path

directory = path.abspath(path.join(path.dirname("./"), "."))
if not directory in sys.path:
    sys.path.append(directory)


class datacardMaker:
    
    def __init__(self, pathToDatacard):
        if os.path.exists(pathToDatacard):
            print "loading datacard from", pathToDatacard
            with open(pathToDatacard) as datacard:
                lines = datacard.read().splitlines()
                
                self.shapelines = 
        else:
            print "could not load %s: no such file" % pathToDatacard
        
