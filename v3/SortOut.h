#pragma once

//Includes
#include "hr_timer.h"

//Precompiler Constants
#define MAX_ROWS 2000
#define MAX_COLS 1000

#define MAX_ELEMENTS MAX_ROWS * MAX_COLS


#define SEGMENTS_DATA 25
#define SEGMENTS_ALLDATA 2048 * 32

class SortOut {

	public:
		//Constructors & Destructor
		SortOut();
		~SortOut();

	private:
		//Methods
		void getData(void);
		void getAllData();
		void sortData(void);
		void sortAllData();
		void testData(void);
		void outputData(void);
		void outputData2();
		void outputAllData();
		void outputTimes(void);
		void merge(int, int, int*, int*, int*);
		void bubblesortSplit(int*, const int, const int);
		void bubblesortSplitV2(int*, const int, const int);
		void bubblesort(int*, int);
		void calcMovingAve(); 
		void outputAveRows();
		void outputFile(const char* filename, const char* content);
		//Members
		int data[MAX_ROWS][MAX_COLS];
		int allData[MAX_ELEMENTS];
		int avg[MAX_COLS/100][MAX_ROWS];
		CStopWatch swGetData, swGetAllData, swSortData, swSortAllData, swOutputData, swOutputAllData, swCalcMovingAve, swOutputMovingAvg;
};