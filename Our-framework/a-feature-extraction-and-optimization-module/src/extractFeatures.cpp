

#include "extractFeatures.h"

// 1D
const int sobelKernel1D[3] = {2, 0, -2};
const int prewittKernel1D[3] = {1, 0, -1};

// 2D
const int sobelKernel2D_Y[3][3] = {{1, 2, 1}, {0, 0, 0}, {-1, -2, -1}};
const int sobelKernel2D_X[3][3] = {{-1, 0, 1}, {-2, 0, 2}, {-1, 0, 1}};

// 3D
const int sobelKernel3D_X[3][3][3] = {{{-1, 0, 1},{-2, 0, 2}, {-1, 0, 1}},
                                        {{-2, 0, 2}, {-4, 0, 4}, {-2, 0, 2}},
                                        {{-1, 0, 1}, {-2, 0, 2}, {-1, 0, 1}}
                                        };
const int sobelKernel3D_Y[3][3][3] = {{{1, 2, 1},{0, 0, 0}, {-1, -2, -1}},
                                        {{2, 4, 2}, {0, 0, 0}, {-2, -4, -2}},
                                        {{1, 2, 1}, {0, 0, 0}, {-1, -2, -1}}
                                        };

const int sobelKernel3D_Z[3][3][3] = {{{-1, -2, -1}, {-2, -4, -2}, {-1, -2, -1}},
                                        {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}},
										{{1, 2, 1},{2, 4, 2}, {1, 2, 1}}
                                        };





// --- Utilities functions start
void* read_dataset(data_type type, const char *data_path, const int num_dims, const int *dims, int *num_entries_read) {
	printf("Reading the dataset %s\n", data_path);
	int num_data_entries = 1;
	for (int i = 0; i < num_dims; i++) num_data_entries *= dims[i];
	printf("num_data_entries %d\n", num_data_entries);

	void *data_buffer = NULL;
	if (type == data_type_float) data_buffer = (float* ) malloc(sizeof(float) * (const int)(num_data_entries));
	else if (type == data_type_double) data_buffer = (double* ) malloc(sizeof(double) * (const int)(num_data_entries));

	
	FILE *fp = fopen(data_path, "rb");
	if (fp) {
		printf("%s file opened\n", data_path);
	}	
	else{
		printf("Error in opening the file");
		exit(0);
	}

	
	int ret = (type == data_type_float) ? fread(data_buffer, sizeof(float), num_data_entries, fp): fread(data_buffer, sizeof(double), num_data_entries, fp);
	if (ret > 0) {
		*num_entries_read = ret;
		fclose(fp);
		return data_buffer;
	}
	else {
		*num_entries_read = 0;
		fclose(fp);
		return NULL;
	}

}


struct timeval costStart; 
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



double change_scale(double old_val, double old_min, double old_max, double new_min, double new_max) {
	double old_range = old_max - old_min;
	double new_range = new_max - new_min;
	double ratio = (old_val - old_min) / old_range;
	return (ratio * new_range + new_min);
}




std::vector<std::string> split_by_delimiters(char* str, const char* delimiters)
{
	char *ptr = std::strtok(str, delimiters);
	std::vector<std::string>words;

	while (ptr != NULL) {
		words.push_back(ptr);
 	        ptr = std::strtok(NULL, delimiters);		
	}	

	return words;
} 
// --- Utilities functions end







std::pair<std::vector <double>, double> extract_features_1d_kernel_based (data_type type, void *data, const int numEntries, const int numDims, 
										const int* dims,  double errorBound, void *sampledValueRange, void *sampledDataMin,
										double preprocessingTime)
{
	printf("%s start\n", __func__);
	printf("Dimensions in dataset: %zu\n", dims[0]);


	std::vector<double>features;
	int histogramBin;
	std::map<int, size_t> texturesHistFreq, dispersivenessHistFreq;

	double texturesIntensity = 0.0;
	double numTexturesHistContents = 0.0;
	double texturesHistMin = 0.0, texturesHistMax = 0.0;
	double isFirstTexturesCheck = true;
	std::vector<double> kernelResults;

	double dispersivenessFeatureExtractionTime = 0.0, 
			variationFeatureExtractionTime = 0.0,
			texturesIntensityFeatureExtractionTime = 0.0;


	
	size_t numDataPointsInvolved = 0;
	double numTraversedDataValues = 0;
	double dataVariation = 0.0;
	double absErrorBound = errorBound * ((type == data_type_float) ? *( (float*) sampledValueRange): *( (double*) sampledValueRange));

	printf("Feature extract starts....\n");
	printf("Current error bound: %10lf\n", errorBound);

	double dispersivenessHistBinSize = absErrorBound;
	double overallMinValue =  (type == data_type_float) ? (double)*( (float*) sampledDataMin): *( (double*) sampledDataMin);
	double overallValueRange = (type == data_type_float) ? (double)*( (float*) sampledValueRange): *( (double*) sampledValueRange);

	for (size_t i = 1; i < dims[0]; i += SAMPLING_OFFSET_1D) {
			size_t I = i;	
			

			// Data dispersiveness
			cost_start();
			double realValue = (type == data_type_float) ? (double)((float*)data)[I] : ((double*)data)[I];
			numTraversedDataValues ++;

			// Histogram
			histogramBin = (realValue - overallMinValue) / dispersivenessHistBinSize;
			if( dispersivenessHistFreq.find(histogramBin) == dispersivenessHistFreq.end() ) { dispersivenessHistFreq[histogramBin] = 0; }
			dispersivenessHistFreq[histogramBin] ++;
			
			numDataPointsInvolved++;
			cost_end();
			dispersivenessFeatureExtractionTime += executionTimeInSec;


			// Data variation
			cost_start();
			if (I - 1 >= 0 && I - 1 < dims[0] &&
				I + 1 >= 0 && I + 1 < dims[0]) 
			{
				double normalizedValue =  (type == data_type_float) ? (double)( ((float*)data)[I] - *( (float*) sampledDataMin)) / ((double)*( (float*) sampledValueRange)):
							(( ((double*)data)[I] - *( (double*) sampledDataMin)) / *( (double*) sampledValueRange));

				double normalizedNeighbor1 =  (type == data_type_float) ? (double)(( ((float*)data)[I - 1] - *( (float*) sampledDataMin))) / ((double)*( (float*) sampledValueRange)):
							(( ((double*)data)[I - 1] - *( (double*) sampledDataMin)) / *( (double*) sampledValueRange));
				double normalizedNeighbor2 =  (type == data_type_float) ? (double)(( ((float*)data)[I + 1] - *( (float*) sampledDataMin))) / ((double)*( (float*) sampledValueRange)):
							(( ((double*)data)[I + 1] - *( (double*) sampledDataMin)) / *( (double*) sampledValueRange));

				double prediction = (normalizedNeighbor1 + normalizedNeighbor2) / 2.0;
				dataVariation += std::fabs(prediction - normalizedValue);
			}
			cost_end();
			variationFeatureExtractionTime += executionTimeInSec;


			// Textures intensity
			cost_start();
			if (I - 1 >= 0 && I + 1 < dims[0] && 
					    I + 1 >= 0 && I + 1 < dims[0]){

				numDataPointsInvolved += 2;
				
				double sobelKernelResult = (type == data_type_float) ?  (double)((float*)data)[I - 1] * sobelKernel1D[0] + (double)((float*)data)[I + 1] * sobelKernel1D[2] : 
												 ((double*)data)[I - 1] * sobelKernel1D[0] + ((double*)data)[I + 1] * sobelKernel1D[2];


				double magnitude = sobelKernelResult;
				numTexturesHistContents ++;
				if (isFirstTexturesCheck) {texturesHistMax = texturesHistMin = magnitude; isFirstTexturesCheck = false;}
				else {
					if (texturesHistMin > magnitude) texturesHistMin = magnitude;
					if (texturesHistMax < magnitude) texturesHistMax = magnitude;
				}
				kernelResults.push_back(magnitude);
			}
			cost_end();
			texturesIntensityFeatureExtractionTime += executionTimeInSec;

		
	}

	printf("Feature extract ends....\n");

	
	
	
	// Feature calculation start
	dataVariation /= numTraversedDataValues;

	cost_start();
	double dataDispersiveness = 0.0;
	for (const auto & it: dispersivenessHistFreq) {
		double curFreq = (double) it.second;
		dataDispersiveness -= (curFreq / numTraversedDataValues) * log2(curFreq / numTraversedDataValues);
	}
	cost_end();
	dispersivenessFeatureExtractionTime += executionTimeInSec;




	double texturesHistBinSize = absErrorBound;
	cost_start();
	for (size_t i = 0; i < (size_t)numTexturesHistContents; i++){
		histogramBin = (kernelResults[i] - texturesHistMin) / texturesHistBinSize;
		if( texturesHistFreq.find(histogramBin) == texturesHistFreq.end() ) { texturesHistFreq[histogramBin] = 0; }
        texturesHistFreq[histogramBin] ++;
	}

	for (const auto & it: texturesHistFreq) {
		double curFreq = (double) it.second;
		texturesIntensity -= (curFreq / numTexturesHistContents) * log2(curFreq / numTexturesHistContents);
	}
	cost_end();
	texturesIntensityFeatureExtractionTime += executionTimeInSec;


	printf("Number of Sampled points: %zu\n", numDataPointsInvolved);
	printf("Sampling rate: %.2lf%\n", (100.0 * numDataPointsInvolved) / (1.0 * dims[0]));
	printf("Data dispersiveness feature extraction time: %.6lf sec\n", dispersivenessFeatureExtractionTime);
	printf("Data variation feature extraction time: %.6lf sec\n", variationFeatureExtractionTime);
	printf("Textures intensity feature extraction time: %.6lf sec\n", texturesIntensityFeatureExtractionTime);
	printf("Total features extraction time with Dispersiveness + Variation + Textures intensity as well as Preprocessing cost: %.8lf sec\n", 
		dispersivenessFeatureExtractionTime + variationFeatureExtractionTime + texturesIntensityFeatureExtractionTime + preprocessingTime);


	// Return the feature values
	features.push_back(dataDispersiveness);
	features.push_back(dataVariation * 100.0); // scaling
	features.push_back(texturesIntensity);

	// Clear
	std::map<int, size_t>().swap(texturesHistFreq);
	std::vector<double>().swap(kernelResults);
	std::map<int, size_t>().swap(dispersivenessHistFreq);


	

	printf("%s end\n", __func__);	
	return std::make_pair(features, dispersivenessFeatureExtractionTime + variationFeatureExtractionTime + texturesIntensityFeatureExtractionTime + preprocessingTime);



}

