#!/bin/bash


echo "Make sure to set required error bound in sz.config"

if [ "$#" -ne 7 ]; then
    echo "Usage: <SZ executable path> <sz.config path> <sampled dataset path> <number of data elements in sampled dataset> <sampled dataset size> <number of data elements in original dataset> <original dataset size>"
    echo "Sample command with Hurricane dataset: ./run_sz_estimation.sh <Root path>/baselines/b-IPDPS18/SZ/build/bin/testdouble_compress <Root path>/baselines/b-IPDPS18/SZ/build/bin/sz.config <Root path>/baselines/b-IPDPS18/SZ/build/bin/Vf48.d64.bin.sample 250000 2000000 25000000 200000000"
    echo "Sample command with Miranda dataset: ./run_sz_estimation.sh <Root path>/baselines/b-IPDPS18/SZ/build/bin/testdouble_compress <Root path>/baselines/b-IPDPS18/SZ/build/bin/sz.config <Root path>/baselines/b-IPDPS18/SZ/build/bin/velocityy.d64.sample 377480 3019840 37748736 301989888"
    exit 1
fi

SZExecPath=$1
SZConfigPath=$2
sampleDatasetPath=$3
sampleDataCount=$4
sampleFileSize=$5
originalDataCount=$6
originalFileSize=$7


cd ../SZ/build/bin
rm output.sz_execution.log
# start_time=$SECONDS
# start_time=$(date +%s)
TIMEFORMAT='Time to extract SZ compression metrics: %5R seconds.'
time {
$SZExecPath $SZConfigPath $sampleDatasetPath $sampleDataCount &> output.sz_execution.log
}
# end_time=$(date +%s)
# curElapsed=$(( SECONDS - start_time ))
# szMetricsElapsed=$(( end_time - start_time ))
# totalElapsed=$szMetricsElapsed

rm szCompressionMetrics.log gaussInput.txt
python extractInformation.py output.sz_execution.log szCompressionMetrics.log $sampleFileSize $originalDataCount
rm ../../../gauss/gaussInput.txt
cp gaussInput.txt ../../../gauss


cd ../../../gauss
rm output_gauss_model.log
# start_time=$SECONDS
# start_time=$(date +%s)
TIMEFORMAT='Time to run Gauss model: %5R seconds.'
time {
./gauss_run.sh gaussInput.txt $originalDataCount &> output_gauss_model.log
# end_time=$(date +%s)
}
# curElapsed=$(( SECONDS - start_time ))
# gaussModelElapsed=$(( end_time - start_time ))
# totalElapsed=$(($totalElapsed + $gaussModelElapsed))


cd ../sz-compression-ratio-estimation
python estimateCR.py ../SZ/build/bin/szCompressionMetrics.log ../gauss/output_gauss_model.log $originalFileSize

# echo "Time to extract SZ compression metrics:" $szMetricsElapsed " sec"
# echo "Time to run Gauss model:" $gaussModelElapsed " sec"
# echo "Total time to estimate compression ratio is:" $totalElapsed " sec"
