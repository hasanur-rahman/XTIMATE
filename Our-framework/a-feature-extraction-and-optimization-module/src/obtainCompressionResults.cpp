
#include "obtainCompressionResults.h"

#define EPS 0.00000000001
#define MAX_CRATIO 2500



const std::string SZ3_EXEC = "../../../Compressors-executables/a-SZ3/build/install/bin/sz3";
const std::string SZ3_CONFIG_FILE = "../../../Compressors-executables/a-SZ3/examples/sz3.config";
const std::string SZ_1_4_EXEC = "../../../Compressors-executables/b-SZ1.4/example/testdouble_compress";
const std::string SZ_1_4_CONFIG_FILE = "../../../Compressors-executables/b-SZ1.4/example/sz.config";
const std::string SZ_1_4_CONFIG_BASE_FILE = "../../../Compressors-executables/b-SZ1.4/example/sz.config.base";
const std::string ZFP0_5_0_EXEC = "../../../Compressors-executables/c-ZFP0.5.0/examples/zfp0.5.0";
const std::string MGARD_PLUS_EXEC = "../../../Compressors-executables/d-MGARD+/mgardx";
const std::string WHITESPACE = " \n\r\t\f\v";


//////// Utilities start /////////
struct Data {
	float eb;
	float ratio;
};


struct timeval costStartCResults;
double cExecutionTimeInSec = 0;
	

void cost_start_cresults()
{
	cExecutionTimeInSec = 0.0;
	gettimeofday(&costStartCResults, NULL);
}

void cost_end_cresults()
{
	double elapsed;
	struct timeval costEnd;
	gettimeofday(&costEnd, NULL);
	elapsed = ((costEnd.tv_sec*1000000+costEnd.tv_usec)-(costStartCResults.tv_sec*1000000+costStartCResults.tv_usec))/1000000.0;
	cExecutionTimeInSec += elapsed;
}

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

void findAndReplaceInFile(std::string inputFileName, std::string outputFileName, std::string searchString, std::string replacementStr)
{
	std::fstream inputFile;
    inputFile.open(inputFileName, std::ios::in);
    
    std::fstream outputFile;
    outputFile.open(outputFileName, std::fstream::out);

	if (!inputFile.is_open()) printf("not open\n");
    
    size_t pos;
    size_t len = searchString.length();

	std::string extractLine;
    while (std::getline(inputFile, extractLine)) {
		if (extractLine.find(searchString) != std::string::npos){ 
			pos = extractLine.find(searchString);
			extractLine.replace(pos,len,replacementStr);
			outputFile << extractLine << "\n";
		}
		else {
		    outputFile << extractLine << "\n";
		}
	}
	
	inputFile.close();
	outputFile.close();
}



//////// Utilities end /////////

