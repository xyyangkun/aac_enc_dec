/******************************************************************************
 *
 *       Filename:  test_aac_enc.c
 *
 *    Description:  test aac enc
 *
 *        Version:  1.0
 *        Created:  2022年01月04日 14时07分57秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  yangkun (yk)
 *          Email:  xyyangkun@163.com
 *        Company:  yangkun.com
 *
 *****************************************************************************/
/*!
* \file test_aac_enc.c
* \brief 读取wav文件，s16, 48k， 立体声pcm数据，使用fdk进行aac编码, 
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

#include "wavreader.h"

int main(int argc, char **argv)
{
	printf("test\n ");
	FDK_HANDLE hd;
	int ret;

	FILE *out;
	void *wav;
	int format, sample_rate, channels, bits_per_sample;
	unsigned int input_size;
	uint8_t* input_buf;
	const char *infile = "founders.wav";
	const char *outfile = "out.aac";

	int read, i;


	out = fopen(outfile, "wb");
	if (!out) {
		perror(outfile);
		return 1;
	}
	

	wav = wav_read_open(infile);
	if (!wav) {
		fprintf(stderr, "Unable to open wav file %s\n", infile);
		return 1;
	}
	if (!wav_get_header(wav, &format, &channels, &sample_rate, &bits_per_sample, NULL)) {
		fprintf(stderr, "Bad wav file %s\n", infile);
		return 1;
	}
	printf("format:%d, channels:%d sample_rate:%d bits_per_sample:%d\n",
			format, channels, sample_rate, bits_per_sample);
	if (format != 1) {
		fprintf(stderr, "Unsupported WAV format %d\n", format);
		return 1;
	}
	if (bits_per_sample != 16 && sample_rate!= 48000 && channels !=2) {
		fprintf(stderr, "Unsupported WAV sample depth %d\n", bits_per_sample);
		return 1;
	}

	input_size = channels*2*1024;
	input_buf = (uint8_t*) malloc(input_size);

	ret = fdk_create(&hd);
	if(ret != 0)
	{
		printf("ERROR in crate fdk enc\n");
		exit(1);
	}

	uint8_t outbuf[20480];
	unsigned int out_buf_size;
	int count = 0;
	// 循环读取wav文件
	while (1) 
	{
		printf("input_size:%d\n", input_size);
		read = wav_read_data(wav, input_buf, input_size);

		out_buf_size = sizeof(outbuf);
		// 当数据读取完后，还会向编码器传送0字节数据（会传3次，第3次break），
		// 编码器缓存数据进行编码
		ret = fdk_enc(hd, input_buf, read , outbuf, &out_buf_size);
		printf("count=%d, read=%d enc size: out_buf_size:%d, ret=%d\n", count, read, out_buf_size, ret);
		if(ret != 0)
		{
			printf("fdk over\n");
			break;
		}

		count ++;

		fwrite(outbuf, 1, out_buf_size, out);
	}


	ret = fdk_remove(&hd);
	if(ret != 0)
	{
		printf("ERROR in remove fdk enc\n");
		exit(1);
	}

	wav_read_close(wav);

	free(input_buf);
}
