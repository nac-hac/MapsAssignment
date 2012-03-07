#pragma once

//Includes
#include "hr_timer.h"

//Precompiler Constants
#define MAX_ROWS 2048
#define MAX_COLS 1024
#define MAX_COLS2 1000
#define MAX_ROWS2 2000

#define MAX_ELEMENTS MAX_ROWS * MAX_COLS


#define SEGMENTS_DATA 4
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
		void outputAllData();
		void outputTimes(void);
		void merge(int, int, int*, int*, int*);
		void bubblesortSplit(int*, const int, const int);
		void bubblesort(int*, int);
		void calcMovingAve(); 
		void outputAveRows();

		//Members
		int data[MAX_ROWS][MAX_COLS];
		int allData[MAX_ELEMENTS];
		int avg[MAX_COLS2/100][MAX_ROWS2];
		CStopWatch swGetData, swGetAllData, swSortData, swSortAllData, swOutputData, swOutputAllData, swCalcMovingAve, swOutputMovingAvg;
};