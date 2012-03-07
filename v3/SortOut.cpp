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

	sortData();
	sortAllData();

	testData();

	outputData();	
	outputAllData();

	calcMovingAve();

	outputAveRows();

	outputTimes();

	while (! _kbhit());  //to hold console	
}

SortOut::~SortOut() {

}

void SortOut::getData() {
	printf("*** getting data ***");
	swGetData.startTimer();

	srand(123); //arbitrary random number seed
	//#pragma omp parallel for (tests will fail)
	for(int i=0; i<MAX_ROWS; i++) 
	{
		for(int j=0; j<MAX_COLS; j++) 
		{
			data[i][j] = rand(); //RAND_MAX = 32767
		}
	}

	swGetData.stopTimer();
	printf("\n\nDone.");
}

void SortOut::getAllData()
{
	swGetAllData.startTimer();
	srand(123);

	#pragma omp parallel for
	for(int i = 0; i < MAX_COLS*MAX_ROWS; ++i)
	{
		allData[i] = rand();
	}

	swGetAllData.stopTimer();
}

void SortOut::sortData()
{
	printf("\n\n*** sorting data ***");
	swSortData.startTimer();

	for (int i = 0; i < MAX_ROWS; ++i) 
	{
		bubblesortSplit(data[i], MAX_COLS, SEGMENTS_DATA);
	}

	swSortData.stopTimer();
	printf("\n\nDone.");
}

void SortOut::sortAllData()
{
	printf("\n\n*** sorting data ***");
	swSortAllData.startTimer();

	bubblesortSplit(allData, MAX_COLS*MAX_ROWS, SEGMENTS_ALLDATA);

	swSortAllData.stopTimer();
	printf("\n\nDone.");
}

void SortOut::bubblesortSplit(int* a, const int max, const int numberOfSegments) 
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
	//#pragma parallel for - no effect
		for (int i = 0; i < numberOfSegments; ++i) 
		{			
				bubblesort(segments[i], segmentSize);
		}	

	int segmentCounter(numberOfSegments);	
	
	//This loop merges an array of segments in the following parallelisable way 
	// 0 1 2 3 -> 0-1 2-3 -> 0-1-2-3
	while(segmentCounter != 1) {	
		
		//After each loop pointer offset is doubled to point to the first member of the new larger arrays
		//#pragma omp parallel for //- MAKES SLOWER 
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
		segmentCounter /= 2;

		//offsets pointer addresses 
		for(int i = 1; i < segmentCounter; ++i)
		{
			segments[i] = segments[i*2]; 
		}		
	}

	//Function which merges arrays of unequal size

	/*	//Merge the segments two at a time:
	//Example with 4 segments: 0 1 2 3 -> 0-1 2 3 -> 0-1-2 3 -> 0-1-2-3

	int temp[MAX_COLS];

	for (int i = 0; i < numberOfThreads-1; ++i) 
	{
		merge(segmentSize*(i+1), segmentSize, segments[0], segments[i+1], temp);

		for(int j = 0; j < segmentSize*(i+2); j++) 
		{
			a[j] = temp[j];
		}
	}
	*/
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

//Source: ?
void SortOut::merge(int m, int n, int* A, int* B, int* C) {
      int i(0), j(0), k(0);

      while (i < m && j < n) {
            if (A[i] <= B[j]) {
                  C[k] = A[i];
                  i++;
            } else {
				C[k] = B[j];
				j++;
            }
            k++;
      }

      if (i < m) {
		  for (int p = i; p < m; p++) {
			  C[k] = A[p];
			  k++;
		  }
	  } else {
		  for (int p = j; p < n; p++) {
			  C[k] = B[p];
			  k++;
		  }
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


void SortOut::outputData() {
	printf("\n\n*** outputting data to sodata.txt... ***");
	swOutputData.startTimer();

	std::string output;
	for(int i=0; i<MAX_ROWS; i++) {
		for(int j=0; j<MAX_COLS; j++) {
			char buffer [33];
			_itoa_s(data[i][j], buffer, 10);
			output += buffer;
			output += "\t";
		}
		output += "\n";
	}
	FILE * pFile;
	fopen_s(&pFile, "sodata.txt", "w");
	fputs(output.c_str(), pFile);
	fclose (pFile);

	swOutputData.stopTimer();
	printf("\n\nDone.");
}

void SortOut::outputAllData()
{
	printf("\n\n*** outputting data to soAllData.txt... ***");
	swOutputAllData.startTimer();

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
			
	FILE * pFile;
	fopen_s(&pFile, "soAllData.txt", "w");
	fputs(output.c_str(), pFile);
	fclose (pFile);

	swOutputAllData.stopTimer();
	printf("\n\nDone.");
}

void SortOut::calcMovingAve()
{	
	std::cout << "\nSorting averages or something\n";

	int sum;

	swCalcMovingAve.startTimer();	

	for(int j(0); j < MAX_ROWS2; ++j)
	{		
		for(int k(0); k < 10; ++k)
		{
			sum = 0;

			for(int i(0); i < 100; ++i)
			{
				sum+=data[j][i + 100*k];
			}
			avg[k][j] = sum/100;		
		}
	}

	swCalcMovingAve.stopTimer();

	std::cout << "\ndone\n";
}

void SortOut::outputAveRows()
{
	
	printf("\n\n*** outputting data to movingAvg.txt... ***");
	std::string output;

	swOutputMovingAvg.startTimer();

	for(int i=0; i < MAX_ROWS2; i++) 
	{
		for(int j=0; j< 10; j++) 
		{
			char buffer [33];
			_itoa_s(avg[j][i], buffer, 10);
			output += buffer;
			output += "\t";
		}
		output += "\n";
	}

	FILE * pFile;
	fopen_s(&pFile, "movingAvg.txt", "w");
	fputs(output.c_str(), pFile);
	fclose (pFile);

	swOutputMovingAvg.stopTimer();

	printf("\n\nDone.");
}

void SortOut::outputTimes()
{
	printf("\n\n*** outputting times ***");
	std::cout << "\n\nGet: " << swGetData.getElapsedTime() << " seconds\n";
	std::cout << "Sort data: " << swSortData.getElapsedTime() << " seconds\n";
	std::cout << "Output: " << swOutputData.getElapsedTime() << " seconds\n";
	std::cout << "Sort + Output: " << swSortData.getElapsedTime()+swOutputData.getElapsedTime() << " seconds\n\n";
	std::cout << "Sort all data: " << swSortAllData.getElapsedTime() << " seconds\n\n";
	std::cout << "\n\nMoving Averages: " << swCalcMovingAve.getElapsedTime();
	std::cout << "\nOutput: " << swOutputMovingAvg.getElapsedTime();
}