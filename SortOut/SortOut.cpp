/****************************************************************

	SortOut - integer sorting and file output program

	Goldberg, Peter
	Hornung, Nico

	MAPS - SHU - 17/04/2012

*****************************************************************/

#include <fstream>			//for file output
#include <iostream>			//for console output
#include <conio.h>			//for kbhit
#include "hr_timer.h"		//for stopwatches
#include <stdio.h>			//for fputs
#include <omp.h>			//for OpenMP constructs
#include <emmintrin.h>		//for SIMD intrinsics
using namespace std;

#define MAX_ROWS 2000
#define MAX_COLS 1000
#define MAX_CHARS 6

//SIMD Globals//===========================================================

#define MagicNumber		0x199A	

std::wstring output;

__declspec(align(16)) short div10000[8] = {MagicNumber, MagicNumber, MagicNumber, MagicNumber,MagicNumber, MagicNumber, MagicNumber, MagicNumber};

__declspec(align(16)) short ten[8] = {10,10,10,10,10,10,10,10};

__declspec(align(16)) short fortyEight[8] = {48,48,48,48,48,48,48,48};

__declspec(align(16)) short idataA[8];
__declspec(align(16)) wchar_t cString[8];
__declspec(align(16)) wchar_t cString2[8];
__declspec(align(16)) wchar_t cString3[8];
__declspec(align(16)) wchar_t cString4[8];
__declspec(align(16)) wchar_t cString5[8];

//__mi128i pointers to data
__m128i* pidataA  = (__m128i*) idataA;
__m128i* pString  = (__m128i*) cString;	
__m128i* pString2 = (__m128i*) cString2;
__m128i* pString3 = (__m128i*) cString3;
__m128i* pString4 = (__m128i*) cString4;
__m128i* pString5 = (__m128i*) cString5;

// char array for output
wchar_t test[13000000];
//=========================================================================

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

int main()
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

		#pragma omp parallel for
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

	#pragma omp parallel for private (numString) schedule (static, MAX_ROWS/2)
	for(int i=0; i<MAX_ROWS; i++){
		for(int j=0; j<MAX_COLS; j++){
			itoa(data[i][j], numString, 10);
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
	
#pragma omp parallel for private (numString) schedule (static, MAX_ROWS/2)
	for(int i=0; i < MAX_ROWS; i++)
	{
		for(int j=0; j < MAX_COLS; j++)
		{
			myitoa(data[i][j], numString);
			
			if(i < MAX_ROWS/2) 
			{
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
	void mySIMDitoa(short, short, short, short, short, short, short, short);	
	
	cout << "\n\noutputting data to sodata_mySIMDitoa.txt...";	
	
	int count(-1); //counter for the char array

	for(int i = 0; i < MAX_ROWS; ++i)
	{
		for(int j = 0; j < MAX_COLS/8; ++j)
		{
			mySIMDitoa(data[i][j*8], data[i][(j*8)+1], data[i][(j*8)+2], data[i][(j*8)+3], data[i][(j*8)+4], data[i][(j*8)+5], data[i][(j*8)+6], data[i][(j*8)+7]);	
			
			//Writes the chars to a single char array - this is the bottle neck in our code.
			for(int k = 0; k < 8; ++k)
			{
				//Any numbers that were greater than 16000 that ended in 9 had the 9 replaced with a slash, this corrects the problem.
				if(cString5[k] == 47)
				{
					cString5[k] = 57;
					--cString4[k];
				}

				test[++count] = cString[k];
				test[++count] = cString2[k];
				test[++count] = cString3[k];
				test[++count] = cString4[k];
				test[++count] = cString5[k];
				test[++count] = ' ';
			}
		}	
		
		test[++count] = '\n';		
	}		

	FILE * sodata;
	sodata = fopen ("sodata_mySIMDitoa.txt","w");
	fputws(test, sodata);
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

void mySIMDitoa(short num, short num2, short num3, short num4, short num5, short num6, short num7, short num8)
{
	idataA[0] = num;
	idataA[1] = num2;
	idataA[2] = num3;
	idataA[3] = num4;	
	idataA[4] = num5;
	idataA[5] = num6;
	idataA[6] = num7;	
	idataA[7] = num8;	

	__asm {

	
		mov eax, pString		;point to char array
		mov ebx, pidataA		;numbers array
								
		movdqa xmm1, [ebx]		;numbers
		movdqa xmm2, xmm1		;copies the numbers
		movdqa xmm3, div10000	;magic num div10000 in xmm reg	
		movdqa xmm4, fortyEight	;moves the forty eights into xmm4
						
		pmulhuw	xmm1, xmm3		;This knocks the last number off of the 5 digits so we have abcd of abcde
		movdqa	xmm5, xmm1		;stores abcd in xmm5
		pmulhuw xmm1, xmm3		;knocks the last number off so we have abc
		movdqa  xmm6, xmm1		;stores abc in xmm6
		pmulhuw xmm1, xmm3		;knocks off the last number giving ab
		movdqa	xmm7, xmm1		;stores ab in xmm7
		pmulhuw xmm1, xmm3		;knocks off last number giving a

		movdqa xmm0, xmm1		;copies a
		paddd  xmm1, xmm4		;make a a char

		movdqa [eax], xmm1		;moves the numbers into the char array

		pmullw xmm0, ten		;multiplies a by ten giving a0

		movdqa xmm1, xmm7		;copies ab to xmm1

		psubd xmm7, xmm0		;subtracts a0 from ab giving b

		paddd xmm7, xmm4		;adds 48 to b making it a char

		mov eax, pString2
		movdqa [eax], xmm7		;puts it into char array

		pmullw xmm1, ten		;multiplies ab by ten - ab0

		movdqa xmm0, xmm6		;makes a copy of abc

		psubd  xmm6, xmm1		;subtracts ab0 from abc giving c

		paddd xmm6, xmm4		;makes c 'c'
		mov eax, pString3
		movdqa [eax], xmm6		;maoves 'c' into char array

		pmullw xmm0, ten		;gives abc0

		movdqa xmm6, xmm5		;copies abcd

		psubd  xmm6, xmm0		;subtracts abc0 from abcd

		paddd xmm6, xmm4		;adds 48 to d
		mov eax, pString4
		movdqa [eax], xmm6		;moves it into char array

		pmullw xmm5, ten		;multiplies abcd by ten

		psubd xmm2, xmm5		;finally, takes abcd0 from abcde giving e

		paddd xmm2, xmm4		;makes e a char
		mov eax, pString5
		movdqa [eax], xmm2		;pushes it into char array		
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