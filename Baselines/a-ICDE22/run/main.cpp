#include <fstream>
#include <string.h>
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
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sstream>
#include "predictRatio.h"

// Jobs
// Read each file in a folder and 
// Performs feature extraction and
// Generate train and test data



struct timeval costStartMain; /*only used for recording the cost*/
double curExecutionTimeMain = 0;
	

void cost_start_main()
{
	curExecutionTimeMain = 0;
	gettimeofday(&costStartMain, NULL);
}

void cost_end_main()
{
	double elapsed;
	struct timeval costEnd;
	gettimeofday(&costEnd, NULL);
	elapsed = ((costEnd.tv_sec*1000000+costEnd.tv_usec)-(costStartMain.tv_sec*1000000+costStartMain.tv_usec))/1000000.0;
	curExecutionTimeMain += elapsed;
}

void get_file_name(char *filepath, char** datafileName){
	char* split=strtok((char*) filepath,"/");

	while(split != NULL)
	{
		*datafileName=split;
		split=strtok(NULL,"/");
	}
}

bool is_regular_file(const char *path)
{
    struct stat path_stat;
    return stat(path, &path_stat) == 0 && S_ISREG(path_stat.st_mode);
}

bool is_dir(const char *path)
{
    struct stat path_stat;
	return (stat(path, &path_stat) == 0 && S_ISDIR(path_stat.st_mode));
}

void get_files(const char *filesPath, int *num_files, char*** filesArray, int *maxNumFilenameChars)
{
	DIR *dir;
	struct dirent *ent;
	char *fullPath = (char *) malloc(sizeof(char) * 500);

	*num_files = 0;
	*maxNumFilenameChars = 0;
	// first get the num of files to allocate
	if ((dir = opendir (filesPath)) != NULL) {
		while ((ent = readdir (dir)) != NULL ) {
			sprintf(fullPath, "%s/%s", filesPath, ent->d_name);
			if (is_regular_file((const char *) fullPath)) (*num_files) ++;
		}
		closedir (dir);
	} else {
		printf("Could not open the directory: %s\n", filesPath);
		exit(0);
	}

	

	// second, store the files path
	*filesArray = (char**) malloc(sizeof(char*) * (*num_files));
	int iter = 0;
	if ((dir = opendir (filesPath)) != NULL) {
		while ((ent = readdir (dir)) != NULL ) {
			sprintf(fullPath, "%s/%s", filesPath, ent->d_name);
			if (is_regular_file((const char *) fullPath)){
				int curFileLen = 0;
				while (fullPath[curFileLen]) curFileLen ++;
				if (curFileLen > *maxNumFilenameChars) *maxNumFilenameChars = curFileLen;
				(*filesArray)[iter] = (char*) malloc(sizeof(char) * (curFileLen + 1));
				strcpy((*filesArray)[iter], (const char*) fullPath);
				iter ++;
			}
		}
		free(fullPath);
		closedir (dir);
	} else {
		printf("Could not open the directory: %s\n", filesPath);
		free(fullPath);
		exit(0);
	}
}

void usage()
{
	printf("Usage: ratioQualityModeling -i <dataset files dir path> -o <output dir path> -f or -d (single or double precision floating point dataset) -<number of dimensions> <dimensions A(slowest) B ... C(fastest)>\n");
	printf("Example: ratioQualityModeling -i /home/user/input/ -o /home/user/output -f -3 256 384 384\n");
	exit(0);
}

