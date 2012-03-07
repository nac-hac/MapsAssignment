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

int data[MAX_ROWS][MAX_COLS];
int* allData = new int[MAX_ROWS*MAX_COLS];

void sortRows(); // sort each row of data in ascending order
void outputSortedRows(); // output the sorted rows as soDataRows.txt
void sortAll(); // sort all data (2,000,000 numbers) in ascending order
void outputSortedAll(); // output the 2,000,000 sorted numbers as soDataAll.txt (2,000 rows of 1,000 numbers)
void calcMovingAve(); // calculate a 100 number moving integer average for each number in each sorted row
void outputAveRows(); // output the rows of moving averages as soDataAve.txt

void getData();   //gets randomised data
void getAllData();
void sortData();
void sortAllData();
void testData(); //ensures data is of correct size
//void outputData();
void outputTimes(); //displays sort times in console
string outputStrings2();
string outputStrings3();
void writeInFile(string odata); //outputs string data to file
void writeInFile2(string odata2); //outputs string data to file

CStopWatch 
	s_get,
	s_get2,
	s_sort,
	s_sort2,
	s_output,
	s_output2;

int main(void)
{
	string odata, odata2;	

	s_get.startTimer();
	getData();
	s_get.stopTimer();
	
	s_get2.startTimer();
	getAllData();
	s_get2.stopTimer();
	
	s_sort.startTimer();
	sortData();
	s_sort.stopTimer();

	s_sort2.startTimer();
	sortAllData();
	s_sort2.stopTimer();

	testData();

	//outputData();
	s_output.startTimer();
	odata = outputStrings2();
	s_output.stopTimer();

	s_output.startTimer();
	odata2 = outputStrings3();
	s_output.stopTimer();

	writeInFile(odata);
	writeInFile2(odata2);

	delete[] allData;

	cout << "\n\n*** finished ***";
	
	outputTimes();	

	cout << "\n\n\nPress any key to exit...";
	while (! _kbhit());  //to hold console
}

void getData()
{
	cout << "\n*** getting data ***";

	srand(123);
	
	#pragma omp parallel for
	
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
	srand(123);
	#pragma omp parallel for

		for(int i = 0; i < MAX_COLS*MAX_ROWS; ++i)
		{
			allData[i] = rand();
		}
}

void sortData()
{
	void bubblesortTwoSections(int *a, int max);
	void bubblesortFourSections(int *a, int max);
	void bubblesortEightSections(int *a, int max);
	void bubblesortSixteenSections(int *a, int max);
	void bubble(int *a, int n);
	void split_bubble(int *a, int max);
	void split_bubble_omp(int *a, int max);

	cout << "\n\n*** sorting data ***";
		
	#pragma omp parallel for

		for(int i = 0; i < MAX_ROWS; i++)
		{
			bubblesortSixteenSections(data[i], MAX_COLS);
		}
}

void sortAllData()
{
	void bubblesortSixteenSectionsOneArray(int * a, int max);

	bubblesortSixteenSectionsOneArray(allData, MAX_COLS*MAX_ROWS);
}
/*
void split_bubble(int *a, int max)
{
	//bubble sort first half of row
	for(int n=max-1; n>=max/2; n--)
	{
        for(int j=max/2; j<n; j++)
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

	//bubble sort second half of row
	for(int n=max/2 - 1; n>=0; n--)
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

	//merge the two sorted halves
	int s[MAX_COLS];
	int jlow = 0;
	int jhigh = MAX_COLS/2;
	int j = 0;
	while(jlow < MAX_COLS/2 && jhigh < MAX_COLS)
	{
		if(a[jlow] <= a[jhigh])
			s[j++] = a[jlow++];
		else
			s[j++] = a[jhigh++];
	}
	while(jlow < MAX_COLS/2) s[j++] = a[jlow++];
	while(jhigh < MAX_COLS) s[j++] = a[jhigh++];

	for(j = 0; j < MAX_COLS; j++) a[j] = s[j];
}*/

