# Improving Prediction-Based Lossy Compression Dramatically via Ratio-Quality Modeling

This paper is published in 2022 IEEE 38th International Conference on Data Engineering (ICDE). This ratio-quality modeling is one of our baselines. This model uses SZ3 compressor. Here is the github link of the artifact: [SZ3-predict-2022](https://github.com/jinsian/SZ3-predict-2022). Please follow the link for more details.

Most of input arguments correspond to SZ3 arguments. ***The model takes a few arguments which are as follows:***
- -f : single precision dataset
- -i < input dataset name >
- -z < compressed data path >
- -< n > < x y z .. > : Number of dimensions *n* followed by the size of each dimension
- -c < sz.config file path >
- -M REL < error bound > : Relative error bound mode *-M REL* followed by user-specified error bound


*Note that* all the data provided in our paper can be found in `paper-data` folder.



