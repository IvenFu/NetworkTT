/**	@RTCP.h
*	@note 111.
*	@brief rtcp 基类
*
*	@author		222
*	@date		2017/11/02
*
*	@note 
*
*	@warning 
*/

#ifndef _RTCP_H_
#define _RTCP_H_

#include "TPR_Time.h"
#include "TPR_Thread.h"
#include "TPR_Semaphore.h"
#include "baseImpl.h"
#include "callback.h"
#include "rtcpDefine.h"
#include "rtcpParser.h"

class RtcpParser;
class RtcpCallback;

/**	@class	 CRtcpPacket 
 *	@brief   RTCP包定义
 *	@note	 
 */
class CRtcpPacket
{
public:
	/*构造函数*/
	CRtcpPacket();

	/*析构函数*/
	~CRtcpPacket();

	/*分析RTCP数据*/
	int Parse(unsigned char* pData,unsigned int nLen);

	unsigned char* m_pData;		//数据首地址
	unsigned int m_nLen;		//RTCP包长度
	TPR_TIME_T m_nArriveTimeUs;	//数据包达到时间 单位us
};

#define BUF_LEN 2048

/**	@class	 RtcpBuffer 
 *	@brief   RTCP缓冲区
 *	@note	 
 */
class RtcpBuffer
{
public:
	/*构造函数*/
	RtcpBuffer();

	/*析构函数*/
	~RtcpBuffer();
	
	/*清除缓冲*/
	void Clear();
	
	/*设置对应位置字节数据*/
	int SetPosDWord( unsigned int dWord, unsigned int Pos);
	
	/*插入指定长度数据*/
	int Enqueue(unsigned char* pData, int iLen);

	/*插入2字节数据*/
	int EnqueueWord(unsigned short sWord);

	/*插入4字节数据*/
	int EnqueueDWord(unsigned int nWord);
	
	/*获取数据地址和长度*/
	int GetValidData(unsigned char** ppData, int* pLen);

private:
	unsigned char m_buf[BUF_LEN];		//缓冲区数组
	unsigned int m_nPos;				//有效数据位置
};

/**	@class	 RTCP 
 *	@brief   RTCP基类定义
 *	@note	 
 */
class RTCP
{
public:
	/*构造函数*/
	RTCP(OuterParam& struOuterParam,InnerParam& innerParam,RtcpCallback& callback);

	/*析构函数*/
	~RTCP();
	
	/*送入数据*/
	int InputData(unsigned char* pData, unsigned int uDataLen);
	
	/*开启内部线程运作*/
	int Start();
	
	/*停止内部线程运作*/
	int Stop();
	
protected:
	/*产生RTCP包*/
	int OutputRtcp(RTCP_OUTPUT_TYPE enOutputType,void* pBuildParam);

	/*构建RTCP包通用报头*/
	int AddCommonHead(RTCP_PT_TYPE enType, int nPacketLength, int numReportingSources);

	RtcpBuffer m_buffer;			//rtcp缓冲区
	const unsigned int m_cOurSsrc;	//rtcp ssrc
	RtcpCallback& m_rtcpCallback;	//rtcp 动作回调
	InnerParam& m_innerParam;		//内部参数

private:
	/*RTCP内部线程*/
	static TPR_VOIDPTR __stdcall RtcpThread( TPR_VOIDPTR pUserData);

	/*RTCP内部线程函数*/
	int RtcpThreadRel();
	
	/*构建RTCP报文*/
	virtual int BuildRtcpBasic(RTCP_OUTPUT_TYPE enOutputType,void* pBuildParam);
	
	/*输出RTCP数据*/
	void Flush();

	TPR_BOOL m_bStart;				//是否开启线程标志
	TPR_HANDLE m_hThread;			//线程句柄
	OuterParam& m_struOuterParam;	//外部参数
	TPR_SEM_T m_hSem;				//信号量
	TPR_Mutex m_lock;				//缓冲区锁
	RtcpParser* m_pRtcpParser;		//rtcp报文解析类
};
#endif

