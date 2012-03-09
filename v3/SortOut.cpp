/********************************************************

	SortOut - integer sorting and file output program

*********************************************************/

#include "SortOut.h"

#include <fstream>			//for file output
#include <iostream>			//for console output
#include <conio.h>			//for kbhit
#include <omp.h>

int main() {
	SortOut* sortOut = new SortOut();
	delete sortOut;
}

SortOut::SortOut() {	
	getData();	
	getAllData();

	sortRows();
	sortAll();

	testData();

	outputSortedRows();
	outputSortedAll();

	calcMovingAve();
	outputAveRows();

	outputTimes();

	while (! _kbhit());  //to hold console	
}

SortOut::~SortOut() {

}

// methods to be timed

// *********** sortRows **********

// for

void SortOut::sortRows()
{
	printf("\n\n*** sorting data ***");
	swSortRows.startTimer();

	#pragma omp parallel for
	for (int i = 0; i < MAX_ROWS; ++i) 
	{
		//bubblesort(data[i], MAX_COLS);
		bubblesortSplitV2(data[i], MAX_COLS, SEGMENTS_DATA);
	}

	swSortRows.stopTimer();
	printf("\n\nDone.");
}

//section
/*
void SortOut::sortRows()
{
	printf("\n\n*** sorting data ***");
	swSortRows.startTimer();

	for (int i = 0; i < MAX_ROWS; ++i) 
	{
		bubblesortSplitV2(data[i], MAX_COLS, SEGMENTS_DATA);
	}

	swSortRows.stopTimer();
	printf("\n\nDone.");
}
*/


// *********** outputSortedRows **********
/*
// for
void SortOut::outputSortedRows() {
	printf("\n\n*** outputting data to soDataRows.txt... ***");
	swOutputSortedRows.startTimer();

	std::string output[MAX_ROWS];
	#pragma omp parallel for
	for(int i=0; i<MAX_ROWS; i++) {
		for(int j=0; j<MAX_COLS; j++) {
			char buffer [33];
			_itoa_s(data[i][j], buffer, 10);
			output[i] += buffer;
			output[i] += "\t";
		}
		output[i] += "\n";
	}

	for (int i = 1; i < MAX_ROWS; ++i) {
		output[0] += output[i];
	}

	outputFile("soDataRows.txt",  output[0].c_str());

	swOutputSortedRows.stopTimer();
	printf("\n\nDone.");
}
*/

//section
void SortOut::outputSortedRows()
{
	printf("\n\n*** outputting data to soDataRows.txt... ***");
	swOutputSortedRows.startTimer();
	
	std::string outputSection[MAX_ROWS/4], outputSection2[MAX_ROWS/4], outputSection3[MAX_ROWS/4], outputSection4[MAX_ROWS/4], output[MAX_ROWS];

	#pragma omp parallel sections
	{
		#pragma omp section
		{
			for(int i = 0; i < MAX_ROWS/4; ++i)
			{
				for(int j = 0; j < MAX_COLS; ++j)
				{
					char buffer[33];
					_itoa_s(data[i][j], buffer, 10);
					outputSection[i] += buffer;
					outputSection[i] += "\t";
				}

				outputSection[i] += "\n";
			}
		}

		#pragma omp section	
		{
			for(int i = MAX_ROWS/4; i < MAX_ROWS/2 ; ++i)
			{
				for(int j = 0; j < MAX_COLS; ++j)
				{
					char buffer[33];
					_itoa_s(data[i][j], buffer, 10);
					outputSection2[i - MAX_ROWS/4] += buffer;
					outputSection2[i - MAX_ROWS/4] += "\t";
				}
				outputSection2[i - MAX_ROWS/4]+= "\n";
			}
		}

		#pragma omp section	
		{
			for(int i = MAX_ROWS/2; i < 3*MAX_ROWS/4 ; ++i)
			{
				for(int j = 0; j < MAX_COLS; ++j)
				{
					char buffer[33];
					_itoa_s(data[i][j], buffer, 10);
					outputSection3[i - MAX_ROWS/2] += buffer;
					outputSection3[i - MAX_ROWS/2] += "\t";
				}
				outputSection3[i - MAX_ROWS/2]+= "\n";
			}
		}

		#pragma omp section	
		{
			for(int i = 3*MAX_ROWS/4; i < MAX_ROWS ; ++i)
			{
				for(int j = 0; j < MAX_COLS; ++j)
				{
					char buffer[33];
					_itoa_s(data[i][j], buffer, 10);
					outputSection4[i - 3*MAX_ROWS/4] += buffer;
					outputSection4[i - 3*MAX_ROWS/4] += "\t";
				}
				outputSection4[i - 3*MAX_ROWS/4]+= "\n";
			}
		}
	}

	for (int i = 0; i < MAX_ROWS/4; ++i) 
	{
		output[0] += outputSection[i];	
	}
	
	for (int i = MAX_ROWS/4; i < MAX_ROWS/2; ++i) 
	{
		output[0] += outputSection2[i];	
	}
	
	for (int i = MAX_ROWS/2; i < 3*MAX_ROWS/4; ++i) 
	{
		output[0] += outputSection3[i];	
	}
	
	for (int i = 3*MAX_ROWS/4; i < MAX_ROWS; ++i) 
	{
		output[0] += outputSection4[i];	
	}
	
	outputFile("soDataRows.txt",  output[0].c_str());

	swOutputSortedRows.stopTimer();
	printf("\n\nDone.");

}


