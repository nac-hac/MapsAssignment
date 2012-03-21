/****************************************************************

	SortOut - integer sorting and file output program

	sortout_forAssignment2.cpp	- original (serial) getData
								- omp parallel sections:   split bubble sorting
								- omp parallel for: 	   outputStrings2

*****************************************************************/

#include <fstream>			//for file output
#include <iostream>			//for console output
#include <conio.h>			//for kbhit
#include "hr_timer.h"		//for stopwatches
#include <stdio.h>			//for fputs
#include <omp.h>			//for OpenMP constructs
using namespace std;

#define MAX_ROWS 2000
#define MAX_COLS 1000
#define MAX_CHARS 6

int data[MAX_ROWS][MAX_COLS];

CStopWatch
	swGetData,
	swSortData,
	swOutput_itoa,
	swOutput_myitoa,
	swOutput_mySIMDitoa;

void getData();
void sortDataOmpFor();
void testData();
void output2StringsOmpFor_itoa();
void output2StringsOmpFor_myitoa();
void output2StringsOmpFor_mySIMDitoa();
void outputTimes();


int main(void)
{
	swGetData.startTimer();
		getData();
	swGetData.stopTimer();

	swSortData.startTimer();
		sortDataOmpFor();
	swSortData.stopTimer();

		testData();

	swOutput_itoa.startTimer();
		output2StringsOmpFor_itoa();
	swOutput_itoa.stopTimer();

	swOutput_myitoa.startTimer();
		output2StringsOmpFor_myitoa();
	swOutput_myitoa.stopTimer();

	swOutput_mySIMDitoa.startTimer();
		output2StringsOmpFor_mySIMDitoa();
	swOutput_mySIMDitoa.stopTimer();

		outputTimes();

	cout << "\n\nDone.";
	while (! _kbhit());  //to hold console
}


void getData()
{
	cout << "getting data...";
	srand(123); //arbitrary random number seed
	for(int i=0; i<MAX_ROWS; i++)
		for(int j=0; j<MAX_COLS; j++)
			data[i][j] = rand(); //RAND_MAX = 32767
}


void sortDataOmpFor()
{
	void split_bubble(int * a, int n);

	cout << "\nsorting data...";

		//#pragma omp parallel for
		for(int i=0; i<MAX_ROWS; i++){
			//bubble sort row i
			split_bubble(data[i], MAX_COLS);
		}
}


