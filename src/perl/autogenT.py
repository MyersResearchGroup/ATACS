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

### TODO ###
#Provide a method to specify different numbers of thresholds for different variables?
#Use properties to aid in the threshold generation?
#Update genBins to use the partial results list
#Add a brief comment regarding how to add optimization and cost functions

import re, os.path, cText, copy, sys
from optparse import OptionParser

#Regular expressions
numPointsR = re.compile("Points: ")
lSpaceR = re.compile("^\s+")
newLR = re.compile("\n+")
tSpaceR = re.compile("\s+$")

##############################################################################
# A class to hold the lists of places and transitions in the graph.
##############################################################################
class Variable:
	"A continuous variable in the system being modeled."
	def __init__(self,nameStr):
		self.name = nameStr #the name of the variable
	def __str__(self):
		retStr = self.name
		return retStr
## End Class Variable ########################################################

##############################################################################
# Remove leading & trailing space as well as trailing new line characters.
##############################################################################
def cleanLine(line):
	lineNS = re.sub(lSpaceR,"",line)
	lineNL = re.sub(newLR,"",lineNS)
	lineTS = re.sub(tSpaceR,"",lineNL)
	return lineTS

##############################################################################
# Creates a 2 dimensional array of lists rows x cols with each value
# initialized to initVal. 
##############################################################################
def create2Darray(rows,cols,initVal):
	newL = []
	for i in range(rows):
		initL = []
		for j in range(cols):
			initL.append(initVal)
		newL.append(initL)
	return newL

##############################################################################
# Create the list of variables.  All data files must have the same variables
# in the same order.
##############################################################################
def extractVars(datFile):
	varsL = []
	line = ""
	inputF = open(datFile, 'r')
	for i in range(3):
		line = inputF.readline()
	varNames = cleanLine(line)
	varNamesL = varNames.split(" ")
	for varStr in varNamesL:
		varsL.append(Variable(varStr))
	varsL[0].dmvc = False
	inputF.close()
	return varsL

##############################################################################
# Parse a .dat file ensuring that the varsL matches the global list.
##############################################################################
def parseDatFile(datFile,varsL):
	inputF = open(datFile, 'r')
	linesL = inputF.readlines()
	
	numPoints = -1
	pointStrL = numPointsR.split(cleanLine(linesL[1]))
	#print "pointStrL:"+str(pointStrL)
	numPoints = int(pointStrL[1])

	varNames = cleanLine(linesL[2])
	varNamesL = []
	varNamesL = varNames.split(" ")
	if len(varNamesL) == len(varsL):
		for i in range(len(varNamesL)):
			if varNamesL[i] != varsL[i].name:
				cStr = cText.cSetFg(cText.RED)
				cStr += "ERROR:"
				cStr += cText.cSetAttr(cText.NONE)
				print cStr+" Expected "+varsL[i].name+" in position "+str(i)+" but received "+varNamesL[i]+" in file: "+datFile
				sys.exit()
	else:
		cStr = cText.cSetFg(cText.RED)
		cStr += "ERROR:"
		cStr += cText.cSetAttr(cText.NONE)
		print cStr + " Expected "+str(len(varsL))+" variables but received "+str(len(varNamesL))+" in file: "+datFile
		sys.exit()
	
	datL = []
	for i in range(3,len(linesL)):
		valStrL = cleanLine(linesL[i]).split(" ")
		valL = []
		for s in valStrL:
			valL.append(float(s))
		datL.append(valL)
	inputF.close()
	#print "datL:"+str(datL)
	return datL, numPoints

##############################################################################
# Reorder the datL so each row is a list of data values for the ith
# variable.  Also build a list of the extreme values for each
# variable.
##############################################################################
def reorderDatL(datFileL,varsL):
	datValsL = []
	datValsExtremaL = []
	for i in range(len(varsL)):
		datValsL.append([])
		datValsExtremaL.append([])
		
	for i in range(len(datFileL)):
		#print "Working on: "+datFileL[i]
		datL,numPoints = parseDatFile(datFileL[i],varsL)
		#Reorder the datL array into something more reasonable and then
		#find the min, max for each variable
		for j in range(len(varsL)):
			for k in range(len(datL)):
				datValsL[j].append(datL[k][j])

		for j in range(1,len(varsL)):
			#print "datValsL:"+str(datValsL[j])
			datValsExtremaL[j] = (min(datValsL[j]),max(datValsL[j]))
			#print "datValsExtremaL["+str(j)+"]:"+str(datValsExtremaL[j])
	return datValsL, datValsExtremaL
			
