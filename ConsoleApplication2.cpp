// ConsoleApplication2.cpp : DLL ���� ���α׷��� ���� ������ �Լ��� �����մϴ�.
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
	
	
	//char *infilename="sp0764_computer_sn0.wav"; //input �����ϸ� ���� �߻�
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


	// ���� ó�� ���
	SEOPT option = {0, 1, 0, 0, 0, 1.0}; // option for speech enhancement	

	printf("-Speech Enhancement Mode Setting (Initial Setting) \n");
	printf("SE OPT: %d\n", option.se);
	printf("NE OPT: %d\n", option.ne);
	printf("WT OPT: %d\n", option.wt);
	printf("IT OPT: %d\n", option.it);
	printf("GE OPT: %f\n", option.ge);


	// ���� ���� ó��
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
	// �켱, ������� ���� ���� ����.
	// ���� �� ���� ��µǴ� ���� Ȯ��.
	// data_size, num_sample, num_sampling_rate(16000 ����� ����), num_channels(1 ����� ����), num_bits_per_sample
	// ���� �������� ������ �ƴ� ����ũ�κ��� �Է��� ���� �޾ƾ� ��. 


	// data���� 32ms�� �������·� �Էµȴٰ� �����ϰ�, 32ms ó���ϵ��� �ڵ�
	int numbuffcall = (int)(floor(double(num_sample-BUFFSIZE)/BUFFSIZE))+1; // num_sample: length of input wav file
	int inx;
	int initcount = 0;
	int curtime = 0;

	for(i=0; i < numbuffcall; i++)
	{
		

		// numbuffcall �� ���Ϸκ��� �д� ������. -> input�� ���Ϸ� �����ؾ��Ѵٴ� ��.
		// �̾ �翬�� output�� ���Ϸ� ����� �Ǿ���Ѵ�. 
		// �����Է�, �������. 
		// ����ũ �Է����� �޴� ���� �� �ڵ�� �̾� ���̷��� �߱� �����ε�, => 16000Hz �������� ������ ���� �ϴ� ����.
		// ��� ���� �� ù 3�� ���ۿ��� ��������

		if (initcount < NUM_INITBUF+1)
		{
			option.mchginit = initcount;
			initcount = initcount+1;
		}		

		inx = i*2*BUFFSIZE; //data�� byte���̰�, ������ 16��Ʈ ���ڵ��̹Ƿ� ���� 1������ 2byte��. ���� (2*BUFFSIZE)��ŭ �о�� 32ms��

		// ���� 32ms ���� �Է�
		for(j=0; j<2*BUFFSIZE; j++)
		{			
			spdata.bufdata[j] = data[inx+j];
		}

		callnr(option, &spdata, &cvdata);

		// ���� ���� ó�� ���ؼ� ���� ��� ���
		// ���� 32ms ó�� ��� enhdata ���
		for(j=0; j<2*BUFFSIZE; j++)
		{			
			enhau[inx+j] = spdata.enhdata[j];
		}
	}
					
	

	return NR_SUCCESS;

}