std::pair<std::vector <double>, double> extract_features_2d_kernel_based (data_type type, void *data, const int numEntries, const int numDims, 
										const int* dims,  double errorBound, void *sampledValueRange, void *sampledDataMin,
										double preprocessingTime)
{
	printf("%s start\n", __func__);
	printf("Dimensions in dataset: %zu %zu\n", dims[0], dims[1]);

	size_t XY = dims[0]*dims[1];
	size_t Y = dims[1];

	std::vector<double>features;
	int histogramBin;
	double magnitude;
	std::map<int, size_t> texturesHistFreq, dispersivenessHistFreq;
	

	double texturesIntensity = 0.0;
	double numTexturesHistContents = 0.0;
	double texturesHistMin = 0.0, texturesHistMax = 0.0;
	double isFirstTexturesCheck = true;
	std::vector<double> kernelResults;


	double dispersivenessFeatureExtractionTime = 0.0, 
			variationFeatureExtractionTime = 0.0,
			texturesIntensityFeatureExtractionTime = 0.0;


	
	size_t numDataPointsInvolved = 0;
	double numTraversedDataValues = 0;
	double dataVariation = 0.0;
	double absErrorBound = errorBound * ((type == data_type_float) ? *( (float*) sampledValueRange): *( (double*) sampledValueRange));
	

	printf("Feature extract starts....\n");
	printf("Current error bound: %10lf\n", errorBound);

	double dispersivenessHistBinSize = absErrorBound;
	double overallMinValue =  (type == data_type_float) ? (double)*( (float*) sampledDataMin): *( (double*) sampledDataMin);
	double overallValueRange = (type == data_type_float) ? (double)*( (float*) sampledValueRange): *( (double*) sampledValueRange);


	for (size_t i = 1; i < dims[0]; i += SAMPLING_OFFSET_2D) {
		for (size_t j = 1; j < dims[1]; j += SAMPLING_OFFSET_2D ) {
			size_t I = i*Y+j;	
			
			// Data dispersiveness
			cost_start();
			double realValue = (type == data_type_float) ? (double)((float*)data)[I] : ((double*)data)[I];
			numTraversedDataValues ++;

			// Histogram
			histogramBin = (realValue - overallMinValue) / dispersivenessHistBinSize;
			if( dispersivenessHistFreq.find(histogramBin) == dispersivenessHistFreq.end() ) { dispersivenessHistFreq[histogramBin] = 0; }
			dispersivenessHistFreq[histogramBin] ++;
			
			numDataPointsInvolved++;
			cost_end();
			dispersivenessFeatureExtractionTime += executionTimeInSec;


			// Data variation
			cost_start();
			if (I - Y >= 0 && I - Y < XY &&
				I - 1 >= 0 && I - 1 < XY &&
				I + Y >= 0 && I + Y < XY &&
				I + 1 >= 0 && I + 1 < XY) 
			{
				double normalizedValue = (type == data_type_float) ? (double)( ((float*)data)[I] - *( (float*) sampledDataMin)) / ((double)*( (float*) sampledValueRange)):
							(( ((double*)data)[I] - *( (double*) sampledDataMin)) / *( (double*) sampledValueRange));

				double normalizedNeighbor1 =  (type == data_type_float) ? (double)(( ((float*)data)[I - Y] - *( (float*) sampledDataMin))) / ((double)*( (float*) sampledValueRange)):
							(( ((double*)data)[I - Y] - *( (double*) sampledDataMin)) / *( (double*) sampledValueRange));
				double normalizedNeighbor2 =  (type == data_type_float) ? (double)(( ((float*)data)[I - 1] - *( (float*) sampledDataMin))) / ((double)*( (float*) sampledValueRange)):
							(( ((double*)data)[I - 1] - *( (double*) sampledDataMin)) / *( (double*) sampledValueRange));
				double normalizedNeighbor3 =  (type == data_type_float) ? (double)(( ((float*)data)[I + Y] - *( (float*) sampledDataMin))) / ((double)*( (float*) sampledValueRange)):
							(( ((double*)data)[I + Y] - *( (double*) sampledDataMin)) / *( (double*) sampledValueRange));
				double normalizedNeighbor4 =  (type == data_type_float) ? (double)(( ((float*)data)[I + 1] - *( (float*) sampledDataMin))) / ((double)*( (float*) sampledValueRange)):
							(( ((double*)data)[I + 1] - *( (double*) sampledDataMin)) / *( (double*) sampledValueRange));

				double prediction = (normalizedNeighbor1 + normalizedNeighbor2 + normalizedNeighbor3 + normalizedNeighbor4) / 4.0;
				dataVariation += std::fabs(prediction - normalizedValue);
			}
			cost_end();
			variationFeatureExtractionTime += executionTimeInSec;

			

			// Textures intensity
			cost_start();
			if (I - Y - 1 >= 0 && I - Y - 1 < XY && 
					    I + Y + 1 >= 0 && I + Y + 1 < XY){

				numDataPointsInvolved += 6;


				double sobelKernel2D_YResult = 0.0, sobelKernel2D_XResult = 0.0;


				sobelKernel2D_XResult = (type == data_type_float) ? ( (double)((float*)data)[I - Y - 1] * sobelKernel2D_X[0][0] +
												(double)((float*)data)[I - Y + 1] * sobelKernel2D_X[0][2] + (double)((float*)data)[I - 1] * sobelKernel2D_X[1][0] +
												(double)((float*)data)[I + 1] * sobelKernel2D_X[1][2] + 
												(double)((float*)data)[I + Y - 1] * sobelKernel2D_X[2][0] + 
												(double)((float*)data)[I + Y + 1] * sobelKernel2D_X[2][2]) : 

												( ((double*)data)[I - Y - 1] * sobelKernel2D_X[0][0] + 
												((double*)data)[I - Y + 1] * sobelKernel2D_X[0][2] + ((double*)data)[I - 1] * sobelKernel2D_X[1][0] +
												((double*)data)[I + 1] * sobelKernel2D_X[1][2] + 
												((double*)data)[I + Y - 1] * sobelKernel2D_X[2][0] + 
												((double*)data)[I + Y + 1] * sobelKernel2D_X[2][2]);

				sobelKernel2D_YResult = (type == data_type_float) ? ( (double)((float*)data)[I - Y - 1] * sobelKernel2D_Y[0][0] + (double)((float*)data)[I - Y] * sobelKernel2D_Y[0][1] + 
												(double)((float*)data)[I - Y + 1] * sobelKernel2D_Y[0][2] + 
												(double)((float*)data)[I + Y - 1] * sobelKernel2D_Y[2][0] + (double)((float*)data)[I + Y] * sobelKernel2D_Y[2][1] + 
												(double)((float*)data)[I + Y + 1] * sobelKernel2D_Y[2][2]) : 

												( ((double*)data)[I - Y - 1] * sobelKernel2D_Y[0][0] + ((double*)data)[I - Y] * sobelKernel2D_Y[0][1] + 
												((double*)data)[I - Y + 1] * sobelKernel2D_Y[0][2] + 
												((double*)data)[I + Y - 1] * sobelKernel2D_Y[2][0] + ((double*)data)[I + Y] * sobelKernel2D_Y[2][1] + 
												((double*)data)[I + Y + 1] * sobelKernel2D_Y[2][2]);


				double magnitude = sqrt(sobelKernel2D_YResult * sobelKernel2D_YResult + sobelKernel2D_XResult * sobelKernel2D_XResult);
				numTexturesHistContents ++;
				if (isFirstTexturesCheck) {texturesHistMax = texturesHistMin = magnitude; isFirstTexturesCheck = false;}
				else {
					if (texturesHistMin > magnitude) texturesHistMin = magnitude;
					if (texturesHistMax < magnitude) texturesHistMax = magnitude;
				}
				kernelResults.push_back(magnitude);
				/// 


			}
			cost_end();
			texturesIntensityFeatureExtractionTime += executionTimeInSec;



		}
	}

	printf("Feature extract ends....\n");

	// Feature calculation start
	dataVariation /= numTraversedDataValues;


	cost_start();
	double dataDispersiveness = 0.0;
	for (const auto & it: dispersivenessHistFreq) {
		double curFreq = (double) it.second;
		dataDispersiveness -= (curFreq / numTraversedDataValues) * log2(curFreq / numTraversedDataValues);
	}
	cost_end();
	dispersivenessFeatureExtractionTime += executionTimeInSec;


	double texturesHistBinSize = absErrorBound;
	cost_start();
	for (size_t i = 0; i < (size_t)numTexturesHistContents; i++){
		histogramBin = (kernelResults[i] - texturesHistMin) / texturesHistBinSize;
		if( texturesHistFreq.find(histogramBin) == texturesHistFreq.end() ) { texturesHistFreq[histogramBin] = 0; }
        texturesHistFreq[histogramBin] ++;
	}

	for (const auto & it: texturesHistFreq) {
		double curFreq = (double) it.second;
		texturesIntensity -= (curFreq / numTexturesHistContents) * log2(curFreq / numTexturesHistContents);
	}
	cost_end();
	texturesIntensityFeatureExtractionTime += executionTimeInSec;

	

	printf("Number of Sampled points: %zu\n", numDataPointsInvolved);
	printf("Sampling rate: %.2lf%\n", (100.0 * numDataPointsInvolved) / (1.0 * XY));
	printf("Data dispersiveness feature extraction time: %.6lf sec\n", dispersivenessFeatureExtractionTime);
	printf("Data variation feature extraction time: %.6lf sec\n", variationFeatureExtractionTime);
	printf("Textures intensity feature extraction time: %.6lf sec\n", texturesIntensityFeatureExtractionTime);
	printf("Total features extraction time with Dispersiveness + Variation + Textures intensity as well as Preprocessing cost: %.8lf sec\n", 
		dispersivenessFeatureExtractionTime + variationFeatureExtractionTime + texturesIntensityFeatureExtractionTime + preprocessingTime);



	// Return the feature values
	features.push_back(dataDispersiveness);
	features.push_back(dataVariation * 100.0);
	features.push_back(texturesIntensity);

	// Clear
	std::map<int, size_t>().swap(texturesHistFreq);
	std::vector<double>().swap(kernelResults);
	std::map<int, size_t>().swap(dispersivenessHistFreq);


	printf("%s end\n", __func__);	
	return std::make_pair(features, dispersivenessFeatureExtractionTime + variationFeatureExtractionTime + texturesIntensityFeatureExtractionTime + preprocessingTime);



}



