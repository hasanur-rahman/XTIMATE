# Applications & Domains
We use datasets from 5 different widely-used real-world scientific applications. They represent different domains such as Cosmology, Climate, Molecular dynamics etc. All of the datasets can be downloaded from the SDRBench [database](https://sdrbench.github.io) [1], which is commonly used in lossy compression studies.
We evaluate our framework XTIMATE across different dataset fields of an application domain. We encourage you to read the paper [1] for more details of the datasets.\
***Note that some compressors require to use datasets in double precision, so you need to convert between single precision and double precision using commmand *convertFloatToDouble* and *convertDoubleToFloat* commands from [qcat](https://github.com/szcompressor/qcat) repo***

## Application 1: Exaalt 1D (2869440)
Exaalt has 6 fields: X,Y,Z,Vx,Vy,Vz. We use Y and Vy fields as testing dataset fields.
Exaalt datasets can be downloaded from [here](https://g-8d6b0.fd635.8443.data.globus.org/ds131.2/Data-Reduction-Repo/raw-data/EXAALT/SDRBENCH-EXAALT-2869440.tar.gz) [1].

## Application 2: CESM 2D (1800x3600)
CESM has more than 70 fields. For the balance of our experiment time we choose 11 fields: CLDHGH, CLDLOW, CLDMED, CLDTOT, FLDS, FLNT, FLNS, FREQSH, FREQZM, FSDS, PHIS. We use CLDMED and FLNT as testing dataset fields.
CESM datasets can be downloaded from [here](https://g-8d6b0.fd635.8443.data.globus.org/ds131.2/Data-Reduction-Repo/raw-data/CESM-ATM/SDRBENCH-CESM-ATM-cleared-1800x3600.tar.gz) [1].

## Application 3: Hurricane 3D (100x500x500)
Hurricane has 12 fields: QCLOUD, QGRAUP, QICE, QSNOW, QVAPOR, CLOUD, PRECIP, P, TC, U, V, W. We choose timestep 48 for each field for training and testing datasets. Moreover, we use V and PRECIP as testing dataset fields.
Hurricane datasets can be downloaded from [here](https://g-8d6b0.fd635.8443.data.globus.org/ds131.2/Data-Reduction-Repo/raw-data/Hurricane-ISABEL/SDRBENCH-Hurricane-ISABEL-100x500x500.tar.gz) [1].

## Application 4: Miranda 3D (256x384x384)
Miranda has 7 fields: Density, Diffusivity, Pressure, Velocityx, Velocityy, Velocityz, and Viscocity. We use Viscocity and Velocityy as testing dataset fields.
Miranda datasets can be downloaded from [here](https://g-8d6b0.fd635.8443.data.globus.org/ds131.2/Data-Reduction-Repo/raw-data/Miranda/SDRBENCH-Miranda-256x384x384.tar.gz) [1].

## Application 5: Nyx 3D (512x512x512)
Nyx has 6 fields: Baryon density, Dark matter density, Temperature, Velocityx, Velocityy and Velocityz. We use Temperature and Velocityy as testing dataset fields.
Nyx datasets can be downloaded from [here](https://g-8d6b0.fd635.8443.data.globus.org/ds131.2/Data-Reduction-Repo/raw-data/EXASKY/NYX/SDRBENCH-EXASKY-NYX-512x512x512.tar.gz) [1].

### Note
- If you wish to run our XTIMATE workflow quickly, for your convenience, we have already provided the Exaalt application single precision datasets in the directory. Execute *run_workflow.sh* with exaalt datasets under your preferred compressor. Note that some compressors require double precision datasets.
- Alternatively, you have to download the datasets of an application from the provided link. As the datasets are huge, this will take quite a time. After the downloading and uncompressing, you need to choose a testing dataset, and rest of the datasets in an application should be treated as training datasets. Finally, you have to execute *run_workflow.sh* in the appropriate format (follow the corresponding README). 


### References
[1] K. Zhao, S. Di, X. Liang, S. Li, D. Tao, J. Bessac, Z. Chen, and F. Cappello, “SDRBench: Scientific Data Reduction Benchmark for Lossy Compressors”, International Workshop on Big Data Reduction (IWBDR2020), in conjunction with IEEE Bigdata20

