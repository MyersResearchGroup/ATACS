#!/usr/bin/python

##############################################################################
##   Copyright (c) 2007 by Scott R. Little
##   University of Utah
##
##   Permission to use, copy, modify and/or distribute, but not sell, this
##   software and its documentation for any purpose is hereby granted
##   without fee, subject to the following terms and conditions:
##
##   1.  The above copyright notice and this permission notice must
##   appear in all copies of the software and related documentation.
##
##   2.  The name of University of Utah may not be used in advertising or
##   publicity pertaining to distribution of the software without the
##   specific, prior written permission of University of Utah.
##
##   3.  THE SOFTWARE IS PROVIDED "AS-IS" AND WITHOUT WARRANTY OF ANY KIND,
##   EXPRESS, IMPLIED OR OTHERWISE, INCLUDING WITHOUT LIMITATION, ANY
##   WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.
##
##   IN NO EVENT SHALL UNIVERSITY OF UTAH OR THE AUTHORS OF THIS SOFTWARE BE
##   LIABLE FOR ANY SPECIAL, INCIDENTAL, INDIRECT OR CONSEQUENTIAL DAMAGES
##   OF ANY KIND, OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA
##   OR PROFITS, WHETHER OR NOT ADVISED OF THE POSSIBILITY OF DAMAGE, AND ON
##   ANY THEORY OF LIABILITY, ARISING OUT OF OR IN CONNECTION WITH THE USE
##   OR PERFORMANCE OF THIS SOFTWARE.
##
##############################################################################
## DESCRIPTION: ATACS produces a _min.vcd and a _max.vcd file when an
## failure is found during verification of an LHPN.  This file is an
## invalid VCD (voltage change dump) file.  This script processes the
## ATACS generated VCD file and properly formats it.  It also produces
## two other files, a .txt file and a .raw file.  The .txt file is
## suitable for plotting with gnuplot or Matlab.  The .raw file is an
## ASCII SPICE RAW file and can be plotted with any viewer that
## supports this format.
##############################################################################

### TODO ###
#Breakup main into functions

import sys, re, os.path, datetime

#Regular expressions
lSpaceR = re.compile("^\s+")
newLR = re.compile("\n+")
tSpaceR = re.compile("\s+$")
lNumR = re.compile("^r(-|\d)")

class ValueChange:
	"A value change including time, variable value, and variable id."
	def __init__(self):
		self.time = -1 #integer representing time
		self.value = None #real representing the variable value
		self.var = None #string representing the variable id
	def __init__(self,time,value,var):
		self.time = time
		self.value = value
		self.var = var
	def __str__(self):
		return "#"+str(self.time)+" ("+self.value+","+self.var+")"
	def __eq__(self,other):
		if self.time == other.time and self.value == other.value and self.var == other.var:
			return True
		else:
			return False
	def __ne__(self,other):
		return not (self == other)
	def __cmp__(self,other):
		if self.time < other.time:
			return -1
		elif self.time == other.time:
			return 0
		else:
			return 1

##############################################################################
# Remove leading & trailing space as well as trailing new line characters.
##############################################################################
def cleanLine(line):
	lineNS = re.sub(lSpaceR,"",line)
	lineNL = re.sub(newLR,"",lineNS)
	lineTS = re.sub(tSpaceR,"",lineNL)
	return lineTS

