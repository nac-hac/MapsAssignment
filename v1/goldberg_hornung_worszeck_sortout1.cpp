/********************************************************

	SortOut - integer sorting and file output program

*********************************************************/

#include <fstream>			//for file output
#include <iostream>			//for console output
#include <conio.h>			//for kbhit

#include "hr_time.h"

using namespace std;

#define MAX_ROWS	2000
#define MAX_COLS	1000
#define MAX_CHARS	33
#define SECTIONS	100

int data[MAX_ROWS][MAX_COLS];
int* singleArray = new int[1024*2048];
int avg[10][2000];

void getData();  //gets data for the 2-d array
void getAllData(); //gets data for 1-d array
void sortData(); //sorts 2-d array
void sortSingleArray();  //sorts 1-d array
void testData();   
void outputData();  //outputs 2-d array to a file
void outputData2(); //outputs 1-d array to a file
void outputTimes(); //displays times in console
void calcMovingAve();  //works out the average value in each 100 section of rows
void outputAveRows(); //puts the moving averages into a file

CStopWatch
	sw_getData,
	sw_getAllData,
	sw_sortRows,
	sw_outputSortedRows,
	sw_sortSingleArray,
	sw_outputSortedSingle,
	sw_calcMovingAve,
	sw_outputMovingAvg;

int main()
{
	sw_getData.startTimer();
	getData();				//Function gets data for int data[MAX_ROWS][MAX_COLS]
	sw_getData.stopTimer();

	sw_getAllData.startTimer();
	getAllData();			//Function gets data for int* singleArray = new int[MAX_COLS*MAX_ROWS]
	sw_getAllData.stopTimer();

	cout << "\n\nDone\n";
	
	sw_sortRows.startTimer();
	sortData();
	sw_sortRows.stopTimer();
	
	cout << "\n\nSorted two dimensional array...\n";

	sw_sortSingleArray.startTimer();
	sortSingleArray();
	sw_sortSingleArray.stopTimer();

	cout << "\n\nSorted single array...\n";

	testData();

	sw_outputSortedRows.startTimer();
	outputData();
	sw_outputSortedRows.stopTimer();

	sw_outputSortedSingle.startTimer();
	outputData2();
	sw_outputSortedSingle.stopTimer();

	sw_calcMovingAve.startTimer();
	calcMovingAve();
	sw_calcMovingAve.stopTimer();

	sw_outputMovingAvg.startTimer();
	outputAveRows();
	sw_outputMovingAvg.stopTimer();

	outputTimes(); 

	cout << "\n\n*** finished ***";	

	delete[] singleArray;

	cout << "\n\n\nPress any key to exit...";
	while (! _kbhit());  //to hold console
}

void getData()
{
	cout << "\n*** getting data ***";

	srand(123);
		
	for(int i = 0; i < MAX_ROWS; i++)
	{
		for(int j = 0; j < MAX_COLS; j++)
		{
			data[i][j] = rand();
		}
	}
}

void getAllData()
{
	cout << "\n\n***Getting single array data***";

	srand(123);

	for(int i(0) ; i < MAX_ROWS*MAX_COLS; ++i)
	{
		singleArray[i] = rand();
	}
}

void sortData()
{
	void bubble(int *a, int n);
	void merge(int m, int n, int *A, int* B, int* C);

	printf("\n\n*** sorting data ***");

	#pragma omp parallel for schedule(static, 250)

		for(int i = 0; i < MAX_ROWS; i++)
		{
			bubble(data[i], MAX_COLS);						
		}
}

void sortSingleArray()
{
	void bubblesortSplit(int* a, const int max, const int numberOfThreads);
	printf("\n\n*** sorting data ***");
	
	bubblesortSplit(singleArray, 1024*2048, 1024*128);
	
	printf("\n\nDone.");	
}

void bubblesortSplit(int* a, const int max, const int numberOfThreads) 
{
	void bubble(int *a, int n);
	void merge(int m, int n, int *A, int* B, int* C);
	//Calculate this only once
	int segmentSize = max/numberOfThreads;
	
	//An array of an amount of "numberOfThreads" int pointers
	int** segments = new int*[numberOfThreads];	

	//Let each pointer point to a factorized offset of "segmentsize" of the inputarray
	for (int i = 0; i < numberOfThreads; ++i) 
	{
		segments[i] = &a[i*segmentSize];
	}

		//Sort each segment
	#pragma parallel for

		for (int i = 0; i < numberOfThreads; ++i) 
		{			
				bubble(segments[i], segmentSize);
		}	

	int segmentCounter(numberOfThreads);	
	
	do
	{	
		// Better solution, because parallelizable:
		// 0 1 2 3 -> 0-1 2-3 -> 0-1-2-3
				
		//#pragma omp parallel for - MAKES SLOWER WTF
			for (int i = 0; i < segmentCounter/2; ++i) 
			{
				int *temp = new int[2*segmentSize];	//If the merge isn't parallelised then the array can be taken outside the loop
				merge(segmentSize, segmentSize, segments[i*2], segments[i*2 + 1], temp);
				
				for(int j = 0; j < segmentSize*2; j++) 
				{
					a[j + segmentSize*i*2] = temp[j];
				}	

				delete[] temp;	
			}

			segmentSize*=2;		
			segmentCounter /= 2;

			for(int i = 1; i < segmentCounter; ++i)
			{
				segments[i] = segments[i*2]; 
			}
				
		}
		while(segmentCounter != 1);			
}

