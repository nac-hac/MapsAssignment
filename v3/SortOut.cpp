/********************************************************

	SortOut - integer sorting and file output program - OMP Parallel For

	Goldberg, Peter
	Hornung, Nico
	Worszeck, Sascha

	MAPS - SHU - 12/03/2012

*********************************************************/

//Includes
#include "SortOut.h"

#include <fstream>			//for file output
#include <sstream>			//for ostringstream
#include <conio.h>			//for kbhit
#include <omp.h>

int main() 
{
	//Run application (everything takes place in constructor)
	//Needs to be created on heap because it's too big for the stack
	SortOut* sortOut = new SortOut();

	//Cleanup memory
	delete sortOut;
}

//Constructor
SortOut::SortOut() {
	getData();	
	sortRows();
	testData();
	output2DArray(data, "soDataRows.txt", swOutputSortedRows);	// outputSortedRows()
	printf("----------------------------------------------\n\n");
	sortAll();
	outputSortedAll();
	printf("----------------------------------------------\n\n");
	calcMovingAve();
	output2DArray(ave, "soDataAve.txt", swOutputAveRows);		// outputAveRows()
	printf("----------------------------------------------\n\n");
	outputTimes();

	while (! _kbhit());  //to hold console	
}

//Destructor
SortOut::~SortOut() {

}

// ********************
// Methods to be timed
// ********************

//Sort data array
void SortOut::sortRows()
{
	printf("*** sortRows ***\n\n");
	swSortRows.startTimer();

	#pragma omp parallel for
	for (int i = 0; i < MAX_ROWS; ++i) 
	{
		//Use V2 because the amount of data in one row is low enough for this version to be slightly faster
		bubblesortSplitV2(data[i], MAX_COLS, SEGMENTS_DATA);
	}

	swSortRows.stopTimer();
	printf("Done.\n\n");
}

//Output the sorted data/moving average array into file
void SortOut::output2DArray(int data[MAX_ROWS][MAX_COLS], const char* fileName, CStopWatch& sw) {
	std::ostringstream os;
	os << "*** outputting data to " << fileName << "... ***\n\n";
	printf(os.str().c_str());
	sw.startTimer();

	//Create an array of strings - each element contains the text-data of one row
	//We need this so we can parallelize the char appends
	std::string output[MAX_ROWS];

	#pragma omp parallel for
	for(int i=0; i < MAX_ROWS; ++i) 
	{
		for(int j=0; j < MAX_COLS; ++j) 
		{
			char buffer [33];
			_itoa_s(data[i][j], buffer, 10);
			output[i] += buffer;
			output[i] += "\t";
		}
		output[i] += "\n";
	}

	//Append all strings into one, so we can output it to file
	for (int i = 1; i < MAX_ROWS; ++i) 
	{
		output[0] += output[i];
	}

	outputToFile(fileName, output[0].c_str());
	sw.stopTimer();
	printf("Done.\n\n");
}

