#!/bin/bash


if [ "$#" -ne 8 ]; then
    echo "Usage: Need to probide 8 arguments <training datasets dir path> <testing dataset (only one dataset should be in the dir) dir path> <training output dir path> <testing output dir path> <compressor name and version (SZv3 or SZv1.4 or ZFPv0.5.0 or MGARD+)> <number of dimensions in datasets> <Dimension sizes (e.g., "256 384 384")> <A csv file containing user-specified error bounds>"
    echo "./run_workflow.sh ../datasets/Exaalt/single-precision/training ../datasets/Exaalt/single-precision/testing XTIMATE-outputs/training-output XTIMATE-outputs/testing-output SZv3 1 '2869440' XTIMATE-inputs/user_specified_error_bounds.csv"
    exit 1
fi

curDir=$PWD

# inputs
trainingDatasetsFolderPath=$1
testingDatasetsFolderPath=$2
trainingOutputFolderPath=$3
testingOutputFolderPath=$4
compressorSpecification=$5
numDims=$6
dimsSizes=$7
userSpecifiedErrorBoundsFilePath=$8


trainingDatasetsFolderPath="${curDir}/${1}"
testingDatasetsFolderPath="${curDir}/${2}"
trainingOutputFolderPath="${curDir}/${3}"
testingOutputFolderPath="${curDir}/${4}"
userSpecifiedErrorBoundsFilePath="${curDir}/${8}"

