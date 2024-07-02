// #include <iostream>
// #include <fstream>
// #include <string.h>
// #include <unistd.h>
// #include <sys/stat.h>
// #include <dirent.h>
// #include <ctime>
// #include <cstdlib>
// #include <random>
// #include <math.h>
// #include <iostream>
// #include <set>
// #include <map>
// #include <string>
// #include <algorithm>
// #include <sstream>
#include "predictRatio.h"

#define EPS 0.00000000001



const std::string SZ_EXEC = "/lcrc/project/ECP-EZ/hasan/workstation/github-repos/Mine/Ratio-estimation/v-latest/baselines/SZ3-predict-2022/build/install/bin/sz3";
const std::string SZ_CONFIG_FILE = "/lcrc/project/ECP-EZ/hasan/workstation/github-repos/Mine/Ratio-estimation/v-latest/baselines/SZ3-predict-2022/examples/sz3.config";
const std::string WHITESPACE = " \n\r\t\f\v";


//////// Utilities start /////////
struct Data {
	float eb;
	float ratio;
};

// bool custom_compare(const Data &a, const Data &b) {
// 	if ( std::fabs(a.ratio - b.ratio) < EPS ) {
// 		return a.eb < b.eb;
// 	}
// 	else {
// 		return a.ratio < b.ratio;
// 	}
// }

std::string ltrim(const std::string &s)
{
    size_t start = s.find_first_not_of(WHITESPACE);
    return (start == std::string::npos) ? "" : s.substr(start);
}

std::string rtrim(const std::string &s)
{
    size_t end = s.find_last_not_of(WHITESPACE);
    return (end == std::string::npos) ? "" : s.substr(0, end + 1);
}

std::string trim(const std::string &s) {
    return rtrim(ltrim(s));
}


std::vector<std::string> mySplit(std::string const &str, const char delim)
{
    size_t start;
    size_t end = 0;

    std::vector<std::string> out; 
    while ((start = str.find_first_not_of(delim, end)) != std::string::npos)
    {
        end = str.find(delim, start);
        out.push_back(str.substr(start, end - start));
    }

    return out;
}



//////// Utilities end /////////

// stationary point (error bound vs compression ratio)
void predictRatio(const char *datafilePath, 
								const char *splitOnDataPath,
								const int num_dims, 
								const int *dims,
								const char* outputPath)
{

	
	char *command_buffer = (char* )malloc(500 * sizeof(char));

	printf("\n\n===== %s start======\n", __func__);
	printf("dataset file path: %s\n", datafilePath);
	sprintf(command_buffer, "mkdir %s/model-output", outputPath);
	system((const char* ) command_buffer);

	// get the dataset file name
	// example, /home/user/QCLOUDf48.dat --> QCLOUDf48.dat
	char* split=strtok((char*) splitOnDataPath,"/");
	char* datafileName = NULL;

	while(split != NULL)
	{
		datafileName=split;
		split=strtok(NULL,"/");
	}

	printf("datafileName: %s\n", datafileName);

	// Average 25 error bounds to generate stationary points
	float floating_error_bounds[] = {1e-7, 5e-7, 9e-7,
										1e-6, 3e-6, 5e-6, 7e-6, 9e-6, 
										1e-5, 3e-5, 5e-5, 7e-5, 9e-5,
										1e-4, 3e-4, 5e-4, 7e-4, 9e-4,
										1e-3, 2e-3, 3e-3, 4e-3, 5e-3, 6e-3, 7e-3, 8e-3, 9e-3,
										1e-2, 1.5e-2, 2e-2, 2.5e-2, 3e-2, 3.5e-2, 4e-2, 4.5e-2, 5e-2, 
										5.5e-2, 6e-2, 6.5e-2, 7e-2, 7.5e-2, 8e-2, 8.5e-2, 9e-2, 9.5e-2,
										1e-1, 1.5e-1, 2e-1, 2.5e-1}; 
	


	int num_floating_error_bounds = sizeof(floating_error_bounds) / sizeof(floating_error_bounds[0]);

	



	printf("Number of floating error bounds = %d\n", 
		num_floating_error_bounds);

	
	char *rm_buffer = (char* )malloc(500 * sizeof(char));	
	char *output_buffer = (char* )malloc(500 * sizeof(char));	
	sprintf(output_buffer, "%s/icde22-model-output/sz-compression-info.txt", outputPath);
	std::string output_sz(output_buffer);
		


	// free
	free(output_buffer);
		
	
	std::string cur_command;

	char *filename_buffer1 = (char* )malloc(500 * sizeof(char));

	// sz
	sprintf(filename_buffer1, "%s/icde22-model-output/model-output-%s.csv", outputPath, datafileName);
	std::string csv_cmd = "rm -f "; csv_cmd += filename_buffer1;
	system(csv_cmd.c_str());
	std::fstream fout_sz;
	


	std::vector<Data> sz_data;
	std::fstream extractFile;
	std::string extractLine;

	for (int id = 0; id < num_floating_error_bounds; id++){
		float cur_eb = floating_error_bounds[id];
		
					
		
		fout_sz.open((const char* )filename_buffer1, std::fstream::out | std::fstream::app);

		

		// sz
		sprintf(command_buffer, "%s -f -i %s -z %s.sz -%d", SZ_EXEC.c_str(), datafilePath, datafilePath, num_dims);
		cur_command = ""; cur_command += command_buffer;
		for (int i = num_dims-1; i >= 0; i--) {
			cur_command += " ";
			cur_command += std::to_string(dims[i]);
		}
		sprintf(command_buffer, " -c %s -M REL %.12f &> %s", SZ_CONFIG_FILE.c_str(), cur_eb, output_sz.c_str());
		cur_command += command_buffer;
		printf("command: %s\n", cur_command.c_str());
		system(cur_command.c_str());


		extractFile.open(output_sz, std::ios::in);
	

		float sz_cratio = 0.0;
		while (std::getline(extractFile, extractLine)) {
			if (extractLine.find("Predicted compression ratio") != std::string::npos){ 
				
				sz_cratio = std::stof(trim(mySplit(extractLine, ':')[1]));
				break;
			}
		}	

		extractFile.close();


		if (sz_cratio > 0.0) {
			sz_data.push_back({cur_eb, sz_cratio});				
			printf("==== model finished execution [eb %.10f, cratio: %.4f] ======\n\n\n", cur_eb, sz_cratio);
		}	

					
		
		//
		std::vector<Data>:: iterator it;


		for(it = sz_data.begin(); it != sz_data.end(); it++) {
			float ratio = it->ratio;
			float eb = it->eb;

			fout_sz << eb << ", " << ratio << "\n";
		}


		sz_data.clear();


		fout_sz.close();
	
		

		sprintf(rm_buffer, "rm %s.sz.out", datafilePath); 
		system((const char* )rm_buffer);	
		sprintf(rm_buffer, "rm %s.sz", datafilePath); 
		system((const char* )rm_buffer);
		system("rm core.*");		
		
	}
		

	



	// free all
	free(command_buffer);
	free(filename_buffer1);


	std::vector<Data>().swap(sz_data);

	sprintf(rm_buffer, "rm %s", output_sz.c_str());
	system((const char* )rm_buffer);

	free(rm_buffer);
	std::string().swap(output_sz);	
}