//Sort allData array
void SortOut::sortAll()
{
	printf("*** sortAll ***\n\n");
	swSortAll.startTimer();

	for (int i = 0; i < MAX_ROWS; ++i) {
		memcpy((int*)&allData[i*MAX_COLS], (int*)&data[i][0], sizeof(int)*MAX_COLS);
	}

	//Calculate this only once
	int segmentSize = MAX_COLS;
	
	//An array of an amount of "MAX_ROWS" int pointers
	int** segments = new int*[MAX_ROWS];	

	//Let each pointer point to a factorized offset of "segmentsize" of the inputarray
	for (int i = 0; i < MAX_ROWS; ++i) 
	{
		segments[i] = &allData[i*segmentSize];
	}
	
	//Create tail array and insert rest
	int* tail = new int[0];
	int tailSize = 0;

	//This Algorithm merges an array of segments in the following parallelisable way 
	// 10 Elements, hyphen = merged, in = iteration
	// i1: 0 1 2 3 4 5 6 7 8 9 -> 
	// i2: 0-1 2-3 4-5 6-7 8-9 -> 
	// i3: 0-1-2-3 4-5-6-7 Tail: 8-9 ->
	// i4: 0-1-2-3-4-5-6-7 Tail: 8-9 ->
	// i5: 0-1-2-3-4-5-6-7-8-9
	for (int segmentCounter = MAX_ROWS; segmentCounter > 1; segmentCounter/=2) {
		//If segment amount is not an even number, then put last segment into tail
		if (segmentCounter%2 != 0) {
			int *temp = new int[segmentSize+tailSize];
			merge(tailSize, segmentSize, tail, segments[segmentCounter-1], temp);
			//Avoid copying by deleting old tail and putting tailpointer on temp
			delete[] tail;
			tail = &(*temp);
			tailSize += segmentSize;
			--segmentCounter;
		}

		//Now merge segments, 2 at a time, in parallel
		//After each loop pointer offset is doubled to point to the first member of the new larger arrays
		int segmentCounterHalved = segmentCounter/2; //calculate this only once

		#pragma omp parallel for
		for (int i = 0; i < segmentCounterHalved; ++i) 
		{
			int segmentOffset = 2*i;  //calculates offset only once
			int *temp = new int[2*segmentSize];	//creates temp array to store merge
			merge(segmentSize, segmentSize, segments[segmentOffset], segments[segmentOffset + 1], temp); //merge two arrays, store in temp			
			memcpy((int*)&allData[segmentSize*segmentOffset], temp, sizeof(int)*segmentSize*2); //copy temp back into the data array
			delete[] temp; //delete to free heap
		}

		segmentSize*=2;	//double segment size as segements have doubled in size

		//Offset pointer addresses because segments became larger
		for(int i = 1; i < segmentCounterHalved; ++i)
		{
			segments[i] = segments[i*2]; 
		}
	}

	//Merge data array with tail
	int* temp = new int[MAX_ELEMENTS];
	merge(segmentSize, tailSize, allData, tail, temp);
	memcpy(allData, temp, sizeof(int)*MAX_ELEMENTS);

	//Cleanup heap
	delete[] tail;
	delete[] segments;

	swSortAll.stopTimer();
	printf("Done.\n\n");
}

//Output allData array into file
void SortOut::outputSortedAll()
{
	printf("*** outputting data to soDataAll.txt... ***\n\n");
	swOutputSortedAll.startTimer();

	//Create an array of strings - each element contains the text-data of one row
	//We need this so we can parallelize the char appends
	std::string output[MAX_ROWS];

	#pragma omp parallel for
	for(int i = 0; i < MAX_ROWS; ++i)
	{
		for(int j = 0; j < MAX_COLS; ++j)
		{
			char buffer [33];
			_itoa_s(allData[i*MAX_COLS + j], buffer, 10);
			output[i] += buffer;
			output[i] += "\t";
		}
		output[i] += "\n";
	}
	
	//Append all strings into one, so we can output it to file
	for (int i = 1; i < MAX_ROWS; ++i) 
	{
		output[0] += output[i];
	}

	outputToFile("soDataAll.txt", output[0].c_str());
	swOutputSortedAll.stopTimer();
	printf("Done.\n\n");
}

//Calculate the average of the next 100 elements for each element
void SortOut::calcMovingAve()
{
	printf("*** calcMovingAve ***\n\n");
	swCalcMovingAve.startTimer();

	#pragma omp parallel for
		for(int i = 0; i < MAX_ROWS; ++i)
		{
			for (int j = 0; j < MAX_COLS; ++j) 
			{
				int numAveElements;
				if (j <= MAX_COLS - 100)
					numAveElements = 100;
				else 
					numAveElements = MAX_COLS - j;

				int sum = 0;
				int colMax = j + numAveElements;	// calculate this only once
				for (int k = j; k < colMax; ++k)
					sum += data[i][k];

				ave[i][j] = sum/numAveElements;
			}
		}

	swCalcMovingAve.stopTimer();
	printf("Done.\n\n");
}

// ********************
// helper methods
// ********************

//Feed data into data array
//Dont parallelize because results otherwise wouldn't be testable
void SortOut::getData() {

	printf("*** getData ***\n\n");

	srand(123); //arbitrary random number seed

	for(int i=0; i<MAX_ROWS; i++) 
	{
		for(int j=0; j<MAX_COLS; j++) 
		{
			data[i][j] = rand(); //RAND_MAX = 32767
		}
	}

	printf("Done.\n\n");
}

