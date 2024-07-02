import numpy as np
import os, sys, csv
import math

EPSILON=0.000000001

testingDatasetFeatureExtractionTimeFilePath = str(sys.argv[1])
mlModelInferenceTimeFilePath = str(sys.argv[2])


avgFeatureExtractionTimeacrossErrorBounds = 0.0
numofElements = 0.0

with open(testingDatasetFeatureExtractionTimeFilePath, 'r') as f:
    csvreader = csv.reader(f)
    for row in csvreader:
        datasetFieldName = str(row[0])
        err = float(row[1])
        extractionTime = float(row[2])

        avgFeatureExtractionTimeacrossErrorBounds += extractionTime
        numofElements += 1

avgFeatureExtractionTimeacrossErrorBounds /= numofElements

mlModelAvgInferenceTime = 0.0
with open(mlModelInferenceTimeFilePath, 'r') as f:
    lines = f.readlines()

    for line in lines:
        line = line.strip()

        if "Average inference time" in line:
            mlModelAvgInferenceTime = float(line.split(":")[1].strip())

os.system("mkdir outputs")
with open("outputs/XTIMATE_performance.log", 'w') as f:
    f.write("Average feature extraction time by XTIMATE: %.5f sec\n"%(avgFeatureExtractionTimeacrossErrorBounds))
    f.write("Average inference time by XTIMATE: %.5f sec\n"%(mlModelAvgInferenceTime))
    f.write("Total online analysis time by XTIMATE: %.5f sec\n"%(avgFeatureExtractionTimeacrossErrorBounds + mlModelAvgInferenceTime ))