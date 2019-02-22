from sys import path as spath
baserepodir = "/nfs/dust/cms/user/pkeicher/projects/base/classes"
if not baserepodir in spath:
	spath.append(baserepodir)
from CMS_pad_base import CMS_pad_base
class CMS_pad(CMS_pad_base):
	def __init__(self):
		super(CMS_pad, self).__init__()