#ifndef _OBTAINSTATIONARYPOINTS_H
#define _OBTAINSTATIONARYPOINTS_H

#include <iostream>
#include <fstream>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <dirent.h>
#include <ctime>
#include <cstdlib>
#include <random>
#include <math.h>
#include <iostream>
#include <set>
#include <map>
#include <string>
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <fstream>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <dirent.h>
#include <ctime>
#include <cstdlib>
#include <random>
#include <math.h>
#include <algorithm>
#include <map>
#include <cstring>
#include <iomanip>
#include <sys/time.h> 
#include <map>

void generate_szv3_compression_results(const char *datafilePath, 
								const char *splitOnDataPath,
								const int num_dims, 
								const int *dims,
								const char* outputPath);

void generate_szv3_compression_results_specific(const char *datafilePath, 
								const char* userSpecifiedErrorBoundsFilePath,
								const char *splitOnDataPath,
								const int num_dims, 
								const int *dims,
								const char* outputPath);


void generate_szv1_4_compression_results(const char *datafilePath, 
								const char *splitOnDataPath,
								const int num_dims, 
								const int *dims,
								const char* outputPath);

void generate_szv1_4_compression_results_specific(const char *datafilePath,
								const char* userSpecifiedErrorBoundsFilePath,
								const char *splitOnDataPath,
								const int num_dims, 
								const int *dims,
								const char* outputPath);

void generate_mgard_plus_compression_results(const char *datafilePath, 
								const char *splitOnDataPath,
								const int num_dims, 
								const int *dims,
								const char* outputPath);

void generate_mgard_plus_compression_results_specific(const char *datafilePath, 
								const char* userSpecifiedErrorBoundsFilePath,
								const char *splitOnDataPath,
								const int num_dims, 
								const int *dims,
								const char* outputPath);

void generate_zfpv0_5_0_compression_results(const char *datafilePath, 
								const char *splitOnDataPath,
								const int num_dims, 
								const int *dims,
								const char* outputPath);

void generate_zfpv0_5_0_compression_results_specific(const char *datafilePath, 
								const char* userSpecifiedErrorBoundsFilePath,
								const char *splitOnDataPath,
								const int num_dims, 
								const int *dims,
								const char* outputPath);

#endif