import HLUtil
import sys

class OutputCatcher:
	def __init__(self):
		self.data=''
		
	def write(self, stuff):
		HLUtil.Msg(stuff)	

oc = OutputCatcher();		
sys.stdout = oc	
sys.__stdout__ = oc	


class OutputErrCatcher:
	def __init__(self):
		self.data=''
		
	def write(self, stuff):
		HLUtil.Warning(stuff)


oec = OutputErrCatcher();
sys.stderr = oec
sys.__stderr__ = oec