//Test sorted rows result for correctness
//Only works with seed(123), MAX_ROWS = 2000 and MAX_COLS = 1000
void SortOut::testData() {
	printf("*** testData ***\n\n");
	if (data[0][0] == 87 &&
		data[MAX_ROWS/2][MAX_COLS/2] == 16440 &&
		data[MAX_ROWS-1][MAX_COLS-1] == 32760)
		printf("TEST SUCCEEDED!\n\n");
	else
		printf("TEST FAILED!\n\n");
}

//Output all timer data to console
void SortOut::outputTimes()
{
	printf("*** outputTimes ***\n\n");

	std::ostringstream os;
	os	<< "sortRows:\t\t"			<< swSortRows.getElapsedTime()			<< " seconds\n";
	os	<< "outputSortedRows:\t"	<< swOutputSortedRows.getElapsedTime()	<< " seconds\n";
	os	<< "sortAll:\t\t"			<< swSortAll.getElapsedTime()			<< " seconds\n";
	os	<< "outputSortedAll:\t"		<< swOutputSortedAll.getElapsedTime()	<< " seconds\n";
	os	<< "calcMovingAve:\t\t"		<< swCalcMovingAve.getElapsedTime()		<< " seconds\n";
	os	<< "outputAveRows:\t\t"		<< swOutputAveRows.getElapsedTime()		<< " seconds\n";
	os	<< "\t\t\t==========\n";
	os	<< "TOTAL:\t\t\t"
		<<	swSortRows.getElapsedTime()+swOutputSortedRows.getElapsedTime()+
			swSortAll.getElapsedTime()+swOutputSortedAll.getElapsedTime()+
			swCalcMovingAve.getElapsedTime()+swOutputAveRows.getElapsedTime() 
		<< " seconds.\n";
	printf(os.str().c_str());
}

//Handles file creation and output
void SortOut::outputToFile(const char* filename, const char* content) {
	FILE * pFile;
	fopen_s(&pFile, filename, "w");
	fputs(content, pFile);
	fclose (pFile);
}

//Split array into segments for efficient bubblesorting with a parallelizable merge
//V2 has better performance on lesser elements, because the merge of V1 comes with a big overhead.
//This is especially due to a lot of expensive heap allocation which is neccessary to let the algorithm work with any parameter
void SortOut::bubblesortSplitV1(int* data, const int dataSize, const int numberOfSegments) 
{
	//Calculate this only once
	int rest = dataSize % numberOfSegments;
	int segmentSize = dataSize/numberOfSegments;
	
	//An array of an amount of "numberOfThreads" int pointers
	int** segments = new int*[numberOfSegments];	

	//Let each pointer point to a factorized offset of "segmentsize" of the inputarray
	for (int i = 0; i < numberOfSegments; ++i) 
	{
		segments[i] = &data[i*segmentSize];
	}

	//Sort each segment in parallel
	#pragma omp parallel for
	for (int i = 0; i < numberOfSegments; ++i) 
	{			
		bubblesort(segments[i], segmentSize);
	}	

	//Sort rest
	bubblesort((int*)&data[dataSize-rest], rest);
	
	//Create tail array and insert rest
	int* tail = new int[rest];
	memcpy(tail, (int*)&data[dataSize-rest], sizeof(int)*rest);
	int tailSize = rest;

	//This Algorithm merges an array of segments in the following parallelisable way 
	// 10 Elements, hyphen = merged, in = iteration
	// i1: 0 1 2 3 4 5 6 7 8 9 -> 
	// i2: 0-1 2-3 4-5 6-7 8-9 -> 
	// i3: 0-1-2-3 4-5-6-7 Tail: 8-9 ->
	// i4: 0-1-2-3-4-5-6-7 Tail: 8-9 ->
	// i5: 0-1-2-3-4-5-6-7-8-9
	for (int segmentCounter = numberOfSegments; segmentCounter > 1; segmentCounter/=2) {
		//If segment amount is not an even number, then put last segment into tail
		if (segmentCounter%2 != 0) {
			int *temp = new int[segmentSize+tailSize];
			merge(tailSize, segmentSize, tail, segments[segmentCounter-1], temp);
			//Avoid copying by deleting old tail and putting tailpointer on temp
			delete[] tail;
			tail = &(*temp);
			tailSize += segmentSize;
			--segmentCounter;
		}

		//Now merge segments, 2 at a time, in parallel
		//After each loop pointer offset is doubled to point to the first member of the new larger arrays
		int segmentCounterHalved = segmentCounter/2; //calculate this only once

		#pragma omp parallel for
		for (int i = 0; i < segmentCounterHalved; ++i) 
		{
			int segmentOffset = 2*i;  //calculates offset only once
			int *temp = new int[2*segmentSize];	//creates temp array to store merge
			merge(segmentSize, segmentSize, segments[segmentOffset], segments[segmentOffset + 1], temp); //merge two arrays, store in temp			
			memcpy((int*)&data[segmentSize*segmentOffset], temp, sizeof(int)*segmentSize*2); //copy temp back into the data array
			delete[] temp; //delete to free heap
		}

		segmentSize*=2;	//double segment size as segements have doubled in size

		//Offset pointer addresses because segments became larger
		for(int i = 1; i < segmentCounterHalved; ++i)
		{
			segments[i] = segments[i*2]; 
		}
	}

	//Merge data array with tail
	int* temp = new int[dataSize];
	merge(segmentSize, tailSize, data, tail, temp);
	memcpy(data, temp, sizeof(int)*dataSize);

	//Cleanup heap
	delete[] tail;
	delete[] segments;
}

