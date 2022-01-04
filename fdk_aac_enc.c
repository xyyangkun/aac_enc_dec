/******************************************************************************
 *
 *       Filename:  fdk_aac_enc.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2022年01月04日 10时12分57秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  yangkun (yk)
 *          Email:  xyyangkun@163.com
 *        Company:  yangkun.com
 *
 *****************************************************************************/
/*!
* \file fdk_aac_enc.c
* \brief 使用fdk进行aac编码
* 
* 
* \author yangkun
* \version 1.0
* \date 2022.1.4
*/
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <assert.h>
#include <stdint.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "fdk-aac/aacenc_lib.h"

#include "fdk_aac_enc.h"

typedef struct s_fdk_aac_param
{
	HANDLE_AACENCODER handle;
	CHANNEL_MODE mode;
	AACENC_InfoStruct info;
	int channels;

	uint8_t* input_buf;
	int16_t* convert_buf;

	int input_size ;
	uint8_t outbuf[20480];
}t_fdk_aac_param;


int fdk_create(FDK_HANDLE *hd)
{
	t_fdk_aac_param *param = (t_fdk_aac_param *)malloc(sizeof(t_fdk_aac_param));
	if(param == NULL)
	{
		printf("ERROR in malloc:%s\n", strerror(errno));
		exit(1);
	}
	param->mode = MODE_2;

	int bitrate = 64000;
	int aot = 2;
	int afterburner = 1;
	int eld_sbr = 0;
	int vbr = 0;

	int format = 1;
	int sample_rate = 48000;
	param->channels = 2;
	int bits_per_sample = 16;

	if (aacEncOpen(&param->handle, 0, param->channels) != AACENC_OK) {
		fprintf(stderr, "Unable to open encoder\n");
		return 1;
	}
	if (aacEncoder_SetParam(param->handle, AACENC_AOT, aot) != AACENC_OK) {
		fprintf(stderr, "Unable to set the AOT\n");
		return 1;
	}
	if (aot == 39 && eld_sbr) {
		if (aacEncoder_SetParam(param->handle, AACENC_SBR_MODE, 1) != AACENC_OK) {
			fprintf(stderr, "Unable to set SBR mode for ELD\n");
			return 1;
		}
	}
	if (aacEncoder_SetParam(param->handle, AACENC_SAMPLERATE, sample_rate) != AACENC_OK) {
		fprintf(stderr, "Unable to set the sample rate\n");
		return 1;
	}
	if (aacEncoder_SetParam(param->handle, AACENC_CHANNELMODE, param->mode) != AACENC_OK) {
		fprintf(stderr, "Unable to set the channel mode\n");
		return 1;
	}
	if (aacEncoder_SetParam(param->handle, AACENC_CHANNELORDER, 1) != AACENC_OK) {
		fprintf(stderr, "Unable to set the wav channel order\n");
		return 1;
	}
	if (vbr) {
		if (aacEncoder_SetParam(param->handle, AACENC_BITRATEMODE, vbr) != AACENC_OK) {
			fprintf(stderr, "Unable to set the VBR bitrate mode\n");
			return 1;
		}
	} else {
		if (aacEncoder_SetParam(param->handle, AACENC_BITRATE, bitrate) != AACENC_OK) {
			fprintf(stderr, "Unable to set the bitrate\n");
			return 1;
		}
	}
	if (aacEncoder_SetParam(param->handle, AACENC_TRANSMUX, TT_MP4_ADTS) != AACENC_OK) {
		fprintf(stderr, "Unable to set the ADTS transmux\n");
		return 1;
	}
	if (aacEncoder_SetParam(param->handle, AACENC_AFTERBURNER, afterburner) != AACENC_OK) {
		fprintf(stderr, "Unable to set the afterburner mode\n");
		return 1;
	}
	if (aacEncEncode(param->handle, NULL, NULL, NULL, NULL) != AACENC_OK) {
		fprintf(stderr, "Unable to initialize the encoder\n");
		return 1;
	}
	if (aacEncInfo(param->handle, &param->info) != AACENC_OK) {
		fprintf(stderr, "Unable to get the encoder info\n");
		return 1;
	}
	
	param->input_size = param->channels*2*param->info.frameLength;
	param->input_buf = (uint8_t*) malloc(param->input_size);
	param->convert_buf = (int16_t*) malloc(param->input_size);

	printf("frameLength:%d, input_size:%d\n", param->info.frameLength, param->input_size);

	(*hd) =  (void *)param;

	return 0;
}

int fdk_remove(FDK_HANDLE *hd)
{
	if(*hd == NULL)
	{
		printf("ERROR in fdk_remove!!\n");
		return -1;
	}
	t_fdk_aac_param *param = (t_fdk_aac_param*)(*hd);

	free(param->input_buf);
	free(param->convert_buf);
	aacEncClose(&param->handle);
	free(param);
	param = NULL;
	return 0;
}

int fdk_enc(FDK_HANDLE hd, char *_in_buf, unsigned int in_buf_size, void *_out_buf, unsigned int *out_buf_size)
{
	if(hd == NULL)
	{
		printf("ERROR in fdk_remove!!\n");
		return -1;
	}
	t_fdk_aac_param *param = (t_fdk_aac_param*)hd;

	AACENC_BufDesc in_buf = { 0 }, out_buf = { 0 };
	AACENC_InArgs in_args = { 0 };
	AACENC_OutArgs out_args = { 0 };
	int in_identifier = IN_AUDIO_DATA;
	int in_size, in_elem_size;
	int out_identifier = OUT_BITSTREAM_DATA;
	int out_size, out_elem_size;
	int read, i;

	void *in_ptr, *out_ptr;

	AACENC_ERROR err;

	read = in_buf_size;

	for (i = 0; i < read/2; i++) {
		const uint8_t* in = (const uint8_t *)&_in_buf[2*i];
		param->convert_buf[i] = in[0] | (in[1] << 8);
	}
	in_ptr = param->convert_buf;
	in_size = read;
	in_elem_size = 2;

	in_args.numInSamples = read <= 0 ? -1 : read/2;
	in_buf.numBufs = 1;
	in_buf.bufs = &in_ptr;
	in_buf.bufferIdentifiers = &in_identifier;
	in_buf.bufSizes = &in_size;
	in_buf.bufElSizes = &in_elem_size;

	out_ptr = param->outbuf;
	out_size = sizeof(param->outbuf);
	out_elem_size = 1;
	out_buf.numBufs = 1;
	out_buf.bufs = &out_ptr;
	out_buf.bufferIdentifiers = &out_identifier;
	out_buf.bufSizes = &out_size;
	out_buf.bufElSizes = &out_elem_size;

	if ((err = aacEncEncode(param->handle, &in_buf, &out_buf, &in_args, &out_args)) != AACENC_OK) {
		if (err == AACENC_ENCODE_EOF)
		{
			printf("end encoder \n");
			*out_buf_size = 0;
			return -1;
		}
		fprintf(stderr, "Encoding failed\n");
		return 1;
	}
	if (out_args.numOutBytes == 0)
	{
		*out_buf_size = 0;
		return 0;
	}
	
	memcpy(_out_buf, param->outbuf, out_args.numOutBytes);
	*out_buf_size = out_args.numOutBytes;

	return 0;
}
