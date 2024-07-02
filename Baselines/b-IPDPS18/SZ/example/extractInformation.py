import os, sys, csv

inputFilePath = str(sys.argv[1])
outputFilePath = str(sys.argv[2])
FullFileSize = int(sys.argv[3])
originalPointCnt = int(sys.argv[4])

lines = []

with open(inputFilePath, 'r') as f:
	lines = f.readlines()


pointCnt = 0
quantIntv = 0
quantAvg = 0.0
quantStd = 0.0
nodeCnt = 0
hitRatio = 0.00
treeSize = 0
encodeSize = 0
outlierCnt = 0
outlierSize = 0
totalSize = 0

os.system("rm " + outputFilePath)
outputFile = open(outputFilePath, 'a')

gaussOutputFile = open("gaussInput.txt", "w")
for line in lines:
	line = line.strip()
	
	if ("quantization_intervals :" in line):
		quantIntv = int(line.split("quantization_intervals :")[1].strip())

	elif ("type max =" in line) and ("avg =" in  line) and ("std variance =" in line):
		quantAvg = float(line.split("avg =")[1].strip().split(",")[0].strip())	
		quantStd = float(line.split("std variance =")[1].strip())


	elif ("dataLength =" in line) and ("exactDataNum" in line) and ("curve fitting hit ratio" in line):
		#try:
		pointCnt = int(line.split("dataLength =")[1].strip().split(",")[0].strip())
		outlierCnt = int(line.split("exactDataNum =")[1].strip().split(",")[0].strip())
		hitRatio = float(line.split("curve fitting hit ratio =")[1].strip())
		#print(line.split("curve fitting hit ratio =")[1].strip())	

	elif ("nodeCount" in line) :
		nodeCnt = int(line.split(" ")[1].strip())

	elif ("i =" in line) and ("state =" in line) and ("code[state]" in line):
		quantizedCode = int(line.split("state =")[1].split(":")[0].strip())
		if (quantizedCode > 0):
			gaussOutputFile.write(str(quantizedCode) + "\n")

	
	elif ("treeByteSize=" in line) and ("enCodeSize=" in line):
		treeSize = int(line.split("treeByteSize=")[1].strip().split("+")[0].strip())
		encodeSize = int(line.split("enCodeSize=")[1].strip().split("bytes")[0].strip())

	
	elif ("before gzip the byte size is" in line):
		totalSize = int(line.split("before gzip the byte size is")[1].strip())


outlierSize = outlierCnt * 3

gaussOutputFile.close()


with open(outputFilePath, 'w') as f:
	f.write("[Sample] PointCnt: " + str(pointCnt) + "\n")
	f.write("[Sample] QuantIntv: " + str(quantIntv) + "\n")
	f.write("[Sample] NodeCnt: " + str(nodeCnt) + "\n")
	f.write("[Sample] HitRatio: " + str(hitRatio) + "\n")
	f.write("[Sample] TreeSize: " + str(treeSize) +  "\n")
	f.write("[Sample] EncodeSize: " + str(encodeSize) + "\n")
	f.write("[Sample] OutlierCnt: " + str(outlierCnt) + "\n")
	f.write("[Sample] OutlierSize: " + str(outlierSize) + "\n")
	f.write("[Sample] TotalSize: " + str(totalSize) + "\n")
	f.write("[Sample] Compression Ratio: " + str((1.0 * FullFileSize) / (1.0 * totalSize)) + "\n")
	f.write("[Full] OutlierCnt: " + str(int(1.0 * originalPointCnt - hitRatio * originalPointCnt)))
