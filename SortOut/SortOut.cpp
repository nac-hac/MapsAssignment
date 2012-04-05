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
#include <emmintrin.h>		//for SIMD intrinsics
using namespace std;

#define MAX_ROWS 2000
#define MAX_COLS 1000
#define MAX_CHARS 6

//SIMD Globals//===========================================================

#define DIV_10000		0xd1b71759	
#define DIV_100			0x147b	
#define DIV_10			0x199a	

__declspec(align(16)) int div10000[4] = {DIV_10000,0,DIV_10000,0};
__declspec(align(16)) int div100[4] = {DIV_100,0,DIV_100,0};
__declspec(align(16)) int div10[4] = {DIV_10,0,DIV_10,0};

__declspec(align(16)) int ten[4] = {10,0,10,0};
__declspec(align(16)) int hund[4] = {100,0,100,0};
__declspec(align(16)) int thou[4] = {1000,0,1000,0};
__declspec(align(16)) int hundThou[4] = {10000,0,10000,0};
__declspec(align(16)) int fortyEight[4] = {48,0,48,0};

__declspec(align(16)) int idataA[4];
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

//=========================================================================


int data[MAX_ROWS][MAX_COLS];

__declspec(align(16)) int idata[MAX_ROWS][MAX_COLS];

std::wstring output[12];

CStopWatch
	swGetData,
	swSortData,
	swOutput_itoa,
	swOutput_myitoa,
	swOutput_mySIMDitoa;

void getData();
void getiData();
void sortDataOmpFor();
void sortIdataOmpFor();
void testiData();
void testData();
void output2StringsOmpFor_itoa();
void output2StringsOmpFor_myitoa();
void output2StringsOmpFor_mySIMDitoa();
void outputTimes();


int main()
{
	swGetData.startTimer();
		getData();
		getiData();
	swGetData.stopTimer();

	swSortData.startTimer();
		sortDataOmpFor();
		sortIdataOmpFor();
	swSortData.stopTimer();

		testData();
		testiData();

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

void getiData()
{
	cout << "getting data...";
	srand(123); //arbitrary random number seed
	for(int i=0; i<MAX_ROWS; i++)
		for(int j=0; j<MAX_COLS; j++)
			idata[i][j] = rand(); //RAND_MAX = 32767
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

void sortIdataOmpFor()
{
	void split_bubble(int * a, int n);

	cout << "\nsorting data...";

		#pragma omp parallel for
		for(int i=0; i<MAX_ROWS; i++){
			//bubble sort row i
			split_bubble(idata[i], MAX_COLS);
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

void testiData()
{
	if (idata[0][0] == 87 && idata[MAX_ROWS/2][MAX_COLS/2] == 16440 && idata[MAX_ROWS-1][MAX_COLS-1] == 32760)
		cout << "\n\nidata TEST SUCCEEDED!";
	else
		cout << "\n\nidata TEST FAILED!";
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
	void mySIMDitoa(int, int);
	
	std::wstring data;

	cout << "\n\noutputting data to sodata_mySIMDitoa.txt...";

	//#pragma omp parallel for private (data) schedule (static, MAX_ROWS/2)
		for(int i=0; i < 2000; ++i)
		{
			for(int j=0; j < MAX_COLS/2; j++)
			{
				mySIMDitoa(idata[i][j*2], idata[i][(j*2)+1]);

				for(int k = 0; k < 12; ++k)
				{
					data += output[k];
				}
			}

			data += 012;
		}

	FILE * sodata;
	sodata = fopen ("sodata_mySIMDitoa.txt","w");
	fputws(data.c_str(), sodata);
	
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

void mySIMDitoa(int num, int num2)
{
	idataA[0] = num;
	idataA[1] = 0;
	idataA[2] = num2;
	idataA[3] = 0;	

	__asm {

		mov eax, pString	;point to char array
		mov ebx, pidataA	;numbers array
						
		movdqa xmm1, [ebx]		;numbers
		movdqa xmm2, xmm1		;copies the numbers
		movdqa xmm3, div10000	;magic num div10000 in xmm reg	
						
		pmuludq	xmm1, xmm3	;multiplies by the DIV_10000 magic number but splits into two 64 bit registers
		psrlq xmm1, 45		;logical shift right leaves us with the first numbers
		
		movdqa xmm4, xmm1	;copies the quotient
		movdqa xmm5, hundThou	;moves 10000 into xmm5

		movdqa xmm7, fortyEight	;xmm register 
		paddd xmm1, xmm7		;adds 48 to the numbers

		movdqa [eax], xmm1		;pushes the characters into the array

		movdqa xmm1, xmm2		;copies numbers to xmm1
		
		pmuludq xmm4, xmm5		;multiplies the quotient by 10000

		psubd	xmm1, xmm4		;subtracts the quotient from the numbers
								;xmm1 now contains the 4 numbers

		movdqa xmm2, xmm1		;copy numbers to xmm2
		movdqa xmm3, div10		;moves the magic number for div10 over
		movdqa xmm6, div100
		pmuludq xmm1, xmm6		;divides by ten three times to 
		pmuludq xmm1, xmm3		;give division by 1000
		
		psrlq xmm1, 35			;left shifts to give quotient
		movdqa xmm0, xmm1		;copies quotient

		paddd xmm1, xmm7		;adds 48 to make it a char
		
		mov   eax, pString2		;point to string2
		movdqa [eax], xmm1		;push to string2

		movdqa xmm1, xmm2		;numbers are back in xmm1
		movdqa xmm4, thou		;1000 in register

		pmuludq	xmm0, xmm4		;multiplies quotient

		psubq xmm1, xmm0		;subtract to give the three numbers

		movdqa xmm6, xmm1		;copies the 3 numbers

		movdqa xmm3, div100		;magic number division by 100

		pmuludq	xmm1, xmm3		;multiply by div100 magic number

		psrlq xmm1, 19			;logical shift to simulate division

		movdqa xmm0, xmm1		; copies the quotients

		paddd xmm1, xmm7		;makes numbers chars

		mov   eax, pString3		;point to string3
		movdqa [eax], xmm1		;push to string3

		movdqa xmm1, hund		;move hundred into xmm1

		pmuludq xmm1, xmm0		; multiply quotient by 100

		psubd xmm6, xmm1		;subtract to give the two numbers 

		movdqa xmm1, xmm6		;copy the numbers

		movdqa xmm0, div10		;move magic number divison 10 into reg

		pmuludq xmm6, div10		;multiply by magic number

		psrlq xmm6, 16			;logical shift dividing by ten

		movdqa xmm0, xmm6		;store the result

		paddd xmm6, xmm7

		mov   eax, pString4		;point to string4
		movdqa [eax], xmm6		;push to string4

		movdqa xmm5, ten		;move ten to reg

		pmuludq xmm0, xmm5		;multiply quotient by ten

		psubd xmm1, xmm0		;subtract resulting in the last ints
		
		paddd xmm1, xmm7		;finally turn the final ints to characters

		mov   eax, pString5	;point to string5
		movdqa [eax], xmm1		;push to string5	

	}		
	
//#pragma omp parallel for
	
		output[0] = cString[0];		//puts chars into a string for output
		output[1] = cString2[0];
		output[2] = cString3[0];
		output[3] = cString4[0];
		output[4] = cString5[0];
		output[5] = 32;
		output[6] = cString[4];		//puts chars into a string for output
		output[7] = cString2[4];
		output[8] = cString3[4];
		output[9] = cString4[4];
		output[10] = cString5[4];
		output[11] = 32;	
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