// void usage(int id)
// {
// 	printf("Usage: obtainStationaryPoints -i <dataset file path> -3 <Slowest Mid Fastest> %d\n", id);
// 	printf("Example: obtainStationaryPoints -i QCLOUDf48.dat -3 100 500 500");
// 	exit(0);
// }

// int main(int argc, char *argv[])
// {

// 	size_t i = 0;
// 	int num_dims = 0; 
// 	int* dims = NULL;
// 	char* datafilePath = NULL;
// 	char* splitOnDataPath = NULL;
// 	size_t dim3 = 0;
// 	size_t dim2 = 0; 
// 	size_t dim1 = 0;
// 	int len = 0;

// 	for(i=1;i<argc;i++)
// 	{
// 		if (argv[i][0] != '-' || argv[i][2])
// 			usage(1);
// 		switch (argv[i][1])
// 		{
// 		case '1': 
// 			num_dims = 1;
// 			if (++i == argc || sscanf(argv[i], "%zu", &dim1) != 1)
// 				usage(2);
// 			break;
// 		case '2':
// 			num_dims = 2;
// 			if (++i == argc || sscanf(argv[i], "%zu", &dim1) != 1 ||
// 				++i == argc || sscanf(argv[i], "%zu", &dim2) != 1)
// 				usage(3);
// 			break;
// 		case '3':
// 			num_dims = 3;
// 			if (++i == argc || sscanf(argv[i], "%zu", &dim1) != 1 ||
// 				++i == argc || sscanf(argv[i], "%zu", &dim2) != 1 ||
// 				++i == argc || sscanf(argv[i], "%zu", &dim3) != 1)
// 				usage(4);		
// 			break;
// 		case 'i':
// 			if (++i == argc)
// 				usage(5);
// 			datafilePath = argv[i];	
// 			while (argv[i][len] != '\0') len++;
// 			splitOnDataPath = (char* ) malloc(sizeof(char) * len);
// 			strcpy(splitOnDataPath, datafilePath);
// 			break;
// 		default: 
// 			usage(6);
// 			break;
// 		}
// 	}
	
// 	dims = (int*)malloc(sizeof(int) * num_dims);
// 	dims[0] = dim1;
// 	dims[1] = dim2;
// 	dims[2] = dim3;
	
// 	generate_sz_stationary_points(datafilePath, splitOnDataPath, num_dims, dims);

// 	// free
// 	// if (datafilePath) free(datafilePath);
// 	if (splitOnDataPath) free(splitOnDataPath);
// 	// if (dims) free(dims);
// 	return 0;
// }
