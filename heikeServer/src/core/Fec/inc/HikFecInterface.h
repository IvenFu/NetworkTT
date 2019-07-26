

#ifndef _HIK_FEC_INTTERFACE_H_
#define _HIK_FEC_INTTERFACE_H_

#ifdef __cplusplus
extern "C" {
#endif


#define FEC_PACKET_VIDEO_PAYLOAD_TYPE    (0x75)  //视频FEC包 PT值
#define FEC_PACKET_AUDIO_PAYLOAD_TYPE    (0x76)  //音频FEC包 PT值
#define FEC_PACKET_AUDIO_RED_PAYLOAD_TYPE    (0x7F)  //音频RED包 PT值

/* 状态码：小于零表示有错误(0x80000000开始)，零表示失败，大于零表示成功  */
#define HIK_FEC_LIB_S_OK			0x00000000  /* 成功					    */
#define HIK_FEC_LIB_E_FAIL			0x80000001  /* 失败					    */
#define HIK_FEC_LIB_E_PARA_NULL		0x80000002	/* 参数指针为空			    */
#define HIK_FEC_LIB_E_MEM_OVER		0x80000003	/* 内存溢出				    */
#define HIK_FEC_LIB_E_REPEAT		0x80000004	/* 数据包重复送入		    */
#define HIK_FEC_LIB_E_ORDER		    0x80000005	/* 数据包序号不连续（发送端）*/

/*************************结构体声明*****************************************************/

typedef struct _DATA_INFO
{
	bool bMarkbit;		//是否是markbit标记(如果是一组包，表示最后一个包)
	bool bKeyFrame;		//是否是关键帧标记(音频为false)
}DATA_INFO;

typedef struct _DATA_BASE_VEC
{
	unsigned char*         pBuf;
	unsigned int           nDataLen;
	struct _DATA_BASE_VEC* pNext;
}DATA_PACK_VEC;

typedef struct _HikFecProtectionParams
{
	unsigned char aKeyPrtotectionFactor;	// 关键帧保护因子 1 到 255
	unsigned char aDeltaPrtotectionFactor;	// 非关键帧保护因子 1 到 255
}HikFecProtectionParams;

/******************************************************************************
* 接口函数声明
******************************************************************************/

typedef void(*HIKFEC_OutputDataCallBack)(DATA_PACK_VEC* pstFecList, void *pUser);

/******************************************************************************
* 功  能：获取句柄
* 参  数：无
* 返回值：句柄
******************************************************************************/
void * HIKFEC_CreateHandle(bool bEncoder,HikFecProtectionParams* pEncoderParam);

/******************************************************************************
* 功  能：注册数据回调
* 参  数：handle - 句柄(handle由HIKFEC_CreateHandle返回)
*        HIKFEC_OutputDataCallBack   - 回调函数
pUser                - 用户指针
* 返回值：返回错误码
******************************************************************************/
int HIKFEC_RegisterOutputFecCallBack(void *handle, HIKFEC_OutputDataCallBack pCb, void *pUser);

/******************************************************************************
* 功  能：送入RTP数据
* 参  数：handle - 句柄(handle由HIKFEC_CreateHandle返回)
*        pstDataVec   - 输入的RTP数据包
* 返回值：返回错误码
******************************************************************************/
int HIKFEC_InputData(void* handle, DATA_PACK_VEC *pstDataVec,DATA_INFO* pstruInfo);

/******************************************************************************
* 功  能：送入RTP数据
* 参  数：handle - 句柄(handle由HIKFEC_CreateHandle返回)
*        pEncoderParam   - FEC保护系数
* 返回值：返回错误码
******************************************************************************/
int HIKFEC_SetParam(void* handle,HikFecProtectionParams* pEncoderParam);

/******************************************************************************
* 功  能：销毁句柄
* 参  数：handle - 句柄(handle由HIKFEC_CreateHandle返回)
* 返回值：无
******************************************************************************/
void HIKFEC_Release(void *handle);


#ifdef __cplusplus
}
#endif 


#endif