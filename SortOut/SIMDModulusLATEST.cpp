#include <emmintrin.h>	//for integer intrinsics
#include <xmmintrin.h>
#include<fstream>
#include<stdio.h>

#include <iostream>
using namespace std;

#define DIV_10000		0xd1b71759	
#define DIV_100			0x147b	
#define DIV_10			0x199a		

__declspec(align(16)) int idataA[4] = {203,0,14003,0};
__declspec(align(16)) int fortyEight[4] = {48,0,48,0};

__declspec(align(16)) int div10000[4] = {DIV_10000,0,DIV_10000,0};
__declspec(align(16)) int div100[4] = {DIV_100,0,DIV_100,0};
__declspec(align(16)) int div10[4] = {DIV_10,0,DIV_10,0};

__declspec(align(16)) int ten[4] = {10,0,10,0};
__declspec(align(16)) int hund[4] = {100,0,100,0};
__declspec(align(16)) int thou[4] = {1000,0,1000,0};
__declspec(align(16)) int hundThou[4] = {10000,0,10000,0};

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

		mov   eax, pString5		;point to string5
		movdqa [eax], xmm1		;push to string5	

	}		
	
#pragma omp parallel for
	for(int i = 0; i < 2; ++i)
	{
		output[0] += cString[i*4];		//puts chars into a string for output
		output[0] += cString2[i*4];
		output[0] += cString3[i*4];
		output[0] += cString4[i*4];
		output[0] += cString5[i*4];
		output[0] += 32;
	}
	
	FILE * sodata;
	sodata = fopen("lalal.txt", "w");

	fputws(output[0].c_str(),sodata);	
	
	fclose(sodata);
	
	system("pause");

}

