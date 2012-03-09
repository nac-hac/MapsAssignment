#pragma once

//Includes
#include "hr_timer.h"

//Precompiler Constants
#define MAX_ROWS 2000
#define MAX_COLS 1000

#define MAX_ELEMENTS MAX_ROWS * MAX_COLS


#define SEGMENTS_DATA 25
#define SEGMENTS_ALLDATA MAX_COLS

class SortOut {

	public:
		//Constructors & Destructor
		SortOut();
		~SortOut();

	private:
		// methods to be timed
		void sortRows();
		void outputSortedRows();
		void sortAll();
		void outputSortedAll();
		void calcMovingAve(); 
		void outputAveRows();

		// help methods
		void getData();
		void getAllData();
		void testData();
		void outputTimes();
		void outputFile(const char* filename, const char* content);
		void merge(int, int, int*, int*, int*);
		void bubblesortSplit(int*, const int, const int);
		void bubblesortSplitV2(int*, const int, const int);
		void bubblesort(int*, int);
		
		//Members
		int data[MAX_ROWS][MAX_COLS];
		int allData[MAX_ELEMENTS];
		int ave[MAX_COLS/100][MAX_ROWS];
		CStopWatch
			swSortRows,
			swOutputSortedRows,
			swSortAll,
			swOutputSortedAll,
			swCalcMovingAve,
			swOutputAveRows;
};