void bubblesortTwoSections(int * a, int max)
{
	void bubble(int * a, int max);
	void merge(int m, int n, int A[], int B[], int C[]);

	int a1[MAX_COLS/2];
	int a2[MAX_COLS/2];
	
	#pragma omp parallel sections
	{
		#pragma omp section
		{
			for (int i = 0; i < max/2; ++i)
			{
				a1[i] = a[i];
			}
		}

		#pragma omp section
		{
			for (int i = max/2; i < max; ++i)
			{
				a2[i - max/2] = a[i];
			}
		}
	}

	#pragma omp parallel sections
	{
		#pragma omp section
		{
			bubble(a1, max/2);
		}

		#pragma omp section
		{
			bubble(a2, max/2);
		}
	}
	merge(max/2, max/2, a1, a2, a);
}	

void bubblesortFourSections(int * a, int max)
{
	void bubble(int * a, int max);
	void merge(int m, int n, int A[], int B[], int C[]);

	int a1[MAX_COLS/4];
	int a2[MAX_COLS/4];
	int a3[MAX_COLS/4];
	int a4[MAX_COLS/4];

	int temp1[MAX_COLS/2];
	int temp2[MAX_COLS/2];


	#pragma omp parallel sections
	{
		#pragma omp section
		{
			for (int i = 0; i < max/4; ++i)
			{
				a1[i] = a[i];
			}
		}

		#pragma omp section
		{
			for (int i = max/4; i < max/2; ++i)
			{
				a2[i - max/4] = a[i];
			}
		}

		#pragma omp section
		{
			for (int i = max/2; i < 3*max/4; ++i)
			{
				a3[i - max/2] = a[i];
			}
		}

		#pragma omp section
		{
			for (int i = 3*max/4; i < max; ++i)
			{
				a4[i - 3*max/4] = a[i];
			}
		}
	}

	#pragma omp parallel sections
	{
		#pragma omp section
		{
			bubble(a1, max/4);
		}

		#pragma omp section
		{
			bubble(a2, max/4);
		}

		#pragma omp section
		{
			bubble(a3, max/4);
		}

		#pragma omp section
		{
			bubble(a4, max/4);
		}
	}

	#pragma omp parallel sections
	{
		#pragma omp section
		{
			merge(max/4, max/4, a1, a2, temp1);
		}

		#pragma omp section
		{
			merge(max/4, max/4, a3, a4, temp2);
		}
	}

	merge(max/2, max/2, temp1, temp2, a);
}	