mkdir ${trainingOutputFolderPath} ${testingOutputFolderPath}
rm -r ${trainingOutputFolderPath}/* ${testingOutputFolderPath}/* 
mkdir c-model-evaluation/outputs
rm -r c-model-evaluation/outputs/*


cd a-feature-extraction-and-optimization-module
./build_cmake.sh
./clean_cmake_caches.sh
cd build

IFS=' ' read -r -a array <<< "${dimsSizes}"

# XTIMATE training
printf "\n\nXTIMATE training starts ...\n"
if [[ $compressorSpecification == "SZv3" ]]; then
    if [[ $numDims = 1 ]]; then
        ./estimateRatio -i $trainingDatasetsFolderPath -o $trainingOutputFolderPath -c $compressorSpecification -f -$numDims ${array[0]} -m 2 -e dummy.csv
    elif [[ $numDims = 2 ]]; then
        ./estimateRatio -i $trainingDatasetsFolderPath -o $trainingOutputFolderPath -c $compressorSpecification -f -$numDims ${array[0]} ${array[1]} -m 2 -e dummy.csv
    elif [[ $numDims = 3 ]]; then
        ./estimateRatio -i $trainingDatasetsFolderPath -o $trainingOutputFolderPath -c $compressorSpecification -f -$numDims ${array[0]} ${array[1]} ${array[2]} -m 2 -e dummy.csv
    else 
        echo "Number of dimensions should be 1, 2, or 3."
        exit 1
    fi
elif [[ $compressorSpecification == "SZv1.4" ]]; then
    if [[ $numDims = 1 ]]; then
        ./estimateRatio -i $trainingDatasetsFolderPath -o $trainingOutputFolderPath -c $compressorSpecification -d -$numDims ${array[0]} -m 2 -e dummy.csv
    elif [[ $numDims = 2 ]]; then
        ./estimateRatio -i $trainingDatasetsFolderPath -o $trainingOutputFolderPath -c $compressorSpecification -d -$numDims ${array[0]} ${array[1]} -m 2 -e dummy.csv
    elif [[ $numDims = 3 ]]; then
        ./estimateRatio -i $trainingDatasetsFolderPath -o $trainingOutputFolderPath -c $compressorSpecification -d -$numDims ${array[0]} ${array[1]} ${array[2]} -m 2 -e dummy.csv
    else 
        echo "Number of dimensions should be 1, 2, or 3."
        exit 1
    fi
elif [[ $compressorSpecification == "ZFPv0.5.0" ]]; then
    if [[ $numDims = 1 ]]; then
        ./estimateRatio -i $trainingDatasetsFolderPath -o $trainingOutputFolderPath -c $compressorSpecification -d -$numDims ${array[0]} -m 2 -e dummy.csv
    elif [[ $numDims = 2 ]]; then
        ./estimateRatio -i $trainingDatasetsFolderPath -o $trainingOutputFolderPath -c $compressorSpecification -d -$numDims ${array[0]} ${array[1]} -m 2 -e dummy.csv
    elif [[ $numDims = 3 ]]; then
        ./estimateRatio -i $trainingDatasetsFolderPath -o $trainingOutputFolderPath -c $compressorSpecification -d -$numDims ${array[0]} ${array[1]} ${array[2]} -m 2 -e dummy.csv
    else 
        echo "Number of dimensions should be 1, 2, or 3."
        exit 1
    fi
elif [[ $compressorSpecification == "MGARD+" ]]; then
    if [[ $numDims = 1 ]]; then
        ./estimateRatio -i $trainingDatasetsFolderPath -o $trainingOutputFolderPath -c $compressorSpecification -f -$numDims ${array[0]} -m 2 -e dummy.csv
    elif [[ $numDims = 2 ]]; then
        ./estimateRatio -i $trainingDatasetsFolderPath -o $trainingOutputFolderPath -c $compressorSpecification -f -$numDims ${array[0]} ${array[1]} -m 2 -e dummy.csv
    elif [[ $numDims = 3 ]]; then
        ./estimateRatio -i $trainingDatasetsFolderPath -o $trainingOutputFolderPath -c $compressorSpecification -f -$numDims ${array[0]} ${array[1]} ${array[2]} -m 2 -e dummy.csv
    else 
        echo "Number of dimensions should be 1, 2, or 3."
        exit 1
    fi
else 
    echo "Specified compressor is incorrect! You must need to specify the compressor and its version!\n"
    exit 1
fi
printf "XTIMATE training ends\n\n"


IFS=' ' read -r -a array <<< "${dimsSizes}"

# XTIMATE inference
printf "\n\nXTIMATE inference starts ...\n"
if [[ $compressorSpecification == "SZv3" ]]; then
    if [[ $numDims = 1 ]]; then
        ./estimateRatio -i $testingDatasetsFolderPath -o $testingOutputFolderPath -c $compressorSpecification -f -$numDims ${array[0]} -m 0 -e $userSpecifiedErrorBoundsFilePath
        ./estimateRatio -i $testingDatasetsFolderPath -o $testingOutputFolderPath -c $compressorSpecification -f -$numDims ${array[0]} -m 1 -e $userSpecifiedErrorBoundsFilePath
    elif [[ $numDims = 2 ]]; then
        ./estimateRatio -i $testingDatasetsFolderPath -o $testingOutputFolderPath -c $compressorSpecification -f -$numDims ${array[0]} ${array[1]} -m 0 -e $userSpecifiedErrorBoundsFilePath
        ./estimateRatio -i $testingDatasetsFolderPath -o $testingOutputFolderPath -c $compressorSpecification -f -$numDims ${array[0]} ${array[1]} -m 1 -e $userSpecifiedErrorBoundsFilePath
    elif [[ $numDims = 3 ]]; then
        ./estimateRatio -i $testingDatasetsFolderPath -o $testingOutputFolderPath -c $compressorSpecification -f -$numDims ${array[0]} ${array[1]} ${array[2]} -m 0 -e $userSpecifiedErrorBoundsFilePath
        ./estimateRatio -i $testingDatasetsFolderPath -o $testingOutputFolderPath -c $compressorSpecification -f -$numDims ${array[0]} ${array[1]} ${array[2]} -m 1 -e $userSpecifiedErrorBoundsFilePath
    else 
        echo "Number of dimensions should be 1, 2, or 3."
        exit 1
    fi
elif [[ $compressorSpecification == "SZv1.4" ]]; then
    if [[ $numDims = 1 ]]; then
        ./estimateRatio -i $testingDatasetsFolderPath -o $testingOutputFolderPath -c $compressorSpecification -d -$numDims ${array[0]} -m 0 -e $userSpecifiedErrorBoundsFilePath
        ./estimateRatio -i $testingDatasetsFolderPath -o $testingOutputFolderPath -c $compressorSpecification -d -$numDims ${array[0]} -m 1 -e $userSpecifiedErrorBoundsFilePath
    elif [[ $numDims = 2 ]]; then
        ./estimateRatio -i $testingDatasetsFolderPath -o $testingOutputFolderPath -c $compressorSpecification -d -$numDims ${array[0]} ${array[1]} -m 0 -e $userSpecifiedErrorBoundsFilePath
        ./estimateRatio -i $testingDatasetsFolderPath -o $testingOutputFolderPath -c $compressorSpecification -d -$numDims ${array[0]} ${array[1]} -m 1 -e $userSpecifiedErrorBoundsFilePath
    elif [[ $numDims = 3 ]]; then
        ./estimateRatio -i $testingDatasetsFolderPath -o $testingOutputFolderPath -c $compressorSpecification -d -$numDims ${array[0]} ${array[1]} ${array[2]} -m 0 -e $userSpecifiedErrorBoundsFilePath
        ./estimateRatio -i $testingDatasetsFolderPath -o $testingOutputFolderPath -c $compressorSpecification -d -$numDims ${array[0]} ${array[1]} ${array[2]} -m 1 -e $userSpecifiedErrorBoundsFilePath
    else 
        echo "Number of dimensions should be 1, 2, or 3."
        exit 1
    fi
elif [[ $compressorSpecification == "ZFPv0.5.0" ]]; then
    if [[ $numDims = 1 ]]; then
        ./estimateRatio -i $testingDatasetsFolderPath -o $testingOutputFolderPath -c $compressorSpecification -d -$numDims ${array[0]} -m 0 -e $userSpecifiedErrorBoundsFilePath
        ./estimateRatio -i $testingDatasetsFolderPath -o $testingOutputFolderPath -c $compressorSpecification -d -$numDims ${array[0]} -m 1 -e $userSpecifiedErrorBoundsFilePath
    elif [[ $numDims = 2 ]]; then
        ./estimateRatio -i $testingDatasetsFolderPath -o $testingOutputFolderPath -c $compressorSpecification -d -$numDims ${array[0]} ${array[1]} -m 0 -e $userSpecifiedErrorBoundsFilePath
        ./estimateRatio -i $testingDatasetsFolderPath -o $testingOutputFolderPath -c $compressorSpecification -d -$numDims ${array[0]} ${array[1]} -m 1 -e $userSpecifiedErrorBoundsFilePath
    elif [[ $numDims = 3 ]]; then
        ./estimateRatio -i $testingDatasetsFolderPath -o $testingOutputFolderPath -c $compressorSpecification -d -$numDims ${array[0]} ${array[1]} ${array[2]} -m 0 -e $userSpecifiedErrorBoundsFilePath
        ./estimateRatio -i $testingDatasetsFolderPath -o $testingOutputFolderPath -c $compressorSpecification -d -$numDims ${array[0]} ${array[1]} ${array[2]} -m 1 -e $userSpecifiedErrorBoundsFilePath
    else 
        echo "Number of dimensions should be 1, 2, or 3."
        exit 1
    fi
elif [[ $compressorSpecification == "MGARD+" ]]; then
    if [[ $numDims = 1 ]]; then
        ./estimateRatio -i $testingDatasetsFolderPath -o $testingOutputFolderPath -c $compressorSpecification -f -$numDims ${array[0]} -m 0 -e $userSpecifiedErrorBoundsFilePath
        ./estimateRatio -i $testingDatasetsFolderPath -o $testingOutputFolderPath -c $compressorSpecification -f -$numDims ${array[0]} -m 1 -e $userSpecifiedErrorBoundsFilePath
    elif [[ $numDims = 2 ]]; then
        ./estimateRatio -i $testingDatasetsFolderPath -o $testingOutputFolderPath -c $compressorSpecification -f -$numDims ${array[0]} ${array[1]} -m 0 -e $userSpecifiedErrorBoundsFilePath
        ./estimateRatio -i $testingDatasetsFolderPath -o $testingOutputFolderPath -c $compressorSpecification -f -$numDims ${array[0]} ${array[1]} -m 1 -e $userSpecifiedErrorBoundsFilePath
    elif [[ $numDims = 3 ]]; then
        ./estimateRatio -i $testingDatasetsFolderPath -o $testingOutputFolderPath -c $compressorSpecification -f -$numDims ${array[0]} ${array[1]} ${array[2]} -m 0 -e $userSpecifiedErrorBoundsFilePath
        ./estimateRatio -i $testingDatasetsFolderPath -o $testingOutputFolderPath -c $compressorSpecification -f -$numDims ${array[0]} ${array[1]} ${array[2]} -m 1 -e $userSpecifiedErrorBoundsFilePath
    else 
        echo "Number of dimensions should be 1, 2, or 3."
        exit 1
    fi
else 
    echo "Specified compressor is incorrect! You must need to specify the compressor and its version!\n"
    exit 1
fi
printf "XTIMATE inference ends\n\n"

cd ../..

cd b-ml-module
mkdir ../XTIMATE-outputs/workflow-output
rm -r ../XTIMATE-outputs/workflow-output/*
python randomForestRegressor.py ../XTIMATE-outputs/training-output/training_data.csv ../XTIMATE-outputs/testing-output/testing_data.csv ../XTIMATE-outputs/workflow-output
cd ..

cd c-model-evaluation
mkdir outputs
rm -r outputs/*
python measureXTIMATEaccuracy.py ../XTIMATE-outputs/workflow-output/ml_model_evaluation_results.csv ../XTIMATE-outputs/testing-output/compression-results/compression-results.csv
python measureXTIMATEefficiency.py ../XTIMATE-outputs/testing-output/feature_extraction_time.csv ../XTIMATE-outputs/workflow-output/ml_model_performance.log
mv outputs/* ../XTIMATE-outputs/workflow-output/
cd ..


# for element in "${array[@]}"
# do
#     echo "$element"
# done

printf "XTIMATE execution is finished!\n\n"
echo Please check XTIMATE-outputs folder for comprehensive evaluation results of XTIMATE.