##############################################################################
##############################################################################
def main():
	if len(sys.argv) != 2:
		print "usage: processVCD.py <VCD file>"
		sys.exit()
		
	inputF = open(sys.argv[1], 'r')
	linesL = inputF.readlines()
	
	curTime = -1
	varsD = {}
	varsL = []
	varsNameL = []
	headerStr = ""
	header = True
	for i in range(len(linesL)):
		if linesL[i].startswith("#"):
			cLine = cleanLine(linesL[i])
			timeL = cLine.split("#")
			curTime = int(timeL[1])
			if curTime == 0:
				header = False
			#print "curTime:"+str(curTime)
		elif lNumR.match(linesL[i]):
			cLine = cleanLine(linesL[i])
			varIdL = cLine.split("*")
			#print "varIdL:"+str(varIdL)
			#print str(curTime)+":"+varIdL[0]+":"+varIdL[1]
			if varsD.has_key((curTime,varIdL[1])):
				vc = varsD.get((curTime,varIdL[1]))
				vc.value = varIdL[0]
			else:
				vc = ValueChange(curTime,varIdL[0],varIdL[1])
				varsD[(curTime,varIdL[1])] = vc
				#print "Added:"+str(vc)
		elif header:
			if linesL[i].startswith("$var"):
				cLine = cleanLine(linesL[i])
				varL = cLine.split(" ")
				idL = varL[3].split("*")
				#print "Var: "+idL[1]
				varsL.append(idL[1])
				varsNameL.append(varL[4])
			headerStr += linesL[i]
	inputF.close()
	
	vcL = varsD.values()
	vcL.sort()
	outputF = open(sys.argv[1], 'w+')
	outputF.write(headerStr)
	outTime = -1
	numPoints = 0
	for vc in vcL:
		#print "vc:"+str(vc)
		if outTime != vc.time:
			numPoints += 1
			if outTime == 0:
				outputF.write("$end\n")
				#print "$end"
			outTime = vc.time
			outputF.write("#"+str(outTime)+"\n")
			#print "#"+str(outTime)
			if outTime == 0:
				outputF.write("$dumpvars\n")
				#print "$dumpvars"
		outputF.write(vc.value+"*"+vc.var+"\n")
		#print vc.value+"*"+vc.var
	outputF.close()
	#print "numPoints:"+str(numPoints)

	curValD = {}
	baseFileL = os.path.splitext(sys.argv[1])
	txtFile = baseFileL[0]+".txt"
	outputF = open(txtFile, 'w')
	outTime = -1
	outputF.write("time ")
	for name in varsNameL:
		outputF.write(name+" ")
	outputF.write("\n")
	#print "vcL:"+str(vcL)
	for vc in vcL:
		#print "vc2:"+str(vc)+"--outTime:"+str(outTime)+" vc.time:"+str(vc.time)
		if vc.time == 0:
			#print "1-Adding var:val"+vc.var+":"+vc.value.lstrip('r')
			curValD[vc.var] = vc.value.lstrip('r')
			outTime = vc.time
		elif outTime != vc.time:
			outputF.write(str(outTime)+" ")
			for var in varsL:
				#print "for var:"+str(var)
				outputF.write(curValD[var].lstrip('r')+" ")
			outputF.write("\n")
			#print "2-Adding var:val"+vc.var+":"+vc.value.lstrip('r')
			curValD[vc.var] = vc.value.lstrip('r')
			outTime = vc.time
		else:
			#print "3-Adding var:val"+vc.var+":"+vc.value.lstrip('r')
			curValD[vc.var] = vc.value.lstrip('r')
	outputF.write(str(outTime)+" ")
	for var in varsL:
		outputF.write(curValD[var].lstrip('r')+" ")

	curValD = {}
	baseFileL = os.path.splitext(sys.argv[1])
	txtFile = baseFileL[0]+".raw"
	outputF = open(txtFile, 'w+')
	#Write out the rawfile header
	outputF.write("Title: Autogenerated raw file based on a Lema error trace.\n")
	dt = datetime.datetime.now()
	outputF.write("Date: "+dt.strftime("%a %b %d, %H:%M %Y")+"\n")
	outputF.write("Plotname: "+"Error trace\n")
	outputF.write("Flags: real ascii trap double\n")
	outputF.write("No. Variables: "+str(len(varsL)+1)+"\n")
	outputF.write("No. Points:    "+str(numPoints)+"\n")
	outputF.write("Variables: 0 time time\n")
	varNum = 1
	for name in varsNameL:
		outputF.write(" "+str(varNum)+" "+name+" voltage\n")
		varNum += 1
	outputF.write("Values:\n")
	outTime = -1
	pointNum = 0
	for vc in vcL:
		if vc.time == 0:
			curValD[vc.var] = vc.value.lstrip('r')
			outTime = vc.time
		elif outTime != vc.time:
			outputF.write("%d\t%.16e\n" % (pointNum,float(outTime)))
			pointNum += 1
			for var in varsL:
				outputF.write("\t%.16e\n" % (float(curValD[var].lstrip('r'))))
			curValD[vc.var] = vc.value.lstrip('r')
			outTime = vc.time
		else:
			curValD[vc.var] = vc.value.lstrip('r')
	outputF.write("%d\t%.16e\n" % (pointNum,float(outTime)))
	for var in varsL:
		outputF.write("\t%.16e\n" % (float(curValD[var].lstrip('r'))))
			
if __name__ == "__main__":
	main()
