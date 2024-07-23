A Generic and Efficient Framework for Estimating Lossy Compressibility of Scientific Data
=====

* Published in 38th International Conference on Massive Storage Systems and Technology (MSST), 2024. 
* Authors: Md Hasanur Rahman, Sheng Di, Guanpeng Li and Franck Cappello. 
* This project was done with the collaboration of Argonne National Laboratory.

## Research Goal
This paper proposes a compressor-agnostic lossy compression framework **XTIMATE** that is capable of accurately and efficiently estimating scientific data compressibility. Please check our MSST'24 [paper](https://www.msstconference.org/MSST-history/2024/Papers/msst24-1.3.pdf) for more details.

## Paper Abstract
Modern HPC applications produce extremely large amounts of scientific data, which cannot be stored and transferred efficiently. Error-bounded lossy compression has been effective in significantly reducing the volume of data. However, lossy compressors are mainly driven by user-specified errorbound, so the compression ratios would be unknown until compression is completed. In practice, however, many users need to forecast the compression ratio beforehand, such that related data storage, transfer and management could be orchestrated more efficiently and proactively. In this paper, we propose XTIMATE, a compressor-agnostic lossy compression framework capable of accurately and efficiently estimating compression ratio
of scientific data. 
Our key observation is that the characteristics
of data textures play an important role in understanding lossy
compressibility. The key contributions are three-fold. (1) We
provide an in-depth analysis on effective data features, especially
analyzing the characteristics of varying data textures. (2) We
carefully explore the best-fit kernel filter and develop a series of
optimization strategies, in light of both estimation capability and
performance. (3) We comprehensively evaluate XTIMATE based
on three state-of-the-art lossy compressors using 10 real-world
scientific datasets from 5 HPC applications and compare with
state-of-the-art related works. Experiments show that average
estimation error of XTIMATE is only 6.77%. Furthermore,
XTIMATE exhibits a 30× speedup on average compared to the
related works.

## How to Cite
If you want to include our paper in your work, please cite our paper. You can find the bibtex citation [**here**](https://hasanur-rahman.github.io/cites/MSST24-XTIMATE.bib). 


## Introduction to different folders
* Baselines: this folder contains the code and evaluations of our two baselines.
* Compressor-executables: this folder has instructions of how to execute each of the compressors.
* Datasets: we provide the description of dataset source and usage here. We also provide one sample dataset application to test XTIMATE.
* Our-framework: this folder provides code and step-by-step guidelines of how to install and run our framework XTIMATE.

## License

This work is licensed under a [Creative Commons Attribution 4.0 International License](https://creativecommons.org/licenses/by/4.0/).

[![CC BY 4.0](https://licensebuttons.net/l/by/4.0/88x31.png)](https://creativecommons.org/licenses/by/4.0/)