//split array into segments for efficient bubblesorting
void SortOut::bubblesortSplitV2(int* data, const int dataSize, const int numberOfSegments)
{
	//Calculate this only once
	int segmentSize = dataSize/numberOfSegments;
	
	//An array of an amount of "numberOfThreads" int pointers
	int** segments = new int*[numberOfSegments];	

	//Let each pointer point to a factorized offset of "segmentsize" of the inputarray
	for (int i = 0; i < numberOfSegments; ++i) 
	{
		segments[i] = &data[i*segmentSize];
	}

	//Sort each segment
	for (int i = 0; i < numberOfSegments; ++i) 
	{			
		bubblesort(segments[i], segmentSize);
	}	

	//Sort tail
	int rest = dataSize % numberOfSegments;
	int maxNoRest = dataSize-rest; //caculate only once 
	bubblesort((int*)&data[maxNoRest], rest);

	//This Algorithm merges an array of segments in the following way 
	// 6 Elements, hyphen = merged, in = iteration
	// i1: 0 1 2 3 4 5 -> 
	// i2: 0-1 2 3 4 5 -> 
	// i3: 0-1-2 3 4 5 ->
	// i4: 0-1-2-3-4 5 ->
	// i5: 0-1-2-3-4-5
	int* temp = new int[dataSize]; //speedup if created on stack, but decreases flexibility of method
	for (int i = 0; i < numberOfSegments-1; ++i) 
	{
		merge(segmentSize*(i+1), segmentSize, segments[0], segments[i+1], temp); //merge into temp
		memcpy(data, temp, sizeof(int)*segmentSize*(i+2)); //copy temp back to data
	}

	//Merge the rest
	merge(maxNoRest, rest, data, (int*)&data[maxNoRest], temp);
	memcpy(data, temp, sizeof(int)*dataSize);

	//Cleanup heap
	delete[] segments;
	delete[] temp;
}

//Original bubblesort algorithm
//Could be inlined into bubblesortsplitv1 and v2 but decreases readability
void SortOut::bubblesort(int * data, const int dataSize) {
	for(int n= dataSize-1; n >= 0; --n) {
        for(int j = 0; j<n; ++j) {
            if(data[j] > data[j+1]) {
				//swap
                int temp = data[j];   
                data[j] = data[j+1];   
                data[j+1] = temp;   
            }
        }
	}
}

//Merge two sorted arrays into destination array
//Source: http://www.algolist.net/Algorithms/Merge/Sorted_arrays
//Modified and optimized for readibility and speed
void SortOut::merge(int SizeA, int SizeB, int* A, int* B, int* Destination) {
	int aIndex(0), bIndex(0), dIndex(0);

	while (aIndex < SizeA && bIndex < SizeB) {
		if (A[aIndex] <= B[bIndex]) {
			Destination[dIndex] = A[aIndex];
			++aIndex;
		} else {
			Destination[dIndex] = B[bIndex];
			++bIndex;
		}
		++dIndex;
	}

	if (aIndex < SizeA)
		memcpy((int*)&Destination[dIndex], (int*)&A[aIndex], sizeof(int)*(SizeA-aIndex));
	else
		memcpy((int*)&Destination[dIndex], (int*)&B[bIndex], sizeof(int)*(SizeB-bIndex));
}