std::pair<std::vector <double>, double> extract_features_3d_kernel_based (data_type type, void *data, const int numEntries, const int numDims, 
										const int* dims,  double errorBound, void *sampledValueRange, void *sampledDataMin,
										double preprocessingTime)
{
	printf("%s start\n", __func__);
	printf("Dimensions in dataset: %zu %zu %zu\n", dims[0], dims[1], dims[2]);

	size_t XYZ = dims[0]*dims[1]*dims[2];
	size_t YZ = dims[1]*dims[2];
	size_t Z = dims[2];



	std::vector<double>features;
	int histogramBin;
	std::map<int, size_t> texturesHistFreq, dispersivenessHistFreq;
	size_t numDataPointsInvolved = 0;



	double texturesIntensity = 0.0;
	double numTexturesHistContents = 0.0;
	double texturesHistMin = 0.0, texturesHistMax = 0.0;
	double isFirstTexturesCheck = true;
	std::vector<double> kernelResults;


	double dispersivenessFeatureExtractionTime = 0.0, 
			variationFeatureExtractionTime = 0.0,
			texturesIntensityFeatureExtractionTime = 0.0;


	double numTraversedDataValues = 0;
	double dataVariation = 0.0;
	double absErrorBound = errorBound * ((type == data_type_float) ? *( (float*) sampledValueRange): *( (double*) sampledValueRange));
	

	printf("Feature extract starts....\n");
	printf("Current error bound: %10lf\n", errorBound);


	
	double dispersivenessHistBinSize = absErrorBound ;
	double overallMinValue =  (type == data_type_float) ? (double)*( (float*) sampledDataMin): *( (double*) sampledDataMin);
	double overallValueRange = (type == data_type_float) ? (double)*( (float*) sampledValueRange): *( (double*) sampledValueRange);

	
	for (size_t i = 1; i < dims[0]; i += SAMPLING_OFFSET_3D) {
		for (size_t j = 1; j < dims[1]; j += SAMPLING_OFFSET_3D ) {
			for (size_t k = 1; k < dims[2]; k += SAMPLING_OFFSET_3D ) {
				size_t I = i*YZ+j*Z+k;

				// Data dispersiveness
				cost_start();	
				numDataPointsInvolved ++;
				double realValue = (type == data_type_float) ? (double)((float*)data)[I] : ((double*)data)[I];

				// histogram
				histogramBin = std::fabs(realValue - overallMinValue) / dispersivenessHistBinSize;
				if( dispersivenessHistFreq.find(histogramBin) == dispersivenessHistFreq.end() ) { dispersivenessHistFreq[histogramBin] = 0; }
				dispersivenessHistFreq[histogramBin] ++;

				numTraversedDataValues ++;
				cost_end();
				dispersivenessFeatureExtractionTime += executionTimeInSec;


				// Data variation
				cost_start();
				if (I - YZ >= 0 && I - YZ < XYZ && 
					I - Z >= 0 && I - Z < XYZ &&
					I - 1 >= 0 && I - 1 < XYZ &&
					I + YZ >= 0 && I + YZ < XYZ && 
					I + Z >= 0 && I + Z < XYZ &&
					I + 1 >= 0 && I + 1 < XYZ) 
				{

					double normalizedValue = (type == data_type_float) ? (double)(( ((float*)data)[I] - *( (float*) sampledDataMin))) / ((double)*( (float*) sampledValueRange)):
								(( ((double*)data)[I] - *( (double*) sampledDataMin)) / *( (double*) sampledValueRange));

					double normalizedNeighbor1 =  (type == data_type_float) ? (double)(( ((float*)data)[I - YZ] - *( (float*) sampledDataMin))) / ((double)*( (float*) sampledValueRange)):
								(( ((double*)data)[I - YZ] - *( (double*) sampledDataMin)) / *( (double*) sampledValueRange));
					double normalizedNeighbor2 =  (type == data_type_float) ? (double)(( ((float*)data)[I - Z] - *( (float*) sampledDataMin))) / ((double)*( (float*) sampledValueRange)):
								(( ((double*)data)[I - Z] - *( (double*) sampledDataMin)) / *( (double*) sampledValueRange));
					double normalizedNeighbor3 =  (type == data_type_float) ? (double)(( ((float*)data)[I - 1] - *( (float*) sampledDataMin))) / ((double)*( (float*) sampledValueRange)):
								(( ((double*)data)[I - 1] - *( (double*) sampledDataMin)) / *( (double*) sampledValueRange));
					double normalizedNeighbor4 =  (type == data_type_float) ? (double)(( ((float*)data)[I + YZ] - *( (float*) sampledDataMin))) / ((double)*( (float*) sampledValueRange)):
								(( ((double*)data)[I + YZ] - *( (double*) sampledDataMin)) / *( (double*) sampledValueRange));
					double normalizedNeighbor5 =  (type == data_type_float) ? (double)(( ((float*)data)[I + Z] - *( (float*) sampledDataMin))) / ((double)*( (float*) sampledValueRange)):
								(( ((double*)data)[I + Z] - *( (double*) sampledDataMin)) / *( (double*) sampledValueRange));
					double normalizedNeighbor6 =  (type == data_type_float) ? (double)(( ((float*)data)[I + 1] - *( (float*) sampledDataMin))) / ((double)*( (float*) sampledValueRange)):
								(( ((double*)data)[I + 1] - *( (double*) sampledDataMin)) / *( (double*) sampledValueRange));

					double prediction = (normalizedNeighbor1 + normalizedNeighbor2 + normalizedNeighbor3 + normalizedNeighbor4 +
											normalizedNeighbor5 + normalizedNeighbor6) / 6.0;

					
					dataVariation += std::fabs(prediction - normalizedValue);
					
				}
				cost_end();
				variationFeatureExtractionTime += executionTimeInSec;


				// Textures intensity
				cost_start();
				double convolvedSobelYResult = 0.0, convolvedSobelXResult = 0.0, convolvedSobelZResult = 0.0;

				if (I - YZ - 1 >= 0 && I - YZ - 1 < XYZ &&
					I + YZ + 1 >= 0 && I + YZ + 1 < XYZ){ 

					numDataPointsInvolved += 10;

					convolvedSobelXResult = (type == data_type_float) ? 
											((double)((float*)data)[I - YZ - 1] * sobelKernel3D_X[0][1][0] + (double)((float*)data)[I - YZ + 1] * sobelKernel3D_X[0][1][2] +
											(double)((float*)data)[I + Z - 1] * sobelKernel3D_X[1][0][0] + (double)((float*)data)[I + Z + 1] * sobelKernel3D_X[1][0][2] + 
											(double)((float*)data)[I - 1] * sobelKernel3D_X[1][1][0] + (double)((float*)data)[I + 1] * sobelKernel3D_X[1][1][2] + 
											(double)((float*)data)[I - Z - 1] * sobelKernel3D_X[1][2][0] + (double)((float*)data)[I - Z + 1] * sobelKernel3D_X[1][2][2] +
											(double)((float*)data)[I + YZ - 1] * sobelKernel3D_X[2][1][0] + (double)((float*)data)[I + YZ + 1] * sobelKernel3D_X[2][1][2]):

											(((double*)data)[I - YZ - 1] * sobelKernel3D_X[0][1][0] + ((double*)data)[I - YZ + 1] * sobelKernel3D_X[0][1][2] +
											((double*)data)[I + Z - 1] * sobelKernel3D_X[1][0][0] + ((double*)data)[I + Z + 1] * sobelKernel3D_X[1][0][2] + 
											((double*)data)[I - 1] * sobelKernel3D_X[1][1][0] + ((double*)data)[I + 1] * sobelKernel3D_X[1][1][2] + 
											((double*)data)[I - Z - 1] * sobelKernel3D_X[1][2][0] + ((double*)data)[I - Z + 1] * sobelKernel3D_X[1][2][2] + 
											((double*)data)[I + YZ - 1] * sobelKernel3D_X[2][1][0] + ((double*)data)[I + YZ + 1] * sobelKernel3D_X[2][1][2]) ;
											
				}

				if (I - YZ - Z >= 0 && I - YZ - Z < XYZ &&
					I + YZ + Z >= 0 && I + YZ + Z < XYZ){

					numDataPointsInvolved += 6;
					convolvedSobelYResult = (type == data_type_float) ? ((double)((float*)data)[I - YZ + Z] * sobelKernel3D_Y[0][0][1] + (double)((float*)data)[I - YZ - Z]  * sobelKernel3D_Y[0][2][1] +
											(double)((float*)data)[I + Z - 1] * sobelKernel3D_Y[1][0][0] + (double)((float*)data)[I + Z]  * sobelKernel3D_Y[1][0][1] + (double)((float*)data)[I + Z + 1]  * sobelKernel3D_Y[1][0][2] +
											(double)((float*)data)[I - Z - 1]  * sobelKernel3D_Y[1][2][0] + (double)((float*)data)[I - Z]  * sobelKernel3D_Y[1][2][1] + (double)((float*)data)[I - Z + 1]  * sobelKernel3D_Y[1][2][2] +
											(double)((float*)data)[I + YZ + Z]  * sobelKernel3D_Y[2][0][1] +  (double)((float*)data)[I + YZ - Z]  * sobelKernel3D_Y[2][2][1]) :

											(((double*)data)[I - YZ + Z] * sobelKernel3D_Y[0][0][1] + ((double*)data)[I - YZ - Z]  * sobelKernel3D_Y[0][2][1] +
											((double*)data)[I + Z - 1] * sobelKernel3D_Y[1][0][0] + ((double*)data)[I + Z]  * sobelKernel3D_Y[1][0][1] + ((double*)data)[I + Z + 1]  * sobelKernel3D_Y[1][0][2] +
											((double*)data)[I - Z - 1]  * sobelKernel3D_Y[1][2][0] + ((double*)data)[I - Z]  * sobelKernel3D_Y[1][2][1] + ((double*)data)[I - Z + 1]  * sobelKernel3D_Y[1][2][2] +
											((double*)data)[I + YZ + Z]  * sobelKernel3D_Y[2][0][1] +  ((double*)data)[I + YZ - Z]  * sobelKernel3D_Y[2][2][1]) ;

					
				}

				if (I - YZ - Z >= 0 && I - YZ - Z < XYZ &&
					I + YZ + Z >= 0 && I + YZ + Z < XYZ){

					numDataPointsInvolved += 2;
					convolvedSobelZResult = (type == data_type_float) ? ((double)((float*)data)[I - YZ + Z] * sobelKernel3D_Z[0][0][1] + (double)((float*)data)[I - YZ - 1]  * sobelKernel3D_Z[0][1][0] + (double)((float*)data)[I - YZ]  * sobelKernel3D_Z[0][1][1] + 
											(double)((float*)data)[I - YZ + 1]  * sobelKernel3D_Z[0][1][2] + (double)((float*)data)[I - YZ - Z]  * sobelKernel3D_Z[0][2][1] +
											(double)((float*)data)[I + YZ + Z]  * sobelKernel3D_Z[2][0][1] + (double)((float*)data)[I + YZ - 1]  * sobelKernel3D_Z[2][1][0] + (double)((float*)data)[I + YZ]  * sobelKernel3D_Z[2][1][1] + 
											(double)((float*)data)[I + YZ + 1] * sobelKernel3D_Z[2][1][2] + (double)((float*)data)[I + YZ - Z]  * sobelKernel3D_Z[2][2][1]) :

											(((double*)data)[I - YZ + Z] * sobelKernel3D_Z[0][0][1] + ((double*)data)[I - YZ - 1]  * sobelKernel3D_Z[0][1][0] + ((double*)data)[I - YZ]  * sobelKernel3D_Z[0][1][1] + 
											((double*)data)[I - YZ + 1]  * sobelKernel3D_Z[0][1][2] + ((double*)data)[I - YZ - Z]  * sobelKernel3D_Z[0][2][1] +
											((double*)data)[I + YZ + Z]  * sobelKernel3D_Z[2][0][1] + ((double*)data)[I + YZ - 1]  * sobelKernel3D_Z[2][1][0] + ((double*)data)[I + YZ]  * sobelKernel3D_Z[2][1][1] + 
											((double*)data)[I + YZ + 1] * sobelKernel3D_Z[2][1][2] + ((double*)data)[I + YZ - Z]  * sobelKernel3D_Z[2][2][1]) ;

				}


				double convolvedSobelResult = sqrt(convolvedSobelYResult * convolvedSobelYResult + convolvedSobelXResult * convolvedSobelXResult + 
									convolvedSobelZResult * convolvedSobelZResult);
				

				if (convolvedSobelResult > 0.0){
					numTexturesHistContents ++;
					if (isFirstTexturesCheck) {texturesHistMax = texturesHistMin = convolvedSobelResult; isFirstTexturesCheck = false;}
					else {
						if (texturesHistMin > convolvedSobelResult) texturesHistMin = convolvedSobelResult;
						if (texturesHistMax < convolvedSobelResult) texturesHistMax = convolvedSobelResult;
					}
					kernelResults.push_back(convolvedSobelResult);
				}
				cost_end();
				texturesIntensityFeatureExtractionTime += executionTimeInSec;


			}
		}
	}

	printf("Feature extract ends....\n");

	// Feature calculation start
	dataVariation /= numTraversedDataValues;


	cost_start();
	double dataDispersiveness = 0.0;
	for (const auto & it: dispersivenessHistFreq) {
		double curFreq = (double) it.second;
		dataDispersiveness -= (curFreq / numTraversedDataValues) * log2(curFreq / numTraversedDataValues);
	}
	cost_end();
	dispersivenessFeatureExtractionTime += executionTimeInSec;


	double texturesHistBinSize = absErrorBound ; 
	cost_start();
	for (size_t i = 0; i < (size_t)numTexturesHistContents; i++){
		histogramBin = (kernelResults[i] - texturesHistMin) / texturesHistBinSize;
		if( texturesHistFreq.find(histogramBin) == texturesHistFreq.end() ) { texturesHistFreq[histogramBin] = 0; }
        texturesHistFreq[histogramBin] ++;
	}

	for (const auto & it: texturesHistFreq) {
		double curFreq = (double) it.second;
		texturesIntensity -= (curFreq / numTexturesHistContents) * log2(curFreq / numTexturesHistContents);
	}
	cost_end();
	texturesIntensityFeatureExtractionTime += executionTimeInSec;

	
	
	printf("Number of Sampled points: %zu\n", numDataPointsInvolved);
	printf("Sampling rate: %.2lf%\n", (100.0 * numDataPointsInvolved) / (1.0 * XYZ));
	printf("Data dispersiveness feature extraction time: %.6lf sec\n", dispersivenessFeatureExtractionTime);
	printf("Data variation feature extraction time: %.6lf sec\n", variationFeatureExtractionTime);
	printf("Textures intensity feature extraction time: %.6lf sec\n", texturesIntensityFeatureExtractionTime);
	printf("Total features extraction time with Dispersiveness + Variation + Textures intensity as well as Preprocessing cost: %.8lf sec\n", 
		dispersivenessFeatureExtractionTime + variationFeatureExtractionTime + texturesIntensityFeatureExtractionTime + preprocessingTime);


	features.push_back(dataDispersiveness);
	features.push_back(dataVariation * 100.0); // scaling
	features.push_back(texturesIntensity);

	// Clear
	std::map<int, size_t>().swap(texturesHistFreq);
	std::vector<double>().swap(kernelResults);
	std::map<int, size_t>().swap(dispersivenessHistFreq);

	printf("%s end\n", __func__);
	return std::make_pair(features, dispersivenessFeatureExtractionTime + variationFeatureExtractionTime + texturesIntensityFeatureExtractionTime + preprocessingTime);


}



                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                   	