void bubblesortEightSections(int * a, int max)
{
	void bubble(int * a, int max);
	void merge(int m, int n, int A[], int B[], int C[]);

	int a1[MAX_COLS/8];
	int a2[MAX_COLS/8];
	int a3[MAX_COLS/8];
	int a4[MAX_COLS/8];
	int a5[MAX_COLS/8];
	int a6[MAX_COLS/8];
	int a7[MAX_COLS/8];
	int a8[MAX_COLS/8];

	int temp1[MAX_COLS/2];
	int temp2[MAX_COLS/2];

	int temp1a[MAX_COLS/4];
	int temp2a[MAX_COLS/4];
	int temp3a[MAX_COLS/4];
	int temp4a[MAX_COLS/4];
	
	#pragma omp parallel sections
	{
		#pragma omp section
		{
			for (int i = 0; i < max/8; ++i)
			{
				a1[i] = a[i];
			}
		}

		#pragma omp section
		{
			for (int i = max/8; i < max/4; ++i)
			{
				a2[i - max/8] = a[i];
			}
		}

		#pragma omp section
		{
			for (int i = max/4; i < 3*max/8; ++i)
			{
				a3[i - max/4] = a[i];
			}
		}

		#pragma omp section
		{
			for (int i = 3*max/8; i < max/2; ++i)
			{
				a4[i - 3*max/8] = a[i];
			}
		}

		#pragma omp section
		{
			for (int i = max/2; i < 5*max/8; ++i)
			{
				a5[i - max/2] = a[i];
			}
		}

		#pragma omp section
		{
			for (int i = 5*max/8; i < 3*max/4; ++i)
			{
				a6[i - 5*max/8] = a[i];
			}
		}

		#pragma omp section
		{
			for (int i = 3*max/4; i < 7*max/8; ++i)
			{
				a7[i - 3*max/4] = a[i];
			}
		}

		#pragma omp section
		{
			for (int i = 7*max/8; i < max; ++i)
			{
				a8[i - 7*max/8] = a[i];
			}
		}
	}

	#pragma omp parallel sections
	{
		#pragma omp section
		{
			bubble(a1, max/8);
		}

		#pragma omp section
		{
			bubble(a2, max/8);
		}

		#pragma omp section
		{
			bubble(a3, max/8);
		}

		#pragma omp section
		{
			bubble(a4, max/8);
		}

		#pragma omp section
		{
			bubble(a5, max/8);
		}

		#pragma omp section
		{
			bubble(a6, max/8);
		}

		#pragma omp section
		{
			bubble(a7, max/8);
		}

		#pragma omp section
		{
			bubble(a8, max/8);
		}
	}

	#pragma omp parallel sections
	{
		#pragma omp section
		{
			merge(max/8, max/8, a1, a2, temp1a);
		}

		#pragma omp section
		{
			merge(max/8, max/8, a3, a4, temp2a);
		}

		#pragma omp section
		{
			merge(max/8, max/8, a5, a6, temp3a);
		}

		#pragma omp section
		{
			merge(max/8, max/8, a7, a8, temp4a);
		}
	}

	#pragma omp parallel sections
	{
		#pragma omp section
		{
			merge(max/4, max/4, temp1a, temp2a, temp1);
		}

		#pragma omp section
		{
			merge(max/4, max/4, temp3a, temp4a, temp2);
		}
	}

	merge(max/2, max/2, temp1, temp2, a);
}	

