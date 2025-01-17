This code is obtained from SZ1.4 repository, which can be found [here](https://github.com/szcompressor/SZ/releases/tag/v1.4.9).
Their README file is as follows.

--------SZ (version 1.4.9): Error-bounded Lossy Compressor for HPC Data--------
 (C) 2016 by Mathematics and Computer Science (MCS), Argonne National Laboratory.
       See COPYRIGHT in top-level directory.

***Authors: Sheng Di, Dingwen Tao ***
***Supervisor: Franck Cappello ***

=================================
This document simply introduces how to install and use the SZ compressor. More details can be found in doc/userguide.pdf.

===========Installation==========
---------------------------------
configure --prefix=[INSTALL_DIR]
(Please use --enable-fortran if you need Fortran interface)

make

make install


Then, you'll find all the executables in [INSTALL_DIR]/bin and .a and .so libraries in [INSTALL_DIR]/lib

===========Testing Example============
--------------------------------------

Examples can be found in the [SZ_PACKAGE]/example

===Compress===
--------------
./test_compress sz.config testdouble_8_8_8_128.dat 8 8 8 128
./test_compress sz.config testdouble_8_8_128.dat 8 8 128

Decription: 

testdouble_8_8_128.dat and testdouble_8_8_8_128.dat are two binary testing files (small-endian format), which contains a 3d array (128X8X8) and a 4d array (128X8X8X8) respectively. Their data values are shown in the two plain text files, testdouble_8_8_128.txt and testdouble_8_8_8_128.txt. These two data files are from FLASH_Blast2 and FLASH_MacLaurin respectively (both are at time step 100). The compressed data files are namely testdouble_8_8_8_128.dat.sz and testdouble_8_8_128.dat.sz respectively.

sz.config is the configuration file. The key settings are errorBoundMode, absErrBound, and relBoundRatio, which are described below.

*absErrBound refers to the absolute error bound, which is to limit the (de)compression errors to be within an absolute error. For example, absErrBound=0.0001 means the decompressed value must be in [V-0.0001,V+0.0001], where V is the original true value.

*relBoundRatio refers to relative bound ratio, which is to limit the (de)compression errors by considering the global data value range size (i.e., taking into account the range size (max_value - min_value)). For example, suppose relBoundRatio is set to 0.01, and the data set is {100,101,102,103,104,...,110}, so the global value range size is 110-100=10, so the error bound will actually be 10*0.01=0.1, from the perspective of "relBoundRatio".

*errorBoundMode is to indicate the error-bounding way in the compression, such as based on absolute error bound, relative error bound, etc. 
The options are shown below.
	ABS: take only "absolute error bound" into account. That is, relative bound ratio will be ignored.
	REL: take only "relative bound ratio" into account. That is, absolute error bound will be ignored. 
	ABS_AND_REL: take both of the two bounds into account. The compression errors will be limited using both absErrBound and relBoundRatio*rangesize. That is, the two bounds must be both met.
	ABS_OR_REL: take both of the two bounds into account. The compression errors will be limited using either absErrBound or relBoundRatio*rangesize. That is, only one bound is required to be met.
	PW_REL: take "point-wise relative error bound" in the compression. 
===Decompress===
----------------
./test_decompress sz.config testdouble_8_8_8_128.dat.sz 8 8 8 128
./test_decompress sz.config testdouble_8_8_128.dat.sz 8 8 128

The output files are testdouble_8_8_8_128.dat.sz.out and testdouble_8_8_128.dat.sz.out respectively. You can compare .txt file and .out file for checking the compression errors for each data point. For instance, compare testdouble_8_8_8_128.txt and testdouble_8_8_8_128.dat.sz.out.

===Application Programming Interface (API)===
---------------------------------------------
Programming interfaces are procides in two programming languages - C and Java: 
The interfaces are listed below. More details can be found in the user guide. 
----C Interface----

int SZ_Init();

char *SZ_compress(void *data, int *outSize, int r5, int r4, int r3, int r2, int r1);
char *SZ_compress_args(void *data, int *outSize, int errBoundMode, double absErrBound, double relBoundRatio, int r5, int r4, int r3, int r2, int r1);

double *SZ_decompress(char *bytes, int byteLength, int r5, int r4, int r3, int r2, int r1);
void SZ_Finalize();

----JAVA Interface----
----------------------
Java interfaces are provided in SZ-0.5.15, but not in SZ-1.0. If you are interested in Java interfaces, please download and install SZ-0.5.15 instead.

Class name: compression.SingleFillingCurveDecompressVariable

public static void SZ_Init(String configFilePath);

public static byte[] SZ_compress(double[] doubleData, int r5, int r4, int r3, int r2, int r1)

public static byte[] SZ_compress(double[] doubleData, int errBoundMode, double absErrBound, double relBoundRatio, 
			int r5, int r4, int r3, int r2, int r1)

public static double[] SZ_decompress(byte[] bytes, int r5, int r4, int r3, int r2, int r1)

===Limitation of this version===
--------------------------------
In the current version, the maximum original double-array file size (such as the size of testdouble_8_8_128.dat) is 2147483647*8 bytes, i.e., 16GB; and the maximum compressed binary array file (such as .sz file) is 2147483647=2GB. If the data file size to compress is larger than 16GB or if its compressed size is larger than 2GB, please download and use SZ-largesize-version instead. 

===version history===
---------------------
version		New features
SZ 0.2-0.4	Compression ratio is the same as SZ 0.5. The key difference is different implementation ways, such that SZ 0.5 is much faster than SZ 0.2-0.4.
SZ 0.5.1	Support version checking
SZ 0.5.2	finer compression granularity for unpredictable data, and also remove redundant Java storage bytes
SZ 0.5.3 	Integrate with the dynamic segmentation support
SZ 0.5.4	Gzip_mode: defaut --> fast_mode ; Support reserved value	
SZ 0.5.5	runtime memory is shrinked (by changing int xxx to byte xxx in the codes)
		The bug that writing decompressed data may encounter exceptions is fixed.
		Memory leaking bug for ppc architecture is fixed.
SZ 0.5.6	improve compression ratio for some cases (when the values in some segementation are always the same, this segment will be merged forward)
SZ 0.5.7	improve the decompression speed for some cases
SZ 0.5.8	Refine the leading-zero granularity (change it from byte to bits based on the distribution). For example, in SZ0.5.7, the leading-zero is always in bytes, 0, 1, 2, or 3. In SZ0.5.8, the leading-zero part could be xxxx xxxx xx xx xx xx xxxx xxxx (where each x means a bit in the leading-zero part)
SZ 0.5.9	optimize the offset by using simple right-shifting method. Experiments show that this cannot improve compression ratio actually, because simple right-shifting actually make each data be multiplied by 2^ {-k}, where k is # right-shifting bits. The pros is to save bits because of more leading-zero bytes, but the cons is much more required bits to save. A good solution is SZ 0.5.10!
SZ 0.5.10	optimze the offset by using the optimized formula of computing the median_value based on optimized right-shifting method. Anyway, SZ0.5.10 improves compression ratio a lot for hard-to-compress datasets. (Hard-to-compress datasets refer to the cases whose compression ratios are usually very limited)
SZ 0.5.11	In a very few cases, SZ 0.5.10 cannot guarantee the error-bounds to a certain user-specified level. For example, when absolute error bound = 1E-6, the maximum decompression error may be 0.01(>>1E-6) because of the huge value range even in the optimized segments such that the normalized data cannot reach the required precision even stoaring all of the 64 or 32 mantissa bits. SZ 0.5.11 fixed the problem well, with degraded compression ratio less than 1%.
SZ 0.5.12 	A parameter setting called "offset" is added to the configuration file sz.config. The value of offset is an integer in [1,7]. Generally, we recommend offset=2 or 3, while we also find that some other settings (such as offset=7) may lead to better compression ratios in some cases. How to automize/optimize the selection of offset value would be the future work. In addition, the compression speed is improved, by replacing java List by array implementation in the code.
SZ 0.5.13	Compression performance is improved, by replacing some class instances in the source code by primitive data type implementation.
SZ 0.5.14 	fixed a design bug, which improves the compression ratio further.
SZ 0.5.15	improved the compression ratio for single-precision data compression, by tuning the offset.

The version 0.x were all coded in Java, and C/Fortran interfaces were provided by using JNI and C/Fortran wrapper. SZ 1.0 is coded in C purely.
SZ 1.0		Pure C version. In this version, the users don't need to install JDK and make the relative configurations any more. It provides dataEndienType in the sz.config 		file, so it can be used to compress the data file which was generated on different endian-type systems.
SZ 1.2		Improve compression: using 8 intervals (best selection between linear-curve-fitting and quadratic-curve fitting)
SZ 1.3		Significantly improved compression: using 255 intervals, with multi-dimensional data prediction (on 1D, 2D, and 3D)
SZ 1.4		Use 65536 intervals
SZ 1.4.2	Extending the number of intervals from 255 to 65536, by tailoring/reimplementing the Huffman encoding by ourselves.
SZ 1.4.3	Add the intervals_count to the configuration file (sz.config), allowing users to control it.
SZ 1.4.4	Remove segmentation step quantization_intervals
SZ 1.4.5	Optimize the number of intervals: the # intervals will be automatically optimized before the compression if quantization_itnervals is set to 0.
SZ 1.4.6-beta   Three compression modes are provided (SZ_BEST_SPEED, SZ_BEST_COMPRESSION, SZ_DEFAULT_COMPRESSION), the maximum # quantization intervals is 65536.
SZ 1.4.6-hacc	the same as SZ 1.4.6-beta. The only difference is the optimized prediction for 1D array: replacing the linear-curve fitting by neighbor-value fitting.
SZ 1.4.7-beta   Fix some mem leakage bugs. Fix the bugs about memory crash or segmentation faults when the number of data points is pretty large. Fix the sementation fault bug happening when the data size is super small. Fix the error bound may not be guaranteed in some cases.
SZ 1.4.7-hacc	In addition to the updates of SZ 1.4.6-hacc, it also "improves" the prediction, by using segmented sorting.
SZ 1.4.8-hacc 	It is improved based on SZ 1.4.6-hacc. Increase the maximum number of quantization intervals. The users can set the max_quant_intervals in the configuration files, in order to optimize the compression performance and compression ratio in the cases with different precisions. Please see the description in sz.config for details.
SZ 1.4.9-hacc	Support point-wise relative error bound setting.
SZ 1.4.9-beta	Support point-wise relative error bound setting, and optional Fortran compilation.
SZ 1.4.x1-hacc	It is improved based on SZ 1.4.6-hacc. The whole data set is split into multiple segments, and each segment chooses the bestfit prediction method (either neighbor-state-prediction or linear-curve-fitting or more)
