#ifndef _EXTRACTFEATURES_H
#define _EXTRACTFEATURES_H

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


typedef enum {
  data_type_unknown  = 0, /* unspecified type */
  data_type_float  = 1, /* single precision floating point */
  data_type_double = 2  /* double precision floating point */
} data_type;


#define SAMPLING_OFFSET_1D 40 
#define SAMPLING_OFFSET_2D 20
#define SAMPLING_OFFSET_3D 10 
#define EPSILON 0.000000001



int getFeatures(data_type type, const char* datafilePath, const int numDims, const int *dims, const double errorBound,
				const double cRatio, const char* outputPath, char* splitOnDatasetPath);

#endif


