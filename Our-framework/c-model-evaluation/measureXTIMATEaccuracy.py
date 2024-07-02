import numpy as np
import os, sys, csv
import math

EPSILON=0.000000001

predictedResultsFilePath = str(sys.argv[1])
measuredCRatioResultsFilePath = str(sys.argv[2])

predictedResultsTuples = []
measuredCRatioResultsTuples = []

with open(predictedResultsFilePath, 'r') as f:
    csvreader = csv.reader(f)
    for row in csvreader:
        datasetFieldName = str(row[0])
        err = float(row[1])
        predCRatio = float(row[2])

        predictedResultsTuples.append((datasetFieldName, err, predCRatio))
    

with open(measuredCRatioResultsFilePath, 'r') as f:
    csvreader = csv.reader(f)
    for row in csvreader:
        err = float(row[0])
        measuredCRatio = float(row[1])

        measuredCRatioResultsTuples.append((err, measuredCRatio))


# if len(predictedResultsTuples) != len(measuredCRatioResultsTuples) :
#     print("Number of rows in predicted results files and measured results files are not equal! So, cannot be compared! Please correct the errors.")
#     exit(1)

os.system("mkdir outputs")
accwriter = csv.writer(open("outputs/XTIMATE_accuracy_comparison_with_MCRs.csv", 'w'))

avgEstimationError = 0.0
numElements = 0.0

for (datasetFieldName, errBound1, predCRatio) in predictedResultsTuples:
    for (errBound2, measuredCRatio) in measuredCRatioResultsTuples:
        if (math.fabs(errBound1 - errBound2) < EPSILON):
            accwriter.writerow([datasetFieldName, errBound2, measuredCRatio, predCRatio, (100.0 * math.fabs(measuredCRatio - predCRatio)) / measuredCRatio])
            avgEstimationError += (100.0 * math.fabs(measuredCRatio - predCRatio)) / measuredCRatio
            numElements += 1.0;

with open ("outputs/XTIMATE_average_estimation_error.log", 'w') as f:
    f.write("Average estimation error by XTIMATE: %.2f (in percentage)\n"%(avgEstimationError/numElements))
