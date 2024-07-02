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

typedef enum {
  data_type_unknown  = 0, /* unspecified type */
  data_type_float  = 1, /* single precision floating point */
  data_type_double = 2  /* double precision floating point */
} data_type;

void predictRatio(const char *datafilePath, 
								const char *splitOnDataPath,
								const int num_dims, 
								const int *dims,
								const char* outputPath);


#endif