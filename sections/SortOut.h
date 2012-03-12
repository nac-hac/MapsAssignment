/********************************************************

	SortOut - header file - necessary for SortOut.cpp

	Goldberg, Peter
	Hornung, Nico
	Worszeck, Sascha

	MAPS - SHU - 12/03/2012

*********************************************************/

#pragma once

//Includes
#include "hr_timer.h"

//Precompiler Constants
#define MAX_ROWS 2000 //amount of rows in our data array
#define MAX_COLS 1000 //amount of columns in our data array
#define MAX_ELEMENTS MAX_ROWS * MAX_COLS //total amount of elements in our allData array

#define SEGMENTS_DATA 25 //amount of segments the data rows should be divided into
#define SEGMENTS_ALLDATA 1000 //amount of segments allData should be divided into

class SortOut {

	public:
		//Constructors & Destructor
		SortOut();
		~SortOut();

	private:
		// methods to be timed
		void sortRows(); //sort data array
		void output2DArray(int data[MAX_ROWS][MAX_COLS], const char* fileName, CStopWatch& sw); //output the sorted data/moving average array into file
		void sortAll(); //sort allData array
		void outputSortedAll(); //output allData array into file
		void calcMovingAve(); //calculate average for every 100 elements

		// helper methods
		void getData(); //feed data into data array
		void testData(); //test sorted rows result for correctness
		void outputTimes(); //output all timer data to console
		void outputToFile(const char* filename, const char* content); //handles file creation and output
		void merge(int SizeA, int SizeB, int* A, int* B, int* Destination); //merge two sorted arrays into destination array
		void bubblesortSplitV1(int* data, const int dataSize, const int numberOfSegments); //split array into segments for efficient bubblesorting with a parallelizable merge
		void bubblesortSplitV2(int* data, const int dataSize, const int numberOfSegments); //split array into segments for efficient bubblesorting 
		void bubblesort(int* data, const int dataSize); //original bubblesort algorithm
		
		//Members
		int data[MAX_ROWS][MAX_COLS];
		int allData[MAX_ELEMENTS];
		int ave[MAX_ROWS][MAX_COLS];
		CStopWatch	swSortRows,
					swOutputSortedRows,
					swSortAll,
					swOutputSortedAll,
					swCalcMovingAve,
					swOutputAveRows;
};