void bubblesortSixteenSections(int * a, int max)
{
	void bubble(int * a, int max);
	void merge(int m, int n, int A[], int B[], int C[]);

	int a1[MAX_COLS/16];
	int a2[MAX_COLS/16];
	int a3[MAX_COLS/16];
	int a4[MAX_COLS/16];
	int a5[MAX_COLS/16];
	int a6[MAX_COLS/16];
	int a7[MAX_COLS/16];
	int a8[MAX_COLS/16];
	int a9[MAX_COLS/16];
	int a10[MAX_COLS/16];
	int a11[MAX_COLS/16];
	int a12[MAX_COLS/16];
	int a13[MAX_COLS/16];
	int a14[MAX_COLS/16];
	int a15[MAX_COLS/16];
	int a16[MAX_COLS/16];

	int temp1[MAX_COLS/2];
	int temp2[MAX_COLS/2];

	int temp1a[MAX_COLS/4];
	int temp2a[MAX_COLS/4];
	int temp3a[MAX_COLS/4];
	int temp4a[MAX_COLS/4];

	int temp1aa[MAX_COLS/8];
	int temp1ab[MAX_COLS/8];
	int temp1ac[MAX_COLS/8];
	int temp1ad[MAX_COLS/8];
	int temp1ae[MAX_COLS/8];
	int temp1af[MAX_COLS/8];
	int temp1ag[MAX_COLS/8];
	int temp1ah[MAX_COLS/8];
	
	#pragma omp parallel sections
	{
		#pragma omp section
		{
			for (int i = 0; i < max/16; ++i)
			{
				a1[i] = a[i];
			}
		}

		#pragma omp section
		{
			for (int i = max/16; i < max/8; ++i)
			{
				a2[i - max/16] = a[i];
			}
		}

		#pragma omp section
		{
			for (int i = max/8; i < 3*max/16; ++i)
			{
				a3[i - max/8] = a[i];
			}
		}

		#pragma omp section
		{
			for (int i = 3*max/16; i < max/4; ++i)
			{
				a4[i - 3*max/16] = a[i];
			}
		}

		#pragma omp section
		{
			for (int i = max/4; i < 5*max/16; ++i)
			{
				a5[i - max/4] = a[i];
			}
		}

		#pragma omp section
		{
			for (int i = 5*max/16; i < 3*max/8; ++i)
			{
				a6[i - 5*max/16] = a[i];
			}
		}

		#pragma omp section
		{
			for (int i = 3*max/8; i < 7*max/16; ++i)
			{
				a7[i - 3*max/8] = a[i];
			}
		}

		#pragma omp section
		{
			for (int i = 7*max/16; i < max/2; ++i)
			{
				a8[i - 7*max/16] = a[i];
			}
		}

		#pragma omp section
		{
			for (int i = max/2; i < 9*max/16; ++i)
			{
				a9[i - max/2] = a[i];
			}
		}

		#pragma omp section
		{
			for (int i = 9*max/16; i < 5*max/8; ++i)
			{
				a10[i - 9*max/16] = a[i];
			}
		}
		#pragma omp section
		{
			for (int i = 5*max/8; i < 11*max/16; ++i)
			{
				a11[i - 5*max/8] = a[i];
			}
		}
		#pragma omp section
		{
			for (int i = 11*max/16; i < 3*max/4; ++i)
			{
				a12[i - 11*max/16] = a[i];
			}
		}
		#pragma omp section
		{
			for (int i = 3*max/4; i < 13*max/16; ++i)
			{
				a13[i - 3*max/4] = a[i];
			}
		}
		#pragma omp section
		{
			for (int i = 13*max/16; i < 7*max/8; ++i)
			{
				a14[i - 13*max/16] = a[i];
			}
		}
		#pragma omp section
		{
			for (int i = 7*max/8; i < 15*max/16; ++i)
			{
				a15[i - 7*max/8] = a[i];
			}
		}
		#pragma omp section
		{
			for (int i = 15*max/16; i < max; ++i)
			{
				a16[i - 15*max/16] = a[i];
			}
		}
	}

	#pragma omp parallel sections
	{
		#pragma omp section
		{
			bubble(a1, max/16);
		}

		#pragma omp section
		{
			bubble(a2, max/16);
		}

		#pragma omp section
		{
			bubble(a3, max/16);
		}

		#pragma omp section
		{
			bubble(a4, max/16);
		}

		#pragma omp section
		{
			bubble(a5, max/16);
		}

		#pragma omp section
		{
			bubble(a6, max/16);
		}

		#pragma omp section
		{
			bubble(a7, max/16);
		}

		#pragma omp section
		{
			bubble(a8, max/16);
		}

		#pragma omp section
		{
			bubble(a9, max/16);
		}

		#pragma omp section
		{
			bubble(a10, max/16);
		}

		#pragma omp section
		{
			bubble(a11, max/16);
		}

		#pragma omp section
		{
			bubble(a12, max/16);
		}

		#pragma omp section
		{
			bubble(a13, max/16);
		}

		#pragma omp section
		{
			bubble(a14, max/16);
		}

		#pragma omp section
		{
			bubble(a15, max/16);
		}

		#pragma omp section
		{
			bubble(a16, max/16);
		}
	}

	#pragma omp parallel sections
	{
		#pragma omp section
		{
			merge(max/16, max/16, a1, a2, temp1aa);
		}

		#pragma omp section
		{
			merge(max/16, max/16, a3, a4, temp1ab);
		}

		#pragma omp section
		{
			merge(max/16, max/16, a5, a6, temp1ac);
		}

		#pragma omp section
		{
			merge(max/16, max/16, a7, a8, temp1ad);
		}

		#pragma omp section
		{
			merge(max/16, max/16, a9, a10, temp1ae);
		}

		#pragma omp section
		{
			merge(max/16, max/16, a11, a12, temp1af);
		}

		#pragma omp section
		{
			merge(max/16, max/16, a13, a14, temp1ag);
		}

		#pragma omp section
		{
			merge(max/16, max/16, a15, a16, temp1ah);
		}
	}

	#pragma omp parallel sections
	{
		#pragma omp section
		{
			merge(max/8, max/8, temp1aa, temp1ab, temp1a);
		}

		#pragma omp section
		{
			merge(max/8, max/8, temp1ac, temp1ad, temp2a);
		}

		#pragma omp section
		{
			merge(max/8, max/8, temp1ae, temp1af, temp3a);
		}

		#pragma omp section
		{
			merge(max/8, max/8, temp1ag, temp1ah, temp4a);
		}
	}

	#pragma omp parallel sections
	{
		#pragma omp section
		{
			merge(max/4, max/4, temp1a, temp2a, temp1);
		}

		#pragma omp section
		{
			merge(max/4, max/4, temp3a, temp4a, temp2);
		}
	}

	merge(max/2, max/2, temp1, temp2, a);
}