// compression results (error bound vs compression ratio)
void generate_szv3_compression_results(const char *datafilePath, 
								const char *splitOnDataPath,
								const int num_dims, 
								const int *dims,
								const char* outputPath)
{

	
	char *command_buffer = (char* )malloc(1000 * sizeof(char));

	printf("\n\n===== %s start======\n", __func__);
	printf("dataset file path: %s\n", datafilePath);
	sprintf(command_buffer, "mkdir %s/compression-results", outputPath);
	system((const char* ) command_buffer);

	// get the dataset file name
	// example, /home/user/QCLOUDf.dat --> QCLOUDf.dat
	char* split=strtok((char*) splitOnDataPath,"/");
	char* datafileName = NULL;

	while(split != NULL)
	{
		datafileName=split;
		split=strtok(NULL,"/");
	}

	printf("datafileName: %s\n", datafileName);

	float floating_error_bounds[] = {1e-7,5e-7, 9e-7,
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

	
	char *rm_buffer = (char* )malloc(1000 * sizeof(char));	
	char *output_buffer = (char* )malloc(1000 * sizeof(char));	
	sprintf(output_buffer, "%s/compression-results/sz3-compression-info.txt", outputPath);
	std::string output_sz(output_buffer);
		


	// free
	free(output_buffer);
		
	
	std::string cur_command;

	char *filename_buffer1 = (char* )malloc(1000 * sizeof(char));

	// sz
	sprintf(filename_buffer1, "%s/compression-results/compression-results-%s.csv", outputPath, datafileName);
	std::string csv_cmd = "rm -f "; csv_cmd += filename_buffer1;
	system(csv_cmd.c_str());
	std::fstream fout_sz;
	


	std::vector<Data> sz_data;
	std::fstream extractFile;
	std::string extractLine;

	for (int id = 0; id < num_floating_error_bounds; id++){
		float cur_eb = floating_error_bounds[id];
		
					
		
		fout_sz.open((const char* )filename_buffer1, std::fstream::out | std::fstream::app);

		

		// sz3
		sprintf(command_buffer, "%s -f -i %s -o %s.sz.out -%d", SZ3_EXEC.c_str(), datafilePath, datafilePath, num_dims);
		cur_command = ""; cur_command += command_buffer;
		for (int i = num_dims-1; i >= 0; i--) {
			cur_command += " ";
			cur_command += std::to_string(dims[i]);
		}
		sprintf(command_buffer, " -c %s -M REL %.12f -a &> %s", SZ3_CONFIG_FILE.c_str(), cur_eb, output_sz.c_str());
		cur_command += command_buffer;
		printf("command: %s\n", cur_command.c_str());

		cost_start_cresults();
		system(cur_command.c_str());
		cost_end_cresults();



		extractFile.open(output_sz, std::ios::in);
	

		float sz_cratio = 0.0, compressionTime = 0.0;
		while (std::getline(extractFile, extractLine)) {
			if (extractLine.find("compression ratio") != std::string::npos && sz_cratio < EPS){ 
				sz_cratio = std::stof(trim(mySplit(extractLine, '=')[1]));
			}
			if (extractLine.find("compression time") != std::string::npos && compressionTime < EPS){ 
				compressionTime = std::stof(trim(mySplit(extractLine, '=')[1]));
			}
		}	

		printf("Compression time with error bound %.8f: %.8f\n", cur_eb, compressionTime);

		extractFile.close();


		if (sz_cratio > 1.0 &&  sz_cratio < MAX_CRATIO + EPS) {
			sz_data.push_back({cur_eb, sz_cratio});				
			printf("==== sz finished for eb %.10f, cratio: %.4f ======\n\n\n", cur_eb, sz_cratio);
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

void generate_szv3_compression_results_specific(const char *datafilePath, 
								const char* userSpecifiedErrorBoundsFilePath,
								const char *splitOnDataPath,
								const int num_dims, 
								const int *dims,
								const char* outputPath)
{
	char *command_buffer = (char* )malloc(1000 * sizeof(char));

	printf("\n\n===== %s start======\n", __func__);
	printf("dataset file path: %s\n", datafilePath);
	sprintf(command_buffer, "mkdir %s/compression-results", outputPath);
	system((const char* ) command_buffer);

	// get the dataset file name
	// example, /home/user/QCLOUDf.dat --> QCLOUDf.dat
	char* split=strtok((char*) splitOnDataPath,"/");
	char* datafileName = NULL;

	while(split != NULL)
	{
		datafileName=split;
		split=strtok(NULL,"/");
	}

	printf("datafileName: %s\n", datafileName);
	
	std::vector<float> userSpecifiedErrorBounds;

	char *errorBoundsBuffer = (char* ) malloc(sizeof(char) * 700);
	sprintf(errorBoundsBuffer, "%s", userSpecifiedErrorBoundsFilePath);
	std::fstream errorBoundsFile(errorBoundsBuffer, std::ios::in);
	std::string line, word;
	std::vector<std::string> csvRow;
	while (getline(errorBoundsFile, line))
	{
		csvRow.clear();
		std::stringstream s(line);
		while (getline(s, word, ',')) {
			csvRow.push_back(word);
		}

		if (csvRow.size() != 1) {
			printf("There must only be one column corresponding to user-specified error bounds\n");
			exit(0);
		}

		float curErrorBound = atof(csvRow[0].c_str());
		userSpecifiedErrorBounds.push_back(curErrorBound);
	}
	


	int num_floating_error_bounds = userSpecifiedErrorBounds.size();

	



	printf("Number of floating error bounds = %d\n", 
		num_floating_error_bounds);

	
	char *rm_buffer = (char* )malloc(1000 * sizeof(char));	
	char *output_buffer = (char* )malloc(1000 * sizeof(char));	
	sprintf(output_buffer, "%s/compression-results/sz3-compression-info.txt", outputPath);
	std::string output_sz(output_buffer);
		


	// free
	free(output_buffer);
		
	
	std::string cur_command;

	char *filename_buffer1 = (char* )malloc(1000 * sizeof(char));

	// sz
	sprintf(filename_buffer1, "%s/compression-results/compression-results.csv", outputPath);
	std::string csv_cmd = "rm -f "; csv_cmd += filename_buffer1;
	system(csv_cmd.c_str());
	std::fstream fout_sz;
	


	std::vector<Data> sz_data;
	std::fstream extractFile;
	std::string extractLine;

	for (int id = 0; id < num_floating_error_bounds; id++){
		float cur_eb = userSpecifiedErrorBounds[id];
		
					
		
		fout_sz.open((const char* )filename_buffer1, std::fstream::out | std::fstream::app);

		

		// sz3
		sprintf(command_buffer, "%s -f -i %s -o %s.sz.out -%d", SZ3_EXEC.c_str(), datafilePath, datafilePath, num_dims);
		cur_command = ""; cur_command += command_buffer;
		for (int i = num_dims-1; i >= 0; i--) {
			cur_command += " ";
			cur_command += std::to_string(dims[i]);
		}
		sprintf(command_buffer, " -c %s -M REL %.12f -a &> %s", SZ3_CONFIG_FILE.c_str(), cur_eb, output_sz.c_str());
		cur_command += command_buffer;
		printf("command: %s\n", cur_command.c_str());

		cost_start_cresults();
		system(cur_command.c_str());
		cost_end_cresults();



		extractFile.open(output_sz, std::ios::in);
	

		float sz_cratio = 0.0, compressionTime = 0.0;
		while (std::getline(extractFile, extractLine)) {
			if (extractLine.find("compression ratio") != std::string::npos && sz_cratio < EPS){ 
				sz_cratio = std::stof(trim(mySplit(extractLine, '=')[1]));
			}
			if (extractLine.find("compression time") != std::string::npos && compressionTime < EPS){ 
				compressionTime = std::stof(trim(mySplit(extractLine, '=')[1]));
			}
		}	

		printf("Compression time with error bound %.8f: %.8f\n", cur_eb, compressionTime);

		extractFile.close();


		if (sz_cratio > 1.0 &&  sz_cratio < MAX_CRATIO + EPS) {
			sz_data.push_back({cur_eb, sz_cratio});				
			printf("==== sz finished for eb %.10f, cratio: %.4f ======\n\n\n", cur_eb, sz_cratio);
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
				

inline bool isFileExists (const std::string& name) {
  struct stat buffer;   
  return (stat (name.c_str(), &buffer) == 0); 
}



void generate_szv1_4_compression_results(const char *datafilePath, 
								const char *splitOnDataPath,
								const int num_dims, 
								const int *dims,
								const char* outputPath)
{

	
	char *command_buffer = (char* )malloc(1000 * sizeof(char));

	printf("\n\n===== %s start======\n", __func__);
	printf("dataset file path: %s\n", datafilePath);
	sprintf(command_buffer, "mkdir %s/compression-results", outputPath);
	system((const char* ) command_buffer);

	// get the dataset file name
	// example, /home/user/QCLOUDf.dat --> QCLOUDf.dat
	char* split=strtok((char*) splitOnDataPath,"/");
	char* datafileName = NULL;

	while(split != NULL)
	{
		datafileName=split;
		split=strtok(NULL,"/");
	}

	printf("datafileName: %s\n", datafileName);

	
	float floating_error_bounds[] = {1e-7,5e-7, 9e-7,
										1e-6, 3e-6, 5e-6, 7e-6, 9e-6, 
										1e-5, 3e-5, 5e-5, 7e-5, 9e-5,
										1e-4, 3e-4, 5e-4, 7e-4, 9e-4,
										1e-3, 2e-3, 3e-3, 4e-3, 5e-3, 6e-3, 7e-3, 8e-3, 9e-3,
										1e-2, 1.5e-2, 2e-2, 2.5e-2, 3e-2, 3.5e-2, 4e-2, 4.5e-2, 5e-2, 
										5.5e-2, 6e-2, 6.5e-2, 7e-2, 7.5e-2, 8e-2, 8.5e-2, 
										9e-2, 9.5e-2,1e-1}; 


	


	int num_floating_error_bounds = sizeof(floating_error_bounds) / sizeof(floating_error_bounds[0]);

	



	printf("Number of floating error bounds = %d\n", 
		num_floating_error_bounds);

	
	char *rm_buffer = (char* )malloc(1000 * sizeof(char));	
	char *output_buffer = (char* )malloc(1000 * sizeof(char));	
	sprintf(output_buffer, "%s/compression-results/sz1.4-compression-info.txt", outputPath);
	std::string output_sz(output_buffer);
		


	// free
	free(output_buffer);
		
	
	std::string cur_command;

	char *filename_buffer1 = (char* )malloc(1000 * sizeof(char));

	// sz
	sprintf(filename_buffer1, "%s/compression-results/compression-results-%s.csv", outputPath, datafileName);
	std::string csv_cmd = "rm -f "; csv_cmd += filename_buffer1;
	system(csv_cmd.c_str());
	std::fstream fout_sz;
	std::vector<Data> sz_data;
	std::fstream extractFile;
	std::string extractLine;	

	std::string pp = "%s";

	sprintf(command_buffer, "stat -c%s %s &> %s", pp.c_str(), datafilePath, output_sz.c_str());
	printf("stat command: %s\n", command_buffer);
	cur_command = ""; cur_command += command_buffer;
	system(cur_command.c_str());
	
	double inputFileSize = 0.0;
	
	extractFile.open(output_sz, std::ios::in);
	std::getline(extractFile, extractLine);
	extractFile.close();
	inputFileSize = std::stof(trim(extractLine));

	sprintf(command_buffer, "rm %s", output_sz.c_str());
	cur_command = ""; cur_command += command_buffer;
	system(cur_command.c_str());
	


	for (int id = 0; id < num_floating_error_bounds; id++){
		float cur_eb = floating_error_bounds[id];

		printf("Current error bound %f\n", cur_eb);
		
		
		char buffer[20];  // maximum expected length of the float
		std::snprintf(buffer, 20, "%.10f", cur_eb);
		std::string replacementStr(buffer);
		findAndReplaceInFile(SZ_1_4_CONFIG_BASE_FILE, SZ_1_4_CONFIG_FILE, "FOOBAR", replacementStr);		
		

		fout_sz.open((const char* )filename_buffer1, std::fstream::out | std::fstream::app);

	

		// sz1.4
		sprintf(command_buffer, "%s %s %s", SZ_1_4_EXEC.c_str(), SZ_1_4_CONFIG_FILE.c_str(), datafilePath);
		cur_command = ""; cur_command += command_buffer;
		for (int i = num_dims-1; i >= 0; i--) {
			cur_command += " ";
			cur_command += std::to_string(dims[i]);
		}
		sprintf(command_buffer, " &> %s",   output_sz.c_str());  cur_command += command_buffer;
		printf("command: %s\n", cur_command.c_str());
		cost_start_cresults();
		system(cur_command.c_str());
		cost_end_cresults();

		extractFile.open(output_sz, std::ios::in);
	

		float compressionTime = 0.0;
		while (std::getline(extractFile, extractLine)) {
			if (extractLine.find("timecost") != std::string::npos && compressionTime < EPS){ 
				compressionTime = std::stof(trim(mySplit(extractLine, '=')[1]));
				break;
			}
		}	

		extractFile.close();

		printf("Compression time with error bound %.8f: %.8f\n", cur_eb, compressionTime);
		


		
	

		float sz_cratio = 0.0;
		double compressedFileSize = 0.0;

		sprintf(command_buffer, "%s.sz", datafilePath);
		cur_command = ""; cur_command += command_buffer;
		if (!isFileExists(cur_command)) {
			fout_sz.close();
	
			printf("No such file %s!!\n", command_buffer);

			sprintf(rm_buffer, "rm %s.sz.out", datafilePath); 
			system((const char* )rm_buffer);	
			sprintf(rm_buffer, "rm %s.sz", datafilePath); 
			system((const char* )rm_buffer);
			system("rm core.*");
			continue;
		}

		sprintf(command_buffer, "stat -c%s %s.sz > %s", pp.c_str(), datafilePath, output_sz.c_str());
		cur_command = ""; cur_command += command_buffer;
		system(cur_command.c_str());
		
		
		extractFile.open(output_sz, std::ios::in);
		std::getline(extractFile, extractLine);
		extractFile.close();
		try
  		{
			compressedFileSize = std::stof(trim(extractLine));
		}
		catch (int e) {
			fout_sz.close();
	
			printf("exceptions!!\n");

			sprintf(rm_buffer, "rm %s.sz.out", datafilePath); 
			system((const char* )rm_buffer);	
			sprintf(rm_buffer, "rm %s.sz", datafilePath); 
			system((const char* )rm_buffer);
			system("rm core.*");
			continue;
		}

		sz_cratio = (float) (inputFileSize / compressedFileSize);
		
			

		


		if (sz_cratio > 1.0 &&  sz_cratio < MAX_CRATIO + EPS) {
			sz_data.push_back({cur_eb, sz_cratio});				
			printf("==== sz finished for eb %.10f, cratio: %.4f ======\n\n\n", cur_eb, sz_cratio);

		}	

		std::vector<Data>:: iterator it;


		for(it = sz_data.begin(); it != sz_data.end(); it++) {
			float ratio = it->ratio;
			float eb = it->eb;

			fout_sz << eb << ", " << ratio << "\n";
		}


		sz_data.clear();

					
		
		//
		


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

void generate_szv1_4_compression_results_specific(const char *datafilePath,
								const char* userSpecifiedErrorBoundsFilePath,
								const char *splitOnDataPath,
								const int num_dims, 
								const int *dims,
								const char* outputPath)
{
	char *command_buffer = (char* )malloc(1000 * sizeof(char));

	printf("\n\n===== %s start======\n", __func__);
	printf("dataset file path: %s\n", datafilePath);
	sprintf(command_buffer, "mkdir %s/compression-results", outputPath);
	system((const char* ) command_buffer);

	// get the dataset file name
	// example, /home/user/QCLOUDf.dat --> QCLOUDf.dat
	char* split=strtok((char*) splitOnDataPath,"/");
	char* datafileName = NULL;

	while(split != NULL)
	{
		datafileName=split;
		split=strtok(NULL,"/");
	}

	printf("datafileName: %s\n", datafileName);

	
	std::vector<float> userSpecifiedErrorBounds;

	char *errorBoundsBuffer = (char* ) malloc(sizeof(char) * 700);
	sprintf(errorBoundsBuffer, "%s", userSpecifiedErrorBoundsFilePath);
	std::fstream errorBoundsFile(errorBoundsBuffer, std::ios::in);
	std::string line, word;
	std::vector<std::string> csvRow;
	while (getline(errorBoundsFile, line))
	{
		csvRow.clear();
		std::stringstream s(line);
		while (getline(s, word, ',')) {
			csvRow.push_back(word);
		}

		if (csvRow.size() != 1) {
			printf("There must only be one column corresponding to user-specified error bounds\n");
			exit(0);
		}

		float curErrorBound = atof(csvRow[0].c_str());
		userSpecifiedErrorBounds.push_back(curErrorBound);
	}
	


	int num_floating_error_bounds = userSpecifiedErrorBounds.size();

	



	printf("Number of floating error bounds = %d\n", 
		num_floating_error_bounds);

	
	char *rm_buffer = (char* )malloc(1000 * sizeof(char));	
	char *output_buffer = (char* )malloc(1000 * sizeof(char));	
	sprintf(output_buffer, "%s/compression-results/sz1.4-compression-info.txt", outputPath);
	std::string output_sz(output_buffer);
		


	// free
	free(output_buffer);
		
	
	std::string cur_command;

	char *filename_buffer1 = (char* )malloc(1000 * sizeof(char));

	// sz
	sprintf(filename_buffer1, "%s/compression-results/compression-results.csv", outputPath);
	std::string csv_cmd = "rm -f "; csv_cmd += filename_buffer1;
	system(csv_cmd.c_str());
	std::fstream fout_sz;
	std::vector<Data> sz_data;
	std::fstream extractFile;
	std::string extractLine;	

	std::string pp = "%s";

	sprintf(command_buffer, "stat -c%s %s &> %s", pp.c_str(), datafilePath, output_sz.c_str());
	printf("stat command: %s\n", command_buffer);
	cur_command = ""; cur_command += command_buffer;
	system(cur_command.c_str());
	
	double inputFileSize = 0.0;
	
	extractFile.open(output_sz, std::ios::in);
	std::getline(extractFile, extractLine);
	extractFile.close();
	inputFileSize = std::stof(trim(extractLine));

	sprintf(command_buffer, "rm %s", output_sz.c_str());
	cur_command = ""; cur_command += command_buffer;
	system(cur_command.c_str());
	


	for (int id = 0; id < num_floating_error_bounds; id++){
		float cur_eb = userSpecifiedErrorBounds[id];

		printf("Current error bound %f\n", cur_eb);
		
		
		char buffer[20];  // maximum expected length of the float
		std::snprintf(buffer, 20, "%.10f", cur_eb);
		std::string replacementStr(buffer);
		findAndReplaceInFile(SZ_1_4_CONFIG_BASE_FILE, SZ_1_4_CONFIG_FILE, "FOOBAR", replacementStr);		
		

		fout_sz.open((const char* )filename_buffer1, std::fstream::out | std::fstream::app);

	

		// sz1.4
		sprintf(command_buffer, "%s %s %s", SZ_1_4_EXEC.c_str(), SZ_1_4_CONFIG_FILE.c_str(), datafilePath);
		cur_command = ""; cur_command += command_buffer;
		for (int i = num_dims-1; i >= 0; i--) {
			cur_command += " ";
			cur_command += std::to_string(dims[i]);
		}
		sprintf(command_buffer, " &> %s",   output_sz.c_str());  cur_command += command_buffer;
		printf("command: %s\n", cur_command.c_str());
		cost_start_cresults();
		system(cur_command.c_str());
		cost_end_cresults();

		extractFile.open(output_sz, std::ios::in);
	

		float compressionTime = 0.0;
		while (std::getline(extractFile, extractLine)) {
			if (extractLine.find("timecost") != std::string::npos && compressionTime < EPS){ 
				compressionTime = std::stof(trim(mySplit(extractLine, '=')[1]));
				break;
			}
		}	

		extractFile.close();

		printf("Compression time with error bound %.8f: %.8f\n", cur_eb, compressionTime);
		


		
	

		float sz_cratio = 0.0;
		double compressedFileSize = 0.0;

		sprintf(command_buffer, "%s.sz", datafilePath);
		cur_command = ""; cur_command += command_buffer;
		if (!isFileExists(cur_command)) {
			fout_sz.close();
	
			printf("No such file %s!!\n", command_buffer);

			sprintf(rm_buffer, "rm %s.sz.out", datafilePath); 
			system((const char* )rm_buffer);	
			sprintf(rm_buffer, "rm %s.sz", datafilePath); 
			system((const char* )rm_buffer);
			system("rm core.*");
			continue;
		}

		sprintf(command_buffer, "stat -c%s %s.sz > %s", pp.c_str(), datafilePath, output_sz.c_str());
		cur_command = ""; cur_command += command_buffer;
		system(cur_command.c_str());
		
		
		extractFile.open(output_sz, std::ios::in);
		std::getline(extractFile, extractLine);
		extractFile.close();
		try
  		{
			compressedFileSize = std::stof(trim(extractLine));
		}
		catch (int e) {
			fout_sz.close();
	
			printf("exceptions!!\n");

			sprintf(rm_buffer, "rm %s.sz.out", datafilePath); 
			system((const char* )rm_buffer);	
			sprintf(rm_buffer, "rm %s.sz", datafilePath); 
			system((const char* )rm_buffer);
			system("rm core.*");
			continue;
		}

		sz_cratio = (float) (inputFileSize / compressedFileSize);
		
			

		


		if (sz_cratio > 1.0 &&  sz_cratio < MAX_CRATIO + EPS) {
			sz_data.push_back({cur_eb, sz_cratio});				
			printf("==== sz finished for eb %.10f, cratio: %.4f ======\n\n\n", cur_eb, sz_cratio);

		}	

		std::vector<Data>:: iterator it;


		for(it = sz_data.begin(); it != sz_data.end(); it++) {
			float ratio = it->ratio;
			float eb = it->eb;

			fout_sz << eb << ", " << ratio << "\n";
		}


		sz_data.clear();

					
		
		//
		


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

void generate_mgard_plus_compression_results(const char *datafilePath, 
								const char *splitOnDataPath,
								const int num_dims, 
								const int *dims,
								const char* outputPath)
{
	char *command_buffer = (char* )malloc(1000 * sizeof(char));

	printf("\n\n===== %s start======\n", __func__);
	printf("dataset file path: %s\n", datafilePath);
	sprintf(command_buffer, "mkdir %s/compression-results", outputPath);
	system((const char* ) command_buffer);

	// get the dataset file name
	// example, /home/user/QCLOUDf.dat --> QCLOUDf.dat
	char* split=strtok((char*) splitOnDataPath,"/");
	char* datafileName = NULL;

	while(split != NULL)
	{
		datafileName=split;
		split=strtok(NULL,"/");
	}

	printf("datafileName: %s\n", datafileName);


	float floating_error_bounds[] = {1e-6, 3e-6, 5e-6, 7e-6, 9e-6, 
										1e-5, 3e-5, 5e-5, 7e-5, 9e-5,
										1e-4, 3e-4, 5e-4, 7e-4, 9e-4,
										1e-3, 2e-3, 3e-3, 4e-3, 5e-3, 6e-3, 7e-3, 8e-3, 9e-3,
										1e-2, 1.5e-2, 2e-2, 2.5e-2, 3e-2, 3.5e-2, 4e-2, 4.5e-2, 5e-2, 
										5.5e-2, 6e-2, 6.5e-2, 7e-2, 7.5e-2, 8e-2, 8.5e-2, 9e-2, 9.5e-2,1e-1}; 
	

	int num_floating_error_bounds = sizeof(floating_error_bounds) / sizeof(floating_error_bounds[0]);

	



	printf("Number of floating error bounds = %d\n", 
		num_floating_error_bounds);

	
	char *rm_buffer = (char* )malloc(1000 * sizeof(char));	
	char *output_buffer = (char* )malloc(1000 * sizeof(char));	
	sprintf(output_buffer, "%s/compression-results/mgardx-compression-info.txt", outputPath);
	std::string output_mgardx(output_buffer);
		


	// free
	free(output_buffer);
		
	
	std::string cur_command;

	char *filename_buffer1 = (char* )malloc(1000 * sizeof(char));

	// sz
	sprintf(filename_buffer1, "%s/compression-results/compression-results-%s.csv", outputPath, datafileName);
	std::string csv_cmd = "rm -f "; csv_cmd += filename_buffer1;
	system(csv_cmd.c_str());
	std::fstream fout_mgardx;
	


	std::vector<Data> mgardx_data;
	std::fstream extractFile;
	std::string extractLine;

	for (int id = 0; id < num_floating_error_bounds; id++){
		float cur_eb = floating_error_bounds[id];
		
					
		
		fout_mgardx.open((const char* )filename_buffer1, std::fstream::out | std::fstream::app);

		
		

		//mgardx
		sprintf(command_buffer, "%s %s 0 %.10f 3 1 3", MGARD_PLUS_EXEC.c_str(), datafilePath, cur_eb);
		cur_command = ""; cur_command += command_buffer;
		if (num_dims == 1) {
			cur_command += " 1 1";
		}
		else if (num_dims == 2) {
			cur_command += " 1";
		}
		for (int i = 0; i < num_dims; i++) {
			cur_command += " ";
			cur_command += std::to_string(dims[i]);
		}
		sprintf(command_buffer, " > %s 2>&1", output_mgardx.c_str());
		cur_command += command_buffer;

		printf("command: %s\n", cur_command.c_str());
		system(cur_command.c_str());
		


		extractFile.open(output_mgardx, std::ios::in);
	

		float mgardx_cratio = 0.0, compressionTime = 0.0;
		while (std::getline(extractFile, extractLine)) {
			if (extractLine.find("Compression ratio") != std::string::npos && mgardx_cratio < EPS){ 
				std::string cratioStr = trim(mySplit(extractLine, ',')[2]);
				mgardx_cratio = std::stof(trim(mySplit(cratioStr, '=')[1]));
			}
			if (extractLine.find("Compression time") != std::string::npos && compressionTime < EPS){ 
				compressionTime = std::stof(trim(mySplit(extractLine, ':')[1]));
			}
		}


		extractFile.close();

		printf("Compression time with error bound %.8f: %.8lf\n", cur_eb, compressionTime);



		if (mgardx_cratio > 1.0 &&  mgardx_cratio < MAX_CRATIO + EPS) {
			mgardx_data.push_back({cur_eb, mgardx_cratio});				
			printf("==== mgardx finished for eb %.10f, cratio: %.4f ======\n\n\n", cur_eb, mgardx_cratio);
		}	

					
		
		//
		std::vector<Data>:: iterator it;


		for(it = mgardx_data.begin(); it != mgardx_data.end(); it++) {
			float ratio = it->ratio;
			float eb = it->eb;

			fout_mgardx << eb << ", " << ratio << "\n";
		}


		mgardx_data.clear();


		fout_mgardx.close();
	
		

		sprintf(rm_buffer, "rm %s.mgard.out", datafilePath); 
		system((const char* )rm_buffer);	
		sprintf(rm_buffer, "rm %s.mgard", datafilePath); 
		system((const char* )rm_buffer);
		system("rm core.*");		
		
	}
		

	



	// free all
	free(command_buffer);
	free(filename_buffer1);


	std::vector<Data>().swap(mgardx_data);

	sprintf(rm_buffer, "rm %s", output_mgardx.c_str());
	system((const char* )rm_buffer);

	free(rm_buffer);
	std::string().swap(output_mgardx);	
}

void generate_mgard_plus_compression_results_specific(const char *datafilePath, 
								const char* userSpecifiedErrorBoundsFilePath,
								const char *splitOnDataPath,
								const int num_dims, 
								const int *dims,
								const char* outputPath)
{

	char *command_buffer = (char* )malloc(1000 * sizeof(char));

	printf("\n\n===== %s start======\n", __func__);
	printf("dataset file path: %s\n", datafilePath);
	sprintf(command_buffer, "mkdir %s/compression-results", outputPath);
	system((const char* ) command_buffer);

	// get the dataset file name
	// example, /home/user/QCLOUDf.dat --> QCLOUDf.dat
	char* split=strtok((char*) splitOnDataPath,"/");
	char* datafileName = NULL;

	while(split != NULL)
	{
		datafileName=split;
		split=strtok(NULL,"/");
	}

	printf("datafileName: %s\n", datafileName);


	std::vector<float> userSpecifiedErrorBounds;

	char *errorBoundsBuffer = (char* ) malloc(sizeof(char) * 700);
	sprintf(errorBoundsBuffer, "%s", userSpecifiedErrorBoundsFilePath);
	std::fstream errorBoundsFile(errorBoundsBuffer, std::ios::in);
	std::string line, word;
	std::vector<std::string> csvRow;
	while (getline(errorBoundsFile, line))
	{
		csvRow.clear();
		std::stringstream s(line);
		while (getline(s, word, ',')) {
			csvRow.push_back(word);
		}

		if (csvRow.size() != 1) {
			printf("There must only be one column corresponding to user-specified error bounds\n");
			exit(0);
		}

		float curErrorBound = atof(csvRow[0].c_str());
		userSpecifiedErrorBounds.push_back(curErrorBound);
	}
	


	int num_floating_error_bounds = userSpecifiedErrorBounds.size();

	



	printf("Number of floating error bounds = %d\n", 
		num_floating_error_bounds);

	
	char *rm_buffer = (char* )malloc(1000 * sizeof(char));	
	char *output_buffer = (char* )malloc(1000 * sizeof(char));	
	sprintf(output_buffer, "%s/compression-results/mgardx-compression-info.txt", outputPath);
	std::string output_mgardx(output_buffer);
		


	// free
	free(output_buffer);
		
	
	std::string cur_command;

	char *filename_buffer1 = (char* )malloc(1000 * sizeof(char));

	// sz
	sprintf(filename_buffer1, "%s/compression-results/compression-results.csv", outputPath);
	std::string csv_cmd = "rm -f "; csv_cmd += filename_buffer1;
	system(csv_cmd.c_str());
	std::fstream fout_mgardx;
	


	std::vector<Data> mgardx_data;
	std::fstream extractFile;
	std::string extractLine;

	for (int id = 0; id < num_floating_error_bounds; id++){
		float cur_eb = userSpecifiedErrorBounds[id];
		
					
		
		fout_mgardx.open((const char* )filename_buffer1, std::fstream::out | std::fstream::app);

		
		

		//mgardx
		sprintf(command_buffer, "%s %s 0 %.10f 3 1 3", MGARD_PLUS_EXEC.c_str(), datafilePath, cur_eb);
		cur_command = ""; cur_command += command_buffer;
		if (num_dims == 1) {
			cur_command += " 1 1";
		}
		else if (num_dims == 2) {
			cur_command += " 1";
		}
		for (int i = 0; i < num_dims; i++) {
			cur_command += " ";
			cur_command += std::to_string(dims[i]);
		}
		sprintf(command_buffer, " > %s 2>&1", output_mgardx.c_str());
		cur_command += command_buffer;

		printf("command: %s\n", cur_command.c_str());
		system(cur_command.c_str());
		


		extractFile.open(output_mgardx, std::ios::in);
	

		float mgardx_cratio = 0.0, compressionTime = 0.0;
		while (std::getline(extractFile, extractLine)) {
			if (extractLine.find("Compression ratio") != std::string::npos && mgardx_cratio < EPS){ 
				std::string cratioStr = trim(mySplit(extractLine, ',')[2]);
				mgardx_cratio = std::stof(trim(mySplit(cratioStr, '=')[1]));
			}
			if (extractLine.find("Compression time") != std::string::npos && compressionTime < EPS){ 
				compressionTime = std::stof(trim(mySplit(extractLine, ':')[1]));
			}
		}


		extractFile.close();

		printf("Compression time with error bound %.8f: %.8lf\n", cur_eb, compressionTime);



		if (mgardx_cratio > 1.0 &&  mgardx_cratio < MAX_CRATIO + EPS) {
			mgardx_data.push_back({cur_eb, mgardx_cratio});				
			printf("==== mgardx finished for eb %.10f, cratio: %.4f ======\n\n\n", cur_eb, mgardx_cratio);
		}	

					
		
		//
		std::vector<Data>:: iterator it;


		for(it = mgardx_data.begin(); it != mgardx_data.end(); it++) {
			float ratio = it->ratio;
			float eb = it->eb;

			fout_mgardx << eb << ", " << ratio << "\n";
		}


		mgardx_data.clear();


		fout_mgardx.close();
	
		

		sprintf(rm_buffer, "rm %s.mgard.out", datafilePath); 
		system((const char* )rm_buffer);	
		sprintf(rm_buffer, "rm %s.mgard", datafilePath); 
		system((const char* )rm_buffer);
		system("rm core.*");		
		
	}
		

	



	// free all
	free(command_buffer);
	free(filename_buffer1);


	std::vector<Data>().swap(mgardx_data);

	sprintf(rm_buffer, "rm %s", output_mgardx.c_str());
	system((const char* )rm_buffer);

	free(rm_buffer);
	std::string().swap(output_mgardx);
}


void generate_zfpv0_5_0_compression_results(const char *datafilePath, 
								const char *splitOnDataPath,
								const int num_dims, 
								const int *dims,
								const char* outputPath)
{

	
	char *command_buffer = (char* )malloc(1000 * sizeof(char));

	printf("\n\n===== %s start======\n", __func__);
	printf("dataset file path: %s\n", datafilePath);
	sprintf(command_buffer, "mkdir %s/compression-results", outputPath);
	system((const char* ) command_buffer);

	// get the dataset file name
	// example, /home/user/QCLOUDf.dat --> QCLOUDf.dat
	char* split=strtok((char*) splitOnDataPath,"/");
	char* datafileName = NULL;

	while(split != NULL)
	{
		datafileName=split;
		split=strtok(NULL,"/");
	}

	printf("datafileName: %s\n", datafileName);


	float floating_error_bounds[] = {1e-7,5e-7, 9e-7,
										1e-6, 3e-6, 5e-6, 7e-6, 9e-6, 
										1e-5, 3e-5, 5e-5, 7e-5, 9e-5,
										1e-4, 3e-4, 5e-4, 7e-4, 9e-4,
										1e-3, 2e-3, 3e-3, 4e-3, 5e-3, 6e-3, 7e-3, 8e-3, 9e-3,
										1e-2, 1.5e-2, 2e-2, 2.5e-2, 3e-2, 3.5e-2, 4e-2, 4.5e-2, 5e-2, 
										5.5e-2, 6e-2, 6.5e-2, 7e-2, 7.5e-2, 8e-2, 8.5e-2, 9e-2, 9.5e-2,1e-1}; 
	
	


	int num_floating_error_bounds = sizeof(floating_error_bounds) / sizeof(floating_error_bounds[0]);

	



	printf("Number of floating error bounds = %d\n", 
		num_floating_error_bounds);

	
	char *rm_buffer = (char* )malloc(1000 * sizeof(char));	
	char *output_buffer = (char* )malloc(1000 * sizeof(char));	
	sprintf(output_buffer, "%s/compression-results/zfp-compression-info.txt", outputPath);
	std::string output_zfp(output_buffer);
		


	// free
	free(output_buffer);
		
	
	std::string cur_command;

	char *filename_buffer1 = (char* )malloc(1000 * sizeof(char));

	// sz
	sprintf(filename_buffer1, "%s/compression-results/compression-results-%s.csv", outputPath, datafileName);
	std::string csv_cmd = "rm -f "; csv_cmd += filename_buffer1;
	system(csv_cmd.c_str());
	std::fstream fout_zfp;
	


	std::vector<Data> zfp_data;
	std::fstream extractFile;
	std::string extractLine;


	for (int id = 0; id < num_floating_error_bounds; id++){
		float cur_eb = floating_error_bounds[id];
		
					
		
		fout_zfp.open((const char* )filename_buffer1, std::fstream::out | std::fstream::app);

		
		

		// zfp0.5.0
		sprintf(command_buffer, "%s -s -i %s -z %s.zfp -o %s.zfp.out -d -a %.10f -%d", ZFP0_5_0_EXEC.c_str(), datafilePath,
				 datafilePath, datafilePath, cur_eb,  num_dims);
		cur_command = ""; cur_command += command_buffer;
		
		for (int i = num_dims-1; i >= 0; i--) {
			cur_command += " ";
			cur_command += std::to_string(dims[i]);
		}
		sprintf(command_buffer, " > %s 2>&1", output_zfp.c_str());
		cur_command += command_buffer;

		printf("command: %s\n", cur_command.c_str());
		// cost_start_cresults();
		system(cur_command.c_str());
		// cost_end_cresults();

		


		extractFile.open(output_zfp, std::ios::in);
	

		float zfp_cratio = 0.0, compressionTime = 0.0;
		while (std::getline(extractFile, extractLine)) {
			if (extractLine.find("compressionRatio") != std::string::npos && zfp_cratio < EPS){ 
				std::stringstream ss(extractLine);
				std::string word;
				while (ss >> word) {
					if (word.find("compressionRatio") != std::string::npos){
						break;
					}
				}
				zfp_cratio = std::stof(trim(mySplit(word, '=')[1]));
			}
			if (extractLine.find("Compression time") != std::string::npos && compressionTime < EPS){ 
				compressionTime = std::stof(trim(mySplit(extractLine, ',')[1]));
			}
		}

		
		extractFile.close();

		printf("Compression time with error bound %.8f: %.8lf\n", cur_eb, compressionTime);


		if (zfp_cratio > 1.0 &&  zfp_cratio < MAX_CRATIO + EPS) {
			zfp_data.push_back({cur_eb, zfp_cratio});				
			printf("==== zfp finished for eb %.10f, cratio: %.4f ======\n\n\n", cur_eb, zfp_cratio);
		}	

					
		
		//
		std::vector<Data>:: iterator it;


		for(it = zfp_data.begin(); it != zfp_data.end(); it++) {
			float ratio = it->ratio;
			float eb = it->eb;

			fout_zfp << eb << ", " << ratio << "\n";
		}


		zfp_data.clear();


		fout_zfp.close();
	
		

		sprintf(rm_buffer, "rm %s.zfp.out", datafilePath); 
		system((const char* )rm_buffer);	
		sprintf(rm_buffer, "rm %s.zfp", datafilePath); 
		system((const char* )rm_buffer);
		system("rm core.*");		
		
	}
		

	



	// free all
	free(command_buffer);
	free(filename_buffer1);


	std::vector<Data>().swap(zfp_data);

	sprintf(rm_buffer, "rm %s", output_zfp.c_str());
	system((const char* )rm_buffer);

	free(rm_buffer);
	std::string().swap(output_zfp);	
}



void generate_zfpv0_5_0_compression_results_specific(const char *datafilePath, 
								const char* userSpecifiedErrorBoundsFilePath,
								const char *splitOnDataPath,
								const int num_dims, 
								const int *dims,
								const char* outputPath)
{

	
	char *command_buffer = (char* )malloc(1000 * sizeof(char));

	printf("\n\n===== %s start======\n", __func__);
	printf("dataset file path: %s\n", datafilePath);
	sprintf(command_buffer, "mkdir %s/compression-results", outputPath);
	system((const char* ) command_buffer);

	// get the dataset file name
	// example, /home/user/QCLOUDf.dat --> QCLOUDf.dat
	char* split=strtok((char*) splitOnDataPath,"/");
	char* datafileName = NULL;

	while(split != NULL)
	{
		datafileName=split;
		split=strtok(NULL,"/");
	}

	printf("datafileName: %s\n", datafileName);


	std::vector<float> userSpecifiedErrorBounds;

	char *errorBoundsBuffer = (char* ) malloc(sizeof(char) * 700);
	sprintf(errorBoundsBuffer, "%s", userSpecifiedErrorBoundsFilePath);
	std::fstream errorBoundsFile(errorBoundsBuffer, std::ios::in);
	std::string line, word;
	std::vector<std::string> csvRow;
	while (getline(errorBoundsFile, line))
	{
		csvRow.clear();
		std::stringstream s(line);
		while (getline(s, word, ',')) {
			csvRow.push_back(word);
		}

		if (csvRow.size() != 1) {
			printf("There must only be one column corresponding to user-specified error bounds\n");
			exit(0);
		}

		float curErrorBound = atof(csvRow[0].c_str());
		userSpecifiedErrorBounds.push_back(curErrorBound);
	}
	


	int num_floating_error_bounds = userSpecifiedErrorBounds.size();

	



	printf("Number of floating error bounds = %d\n", 
		num_floating_error_bounds);

	
	char *rm_buffer = (char* )malloc(1000 * sizeof(char));	
	char *output_buffer = (char* )malloc(1000 * sizeof(char));	
	sprintf(output_buffer, "%s/compression-results/zfp-compression-info.txt", outputPath);
	std::string output_zfp(output_buffer);
		


	// free
	free(output_buffer);
		
	
	std::string cur_command;

	char *filename_buffer1 = (char* )malloc(1000 * sizeof(char));

	// sz
	sprintf(filename_buffer1, "%s/compression-results/compression-results.csv", outputPath);
	std::string csv_cmd = "rm -f "; csv_cmd += filename_buffer1;
	system(csv_cmd.c_str());
	std::fstream fout_zfp;
	


	std::vector<Data> zfp_data;
	std::fstream extractFile;
	std::string extractLine;


	for (int id = 0; id < num_floating_error_bounds; id++){
		float cur_eb = userSpecifiedErrorBounds[id];
		
					
		
		fout_zfp.open((const char* )filename_buffer1, std::fstream::out | std::fstream::app);

		
		

		// zfp0.5.0
		sprintf(command_buffer, "%s -s -i %s -z %s.zfp -o %s.zfp.out -d -a %.10f -%d", ZFP0_5_0_EXEC.c_str(), datafilePath,
				 datafilePath, datafilePath, cur_eb,  num_dims);
		cur_command = ""; cur_command += command_buffer;
		
		for (int i = num_dims-1; i >= 0; i--) {
			cur_command += " ";
			cur_command += std::to_string(dims[i]);
		}
		sprintf(command_buffer, " > %s 2>&1", output_zfp.c_str());
		cur_command += command_buffer;

		printf("command: %s\n", cur_command.c_str());
		// cost_start_cresults();
		system(cur_command.c_str());
		// cost_end_cresults();

		


		extractFile.open(output_zfp, std::ios::in);
	

		float zfp_cratio = 0.0, compressionTime = 0.0;
		while (std::getline(extractFile, extractLine)) {
			if (extractLine.find("compressionRatio") != std::string::npos && zfp_cratio < EPS){ 
				std::stringstream ss(extractLine);
				std::string word;
				while (ss >> word) {
					if (word.find("compressionRatio") != std::string::npos){
						break;
					}
				}
				zfp_cratio = std::stof(trim(mySplit(word, '=')[1]));
			}
			if (extractLine.find("Compression time") != std::string::npos && compressionTime < EPS){ 
				compressionTime = std::stof(trim(mySplit(extractLine, ',')[1]));
			}
		}

		
		extractFile.close();

		printf("Compression time with error bound %.8f: %.8lf\n", cur_eb, compressionTime);


		if (zfp_cratio > 1.0 &&  zfp_cratio < MAX_CRATIO + EPS) {
			zfp_data.push_back({cur_eb, zfp_cratio});				
			printf("==== zfp finished for eb %.10f, cratio: %.4f ======\n\n\n", cur_eb, zfp_cratio);
		}	

					
		
		//
		std::vector<Data>:: iterator it;


		for(it = zfp_data.begin(); it != zfp_data.end(); it++) {
			float ratio = it->ratio;
			float eb = it->eb;

			fout_zfp << eb << ", " << ratio << "\n";
		}


		zfp_data.clear();


		fout_zfp.close();
	
		

		sprintf(rm_buffer, "rm %s.zfp.out", datafilePath); 
		system((const char* )rm_buffer);	
		sprintf(rm_buffer, "rm %s.zfp", datafilePath); 
		system((const char* )rm_buffer);
		system("rm core.*");		
		
	}
		

	


	// free all
	free(command_buffer);
	free(filename_buffer1);


	std::vector<Data>().swap(zfp_data);

	sprintf(rm_buffer, "rm %s", output_zfp.c_str());
	system((const char* )rm_buffer);

	free(rm_buffer);
	std::string().swap(output_zfp);	
}
