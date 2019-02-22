from sys import path as spath
baserepodir = "/nfs/dust/cms/user/pkeicher/projects/base/classes"
if not baserepodir in spath:
	spath.append(baserepodir)
from helperClass_base import helperClass_base
class helperClass(helperClass_base):
	def __init__(self):
		super(helperClass, self).__init__()