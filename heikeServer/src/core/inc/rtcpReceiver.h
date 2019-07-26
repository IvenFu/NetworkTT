/**	@rtcpReceiver.h
*	@note 111.
*	@brief rtcp 接收端派生类
*
*	@author		222
*	@date		2017/11/02
*
*	@note 
*
*	@warning 
*/
#ifndef _RTCP_RECEIVER_H_
#define _RTCP_RECEIVER_H_

#include "timeUtil.h"
#include "rtcpDefine.h"
#include "rtcp.h"
#include "rtcpCallback.h"

class RtcpCallback;

/**	@class	 RtcpReceiver 
 *	@brief   rtcp 接收端派生类
 *	@note	 
 */
class RtcpReceiver: public RTCP
{
public:
	/*构造函数*/
	RtcpReceiver(OuterParam& struOuterParam,InnerParam& innerParam,RtcpCallback& callback);

	/*析构函数*/
	~RtcpReceiver();
	
	/*报告PLI*/
	int ReportPLI();
	
	/*报告FIR*/
	int ReportFIR();

	/*报告Remb*/
	int ReportRemb(TPR_INT64 llBitratebps);
	
	/*设置ssrc*/
	void AddSsrc(unsigned int uSsrc, TPR_BOOL bRealMedia);
private:
	/*构建RTCP报文*/
	int BuildRtcpBasic(RTCP_OUTPUT_TYPE enOutputType,void* pBuildParam);
	
	/*构建RR报文*/
	int AddRR();

	/*构建RR报文块信息*/
	int AddRRBlock();

	/*构建XR ReciveReferenceTimeReport*/
	int AddXRReciveReferenceTimeReport();

	/*构建XR 块信息*/
	int AddXRBlock(RTCP_XR_BT_TYPE enXRBT);

	/*构建PLI报文*/
	int AddPli();

	/*构建FIR报文*/
	int AddFir();
	
	/*构建Remb报文*/
	int AddRemb(RTCP_SPEC_FB_REMB_INFO* pInfo);

	unsigned int m_uMediaSsrc;			//媒体数据ssrc
	vector<TPR_UINT32> m_aSsrcs;		//接收端收到的所有ssrc列表
	TPR_UINT8 m_bySeqFir;				//FIR序号
};

#endif

