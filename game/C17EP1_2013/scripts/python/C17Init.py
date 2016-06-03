import sys

# Clear the current path to make sure we don't have lingering bad directories
sys.path = []

# Append each directory that we want to look in for modules
sys.path.append( C17PyBaseDir )
sys.path.append( C17PyBaseDir + "/lib/python2.6" )
sys.path.append( C17PyBaseDir + "/Utils" )

import HLUtil

HLUtil.DevMsg("--Python Initialized! Version Number: " + sys.version + "\n")