/// @brief 
/// @param type : Data type (float or double)
/// @param datafilePath : Datafile path 
/// @param numDims : Number of dimensions in dataset
/// @param dims : Size of each dimension
/// @param errorBound : Value range-based relative error bound
/// @param cRatio : Corresponding compression ratio
/// @param outputPath : Output file of extracted features
/// @param splitOnDatasetPath: Dataset name
/// @return 
int getFeatures(data_type type, const char* datafilePath, const int numDims, const int *dims, const double errorBound,
				const double cRatio, const char* outputPath, char* splitOnDatasetPath)

{
	
	printf("\n\n\n ================  %s start ================\n", __func__);
	printf("Dataset path %s\n", datafilePath);

	printf("Number of dimensions: %d\n", numDims);
	for (int i = 0; i < numDims; i++) printf("%d ", dims[i]);
	printf("\n");

	
	int retNumEntries =  0;
	

	void* data = NULL;
	data = read_dataset(type, datafilePath, numDims, dims, &retNumEntries);

	printf("num of data entries %d\n", retNumEntries);

	
	if(data == NULL){
		printf("ERROR in %s!! You should provide a dataset filepath\n", __func__);
		return 1;
	}

	void *dataMin, *dataMax;
	void *dataValueRange;



	double preprocessingTime = 0.0;

	double sampleValueRangeCalcTime = 0.0;

	switch (type)
	{
	case data_type_float :
	{

		cost_start();
		float dataMinActualSamples, dataMaxActualSamples;
		
		if (numDims == 1) {
			
			dataMinActualSamples = ((float*)data)[1], dataMaxActualSamples = ((float*)data)[1];
			for (size_t i = 1; i < dims[0]; i += SAMPLING_OFFSET_1D) {
				size_t I = i; 
				dataMinActualSamples = (dataMinActualSamples > ((float*)data)[I]) ? ((float*)data)[I] : dataMinActualSamples ;
				dataMaxActualSamples = (dataMaxActualSamples < ((float*)data)[I]) ? ((float*)data)[I] : dataMaxActualSamples ;

				if (I - 1 >= 0 && I - 1 < dims[0]) {
					dataMinActualSamples = (dataMinActualSamples > ((float*)data)[I - 1]) ? ((float*)data)[I - 1] : dataMinActualSamples ;
					dataMaxActualSamples = (dataMaxActualSamples < ((float*)data)[I - 1]) ? ((float*)data)[I - 1] : dataMaxActualSamples ;
				}	

				if (I + 1 >= 0 && I + 1 < dims[0]) {
					dataMinActualSamples = (dataMinActualSamples > ((float*)data)[I + 1]) ? ((float*)data)[I + 1] : dataMinActualSamples ;
					dataMaxActualSamples = (dataMaxActualSamples < ((float*)data)[I + 1]) ? ((float*)data)[I + 1] : dataMaxActualSamples ;
				}
				
			}
		}
		else if (numDims == 2){
			size_t XY = dims[0]*dims[1];
			size_t Y = dims[1];

			dataMinActualSamples = ((float*)data)[1*Y+1], dataMaxActualSamples = ((float*)data)[1*Y+1];
			for (size_t i = 1; i < dims[0]; i += SAMPLING_OFFSET_2D) {
				for (size_t j = 1; j < dims[1]; j += SAMPLING_OFFSET_2D ) {
					size_t I = i*Y+j; 
					dataMinActualSamples = (dataMinActualSamples > ((float*)data)[I]) ? ((float*)data)[I] : dataMinActualSamples ;
					dataMaxActualSamples = (dataMaxActualSamples < ((float*)data)[I]) ? ((float*)data)[I] : dataMaxActualSamples ;

					if (I - Y >= 0 && I - Y < XY) {
						dataMinActualSamples = (dataMinActualSamples > ((float*)data)[I - Y]) ? ((float*)data)[I - Y] : dataMinActualSamples ;
						dataMaxActualSamples = (dataMaxActualSamples < ((float*)data)[I - Y]) ? ((float*)data)[I - Y] : dataMaxActualSamples ;
					}

					if (I - 1 >= 0 && I - 1 < XY) {
						dataMinActualSamples = (dataMinActualSamples > ((float*)data)[I - 1]) ? ((float*)data)[I - 1] : dataMinActualSamples ;
						dataMaxActualSamples = (dataMaxActualSamples < ((float*)data)[I - 1]) ? ((float*)data)[I - 1] : dataMaxActualSamples ;
					}

					if (I + Y >= 0 && I + Y < XY) {
						dataMinActualSamples = (dataMinActualSamples > ((float*)data)[I + Y]) ? ((float*)data)[I + Y] : dataMinActualSamples ;
						dataMaxActualSamples = (dataMaxActualSamples < ((float*)data)[I + Y]) ? ((float*)data)[I + Y] : dataMaxActualSamples ;
					}

					if (I + 1 >= 0 && I + 1 < XY) {
						dataMinActualSamples = (dataMinActualSamples > ((float*)data)[I + 1]) ? ((float*)data)[I + 1] : dataMinActualSamples ;
						dataMaxActualSamples = (dataMaxActualSamples < ((float*)data)[I + 1]) ? ((float*)data)[I + 1] : dataMaxActualSamples ;
					}

				}
			}
		}
		else if (numDims == 3){
			size_t XYZ = dims[0]*dims[1]*dims[2];
			size_t YZ = dims[1]*dims[2];
			size_t Z = dims[2];

			dataMinActualSamples = ((float*)data)[1*YZ+1*Z+1], dataMaxActualSamples = ((float*)data)[1*YZ+1*Z+1];
			for (size_t i = 1; i < dims[0]; i += SAMPLING_OFFSET_3D ) {
				for (size_t j = 1; j < dims[1]; j += SAMPLING_OFFSET_3D ) {
					for (size_t k = 1; k < dims[2]; k += SAMPLING_OFFSET_3D ) {
						size_t I = i*YZ+j*Z+k; 
						dataMinActualSamples = (dataMinActualSamples > ((float*)data)[I]) ? ((float*)data)[I] : dataMinActualSamples ;
						dataMaxActualSamples = (dataMaxActualSamples < ((float*)data)[I]) ? ((float*)data)[I] : dataMaxActualSamples ;

						if (I - YZ >= 0 && I - YZ < XYZ) {
							dataMinActualSamples = (dataMinActualSamples > ((float*)data)[I - YZ]) ? ((float*)data)[I - YZ] : dataMinActualSamples ;
							dataMaxActualSamples = (dataMaxActualSamples < ((float*)data)[I - YZ]) ? ((float*)data)[I - YZ] : dataMaxActualSamples ;
						}

						if (I -Z >= 0 && I -Z < XYZ) {
							dataMinActualSamples = (dataMinActualSamples > ((float*)data)[I -Z]) ? ((float*)data)[I -Z] : dataMinActualSamples ;
							dataMaxActualSamples = (dataMaxActualSamples < ((float*)data)[I -Z]) ? ((float*)data)[I -Z] : dataMaxActualSamples ;
						}

						if (I - 1 >= 0 && I - 1 < XYZ) {
							dataMinActualSamples = (dataMinActualSamples > ((float*)data)[I - 1]) ? ((float*)data)[I - 1] : dataMinActualSamples ;
							dataMaxActualSamples = (dataMaxActualSamples < ((float*)data)[I - 1]) ? ((float*)data)[I - 1] : dataMaxActualSamples ;
						}

						if (I + YZ >= 0 && I + YZ < XYZ) {
							dataMinActualSamples = (dataMinActualSamples > ((float*)data)[I + YZ]) ? ((float*)data)[I + YZ] : dataMinActualSamples ;
							dataMaxActualSamples = (dataMaxActualSamples < ((float*)data)[I + YZ]) ? ((float*)data)[I + YZ] : dataMaxActualSamples ;
						}

						if (I + Z >= 0 && I + Z < XYZ) {
							dataMinActualSamples = (dataMinActualSamples > ((float*)data)[I + Z]) ? ((float*)data)[I + Z] : dataMinActualSamples ;
							dataMaxActualSamples = (dataMaxActualSamples < ((float*)data)[I + Z]) ? ((float*)data)[I + Z] : dataMaxActualSamples ;
						}

						if (I + 1 >= 0 && I + 1 < XYZ) {
							dataMinActualSamples = (dataMinActualSamples > ((float*)data)[I + 1]) ? ((float*)data)[I + 1] : dataMinActualSamples ;
							dataMaxActualSamples = (dataMaxActualSamples < ((float*)data)[I + 1]) ? ((float*)data)[I + 1] : dataMaxActualSamples ;
						}
						

					}
				}
			}
		}
		
		float actualSamplesValRange = dataMaxActualSamples - dataMinActualSamples;
		printf("[float data type] Using sample data points : min value %.12f, max value %.12f\n", dataMinActualSamples, dataMaxActualSamples);


		dataValueRange = &actualSamplesValRange;
		dataMin = &dataMinActualSamples;
		

		cost_end();
		sampleValueRangeCalcTime = executionTimeInSec;
		
	}
		break;
	case data_type_double:
	{
		cost_start();
		double dataMinActualSamples, dataMaxActualSamples;
		
		if (numDims == 1) {
			
			dataMinActualSamples = ((double*)data)[1], dataMaxActualSamples = ((double*)data)[1];
			for (size_t i = 1; i < dims[0]; i += SAMPLING_OFFSET_1D) {
				size_t I = i; 
				dataMinActualSamples = (dataMinActualSamples > ((double*)data)[I]) ? ((double*)data)[I] : dataMinActualSamples ;
				dataMaxActualSamples = (dataMaxActualSamples < ((double*)data)[I]) ? ((double*)data)[I] : dataMaxActualSamples ;

				if (I - 1 >= 0 && I - 1 < dims[0]) {
					dataMinActualSamples = (dataMinActualSamples > ((double*)data)[I - 1]) ? ((double*)data)[I - 1] : dataMinActualSamples ;
					dataMaxActualSamples = (dataMaxActualSamples < ((double*)data)[I - 1]) ? ((double*)data)[I - 1] : dataMaxActualSamples ;
				}

				if (I + 1 >= 0 && I + 1 < dims[0]) {
					dataMinActualSamples = (dataMinActualSamples > ((double*)data)[I + 1]) ? ((double*)data)[I + 1] : dataMinActualSamples ;
					dataMaxActualSamples = (dataMaxActualSamples < ((double*)data)[I + 1]) ? ((double*)data)[I + 1] : dataMaxActualSamples ;
				}
					
			}
		}
		else if (numDims == 2){
			size_t XY = dims[0]*dims[1];
			size_t Y = dims[1];

			dataMinActualSamples = ((double*)data)[1*Y+1], dataMaxActualSamples = ((double*)data)[1*Y+1];
			for (size_t i = 1; i < dims[0]; i += SAMPLING_OFFSET_2D ) {
				for (size_t j = 1; j < dims[1]; j += SAMPLING_OFFSET_2D ) {
					size_t I = i*Y+j; 
					dataMinActualSamples = (dataMinActualSamples > ((double*)data)[I]) ? ((double*)data)[I] : dataMinActualSamples ;
					dataMaxActualSamples = (dataMaxActualSamples < ((double*)data)[I]) ? ((double*)data)[I] : dataMaxActualSamples ;

					if (I - Y >= 0 && I - Y < XY) {
						dataMinActualSamples = (dataMinActualSamples > ((double*)data)[I - Y]) ? ((double*)data)[I - Y] : dataMinActualSamples ;
						dataMaxActualSamples = (dataMaxActualSamples < ((double*)data)[I - Y]) ? ((double*)data)[I - Y] : dataMaxActualSamples ;
					}

					if (I - 1 >= 0 && I - 1 < XY) {
						dataMinActualSamples = (dataMinActualSamples > ((double*)data)[I - 1]) ? ((double*)data)[I - 1] : dataMinActualSamples ;
						dataMaxActualSamples = (dataMaxActualSamples < ((double*)data)[I - 1]) ? ((double*)data)[I - 1] : dataMaxActualSamples ;
					}

					if (I + Y >= 0 && I + Y < XY) {
						dataMinActualSamples = (dataMinActualSamples > ((double*)data)[I + Y]) ? ((double*)data)[I + Y] : dataMinActualSamples ;
						dataMaxActualSamples = (dataMaxActualSamples < ((double*)data)[I + Y]) ? ((double*)data)[I + Y] : dataMaxActualSamples ;
					}

					if (I + 1 >= 0 && I + 1 < XY) {
						dataMinActualSamples = (dataMinActualSamples > ((double*)data)[I + 1]) ? ((double*)data)[I + 1] : dataMinActualSamples ;
						dataMaxActualSamples = (dataMaxActualSamples < ((double*)data)[I + 1]) ? ((double*)data)[I + 1] : dataMaxActualSamples ;
					}

				}
			}
		}
		else if (numDims == 3) {
			size_t XYZ = dims[0]*dims[1]*dims[2];
			size_t YZ = dims[1]*dims[2];
			size_t Z = dims[2];

			dataMinActualSamples = ((double*)data)[1*YZ+1*Z+1], dataMaxActualSamples = ((double*)data)[1*YZ+1*Z+1];
			for (size_t i = 1; i < dims[0]; i += SAMPLING_OFFSET_3D) {
				for (size_t j = 1; j < dims[1]; j += SAMPLING_OFFSET_3D ) {
					for (size_t k = 1; k < dims[2]; k += SAMPLING_OFFSET_3D ) {
						size_t I = i*YZ+j*Z+k; 
						dataMinActualSamples = (dataMinActualSamples > ((double*)data)[I]) ? ((double*)data)[I] : dataMinActualSamples ;
						dataMaxActualSamples = (dataMaxActualSamples < ((double*)data)[I]) ? ((double*)data)[I] : dataMaxActualSamples ;
						
						if (I - YZ >= 0 && I - YZ < XYZ) {
							dataMinActualSamples = (dataMinActualSamples > ((double*)data)[I - YZ]) ? ((double*)data)[I - YZ] : dataMinActualSamples ;
							dataMaxActualSamples = (dataMaxActualSamples < ((double*)data)[I - YZ]) ? ((double*)data)[I - YZ] : dataMaxActualSamples ;
						}

						if (I -Z >= 0 && I -Z < XYZ) {
							dataMinActualSamples = (dataMinActualSamples > ((double*)data)[I -Z]) ? ((double*)data)[I -Z] : dataMinActualSamples ;
							dataMaxActualSamples = (dataMaxActualSamples < ((double*)data)[I -Z]) ? ((double*)data)[I -Z] : dataMaxActualSamples ;
						}

						if (I - 1 >= 0 && I - 1 < XYZ) {
							dataMinActualSamples = (dataMinActualSamples > ((double*)data)[I - 1]) ? ((double*)data)[I - 1] : dataMinActualSamples ;
							dataMaxActualSamples = (dataMaxActualSamples < ((double*)data)[I - 1]) ? ((double*)data)[I - 1] : dataMaxActualSamples ;
						}

						if (I + YZ >= 0 && I + YZ < XYZ) {
							dataMinActualSamples = (dataMinActualSamples > ((double*)data)[I + YZ]) ? ((double*)data)[I + YZ] : dataMinActualSamples ;
							dataMaxActualSamples = (dataMaxActualSamples < ((double*)data)[I + YZ]) ? ((double*)data)[I + YZ] : dataMaxActualSamples ;
						}

						if (I + Z >= 0 && I + Z < XYZ) {
							dataMinActualSamples = (dataMinActualSamples > ((double*)data)[I + Z]) ? ((double*)data)[I + Z] : dataMinActualSamples ;
							dataMaxActualSamples = (dataMaxActualSamples < ((double*)data)[I + Z]) ? ((double*)data)[I + Z] : dataMaxActualSamples ;
						}

						if (I + 1 >= 0 && I + 1 < XYZ) {
							dataMinActualSamples = (dataMinActualSamples > ((double*)data)[I + 1]) ? ((double*)data)[I + 1] : dataMinActualSamples ;
							dataMaxActualSamples = (dataMaxActualSamples < ((double*)data)[I + 1]) ? ((double*)data)[I + 1] : dataMaxActualSamples ;
						}

					}
				}
			}
		}

		double actualSamplesValRange = dataMaxActualSamples - dataMinActualSamples;
		printf("[double data type] Using sample data points : min value %.12lf, max value %.12lf\n", dataMinActualSamples, dataMaxActualSamples);

		dataValueRange = &actualSamplesValRange;
		dataMin = &dataMinActualSamples;
		
		cost_end();
		sampleValueRangeCalcTime = executionTimeInSec;
		

	}
	break;
	
	default:
		break;
	}

	preprocessingTime += sampleValueRangeCalcTime;

	
	
	printf("Preprocessing time: %.6lf sec\n", preprocessingTime);
	
	
		
	std::vector <double> features;
	double totalFeaturesExtractionTime = 0.0;

	switch (numDims)
	{
	case 1:
	{
		std::pair <std::vector<double>, double> ret = extract_features_1d_kernel_based (type, data, (const int) retNumEntries, (const int) numDims, 
														(const int*) dims,  errorBound, dataValueRange, dataMin, preprocessingTime);
		features = ret.first;
		totalFeaturesExtractionTime = ret.second;
	}
		break;
	case 2:
	{
		std::pair <std::vector<double>, double> ret = extract_features_2d_kernel_based (type, data, (const int) retNumEntries, (const int) numDims, 
														(const int*) dims,  errorBound, dataValueRange, dataMin, preprocessingTime);
		features = ret.first;
		totalFeaturesExtractionTime = ret.second;
	}
		break;
	case 3:
	{
		std::pair <std::vector<double>, double> ret = extract_features_3d_kernel_based (type, data, (const int) retNumEntries, (const int) numDims, 
														(const int*) dims,  errorBound, dataValueRange, dataMin, preprocessingTime);
		features = ret.first;
		totalFeaturesExtractionTime = ret.second;
	}
		break;
	
	default:
		break;
	}

	
	if (features.size() == 0) {
		printf("Number of extracted features should be non-zero!\n");
		exit(0);
	}

		
	
		


	printf("Training or Testing data sample: \n");
	for (double feature: features) {
		printf("%.14lf ", feature);
	}
	printf("%.9lf %.2lf\n", errorBound, cRatio);
	printf("\n");

	// get dataset name
	char* split=strtok((char*) splitOnDatasetPath,"/");
	char* datafileName = NULL;

	while(split != NULL)
	{
		datafileName=split;
		split=strtok(NULL,"/");
	}


	char *buffer = (char* )malloc(500 * sizeof(char));

	std::fstream foutResult;
	sprintf(buffer, "%s/features-extraction/feature_extraction_result_%s.csv", outputPath, datafileName);
	foutResult.open((const char* )buffer, std::fstream::out | std::fstream::app);
	

	char* nameBuffer = (char* )malloc(500 * sizeof(char));
    sprintf(nameBuffer, "%s", datafileName);
    std::string nameBuf(nameBuffer);

	foutResult << nameBuf << ",";	
	for (double curFeature: features) {
		foutResult << curFeature << ",";	
	}
	foutResult << errorBound << "," << cRatio << "\n";
	foutResult.close();


	std::fstream foutTime;
	sprintf(buffer, "%s/features-extraction/feature_extraction_running_time_%s.csv", outputPath, datafileName);
	foutTime.open((const char* )buffer, std::fstream::out | std::fstream::app);
	sprintf(nameBuffer, "%s", datafileName);
	std::string nameData(nameBuffer);
	foutTime << nameData << "," << errorBound << "," << totalFeaturesExtractionTime << "\n";
	foutTime.close();




	

	// Clear
	free(data);
	free(nameBuffer);
	free(buffer);	
	std::string().swap(nameBuf);
	std::string().swap(nameData);
	std::vector<double>().swap(features);
	return 0;


}

