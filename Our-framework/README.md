XTIMATE Workflow
==

This directory contains our workflow. We provide *run_workflow.sh* to automate the process.

## Steps to execute XTIMATE workflow

- You need to have two directory of datasets: (1) training and (2) testing. We recommend that you should have only one dataset at a time in the *testing* directory.
- *run_workflow* needs 8 arguments:
  - Directory of training datasets
  - Directory of one testing dataset
  - Output directory needed to save XTIMATE outputs with training datasets
  - Output directory needed to save XTIMATE outputs with testing dataset
  - Compressor name and version (e.g., SZv3, SZv1.4, ZFPv0.5.0, MGARD+)
  - Number of dimensions in each dataset
  - A string containing dimension sizes
  - A CSV file containing a set of user-specified error bounds


Two sample commands would be as follows:\
\
With 1D application datasets such as Exaalt:
```
./run_workflow.sh ../datasets/Exaalt/single-precision/training ../datasets/Exaalt/single-precision/testing XTIMATE-outputs/training-output XTIMATE-outputs/testing-output SZv3 1 '2869440' XTIMATE-inputs/user_specified_error_bounds.csv
```
\
With 3D application datasets such as Miranda:
```
./run_workflow.sh ../datasets/Miranda/double-precision/training ../datasets/Miranda/double-precision/testing XTIMATE-outputs/training-output XTIMATE-outputs/testing-output ZFPv0.5.0 3 '256 384 384' XTIMATE-inputs/user_specified_error_bounds.csv
```

# Note
* For SZv1.4 and ZFPv0.5.0, the training and testing dataset should be double-precision floating point data.
* All the data presented in our paper can be found `paper-data` folder.
* You need at least GCC compiler version 10 to run our workflow.