void bubblesortSixteenSectionsOneArray(int * a, int max)
{
	void bubble(int * a, int max);
	void merge(int m, int n, int A[], int B[], int C[]);

	int* a1 = new int[MAX_COLS*MAX_COLS/16];
	int* a2 = new int[MAX_COLS*MAX_COLS/16];
	int a3[MAX_COLS*MAX_COLS/16];
	int a4[MAX_COLS*MAX_COLS/16];
	int a5[MAX_COLS*MAX_COLS/16];
	int a6[MAX_COLS*MAX_COLS/16];
	int a7[MAX_COLS*MAX_COLS/16];
	int a8[MAX_COLS*MAX_COLS/16];
	int a9[MAX_COLS*MAX_COLS/16];
	int a10[MAX_COLS*MAX_COLS/16];
	int a11[MAX_COLS*MAX_COLS/16];
	int a12[MAX_COLS*MAX_COLS/16];
	int a13[MAX_COLS*MAX_COLS/16];
	int a14[MAX_COLS*MAX_COLS/16];
	int a15[MAX_COLS*MAX_COLS/16];
	int a16[MAX_COLS*MAX_COLS/16];

	int temp1[MAX_COLS*MAX_ROWS/2];
	int temp2[MAX_COLS*MAX_ROWS/2];

	int temp1a[MAX_COLS*MAX_ROWS/4];
	int temp2a[MAX_COLS*MAX_ROWS/4];
	int temp3a[MAX_COLS*MAX_ROWS/4];
	int temp4a[MAX_COLS*MAX_ROWS/4];

	int temp1aa[MAX_COLS*MAX_ROWS/8];
	int temp1ab[MAX_COLS*MAX_ROWS/8];
	int temp1ac[MAX_COLS*MAX_ROWS/8];
	int temp1ad[MAX_COLS*MAX_ROWS/8];
	int temp1ae[MAX_COLS*MAX_ROWS/8];
	int temp1af[MAX_COLS*MAX_ROWS/8];
	int temp1ag[MAX_COLS*MAX_ROWS/8];
	int temp1ah[MAX_COLS*MAX_ROWS/8];
	
	#pragma omp parallel sections
	{
		#pragma omp section
		{
			for (int i = 0; i < max/16; ++i)
			{
				a1[i] = a[i];
			}
		}

		#pragma omp section
		{
			for (int i = max/16; i < max/8; ++i)
			{
				a2[i - max/16] = a[i];
			}
		}

		#pragma omp section
		{
			for (int i = max/8; i < 3*max/16; ++i)
			{
				a3[i - max/8] = a[i];
			}
		}

		#pragma omp section
		{
			for (int i = 3*max/16; i < max/4; ++i)
			{
				a4[i - 3*max/16] = a[i];
			}
		}

		#pragma omp section
		{
			for (int i = max/4; i < 5*max/16; ++i)
			{
				a5[i - max/4] = a[i];
			}
		}

		#pragma omp section
		{
			for (int i = 5*max/16; i < 3*max/8; ++i)
			{
				a6[i - 5*max/16] = a[i];
			}
		}

		#pragma omp section
		{
			for (int i = 3*max/8; i < 7*max/16; ++i)
			{
				a7[i - 3*max/8] = a[i];
			}
		}

		#pragma omp section
		{
			for (int i = 7*max/16; i < max/2; ++i)
			{
				a8[i - 7*max/16] = a[i];
			}
		}

		#pragma omp section
		{
			for (int i = max/2; i < 9*max/16; ++i)
			{
				a9[i - max/2] = a[i];
			}
		}

		#pragma omp section
		{
			for (int i = 9*max/16; i < 5*max/8; ++i)
			{
				a10[i - 9*max/16] = a[i];
			}
		}
		#pragma omp section
		{
			for (int i = 5*max/8; i < 11*max/16; ++i)
			{
				a11[i - 5*max/8] = a[i];
			}
		}
		#pragma omp section
		{
			for (int i = 11*max/16; i < 3*max/4; ++i)
			{
				a12[i - 11*max/16] = a[i];
			}
		}
		#pragma omp section
		{
			for (int i = 3*max/4; i < 13*max/16; ++i)
			{
				a13[i - 3*max/4] = a[i];
			}
		}
		#pragma omp section
		{
			for (int i = 13*max/16; i < 7*max/8; ++i)
			{
				a14[i - 13*max/16] = a[i];
			}
		}
		#pragma omp section
		{
			for (int i = 7*max/8; i < 15*max/16; ++i)
			{
				a15[i - 7*max/8] = a[i];
			}
		}
		#pragma omp section
		{
			for (int i = 15*max/16; i < max; ++i)
			{
				a16[i - 15*max/16] = a[i];
			}
		}
	}

	#pragma omp parallel sections
	{
		#pragma omp section
		{
			bubble(a1, max/16);
		}

		#pragma omp section
		{
			bubble(a2, max/16);
		}

		#pragma omp section
		{
			bubble(a3, max/16);
		}

		#pragma omp section
		{
			bubble(a4, max/16);
		}

		#pragma omp section
		{
			bubble(a5, max/16);
		}

		#pragma omp section
		{
			bubble(a6, max/16);
		}

		#pragma omp section
		{
			bubble(a7, max/16);
		}

		#pragma omp section
		{
			bubble(a8, max/16);
		}

		#pragma omp section
		{
			bubble(a9, max/16);
		}

		#pragma omp section
		{
			bubble(a10, max/16);
		}

		#pragma omp section
		{
			bubble(a11, max/16);
		}

		#pragma omp section
		{
			bubble(a12, max/16);
		}

		#pragma omp section
		{
			bubble(a13, max/16);
		}

		#pragma omp section
		{
			bubble(a14, max/16);
		}

		#pragma omp section
		{
			bubble(a15, max/16);
		}

		#pragma omp section
		{
			bubble(a16, max/16);
		}
	}

	#pragma omp parallel sections
	{
		#pragma omp section
		{
			merge(max/16, max/16, a1, a2, temp1aa);
		}

		#pragma omp section
		{
			merge(max/16, max/16, a3, a4, temp1ab);
		}

		#pragma omp section
		{
			merge(max/16, max/16, a5, a6, temp1ac);
		}

		#pragma omp section
		{
			merge(max/16, max/16, a7, a8, temp1ad);
		}

		#pragma omp section
		{
			merge(max/16, max/16, a9, a10, temp1ae);
		}

		#pragma omp section
		{
			merge(max/16, max/16, a11, a12, temp1af);
		}

		#pragma omp section
		{
			merge(max/16, max/16, a13, a14, temp1ag);
		}

		#pragma omp section
		{
			merge(max/16, max/16, a15, a16, temp1ah);
		}
	}

	#pragma omp parallel sections
	{
		#pragma omp section
		{
			merge(max/8, max/8, temp1aa, temp1ab, temp1a);
		}

		#pragma omp section
		{
			merge(max/8, max/8, temp1ac, temp1ad, temp2a);
		}

		#pragma omp section
		{
			merge(max/8, max/8, temp1ae, temp1af, temp3a);
		}

		#pragma omp section
		{
			merge(max/8, max/8, temp1ag, temp1ah, temp4a);
		}
	}

	#pragma omp parallel sections
	{
		#pragma omp section
		{
			merge(max/4, max/4, temp1a, temp2a, temp1);
		}

		#pragma omp section
		{
			merge(max/4, max/4, temp3a, temp4a, temp2);
		}
	}

	merge(max/2, max/2, temp1, temp2, a);
}

