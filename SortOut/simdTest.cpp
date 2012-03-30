#include <emmintrin.h>	//for integer intrinsics
#include<fstream>
#include<stdio.h>

#include <iostream>
using namespace std;

__declspec(align(16)) int idataA[4] = {1,2,3,4};
__declspec(align(16)) int idataB[4] = {48,48,48,48};
__declspec(align(16)) int zeros[4] = {0,0,0,0};

__declspec(align(16)) wchar_t cString[8];



void main(void)
{		
	//__mi128i pointers to data
	__m128i* pidataA = (__m128i*) idataA;
	__m128i* pidataB = (__m128i*) idataB;
	__m128i* pZero = (__m128i*) zeros;
	__m128i* pString = (__m128i*) cString;	
		
	__asm {

		mov eax, pString ;point to char array
		mov ebx, pidataA ; numbers array
		mov ecx, pidataB ; 48 array
		mov edx, pZero;
	
		movdqa xmm1, [ebx] ;numbers
		movdqa xmm2, [ecx] ;48
		movdqa xmm0, [edx] ; contains zeros

		//movdqa xmm3, xmm1
		
		//pcmpeqb xmm1, xmm0 ;

		paddd  xmm1, xmm2 ; add 48 to the numbers
		
		movdqa [eax], xmm1;move the digit characters into cString

	}
	
	//This should output two numbers but we only get one! 

	#pragma omp parallel for
	for(int i = 0; i < 8; i +=2)
	{
		cString[i+1] = 32;
	}

	cout << cString[0];

	FILE * sodata;
	sodata = fopen("lalal.txt", "w");	
	fputws(cString,sodata);	
	
	fclose(sodata);
	
	system("pause");

}