int main(int argc, char *argv[]) {

	data_type type = data_type_unknown;
    int numDims = 0; 
	int* dims = NULL;
	char* datafilesPath = NULL;
	char* splitOnDatafilePath = NULL;
	char* outputPath = NULL;
	char** inputFilesArray = NULL;
	int numInputDatasetFiles = 0;
	int maxNumFilenameChars = 0;
	char *strBuffer = (char* ) malloc(sizeof(char) * 500);

    size_t dim3 = 0;
	size_t dim2 = 0; 
	size_t dim1 = 0;
	
	size_t i = 0;
	int len = 0;

	if (argc < 2) {
		usage();
		exit(0);
	}

	printf("main function\n");

    for(i=1;i<argc;i++)
	{
		if (argv[i][0] != '-' || argv[i][2])
			usage();
		switch (argv[i][1])
		{
		case '1': 
			numDims = 1;
			if (++i == argc || sscanf(argv[i], "%zu", &dim1) != 1)
				usage();
			break;
		case '2':
			numDims = 2;
			if (++i == argc || sscanf(argv[i], "%zu", &dim1) != 1 ||
				++i == argc || sscanf(argv[i], "%zu", &dim2) != 1)
				usage();
			break;
		case '3':
			numDims = 3;
			if (++i == argc || sscanf(argv[i], "%zu", &dim1) != 1 ||
				++i == argc || sscanf(argv[i], "%zu", &dim2) != 1 ||
				++i == argc || sscanf(argv[i], "%zu", &dim3) != 1)
				usage();		
			break;
        case 'i':
            if (++i == argc)
				usage();
			datafilesPath = argv[i];	
            break;
		case 'o':
			if (++i == argc)
				usage();
			outputPath = argv[i];	
            break;
		case 'f':
			// if (++i == argc)
			// 	usage();
			type = data_type_float;
			break;
		case 'd':
			// if (++i == argc)
			// 	usage();
			type = data_type_double;
			break;
        default: 
			usage();
			break;
        }
    }

	sprintf(strBuffer, "rm -r %s", outputPath);
	system((const char* ) strBuffer);
	sprintf(strBuffer, "mkdir %s", outputPath);
	system((const char* ) strBuffer);

	dims = (int*)malloc(sizeof(int) * numDims);

	switch (numDims)
	{
	case 1:
		dims[0] = dim1;
	case 2:
		dims[0] = dim1;
		dims[1] = dim2;
		break;
	case 3:
		dims[0] = dim1;
		dims[1] = dim2;
		dims[2] = dim3;
		break;
	default:
		break;
	}

	

    if (datafilesPath != NULL) {
        get_files(datafilesPath, &numInputDatasetFiles, &inputFilesArray, &maxNumFilenameChars);
    }


	double totalTimeCost = 0.0;

	cost_start_main();
	splitOnDatafilePath = (char* ) malloc(sizeof(char) * (maxNumFilenameChars + 1));
	for (int i = 0; i < numInputDatasetFiles; i++) {
		strcpy(splitOnDatafilePath, inputFilesArray[i]);

		// if (type != data_type_float) {printf("[Error]: data type must be float to run the model\n"); exit(0);}
		predictRatio(inputFilesArray[i],
							splitOnDatafilePath,
							numDims, 
							dims,
							outputPath);
		
	}
	cost_end_main();
	totalTimeCost += curExecutionTimeMain;
	


	// char* curDatasetName = (char* ) malloc(sizeof(char) * (maxNumFilenameChars + 1));
	// std::vector<std::string> csvRow;

	// cost_start_main();
	// for (int i = 0; i < numInputDatasetFiles; i++) {
	// 	strcpy(splitOnDatafilePath, inputFilesArray[i]);
	// 	get_file_name(splitOnDatafilePath, &curDatasetName);
	// 	printf("current dataset name: %s\n", curDatasetName);
	// 	sprintf(strBuffer, "%s/stationary-points/stationary-points-%s.csv", outputPath, curDatasetName);
	// 	printf("file path: %s\n", strBuffer);

	// 	std::fstream fin(strBuffer, std::ios::in);
	// 	std::string line, word;

	// 	char *buffer = (char* )malloc(500 * sizeof(char));
	// 	sprintf(buffer, "mkdir %s/features-extraction", outputPath);
	// 	system((const char*) buffer);
	// 	sprintf(buffer, "rm %s/features-extraction/feature_extraction_result_%s.csv", outputPath, curDatasetName);
	// 	system((const char* ) buffer);

	// 	while (getline(fin, line))
	// 	{
	// 		csvRow.clear();
	// 		std::stringstream s(line);
	// 		while (getline(s, word, ',')) {
	// 			csvRow.push_back(word);
	// 		}

	// 		if (csvRow.size() != 2) {
	// 			printf("There must only be two columns in each row of stationary points\n");
	// 			exit(0);
	// 		}

	// 		double curErrorBound = atof(csvRow[0].c_str());
	// 		double curCRatio = atof(csvRow[1].c_str());
	// 		// for (auto colVal: csvRow) {
	// 		// 	printf("%.3lf ", atof(colVal.c_str()));
	// 		// }
			
	// 		printf("%.8lf %.2lf\n", curErrorBound, curCRatio);
	// 		strcpy(splitOnDatafilePath, inputFilesArray[i]);
	// 		getFeatures(type, inputFilesArray[i], numDims, dims, curErrorBound,
	// 				curCRatio, outputPath, splitOnDatafilePath);

	// 	}
	// 	fin.close();
		
	// }
	// cost_end_main();
	// totalTimeCost += curExecutionTimeMain;

	printf("Total Time Cost: %.12lf\n", totalTimeCost);

	
	// sprintf(strBuffer, "tail -n+1 -q %s/features-extraction/feature_extraction_result_*.csv > %s/merged_data.csv", outputPath, outputPath);
	// system((const char*) strBuffer);
	


	free(splitOnDatafilePath);

	return 0;



}