// *********** sortAll **********

// for
void SortOut::sortAll()
{
	// TO DO
}

/*
// section
void SortOut::sortAll()
{
	printf("\n\n*** sorting data ***");
	swSortAll.startTimer();

	bubblesortSplit(allData, MAX_COLS*MAX_ROWS, SEGMENTS_ALLDATA);

	swSortAll.stopTimer();
	printf("\n\nDone.");
}
*/

// *********** outputSortedAll **********

// for
void SortOut::outputSortedAll()
{
	// TO DO
	printf("\n\n*** outputting data to soDataAll.txt... ***");
	swOutputSortedAll.startTimer();

	std::string output;
	for(int j=1; j<=MAX_COLS*MAX_ROWS; j++)
	{
		char buffer [33];
		_itoa_s(allData[j], buffer, 10);
		output += buffer;
		output += "\t";

		if(j % 1000 == 0)
		{
			output += "\n";
		}
	}
			
	outputFile("soDataAll.txt", output.c_str());

	swOutputSortedAll.stopTimer();
	printf("\n\nDone.");
}

/*
// section
void SortOut::outputSortedAll()
{
	TO DO
}
*/

// *********** calcMovingAve **********

// for
void SortOut::calcMovingAve()
{
	swCalcMovingAve.startTimer();
	int sum[MAX_ROWS];

	#pragma omp parallel for
		for(int j = 0; j < MAX_ROWS; ++j)
		{
			for(int k = 0; k < 10; ++k)
			{
				sum[j] = 0;
				
				for(int i = 0; i < 100; ++i)
				{
					sum[j] += data[j][i + 100*k];
				}
				ave[k][j] = sum[j]/100;
			}
		}
	swCalcMovingAve.stopTimer();
}

/*
// section
void SortOut::calcMovingAve()
{
	int sum[MAX_ROWS];

	for(int j = 0; j < MAX_ROWS; ++j)
	{
		sum[j] = 0;
		
		#pragma omp parallel sections
		{
			// first 100 values in one row
			#pragma omp section
			{
				for(int i = 0; i < 100; ++i)
				{
					sum[j] += data[j][i];
				}
				ave[0][j] = sum[j]/100;
			}
			
			// second 100 values in one row
			#pragma omp section
			{
				sum[j] = 0;
				for(int i = 100; i < 200; ++i)
				{
					sum[j] += data[j][i];
				}
				ave[1][j] = sum[j]/100;
			}

			// third 100 values in one row
			#pragma omp section
			{
				sum[j] = 0;
				for(int i = 200; i < 300; ++i)
				{
					sum[j] += data[j][i];
				}
				ave[2][j] = sum[j]/100;
			}

			// fourth 100 values in one row
			#pragma omp section
			{
				sum[j] = 0;
				for(int i = 300; i < 400; ++i)
				{
					sum[j] += data[j][i];
				}
				ave[3][j] = sum[j]/100;
			}

			// fifth 100 values in one row
			#pragma omp section
			{
				sum[j] = 0;
				for(int i = 400; i < 500; ++i)
				{
					sum[j] += data[j][i];
				}
				ave[4][j] = sum[j]/100;
			}

			// sixth 100 values in one row
			#pragma omp section
			{
				for(int i = 500; i < 600; ++i)
				{
					sum[j] += data[j][i];
				}
				ave[5][j] = sum[j]/100;
			}
			
			// seventh 100 values in one row
			#pragma omp section
			{
				sum[j] = 0;
				for(int i = 600; i < 700; ++i)
				{
					sum[j] += data[j][i];
				}
				ave[6][j] = sum[j]/100;
			}

			// eigth 100 values in one row
			#pragma omp section
			{
				sum[j] = 0;
				for(int i = 700; i < 800; ++i)
				{
					sum[j] += data[j][i];
				}
				ave[7][j] = sum[j]/100;
			}

			// ninth 100 values in one row
			#pragma omp section
			{
				sum[j] = 0;
				for(int i = 800; i < 900; ++i)
				{
					sum[j] += data[j][i];
				}
				ave[8][j] = sum[j]/100;
			}

			// tenth 100 values in one row
			#pragma omp section
			{
				sum[j] = 0;
				for(int i = 900; i < 1000; ++i)
				{
					sum[j] += data[j][i];
				}
				ave[9][j] = sum[j]/100;
			}
		}
	}
}
*/


