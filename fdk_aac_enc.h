/******************************************************************************
 *
 *       Filename:  fdk_aac_enc.h
 *
 *    Description: fdk_aac enc
 *
 *        Version:  1.0
 *        Created:  2022年01月04日 09时58分12秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  yangkun (yk)
 *          Email:  xyyangkun@163.com
 *        Company:  yangkun.com
 *
 *****************************************************************************/
/*!
* \file fdk_aac_enc.h
* \brief 使用fdk进行aac编码
* 
* 
* \author yangkun
* \version 1.0
* \date 2022.1.4
*/

#ifndef _FDK_AAC_ENC_H_
#define _FDK_AAC_ENC_H_

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

typedef  void* FDK_HANDLE;

/**
 * @brief 创建声音接收handle, 输入格式s16 48k 立体声
 * @param[in] _cb  音频数据回调
 * @param[in] handle 音频数据回调handle
 * @return 0 success, other failed
 */
int fdk_create(FDK_HANDLE *hd);


/**
 * @brief 产出音频接受handle
 * @return 0 success, other failed
 */
int fdk_remove(FDK_HANDLE *hd);

/**
 * @brief fdk_aac进行编码
 * @param[in] hd fdK handle
 * @param[in] in_buf 需要进行编码的pcm 数据
 * @param[in] in_buf_size 需要编码的pcm数据长度
 * @param[in/out] out_buf 编码后的数据
 * @param[in/out] out_buf_size 编码后的数据长度
 * @return 0 success ,other failed
 */
int fdk_enc(FDK_HANDLE hd, char *in_buf, unsigned int in_buf_size, void *out_buf, unsigned int *out_buf_size);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif//_FDK_AAC_ENC_H_

