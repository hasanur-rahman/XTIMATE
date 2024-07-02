import os, csv, sys, math

EPSILON = 0.000001

szEstimationLog = str(sys.argv[1])
gaussModelEstimationLog = str(sys.argv[2])

print("Usage: szEstimationLog gaussModelEstimationLog")

outlierCntS = 0
outlierSizeS = 0
outlierCntF = 0
nodeCountS = 0
nodeCountF = 0
treeSizeS = 0
encodeSizeS = 0
originalFileSize = float(sys.argv[3])

with open(szEstimationLog, 'r') as f:
    lines = f.readlines()

    for line in lines:
        if "[Sample] OutlierCnt:" in line:
            outlierCntS = float(line.strip().split(":")[1].strip())
        if "[Sample] OutlierSize:" in line:
            outlierSizeS = float(line.strip().split(":")[1].strip())
        if "[Full] OutlierCnt:" in line:
            outlierCntF = float(line.strip().split(":")[1].strip())
        if "[Sample] NodeCnt:" in line:
            nodeCountS = float(line.strip().split(":")[1].strip())
        if "[Sample] TreeSize:" in line:
            treeSizeS = float(line.strip().split(":")[1].strip())
        if "[Sample] EncodeSize:" in line:
            encodeSizeS = float(line.strip().split(":")[1].strip())


with open(gaussModelEstimationLog, 'r') as f:
    lines = f.readlines()

    for line in lines:
        if "NodeCnt" in line:
            nodeCountF = float(line.strip().split(" ")[1].strip()) 


if (outlierCntS < EPSILON) or (outlierSizeS < EPSILON) or (outlierCntF < EPSILON) or (nodeCountS < EPSILON) or (nodeCountF < EPSILON) or (treeSizeS < EPSILON) or (encodeSizeS < EPSILON) or (originalFileSize < EPSILON) :
    print("All of the parameters are expected to be non-zero!")
    exit(1)


outlierSizeF =  int(( (1.0 * outlierSizeS) / (1.0 * outlierCntS) ) * (1.0 * outlierCntF))
treeSizeF = int(((1.0 * treeSizeS) / (1.0 * nodeCountS) ) * (1.0 * nodeCountF))
encodeSizeF = int(((1.0 * encodeSizeS) / (math.log2(nodeCountS))) * (1.0 * math.log2(nodeCountF)))

print("Outlier Size Full: " + str(outlierSizeF))
print("Tree Size Full: " + str(treeSizeF))
print("Encode Size Full: " + str(encodeSizeF))
print("[Full] Total Compressed Size: " + str(outlierSizeF + treeSizeF + encodeSizeF))
print("Compression ratio: {:.4f}".format((1.0 * originalFileSize)  / (1.0 * (outlierSizeF + treeSizeF + encodeSizeF))))