##############################################################################
# Create an initial set of divisions based upon the number of bins and
# the extreme values for each variable.  These initial bins are
# evenly spaced.
##############################################################################
def initDivisionsL(datValsExtremaL,numThresholds,varsL):
	divisionsL = []
	for i in range(len(varsL)):
		divisionsL.append([])
	for i in range(1,len(varsL)):
		#print "i:"+str(i)+" "+str(datValsExtremaL[i])
		interval = float(abs(datValsExtremaL[i][1]-datValsExtremaL[i][0]) / (numThresholds+1))
		#print "interval:"+str(interval)
		for j in range(1,numThresholds+1):
			divisionsL[i].append(datValsExtremaL[i][0]+(interval*j))
	return divisionsL

##############################################################################
# Generate the bin encoding for each data point given the divisions.
##############################################################################
def genBins(datL,divisionsL):
	#print "datL:"+str(datL)
	#print "divisionsL:"+str(divisionsL)
	binsL = create2Darray(len(divisionsL),len(datL[0]),-1)
	for i in range(1,len(divisionsL)):
		for j in range(len(datL[0])):
			for k in range(len(divisionsL[i])):
				if (datL[i][j] <= divisionsL[i][k]):
					binsL[i][j] = k
					break
				else:
					#handles the case when the datum is in the highest bin
					#i.e. for 2 boundary numbers 3 bins are required
					#print "binsL["+str(i)+"]["+str(j)+"] = "+str(k+1)
					binsL[i][j] = k+1
	#print "binsL:"+str(binsL)
	return binsL

##############################################################################
# Determine if two bins are equivalent and return a Boolean.
##############################################################################
def equalBins(a,b,binsL,divisionsL):
	for i in range(1,len(divisionsL)):
		if binsL[i][a] != binsL[i][b]:
			return False
	return True

##############################################################################
# Generate the rates for each data point given the bin encodings.
##############################################################################
def genRates(divisionsL,datL,binsL):
	#Function notes: Rates can be calculated based on transitions or places.  If rates are calculated based on places they are calculated based on the change in the bin for the entire line.  If rates are calculated based on transitions they are calculated based on the change in the bin for each variable.  These two methods have different results and it appears that place based rates are more stable.  To help "smooth" out the rates there are several ways to modify the rate calcualation.  One way is to change the rateSampling variable.  This variable determines how long the bin must remain constant before a rate is calculated for that bin.  It can be a numerical value or "inf."  The "inf" setting only calculates the rate once per bin change.  You can invalidate bin changes of short length using the pathLength variable.  Any run of consecutive bins shorter than pathLength will not have its rate calculated.  The rate is also only calculated if the time values differ for the two points as I have seen examples where this is a problem.
	ratesL = create2Darray(len(divisionsL),len(datL[0]),'-')
	if placeRates:
		#Place based rate calculation
		if rateSampling == "inf":
			mark = 0
			for i in range(len(datL[0])):
				if i < mark:
					continue
				while mark < len(datL[0]) and equalBins(i,mark,binsL,divisionsL):
					mark += 1
				if datL[0][mark-1] != datL[0][i] and (mark-i) >= pathLength:
					for j in range(1,len(divisionsL)):
						ratesL[j][i] = (datL[j][mark-1]-datL[j][i])/(datL[0][mark-1]-datL[0][i])
		else:
			for i in range(len(datL[0])-rateSampling):
				calcRate = True
				for k in range(rateSampling):
					if not equalBins(i,i+k,binsL,divisionsL):
						calcRate = False
						break
				if calcRate and datL[0][i+rateSampling] != datL[0][i]:
					for j in range(1,len(divisionsL)):
						ratesL[j][i] = (datL[j][i+rateSampling]-datL[j][i])/(datL[0][i+rateSampling]-datL[0][i])
	else:
		cStr = cText.cSetFg(cText.YELLOW)
		cStr += "WARNING:"
		cStr += cText.cSetAttr(cText.NONE)
		print cStr+"this feature has not been tested."
		#Transition based rate calculation
		if rateSampling == "inf":
			for j in range(1,len(divisionsL)):
				mark = 0
				for i in range(len(datL[0])):
					if i < mark:
						continue
					while mark < len(datL[0]) and equalBins(i,mark,binsL,divisionsL):
						mark = mark + 1
					if datL[0][mark-1] != datL[0][i]:
						ratesL[j][i] = (datL[j][mark-1]-datL[j][i])/(datL[0][mark-1]-datL[0][i])
		else:
			for i in range(len(datL[0])-rateSampling):
				for j in range(1,len(divisionsL)):
					calcRate = True
					for k in range(rateSampling):
						if not equalBins(i,i+k,binsL,divisionsL):
							calcRate = False
							break
					if calcRate and datL[0][i+rateSampling] != datL[0][i]:
						ratesL[j][i] = (datL[j][i+rateSampling]-datL[j][i])/(datL[0][i+rateSampling]-datL[0][i])
	return ratesL

