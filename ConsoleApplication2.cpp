// ConsoleApplication2.cpp : DLL 응용 프로그램을 위해 내보낸 함수를 정의합니다.
//

#include "stdafx.h"
#include "ConsoleApplication2.h"

#include <limits>
#include <list>
#include <fstream>
#include <iostream>
#include <sstream>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <windows.h>
#include <string>

using namespace std;

#include "spenhbuff.h"
#include "buffercnt.h"
#include "enh_const.h"
#include "bessel.h"
#include "wavbuffer.h"

// Main routine
// int main(int argc, char **argv)



int CONSOLEAPPLICATION2_API _stdcall Func(char *infilename,int data_size, int num_bits_per_sample){
	
	
	//char *infilename="sp0764_computer_sn0.wav"; //input 생략하면 에러 발생
	char *outfilename="sp0764_computer_sn0_enh.wav";
	
	FILE *fpinfile  = NULL;
	FILE *fpoutfile = NULL;

	
	/*variables*/
	int i=0,j=0;
	// clock_t start, end;

	/*wave file format*/
	// FMT fmt; 
	byte* data = NULL;
	// int data_size = 279794; /*data size*/
	
	/*
	int num_channels = fmt.fmtFORMAT.nChannels;
	int num_sampling_rate = fmt.fmtFORMAT.nSamplesPerSec;
	int num_bits_per_sample = fmt.fmtFORMAT.wBitsPerSample;
	int num_bytes_per_sample = num_bits_per_sample/8;
	int numerator = (int)pow((double)2.0,(double)fmt.fmtFORMAT.wBitsPerSample-1.0);
	
	int num_sample = data_size/(num_bytes_per_sample*num_channels);
	*/

	int num_channels = 1;
	int num_sampling_rate = 16000;
	//int num_bits_per_sample = 16;
	int num_bytes_per_sample = num_bits_per_sample/8;
	int numerator = (int)pow((double)2.0,(double)num_bits_per_sample-1.0);
	int num_sample = data_size/(num_bytes_per_sample*num_channels);


	// Sampling Rate Check (only 16000 Hz)
	int sample_freq = num_sampling_rate;
	if ((sample_freq != DEFAULT_SAMPLING_RATE) || (num_channels != 1) || (num_bytes_per_sample !=2))
	{
		cout << "Check sampling rate of the input audio. Only Work for 16000Hz Mono 16bit Audio." << endl;
		return NR_FAILURE;
	}



	printf("data_size: %d\n", data_size);
	printf("num_sample: %d\n", num_sample);
	printf("num_sampling_rate: %d\n", num_sampling_rate);
	printf("num_channel: %d\n", num_channels);
	printf("num_bits_per_sample: %d\n", num_bits_per_sample);


	// 잡음 처리 모드
	SEOPT option = {0, 1, 0, 0, 0, 1.0}; // option for speech enhancement	

	printf("-Speech Enhancement Mode Setting (Initial Setting) \n");
	printf("SE OPT: %d\n", option.se);
	printf("NE OPT: %d\n", option.ne);
	printf("WT OPT: %d\n", option.wt);
	printf("IT OPT: %d\n", option.it);
	printf("GE OPT: %f\n", option.ge);


	// 버퍼 변수 처리
	WAVBUFF spdata;
	CAOV cvdata;

	initbuff(&spdata, &cvdata);


	// for output
	byte* enhau;
	enhau = new byte[data_size];


	// initialized to zero
	for(j=0; j<data_size; j++)
	{			
		enhau[j] = 0;
	}
	

	// THE END
	// 우선, 여기까지 빌드 오류 없음.
	// 실행 시 전부 출력되는 것을 확인.
	// data_size, num_sample, num_sampling_rate(16000 상수로 고정), num_channels(1 상수로 고정), num_bits_per_sample
	// 위의 변수들은 파일이 아닌 마이크로부터 입력을 따로 받아야 함. 


	// data에서 32ms씩 버퍼형태로 입력된다고 가정하고, 32ms 처리하도록 코딩
	int numbuffcall = (int)(floor(double(num_sample-BUFFSIZE)/BUFFSIZE))+1; // num_sample: length of input wav file
	int inx;
	int initcount = 0;
	int curtime = 0;

	for(i=0; i < numbuffcall; i++)
	{
		

		// numbuffcall 은 파일로부터 읽는 변수임. -> input이 파일로 존재해야한다는 뜻.
		// 이어서 당연히 output도 파일로 출력이 되어야한다. 
		// 파일입력, 파일출력. 
		// 마이크 입력으로 받는 것은 전 코드와 이어 붙이려고 했기 때문인데, => 16000Hz 문제에서 에러가 나서 일단 보류.
		// 모드 변경 후 첫 3개 버퍼에서 잡음추정

		if (initcount < NUM_INITBUF+1)
		{
			option.mchginit = initcount;
			initcount = initcount+1;
		}		

		inx = i*2*BUFFSIZE; //data는 byte형이고, 음성은 16비트 인코딩이므로 음성 1샘플은 2byte임. 따라서 (2*BUFFSIZE)만큼 읽어야 32ms임

		// 현재 32ms 버퍼 입력
		for(j=0; j<2*BUFFSIZE; j++)
		{			
			spdata.bufdata[j] = data[inx+j];
		}

		callnr(option, &spdata, &cvdata);

		// 끊김 없는 처리 위해서 이전 결과 출력
		// 이전 32ms 처리 결과 enhdata 출력
		for(j=0; j<2*BUFFSIZE; j++)
		{			
			enhau[inx+j] = spdata.enhdata[j];
		}
	}
					
	

	return NR_SUCCESS;

}









