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
#include <iomanip>
#include <sys/time.h> 
#include <map>
#include <unistd.h>
#include <sys/stat.h>
#include <dirent.h>
#include <ctime>
#include <cstdlib>

#define EPS 0.00000000001


const std::string SZ_EXEC = "../../../build/install/bin/sz3";
const std::string SZ_CONFIG_FILE =  "../../../examples/sz3.config";

const std::string WHITESPACE = " \n\r\t\f\v";

//////// Utilities start /////////
enum LABEL {SZ=0, ZFP, INVALID};
bool isNumber(std::string val)
{
	char* p;
  strtof(val.c_str(), &p);
  return *p == 0;
}


struct Data {
	float eb;
	float ratio;
};

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

struct timeval costStart; /*only used for recording the cost*/
double executionTimeInSec = 0;
	

void cost_start()
{
	executionTimeInSec = 0.0;
	gettimeofday(&costStart, NULL);
}

void cost_end()
{
	double elapsed;
	struct timeval costEnd;
	gettimeofday(&costEnd, NULL);
	elapsed = ((costEnd.tv_sec*1000000+costEnd.tv_usec)-(costStart.tv_sec*1000000+costStart.tv_usec))/1000000.0;
	executionTimeInSec += elapsed;
}


//////// Utilities end /////////




void generate_psnr_vs_ratio(const int identifer,
	const char *datasetname, 
	const char *dataset_filename, 
	const int num_dims, 
	const int *dims)
{

	
	printf("\n\n===== %s start======\n", __func__);
	printf("datasetName %s, dataset_filename %s\n", datasetname, dataset_filename);
	system("mkdir compression-stats");
	char *command_buffer = (char* )malloc(500 * sizeof(char));


	float floating_error_bounds[] = {1e-7, 5e-7, 9e-7,
										1e-6, 3e-6, 5e-6, 7e-6, 9e-6, 
										1e-5, 3e-5, 5e-5, 7e-5, 9e-5,
										1e-4, 3e-4, 5e-4, 7e-4, 9e-4,
										1e-3, 2e-3, 3e-3, 4e-3, 5e-3, 6e-3, 7e-3, 8e-3, 9e-3,
										1e-2, 1.5e-2, 2e-2, 2.5e-2, 3e-2, 3.5e-2, 4e-2, 4.5e-2, 5e-2, 
										5.5e-2, 6e-2, 6.5e-2, 7e-2, 7.5e-2, 8e-2, 8.5e-2, 9e-2, 9.5e-2,
										1e-1};

	int num_floating_error_bounds = sizeof(floating_error_bounds) / sizeof(floating_error_bounds[0]);

	



	printf("Number of floating error bounds = %d\n", 
		num_floating_error_bounds);

	
	char *rm_buffer = (char* )malloc(500 * sizeof(char));	
	char *output_buffer = (char* )malloc(500 * sizeof(char));	
	sprintf(output_buffer, "./compression-stats/sz-compression-info-%d.txt", identifer);
	std::string output_sz(output_buffer);
		


	// free
	free(output_buffer);
		
	
	std::string cur_command;

	char *filename_buffer1 = (char* )malloc(500 * sizeof(char));

	// sz
	sprintf(filename_buffer1, "./compression-stats/sz-%s-%s.csv", datasetname, dataset_filename);
	std::string csv_cmd = "rm -f "; csv_cmd += filename_buffer1;
	system(csv_cmd.c_str());
	std::fstream fout_sz;

	std::fstream extractFile;	
	std::string extractLine;
	

	std::vector<Data> sz_data;

	for (int id = 0; id < num_floating_error_bounds; id++){
		float cur_eb = floating_error_bounds[id];
		
					
		
		fout_sz.open((const char* )filename_buffer1, std::fstream::out | std::fstream::app);

		

		// sz
		// command 1
		sprintf(command_buffer, "%s -f -i %s -z %s.sz -%d", SZ_EXEC.c_str(), dataset_filename, dataset_filename, num_dims);
		cur_command = ""; cur_command += command_buffer;
		for (int i = 0; i < num_dims; i++) {
			cur_command += " ";
			cur_command += std::to_string(dims[i]);
		}
		sprintf(command_buffer, " -c %s -M REL %.12f > %s 2>&1", SZ_CONFIG_FILE.c_str(), cur_eb, output_sz.c_str());
		cur_command += command_buffer;
		printf("command: %s\n", cur_command.c_str());

		cost_start();
		system(cur_command.c_str());
		cost_end();

		double compressionTime = executionTimeInSec;

		//float sz_cratio = extract_result(output_sz, "compression ratio: ");


		extractFile.open(output_sz, std::ios::in);
	

		float sz_cratio = 0.0;
		while (std::getline(extractFile, extractLine)) {
			if (extractLine.find("compression ratio:") != std::string::npos){ 
				
				sz_cratio = std::stof(trim(mySplit(extractLine, ':')[1]));
				break;
			}
		}	

		extractFile.close();

				

		if (sz_cratio > 0.0) {
			sz_data.push_back({cur_eb, sz_cratio});
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


		std::fstream foutTime;
		sprintf(command_buffer, "./compression-time/sz-%s-%s-time.csv", datasetname, dataset_filename);
		foutTime.open((const char* )command_buffer, std::fstream::out | std::fstream::app);
		foutTime << cur_eb << "," << compressionTime << "\n";
		foutTime.close();
		

		sprintf(rm_buffer, "rm %s.sz", dataset_filename); 
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

int main(int argc, char *argv[])
{

	//system("rm -rf result");
	const int num_dims = 3;
	const int dims[] = {512, 512, 512};
	
	int identifier = atoi(argv[1]);
	generate_psnr_vs_ratio((const int)identifier, (const char *)argv[2], (const char *)argv[3], num_dims, dims);
}
