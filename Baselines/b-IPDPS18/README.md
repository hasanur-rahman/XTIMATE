# Tao Lu, Qing Liu, Xubin He, Huizhang Luo, Eric Suchyta, Norbert Podhorszki, Scott Klasky, Matthew Wolf and Tong Liu, Understanding and Modeling Lossy Compression Schemes on HPC Scientific Data, IEEE International Parallel & Distributed Processing Symposium (IPDPS), 2018.

This paper is published in IPDPS 2018. This is one of our baselines. This paper uses SZ1.4 and ZFP0.5.0 to perform the analysis. Github repo can be found [here](https://github.com/taovcu/LossyCompressStudy). Some of the code was not present in the repo but we obtain them from the author *Tao Lu* of the paper. Therefore, we thank the authors for their effort. Moreover, we also automate some of the process for our convenience. For formulas, we follow the paper.

### Steps to experiment with SZ1.4 compression modeling
- First, go to `inputdata` folder to convert original dataset to a sampled one by `buildsample.sh` script. Follow the comment and change the dataset path in the script to do appropriate sampling.
- Go to `sz-compression-ratio-estimation` folder to experiment the compression modeling with SZ1.4
- Execute `run_sz_estimation.sh` to estimate the compression ratio under an error bound specified in `sz.config` file. Details instructions are given in the script itself.


### Steps to experiment with ZFP0.5.0 compression modeling
- Change the dataset path in `tmprun.sh` script. 
- Change the zfp parameters such as user-specified error bound in `run.sh` script
- Change the number of iterations by `loop` variable, if necessary.
- Execute `tmprun.sh` script
- The final line shows the estimate ZFP compression ratio under the given error bound