void merge(int m, int n, int A[], int B[], int C[])
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


void bubble(int * a, int max)
{
	for(int n=max-1; n>=0; n--){   
        for(int j=0; j<n; j++){   
            if(a[j] > a[j+1]){  
				//swap
                int temp = a[j];   
                a[j] = a[j+1];   
                a[j+1] = temp;   
            }   
        }  
	}
}	

void testData()
{
	cout << "\n\n*** testing data ***";
	cout << "\n\n\tdata[0][0]                   = " << data[0][0];					//=87 for srand(123)
	cout << "\n\tdata[MAX_ROWS/2][MAX_COLS/2] = " << data[MAX_ROWS/2][MAX_COLS/2];	//=16440 for srand(123)
	cout << "\n\tdata[MAX_ROWS-1][MAX_COLS-1] = " << data[MAX_ROWS-1][MAX_COLS-1];	//=32760 for srand(123)
}

void outputData()
{
	s_output.startTimer();

	ofstream sodata;
	sodata.open ("sodata.txt");
	
	cout << "\n\n*** outputting data ***";
	cout << "\n\n\toutputting data to sodata.txt...";

	for(int i=0; i<MAX_ROWS; i++){
		for(int j=0; j<MAX_COLS; j++){
			sodata << data[i][j] << "\t";
		}
		sodata << "\n";
	}

	s_output.stopTimer();
	
	sodata << "\n\nTime for getting: " << s_get.getElapsedTime();
	sodata << "\nTime for getting all data: " << s_get2.getElapsedTime();
	sodata << "\nTime for sorting: " << s_sort.getElapsedTime();
	sodata << "\nTime for outputting: " << s_output.getElapsedTime();

	sodata.close();
}

