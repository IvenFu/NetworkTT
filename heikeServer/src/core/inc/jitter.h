/**	@jitter.h
*	@note 111.
*	@brief jitterbuffer类的定义
*
*	@author		222
*	@date		2017/2/21
*	@note 创建

*	@author		333
*	@date		2017/9/27
*	@note 优化jitter估计算法
*
*	@warning 
*/

#ifndef _JITTER_H_
#define _JITTER_H_

#include "TPR_Thread.h"
#include "TPR_Semaphore.h"
#include "buffer.h"
#include "jitterEstimate.h"
#include "Vcmjitter.h"
#include "rtcpReceiver.h"
#include "pli.h"

class Pli;

/**	@class	 CJitterBuffer 
 *	@brief   抖动缓冲类定义
 *	@note	 
 */
class CJitterBuffer
{
public:
	/*构造函数*/
    CJitterBuffer(OuterParam& struOuterParam,InnerParam& innerParam);

    /*析构函数*/
	~CJitterBuffer();

    /*缓冲区插入数据包*/
	int InsertPacket(CRtpPacket& RtpPacket);
	
	/*初始化类*/
	int Init();

	/*反初始化类*/
	int Fini();

	/*开启相关线程*/
    int Start();

	/*停止相关线程*/
    int Stop();

	/*获取抖动延时*/
    int GetjitterDelay();//增加两个接口用于同步

	/*设置抖动缓冲区最小值*/
    int SetMinDelay(unsigned int nDelay);

	/*设置播放速度，用于倍速回放*/
	int SetScale(float fScale);
private:

    /*数据帧输出线程*/
	static TPR_VOIDPTR __stdcall OutThread( TPR_VOIDPTR pUserData);

    /*数据帧输出线程函数*/
	int OutThreadRel();

	//int ExtractFrame();

	/*清楚缓冲区*/
    void CleanBuffer();

	FrameBuffer*    m_pCurFrame;			  //当前正在写的帧
	FrameBuffer*    m_pRecycle;				  //回收的一帧内存
	FrameList       m_decodableList;		  //已完成的帧缓冲
	OuterParam&     m_struOuterParam;         //外部参数
    InnerParam&     m_innerParam;		      //内部参数

	TPR_HANDLE      m_hThread;                //数据帧输出线程句柄
	TPR_BOOL        m_bStart;                 //是否开启标记

	TPR_SEM_T       m_hSem;                   //完整帧信号
	unsigned int    m_nMinDelay;              //最小抖动延时

    VCMJitterSample m_stWaitingForCompletion; //不完整帧估计
    VCMJitterBuffer m_jitterBuffer;           //抖动缓冲区
    VCMTiming       m_VcmTime;                //时延估计类
    TPR_INT64       m_nLastFrameArrive;       //debug 上一帧到达时间
    TPR_INT64       m_nLastFrameOut;          //debug上一帧输出时间
    TPR_BOOL        m_bFirstPacket;           //是否是第一个数据包
	float           m_fScale;				  //播放倍速，用于倍速回放
};


#endif