void split_bubble(int *a, int max)
{
	//bubble sort first half of row
	for(int n=max-1; n>=max/2; n--){
        for(int j=max/2; j<n; j++){
            if(a[j] > a[j+1]){
				//swap
                int temp = a[j];
                a[j] = a[j+1];
                a[j+1] = temp;
            }
        }
	}

	//bubble sort second half of row
	for(int n=max/2 - 1; n>=0; n--){
        for(int j=0; j<n; j++){
            if(a[j] > a[j+1]){
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
}


void testData()
{
	if (data[0][0] == 87 && data[MAX_ROWS/2][MAX_COLS/2] == 16440 && data[MAX_ROWS-1][MAX_COLS-1] == 32760)
		cout << "\n\nTEST SUCCEEDED!";
	else
		cout << "\n\nTEST FAILED!";
}

//builds two half-file strings in parallel using library fn
void output2StringsOmpFor_itoa()
{
	char numString[MAX_CHARS];
	string odataf1, odataf2;
	cout << "\n\noutputting data to sodata_itoa.txt...";

	//#pragma omp parallel for private (numString) schedule (static, MAX_ROWS/2)
	for(int i=0; i<MAX_ROWS; i++){
		for(int j=0; j<MAX_COLS; j++){
			itoa(data[i][j],numString,10);
			if(i < MAX_ROWS/2) {
				odataf1 += numString;
				odataf1+="\t";
			}
			else {
				odataf2 += numString;
				odataf2+="\t";
			}
		}
		if(i < MAX_ROWS/2)
			odataf1+="\n";
		else
			odataf2+="\n";
	}

	FILE * sodata;
	sodata = fopen ("sodata_itoa.txt","w");
	fputs(odataf1.c_str(), sodata);
	fputs(odataf2.c_str(), sodata);
	fclose (sodata);
}

//builds two half-file strings in parallel using custom fn
void output2StringsOmpFor_myitoa()
{
	void myitoa(int num, char * numstr);

	char numString[MAX_CHARS];
	string odataf1, odataf2;
	cout << "\n\noutputting data to sodata_myitoa.txt...";

	//#pragma omp parallel for private (numString) schedule (static, MAX_ROWS/2)
	for(int i=0; i<MAX_ROWS; i++){
		for(int j=0; j<MAX_COLS; j++){
			myitoa(data[i][j],numString);
			if(i < MAX_ROWS/2) {
				odataf1 += numString;
				odataf1+="\t";
			}
			else {
				odataf2 += numString;
				odataf2+="\t";
			}
		}
		if(i < MAX_ROWS/2)
			odataf1+="\n";
		else
			odataf2+="\n";
	}

	FILE * sodata;
	sodata = fopen ("sodata_myitoa.txt","w");
	fputs(odataf1.c_str(), sodata);
	fputs(odataf2.c_str(), sodata);
	fclose (sodata);
}

//builds two half-file strings in parallel using custom fn
void output2StringsOmpFor_mySIMDitoa()
{
	void mySIMDitoa(int num, char * numstr);

	char numString[MAX_CHARS];
	string odataf1, odataf2;
	cout << "\n\noutputting data to sodata_mySIMDitoa.txt...";

	//#pragma omp parallel for private (numString) schedule (static, MAX_ROWS/2)
	for(int i=0; i<MAX_ROWS; i++){
		for(int j=0; j<MAX_COLS; j++){
			mySIMDitoa(data[i][j],numString);
			if(i < MAX_ROWS/2) {
				odataf1 += numString;
				odataf1+="\t";
			}
			else {
				odataf2 += numString;
				odataf2+="\t";
			}
		}
		if(i < MAX_ROWS/2)
			odataf1+="\n";
		else
			odataf2+="\n";
	}

	FILE * sodata;
	sodata = fopen ("sodata_mySIMDitoa.txt","w");
	fputs(odataf1.c_str(), sodata);
	fputs(odataf2.c_str(), sodata);
	fclose (sodata);
}

/*****  myitoa fast integer to ascii fn *****
 *****  copyright Simon Andrews, 2012   *****/
//unsigned integer version
void myitoa(int num, char * numstr)
{
	__asm {
			mov		ebx,numstr		; point ebx to numstr
			mov		esi, num		; store number in esi
			cmp		esi, 0			; if number is 0
			jne		nextdigit
			mov     [ebx],48		; just set numstr to "0"
			mov     [ebx+1],0		; add terminatting null character
			jmp		enditoa			; and end

nextdigit:	mov		eax, 1999999Ah		;1999999Ah = 2^34 / (10 * 2^2) (magic number!)
			imul	esi					;IMUL ESI >> EDX:EAX <-- EAX * ESI
										;Therefore, edx:eax = number * 2^34 / (10 * 2^2)
										;Therefore edx	= number * 2^34 / (10 * 2^2 * 2^32)
										;				= number * 2^34 /(10 * 2^34)
										;				= number / 10 (integer division)

			lea		ecx, [edx + edx*4]	; ecx = edx * 5
			add		ecx,ecx				; ecx = edx * 10
										; therfore ecx = (number div 10) * 10

			mov		eax,esi				; store original number in eax
			sub		eax,ecx				; subtract ecx to leave remainder in eax

			add		eax,48				; add 48 to make eax the digit's ASCII code

			mov		[ebx],al			; store digit character in numstr
			inc		ebx

			mov		esi,edx				; move number div 10 back into esi
			cmp		esi,0				; if number div 10 = 0, we've finished
			jnz		nextdigit

			mov     [ebx],0				; so add terminating null character

; The number is in reverse order of digits
; so we need to reverse the string.
; Note that ebx is currently pointing to the terminating null char of the number string

			mov		edx,numstr	;point edx to start
nextChar:	dec		ebx			;move end pointer back one char		
			cmp		ebx,edx		;compare the pointers
			jle		enditoa		;if they haven't met or passed in the middle
			mov		eax,[edx]	;swap the chars
			mov		ecx,[ebx]
			mov		[ebx],al
			mov		[edx],cl
			inc		edx			;move start pointer forward one char
			jmp     nextChar
enditoa:
	}
}


void mySIMDitoa(int num, char * numstr)
{
	__asm {
			mov		ebx,numstr		; point ebx to numstr
			mov		esi, num		; store number in esi
			cmp		esi, 0			; if number is 0
			jne		nextdigit
			mov     [ebx],48		; just set numstr to "0"
			mov     [ebx+1],0		; add terminatting null character
			jmp		enditoa			; and end

nextdigit:	mov		eax, 1999999Ah		;1999999Ah = 2^34 / (10 * 2^2) (magic number!)
			imul	esi					;IMUL ESI >> EDX:EAX <-- EAX * ESI
										;Therefore, edx:eax = number * 2^34 / (10 * 2^2)
										;Therefore edx	= number * 2^34 / (10 * 2^2 * 2^32)
										;				= number * 2^34 /(10 * 2^34)
										;				= number / 10 (integer division)

			lea		ecx, [edx + edx*4]	; ecx = edx * 5
			add		ecx,ecx				; ecx = edx * 10
										; therfore ecx = (number div 10) * 10

			mov		eax,esi				; store original number in eax
			sub		eax,ecx				; subtract ecx to leave remainder in eax

			add		eax,48				; add 48 to make eax the digit's ASCII code

			mov		[ebx],al			; store digit character in numstr
			inc		ebx

			mov		esi,edx				; move number div 10 back into esi
			cmp		esi,0				; if number div 10 = 0, we've finished
			jnz		nextdigit

			mov     [ebx],0				; so add terminating null character

; The number is in reverse order of digits
; so we need to reverse the string.
; Note that ebx is currently pointing to the terminating null char of the number string

			mov		edx,numstr	;point edx to start
nextChar:	dec		ebx			;move end pointer back one char		
			cmp		ebx,edx		;compare the pointers
			jle		enditoa		;if they haven't met or passed in the middle
			mov		eax,[edx]	;swap the chars
			mov		ecx,[ebx]
			mov		[ebx],al
			mov		[edx],cl
			inc		edx			;move start pointer forward one char
			jmp     nextChar
enditoa:
	}
}

void outputTimes()
{
	cout << "\n\nTime for getting: " << swGetData.getElapsedTime();
	cout << "\n\nTime for sorting: " << swSortData.getElapsedTime();
	cout << "\nTime for output itoa: " << swOutput_itoa.getElapsedTime();
	cout << "\nTime for output myitoa: " << swOutput_myitoa.getElapsedTime();
	cout << "\nTime for output mySIMDitoa: " << swOutput_mySIMDitoa.getElapsedTime();

	float difference = swOutput_myitoa.getElapsedTime() - swOutput_mySIMDitoa.getElapsedTime();

	cout << "\n\nPercentage: " << (difference/swOutput_myitoa.getElapsedTime())*100 << "%";
	
}