void bubble(int * a, int max)
{
	for(int n=max-1; n>=0; n--)
	{   
        for(int j=0; j<n; j++)
		{   
            if(a[j] > a[j+1])
			{  
				//swap
                int temp = a[j];   
                a[j] = a[j+1];   
                a[j+1] = temp;   
            }  
        }  
	}
}

void merge(int m, int n, int *A, int* B, int* C)
{
	int i(0), j(0), k(0);

	while (i < m && j < n)
	{
		if (A[i] <= B[j])
		{
			C[k] = A[i];
			i++;
		}

		else 
		{
			C[k] = B[j];
			j++;
		}
	k++;
	}

	if (i < m)
	{
		for (int p = i; p < m; p++)
		{
			C[k] = A[p];
			k++;
		}
	}

	else 
	{
		for (int p = j; p < n; p++)
		{
			C[k] = B[p];
			k++;
		}
	}
}

void testData()
{
	cout << "\n\ndata[0][0]                   = " << data[0][0];					//=87 for srand(123)
	cout << "\ndata[MAX_ROWS/2][MAX_COLS/2] = " << data[MAX_ROWS/2][MAX_COLS/2];	//=16440 for srand(123)
	cout << "\ndata[MAX_ROWS-1][MAX_COLS-1] = " << data[MAX_ROWS-1][MAX_COLS-1];	//=32760 for srand(123)
}

void outputData()
{
	printf("\n\n*** outputting data to sodata.txt... ***");
	string output;

	for(int i=0; i<MAX_ROWS; i++) 
	{
		for(int j=0; j<MAX_COLS; j++) 
		{
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
	printf("\n\nDone.");
}

void outputData2()
{
	printf("\n\n*** outputting data to soAllData.txt... ***");
	string output;
		
	for(int j=1; j<=1024*2048; j++) 
	{
		char buffer [33];
		_itoa_s(singleArray[j-1], buffer, 10);
		output += buffer;
		output += "\t";		

		if(j % 1024 == 0)
		{
			output += "\n";
		}
	}

	output += "\n";
	
	FILE * pFile;
	fopen_s(&pFile, "soAllData.txt", "w");
	fputs(output.c_str(), pFile);
	fclose (pFile);
	printf("\n\nDone.");
}


void outputTimes() {
	printf("\n\n*** outputting two dim array times ***");
	cout << "\n\nGet: " << sw_getData.getElapsedTime() << " seconds\n";
	cout << "Sort: " << sw_sortRows.getElapsedTime()<< " seconds\n";
	cout << "Output: " << sw_outputSortedRows.getElapsedTime() << " seconds\n";
	cout << "Sort + Output: " << sw_sortRows.getElapsedTime()+sw_outputSortedRows.getElapsedTime() << " seconds\n\n";
	
	puts("\n\n*** outputting times for one dim array ***");
	cout << "\nGet: " << sw_getAllData.getElapsedTime() << " seconds\n";
	cout << "\nSort: " << sw_sortSingleArray.getElapsedTime() << " seconds\n";
	cout << "\nOutput: " << sw_outputSortedSingle.getElapsedTime() << " seconds\n";
	cout << "\nSort + Output: " << sw_sortSingleArray.getElapsedTime() + sw_outputSortedSingle.getElapsedTime() << " seconds\n";

	cout << "\n\nMoving Averages: " << sw_calcMovingAve.getElapsedTime();
	cout << "\nOutput: " << sw_outputMovingAvg.getElapsedTime();
}

void calcMovingAve()
{
	
	int sum;
	/*
	#pragma omp parallel for
		for(int j(0); j < MAX_ROWS; ++j)
		{	
		#pragma omp parallel for
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
*/
	for(int j(2); j < 3; ++j)
	{
		for(int i(0); i < 10; ++i)
		{
			cout << avg[i][j] << "\n";
		}
	}
}

void outputAveRows()
{
	printf("\n\n*** outputting data to movingAvg.txt... ***");
	string output;

	for(int i=0; i < MAX_ROWS; i++) 
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
	printf("\n\nDone.");
}