void outputTimes()
{
	cout << "\n\nTime for getting: " << s_get.getElapsedTime();
	cout << "\nTime for getting all data: " << s_get2.getElapsedTime();	
	cout << "\nTime for sorting: " << s_sort.getElapsedTime();
	cout << "\nTime for outputting: " << s_output.getElapsedTime();
}

//builds file as a string then outputs the whole thing
//in one big fputs!
string outputStrings2()
{

	char numString[MAX_CHARS];
	string odata1;
	cout << "\n\noutputting data to sodata.txt...";

	for(int i = 0; i < (MAX_ROWS); i++)
	{
		for(int j = 0; j < (MAX_COLS); j++)
		{
			itoa(data[i][j], numString, 10);
			odata1 += numString;
			odata1 += "\t";
		}
		odata1 += "\n";
	}

	return odata1;
}

void writeInFile(string odata)
{
	FILE * sodata;
	sodata = fopen ("sodata.txt", "w");
	fputs(odata.c_str(), sodata);
	fclose (sodata);
}

string outputStrings3()
{
	char numString[MAX_CHARS];
	string odata2;
	cout << "\n\noutputting data to sodata.txt...";
	
	for(int j = 0; j < MAX_COLS*MAX_ROWS; j++)
		{
			itoa(allData[j], numString, 10);
			odata2 += numString;
			odata2 += "\t";
		}
		odata2 += "\n";
	
	return odata2;
}

void writeInFile2(string odata2)
{
	FILE * sodata2;
	sodata2 = fopen ("soDataAll.txt", "w");
	fputs(odata2.c_str(), sodata2);
	fclose (sodata2);
}