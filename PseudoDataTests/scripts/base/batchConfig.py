from sys import path as spath
baserepodir = "/nfs/dust/cms/user/pkeicher/projects/base/classes"
if not baserepodir in spath:
	spath.append(baserepodir)
from batchConfig_base import batchConfig_base
class batchConfig(batchConfig_base):
	def __init__(self):
		super(batchConfig, self).__init__()