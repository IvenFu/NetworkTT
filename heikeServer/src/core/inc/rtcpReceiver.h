/**	@rtcpReceiver.h
*	@note 111.
*	@brief rtcp ���ն�������
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
 *	@brief   rtcp ���ն�������
 *	@note	 
 */
class RtcpReceiver: public RTCP
{
public:
	/*���캯��*/
	RtcpReceiver(OuterParam& struOuterParam,InnerParam& innerParam,RtcpCallback& callback);

	/*��������*/
	~RtcpReceiver();
	
	/*����PLI*/
	int ReportPLI();
	
	/*����FIR*/
	int ReportFIR();

	/*����Remb*/
	int ReportRemb(TPR_INT64 llBitratebps);
	
	/*����ssrc*/
	void AddSsrc(unsigned int uSsrc, TPR_BOOL bRealMedia);
private:
	/*����RTCP����*/
	int BuildRtcpBasic(RTCP_OUTPUT_TYPE enOutputType,void* pBuildParam);
	
	/*����RR����*/
	int AddRR();

	/*����RR���Ŀ���Ϣ*/
	int AddRRBlock();

	/*����XR ReciveReferenceTimeReport*/
	int AddXRReciveReferenceTimeReport();

	/*����XR ����Ϣ*/
	int AddXRBlock(RTCP_XR_BT_TYPE enXRBT);

	/*����PLI����*/
	int AddPli();

	/*����FIR����*/
	int AddFir();
	
	/*����Remb����*/
	int AddRemb(RTCP_SPEC_FB_REMB_INFO* pInfo);

	unsigned int m_uMediaSsrc;			//ý������ssrc
	vector<TPR_UINT32> m_aSsrcs;		//���ն��յ�������ssrc�б�
	TPR_UINT8 m_bySeqFir;				//FIR���
};

#endif