// *********** outputAveRows **********
// for
void SortOut::outputAveRows()
{
	printf("\n\n*** outputting data to soDataAve.txt... ***");
	swOutputAveRows.startTimer();
	std::string output[MAX_ROWS];

	#pragma omp parallel for
		for(int i=0; i < MAX_ROWS; i++) 
		{
			for(int j = 0; j < 10; j++) 
			{
				char buffer [33];
				_itoa_s(ave[j][i], buffer, 10);
				output[i] += buffer;
				output[i] += "\t";
			}
			output[i] += "\n";
		}

		for (int i = 1; i < MAX_ROWS; ++i)
		{
			output[0] += output[i];
		}

	outputFile("soDataAve.txt", output[0].c_str());
	swOutputAveRows.stopTimer();
	printf("\n\nDone.");
}

/*
// sections
void SortOut::outputAveRows()
{
	TO DO
}
*/



// ********************
// help methods
// ********************


void SortOut::getData() {

	printf("*** getting data ***");

	srand(123); //arbitrary random number seed

	for(int i=0; i<MAX_ROWS; i++) 
	{
		for(int j=0; j<MAX_COLS; j++) 
		{
			data[i][j] = rand(); //RAND_MAX = 32767
		}
	}
	printf("\n\nDone.");
}

void SortOut::getAllData()
{
	srand(123);

	#pragma omp parallel for
	for(int i = 0; i < MAX_COLS*MAX_ROWS; ++i)
	{
		allData[i] = rand();
	}
}

void SortOut::testData() {
	printf("\n\n*** testing data ***");
	if (data[0][0] == 87 &&
		data[MAX_ROWS/2][MAX_COLS/2] == 16440 &&
		data[MAX_ROWS-1][MAX_COLS-1] == 32760)
		printf("\n\nTEST SUCCEEDED");
	else
		printf("\n\nTEST FAILED!");
}

void SortOut::outputTimes()
{
	printf("\n\n*** outputting times ***");
	std::cout << "\n\nsortRows:\t\t" << swSortRows.getElapsedTime() << " seconds\n";
	std::cout << "outputSortedRows:\t" << swOutputSortedRows.getElapsedTime() << " seconds\n";
	std::cout << "sortAll:\t\t" << swSortAll.getElapsedTime() << " seconds\n";
	std::cout << "outputSortedAll:\t" << swOutputSortedAll.getElapsedTime() << " seconds\n";
	std::cout << "calcMovingAve:\t\t" << swCalcMovingAve.getElapsedTime() << " seconds\n";
	std::cout << "outputAveRows:\t\t" << swOutputAveRows.getElapsedTime() << " seconds\n";
}

void SortOut::outputFile(const char* filename, const char* content) {
	FILE * pFile;
	fopen_s(&pFile, filename, "w");
	fputs(content, pFile);
	fclose (pFile);
}

