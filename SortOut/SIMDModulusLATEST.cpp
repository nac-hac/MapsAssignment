#include <emmintrin.h>	//for integer intrinsics
#include <xmmintrin.h>
#include<fstream>
#include<stdio.h>

#include <iostream>
using namespace std;

#define DIV_10000		0xd1b71759	
#define DIV_100			0x147b	
#define DIV_10			0x199a		

__declspec(align(16)) int idataA[4] = {34567,0,45678,0};
__declspec(align(16)) int fortyEight[4] = {48,0,48,0};

__declspec(align(16)) int div10000[4] = {DIV_10000,0,DIV_10000,0};
__declspec(align(16)) int div100[4] = {DIV_100,0,DIV_100,0};
__declspec(align(16)) int div10[4] = {DIV_10,0,DIV_10,0};

__declspec(align(16)) int ten[4] = {10,0,10,0};
__declspec(align(16)) int hund[4] = {100,0,100,0};
__declspec(align(16)) int thou[4] = {10000,0,10000,0};

__declspec(align(16)) wchar_t cString[8];
__declspec(align(16)) wchar_t cString2[8];
__declspec(align(16)) wchar_t cString3[8];
__declspec(align(16)) wchar_t cString4[8];
__declspec(align(16)) wchar_t cString5[8];

std::wstring output[16];

//Hey Nico - The idea is that we're gonna have 5 char arrays.
//			 Array1 contains the first char, Array2 the second etc.
//			 Afterwards we will concatenate the arrays into one big
//			 output string. It'll take a bit longer to get the full
//			 thing working but fingers crossed. Let me know if you 
//			 find any major flaws in this method! 

void main(void)
{		
	//__mi128i pointers to data
	__m128i* pidataA = (__m128i*) idataA;
	
	__m128i* pString = (__m128i*) cString;	
	__m128i* pString2 = (__m128i*) cString2;
	__m128i* pString3 = (__m128i*) cString3;
	__m128i* pString4 = (__m128i*) cString4;
	__m128i* pString5 = (__m128i*) cString5;

	__asm {

		mov eax, pString	;point to char array
		mov ebx, pidataA	;numbers array
						
		movdqa xmm1, [ebx]		;numbers
		movdqa xmm2, xmm1		;copies the numbers
		movdqa xmm3, div10000	;magic num div10000 in xmm reg	
						
		pmuludq	xmm1, xmm3	;multiplies by the DIV_10000 magic number but splits into two 64 bit registers
		psrlq xmm1, 45		;logical shift right leaves us with the first numbers
		
		movdqa xmm4, xmm1	;copies the quotient
		movdqa xmm5, thou	;moves 10000 into xmm5

		movdqa xmm7, fortyEight	;xmm register 
		paddd xmm1, xmm7		;adds 48 to the numbers

		movdqa [eax], xmm1		;pushes the characters into the array
		
		pmuludq xmm4, xmm5		;multiplies the quotient by 10000

		psubd	xmm2, xmm4		;subtracts the quotient from the numbers
		/*

		cmpeqps xmm1, xmm0 ;compares if equal and stores bools in xmm1

		movdqa xmm5, xmm1  ;copies bools into xmm5

		pand xmm1, xmm2	   ;bitwise and the 48s

		paddd xmm3, xmm1   ;adds only to the 0 to make it a char

		pandn xmm5, xmm2   ;bitwise and not the 48s

		paddd xmm3, xmm5   ;makes the remaining ints chars

		movdqa [eax], xmm3	

		mov eax, pString2 ;point to char array

		movdqa [eax], xmm3	
							
		//pcmpeqb xmm1, xmm0 ;packed compare bits - not needed

		//paddd  xmm1, xmm2 ; add 48 to the numbers
		
		//movdqa [eax], xmm1;move the digit characters into cString

		*/

	}		
	
#pragma omp parallel for
	for(int i = 0; i < 2; ++i)
	{
		output[0] += cString[i*4];		//puts chars into a string for output
	}	
	
	FILE * sodata;
	sodata = fopen("lalal.txt", "w");

	fputws(output[0].c_str(),sodata);	
	
	fclose(sodata);
	
	system("pause");

}