##############################################################################
# Return the minimum rate for a given rate list.
##############################################################################
def minRate(ratesL):
 	#Remove the characters from the list before doing the comparison
	cmpL = []
	for i in range(len(ratesL)):
		if ratesL[i] != '-':
			cmpL.append(ratesL[i])
	if len(cmpL) > 0:
		return min(cmpL)
	else:
		return "-"
	
##############################################################################
# Return the maximum rate for a given rate list.
##############################################################################
def maxRate(ratesL):
  #Remove the characters from the list before doing the comparison
	cmpL = []
	for i in range(len(ratesL)):
		if ratesL[i] != '-':
			cmpL.append(ratesL[i])
	if len(cmpL) > 0:
		return max(cmpL)
	else:
		return "-"

##############################################################################
# Give a score for the even distribution of points for all
# variables. 0 is the optimal score.
##############################################################################
def pointDistCost(datValsL,divisionsL,resL=[],updateVar=-1):
	total = 0
	if updateVar == 0:
		for i in range(len(divisionsL)):
			resL.append(0)
		#Fill up resL
		for i in range(1,len(divisionsL)):
			points = pointDistCostVar(datValsL[i],divisionsL[i])
			total += points
			resL[i] = points
	elif updateVar > 0:
		#Incrementally calculate a total change
		resL[updateVar] = pointDistCostVar(datValsL[updateVar],
																			 divisionsL[updateVar])
		for i in resL:
			total += i
	else:
		#Do a full calculation from scratch
		for i in range(1,len(divisionsL)):
			total += pointDistCostVar(datValsL[i],divisionsL[i])
	return total

##############################################################################
# Give a score for the even distribution of points for an individual
# variable. 0 is the optimal score.
##############################################################################
def pointDistCostVar(datValsL,divisionsL):
	optPointsPerBin = len(datValsL)/(len(divisionsL)+1)
	#print "optPointsPerBin:"+str(optPointsPerBin)
	pointsPerBinL = []
	for i in range(len(divisionsL)+1):
		pointsPerBinL.append(0)
	for i in range(len(datValsL)):
		top = True
		for j in range(len(divisionsL)):
			if datValsL[i] <= divisionsL[j]:
				pointsPerBinL[j] += 1
				top = False
				break
		if top:
			pointsPerBinL[len(divisionsL)] += 1

	#print "pointsPerBinL:"+str(pointsPerBinL)
	score = 0
	for points in pointsPerBinL:
		score += abs(points - optPointsPerBin)
	return score

##############################################################################
# Give a score for the range of rates for all variables.  0 is the
# optimal score.
##############################################################################
def rateRangeCost(datValsL,divisionsL,resL=[],updateVar=-1):
	total = 0
	binsL = genBins(datValsL,divisionsL)
	ratesL = genRates(divisionsL,datValsL,binsL)
	#print "ratesL:"+str(ratesL)
	for i in range(1,len(divisionsL)):
		maxR = maxRate(ratesL[i])
		minR = minRate(ratesL[i])	
		total += abs(maxR-minR)
	return total

##############################################################################
# Look for the optimal thresholds using a greedy algorithm.
##############################################################################
def greedyOpt(divisionsL,datValsL,datValsExtremaL):
	resL = [] #Used to keep partial results for cost functions
	updateVar = 0 #The variable that was updated to help optimize cost function recalculation
	bestDivisionsL = copy.deepcopy(divisionsL)
	bestCost = costFunc(datValsL,divisionsL,resL,updateVar)
	numMoves = 0
	print "Starting optimization..."
	while numMoves < iterations:
		for i in range(1,len(divisionsL)):
			for j in range(len(divisionsL[i])):
				#move left
				if j == 0:
					distance = abs(divisionsL[i][j] - datValsExtremaL[i][0])/2
				else:
					distance = abs(divisionsL[i][j] - divisionsL[i][j-1])/2
				newDivisionsL = copy.deepcopy(divisionsL)
				newDivisionsL[i][j] -= distance
				newCost = costFunc(datValsL,newDivisionsL,resL,i)
				numMoves += 1
				if numMoves % 500 == 0:
					print str(numMoves)+"/"+str(iterations)
				if newCost < bestCost:
					bestCost = newCost
					divisionsL = newDivisionsL
				else:
					#move right
					if j == len(divisionsL[i])-1:
						distance = abs(datValsExtremaL[i][1] - divisionsL[i][j])/2
					else:
						distance = abs(divisionsL[i][j+1] - divisionsL[i][j])/2
					newDivisionsL = copy.deepcopy(divisionsL)
					newDivisionsL[i][j] += distance
					newCost = costFunc(datValsL,newDivisionsL,resL,i)
					numMoves += 1
					if numMoves % 500 == 0:
						print str(numMoves)+"/"+str(iterations)
					if newCost < bestCost:
						bestCost = newCost
						divisionsL = newDivisionsL
				if numMoves > iterations:
					return divisionsL
	return divisionsL