void SortOut::bubblesortSplit(int* a, const int max, const int numberOfSegments) 
{
	//Calculate this only once
	int rest = max % numberOfSegments;
	int segmentSize = max/numberOfSegments;
	
	//An array of an amount of "numberOfThreads" int pointers
	int** segments = new int*[numberOfSegments];	

	//Let each pointer point to a factorized offset of "segmentsize" of the inputarray
	for (int i = 0; i < numberOfSegments; ++i) 
	{
		segments[i] = &a[i*segmentSize];
	}

	//Sort each segment
	#pragma omp parallel for
	for (int i = 0; i < numberOfSegments; ++i) 
	{			
		bubblesort(segments[i], segmentSize);
	}	

	//Sort rest
	bubblesort(&a[max-rest], rest);
	
	//Create tail array and insert rest
	int* tail = new int[rest];
	for (int i = 0; i < rest; ++i) {
		tail[i] = a[max-rest+i];
	}
	int tailSize = rest;

	//This loop merges an array of segments in the following parallelisable way 
	// 0 1 2 3 -> 0-1 2-3 -> 0-1-2-3
	for (int segmentCounter = numberOfSegments; segmentCounter > 1; segmentCounter/=2) {
		//If segment amount is not an even number, then put last segment into tail
		if (segmentCounter%2 != 0) {
			int *temp = new int[segmentSize+tailSize];
			merge(tailSize, segmentSize, tail, segments[segmentCounter-1], temp);
			delete[] tail;
			tail = &(*temp);
			tailSize += segmentSize;
			--segmentCounter;
		}

		//After each loop pointer offset is doubled to point to the first member of the new larger arrays
		#pragma omp parallel for
		for (int i = 0; i < segmentCounter/2; ++i) 
		{
			int segmentOffset = 2*i;  //calculates offset only once

			int *temp = new int[2*segmentSize];	//creates temp array to store merge - If the merge isn't parallelised then the array can be taken outside the loop

			merge(segmentSize, segmentSize, segments[segmentOffset], segments[segmentOffset + 1], temp); //merge two arrays, store in temp			

			for(int j = 0; j < segmentSize*2; j++) 
			{
				a[j + segmentSize*segmentOffset] = temp[j];  //stores temp into a at offsetted position
			}	

			delete[] temp;	
		}

		segmentSize*=2;	//double segment size as segements have doubled in size

		//offsets pointer addresses 
		for(int i = 1; i < segmentCounter/2; ++i)
		{
			segments[i] = segments[i*2]; 
		}
	}

	int* temp = new int[max];
	merge(segmentSize, tailSize, a, tail, temp);

	for (int i = 0; i < max; ++i) {
		a[i] = temp[i];
	}

	delete[] tail;
	delete[] segments;
}

void SortOut::bubblesortSplitV2(int* a, const int max, const int numberOfSegments)
{
	//Calculate this only once
	int segmentSize = max/numberOfSegments;
	
	//An array of an amount of "numberOfThreads" int pointers
	int** segments = new int*[numberOfSegments];	

	//Let each pointer point to a factorized offset of "segmentsize" of the inputarray
	for (int i = 0; i < numberOfSegments; ++i) 
	{
		segments[i] = &a[i*segmentSize];
	}

	//Sort each segment
	//#pragma omp parallel for
	for (int i = 0; i < numberOfSegments; ++i) 
	{			
		bubblesort(segments[i], segmentSize);
	}	

	//Sort tail
	int rest = max % numberOfSegments;
	int maxNoRest = max-rest; //caculate only once 
	bubblesort(&a[maxNoRest], rest);

	//Function which merges arrays of unequal size
	//Merge the segments two at a time:
	//Example with 4 segments: 0 1 2 3 -> 0-1 2 3 -> 0-1-2 3 -> 0-1-2-3

	int temp[MAX_COLS];

	for (int i = 0; i < numberOfSegments-1; ++i) 
	{
		merge(segmentSize*(i+1), segmentSize, segments[0], segments[i+1], temp);
		memcpy(a, temp, sizeof(int)*segmentSize*(i+2));
	}

	//Merge the rest
	merge(maxNoRest, rest, a, (int*)&a[maxNoRest], temp);
	memcpy(a, temp, sizeof(int)*max);

	//Cleanup heap
	delete[] segments;
}

void SortOut::bubblesort(int * a, int max) {
	for(int n= max-1; n >= 0; --n) {
        for(int j = 0; j<n; ++j) {
            if(a[j] > a[j+1]) {
				//swap
                int temp = a[j];   
                a[j] = a[j+1];   
                a[j+1] = temp;   
            }
        }
	}
}

//Source: http://www.algolist.net/Algorithms/Merge/Sorted_arrays
//Optimized and modified for readability
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

	if (aIndex < SizeA) {
		for (aIndex; aIndex < SizeA; aIndex++) {
			Destination[dIndex] = A[aIndex];
			++dIndex;
		}
	} else {
		for (bIndex; bIndex < SizeB; bIndex++) {
			Destination[dIndex] = B[bIndex];
			++dIndex;
		}
	}
}

