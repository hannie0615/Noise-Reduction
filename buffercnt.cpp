
#include "stdafx.h"

#include "spenhbuff.h"
#include "enh_const.h"

#include <string.h>
#include <stdio.h>

#include "wavbuffer.h"


// initialization for buffer processing
int initbuff(WAVBUFF *spdata, CAOV *cvdata)
{
	int i, k;	

	// initialization of spdata
	spdata->num_sampling_rate = DEFAULT_SAMPLING_RATE;
	spdata->num_bits_per_sample = 16;
	spdata->bufsize = BUFFSIZE;

	for (i=0; i<2*BUFFSIZE; i++)
	//for (i=0; i<BUFFSIZE; i=i+2)
	{		
		spdata->bufdata[i] = 0;
		spdata->prev_bufdata[i] = 0;	
		spdata->enhdata[i] = 0;		
	}

	for (i=0; i<BUFFSIZE; i++)	
	{	
		spdata->nextenh[i] = 0;		
	}

	// initialization of cvdata
	for(k=0; k < FRSTEP; k++)
	{
		cvdata->xold[k] = 0.0;
	}

	for(k=0; k < FFTSIZE; k++)
	{
		cvdata->noise_ps[k] = 0.0;
		cvdata->prev_ph[k] = 0.5;
		cvdata->prev_specenh[k] = 0.0;
		cvdata->pwsp[k] = 0.0;
		cvdata->pwno[k] = 0.0;
	}

	return NR_SUCCESS;
}



// buffer call
int callnr(SEOPT opt, WAVBUFF *spdata, CAOV *cvdata)
{
	int i;	
	//static int disp;
	int enhstatus = NR_SUCCESS;

	// 잡음처리 없이 32ms delay
	if ((opt.se == 0) && (opt.it == 0))
	{
		for (i=0; i<2*BUFFSIZE; i++)
		{
			spdata->enhdata[i] = spdata->prev_bufdata[i];
			spdata->prev_bufdata[i] = spdata->bufdata[i];
		}
	}
	else
	{
		if (opt.mchginit < NUM_INITBUF)
		{
			// 잡음 추정 초기화
			//printf("Noise Estimation Initialization: %d\n", opt.mchginit);
			enhstatus = doenhinit(opt, spdata, cvdata);
			//disp = 0;
		}
		else
		{
			// 잡음 저감 함수			
			enhstatus = doenhbuf(opt, spdata, cvdata);
		}		
	}	

	return enhstatus;
}







// different code

// read wav
int wave_read(char* filename,FMT& fmt,byte*&data,int& data_size){
	
	FILE* fp_wav = NULL;

	fp_wav = fopen(filename,"rb");
	if ( fp_wav == NULL ){
	}

	/*read file and check if it is 'RIFF'*/
	if ( chunk_code(fp_wav,'R','I','F','F') != NR_SUCCESS ){
	}

	DWORD remaining;
	/*Read remaining*/
	if ( !fread((void*)&remaining,sizeof(DWORD),1,fp_wav) ){
		fclose(fp_wav);
	}

	/*read file and check if it is 'WAVE'*/
	if ( chunk_code(fp_wav,'W','A','V','E') != NR_SUCCESS ){
	}
	remaining -= 4;

	/*read fmt chunk*/
	if ( !fread((void*)&fmt,sizeof(FMT),1,fp_wav) ){
		fclose(fp_wav);
	}
	remaining -= 4;/*for 'fmt '*/
	remaining -= 4;/*for fmt.fmtSIZE*/
	remaining -= fmt.fmtSIZE;/*for WAVEFORM*/

	/*check fmt*/
	if ( fmt.fmtID[0] != 'f' || fmt.fmtID[1] != 'm' || fmt.fmtID[2] != 't' ){
		fclose(fp_wav);
	}

	/*when there is an additional bytes in fmt chunk*/
	if (fmt.fmtSIZE > sizeof(FMT)){
		byte* tmp = new byte[fmt.fmtSIZE-sizeof(WAVEFORM)];
		if ( !fread((void*)tmp,fmt.fmtSIZE-sizeof(WAVEFORM),1,fp_wav) || tmp==NULL ){
			fclose(fp_wav);
			return NR_IO_WAVE_READ_WAV_READ_ERROR;
		}
		if(tmp) delete[] tmp;
	}

	/*check supported fmt*/
	if ( fmt.fmtFORMAT.wFormatTag != 1){/*1 is for PCM*/
		fclose(fp_wav);
		return NR_IO_WAVE_READ_WAV_READ_ERROR;
	}

	/*read file and check if it is 'DATA'*/
	if ( chunk_code(fp_wav,'d','a','t','a') != NR_SUCCESS ){
		return NR_IO_WAVE_READ_WAV_READ_ERROR;
	}
	remaining -= 4;

	/*now read data of data chunk*/ 
	/*check remaining*/
	if ( remaining < 0 ){
		fclose(fp_wav);
		return NR_IO_WAVE_READ_WAV_READ_ERROR;
	}
	
	/*Get lenght of sound data*/
	DWORD length;
	/*Read length*/
	if ( !fread((void*)&length,sizeof(DWORD),1,fp_wav) ){
		fclose(fp_wav);
		return NR_IO_WAVE_READ_WAV_READ_ERROR;
	}

	data = new byte[length];
	if (data == NULL){/*Insufficient memory*/	
		return NR_IO_WAVE_READ_WAV_MEMORY_ASSIGN;
	}

	data_size = fread((void*)data,sizeof(byte),length,fp_wav);

	if ( data_size < 0 ){
		data_size = 0;
		delete data;
		fclose(fp_wav);	
		return NR_IO_WAVE_READ_WAV_MEMORY_ASSIGN;
	}
	
	fclose(fp_wav);
	return NR_SUCCESS;
}




// Read 'abcd'
int chunk_code(FILE* fp_wav,char a,char b,char c,char d){
	char chunk_type[4];
	
	while(1){
		if ( fread((void*)chunk_type,sizeof(byte),1,fp_wav) ){
			if ( a == chunk_type[0] ){
				if ( fread((void*)chunk_type,sizeof(byte),3,fp_wav) ){
					chunk_type[3] = chunk_type[2];
					chunk_type[2] = chunk_type[1];
					chunk_type[1] = chunk_type[0];
					chunk_type[0] = a;
					break;
				}
				else{
					return NR_IO_CHUNK_CODE_FILE_READ;
				}

			}
			else{
				continue;
			}
		}
		else{
			return NR_IO_CHUNK_CODE_FILE_READ;
		}
	}

	/*Check match between read code and 'abcd'*/
	if (chunk_type[0] != a ||	chunk_type[1] != b || chunk_type[2] != c ||	chunk_type[3] != d ){
		return NR_IO_CHUNK_CODE_TYPE_ERR;
	}

	return NR_SUCCESS;
}



// Bit to unsigned short
unsigned short bit2us(int *b)
{
	int i;
	unsigned short us;

	us = 0;
	for (i = 0; i < 16; i++) us += b[i] * 0x8000 >> i; 

	return us;
}