##############################################################################
# Look for the optimal thresholds using a greedy algorithm for the
##############################################################################
def writeBinsFile(varsL,divisionsL,binsFile):
	outputF = open(binsFile, 'w')
	for i in range(1,len(varsL)):
		outputF.write(varsL[i].name)
		for div in divisionsL[i]:
			outputF.write(" "+str(div))
		outputF.write("\n")
	outputF.close()

##############################################################################
##############################################################################
def main():
	global numThresholds
	global iterations
	global optFunc
	global costFunc
	
	usage = "usage: %prog [options] datFile1 ... datFileN"
	parser = OptionParser(usage=usage)
	parser.set_defaults(binsFile=None,numThresholds=None,costF="p",optF="g")
	parser.add_option("-b", "--bins", action="store", dest="binsFile", help="The name of the .bins file to be created.  If this is not provided the basename of the first input data file is used.")
	parser.add_option("-t", "--thresholds", action="store", dest="numThresholds", help="The number of thresholds to create during autogeneration.")
	parser.add_option("-i", "--iterations", action="store", dest="iterations", help="The number of iterations of the optimization algorithm to run.")
	parser.add_option("-c", "--cost", action="store", dest="costF", help="The cost function to use: r - Minimize the distance between rates; p - Average the number of points in each bin.")
	parser.add_option("-o", "--optimization", action="store", dest="optF", help="The optimization function to use: g - Greedy algorithm.")
	
	(options, args) = parser.parse_args()

	if len(args) > 0:
		datFileL = args
	else:
		print "At least one data file is required."
		parser.print_help()
		sys.exit()

	if not options.binsFile:
		baseFileL = os.path.splitext(datFileL[0])
		binsFile = baseFileL[0]+".bins"
	else:
		binsFile = options.binsFile

	if options.numThresholds:
		numThresholds = int(options.numThresholds)

	if options.iterations:
		iterations = int(options.iterations)

	if options.optF == "g":
		optFunc = greedyOpt
	else:
		cStr = cText.cSetFg(cText.RED)
		cStr += "ERROR:"
		cStr += cText.cSetAttr(cText.NONE)
		print cStr + options.optFunc + " is not a valid option for the optimization function."
		parser.print_help()
		sys.exit()

	if options.costF == "r":
		costFunc = rateRangeCost
	elif options.costF == "p":
		costFunc = pointDistCost
	else:
		cStr = cText.cSetFg(cText.RED)
		cStr += "ERROR:"
		cStr += cText.cSetAttr(cText.NONE)
		print cStr + options.costFunc + " is not a valid option for the cost function."
		parser.print_help()
		sys.exit()

	varsL = extractVars(datFileL[0])
	datValsL, datValsExtremaL = reorderDatL(datFileL,varsL)
	divisionsL = initDivisionsL(datValsExtremaL,numThresholds,varsL)
	print "Iterations: "+str(iterations)
	print "Optimization function: "+optFunc.func_name
	print "Cost function: "+costFunc.func_name
	print "Initial divisionsL:"+str(divisionsL)
	print "Initial score:"+str(costFunc(datValsL,divisionsL))
	divisionsL = optFunc(divisionsL,datValsL,datValsExtremaL)
	print "Final divisionsL:"+str(divisionsL)
	print "Final score:"+str(costFunc(datValsL,divisionsL))
	writeBinsFile(varsL,divisionsL,binsFile)
		
##############################################################################
##############################################################################

###########
# Globals #
###########
numThresholds = 2 #the default number of thresholds to create...it can be overridden from the command line
iterations = 10000 #the default number of iterations
rateSampling = "inf" #How many points should exist between the sampling of different rates..."inf" samples once/threshold
pathLength = 10 #For "inf" rate sampling the number of time points that a "run" must persist for the rate to be calculated.  This is just another parameter to help with the data smoothing.
placeRates = True #When true the script calculates rates based on places.  When false it calculates rates based on transitions although there is very little infrastructure for transition based rates and it isn't well tested.
optFunc = None #The name of the optimization function that will be used.
costFunc = None #The name of the cost function that will be used in the optimization function.


if __name__ == "__main__":
	main()
