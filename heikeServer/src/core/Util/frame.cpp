
#include "frame.h"



CFrame::CFrame()
{
	Reset();
}

CFrame::~CFrame()
{
}

int CFrame::InputRtp(CRtpPacket& RtpPacket,TPR_BOOL bContinue)
{
	if(m_bFrameComplete)
	{
		return NPQERR_GENRAL;
	}
	
	//���marbit����ʧ����֡�ϲ�Ϊһ֡
	//���֡�ĵ�һ������ʧ bContinue ΪTPR_FASLE
	//����м����ʧ��bContinueΪTPR_FALSE

	if(!bContinue)
	{
		m_bContinue = TPR_FALSE;
	}

	m_iSize+= RtpPacket.m_nRawLen;

	if(RtpPacket.m_struHeader.bMarkerBit)
	{
		m_bKeyFrame = RtpPacket.m_unioCodecInfo.struVideo.bKeyFrame;
		m_bFrameComplete = TPR_TRUE;
	}

	return NPQ_OK;
}

void CFrame::Reset()
{
	m_bKeyFrame = TPR_FALSE;
	m_bFrameComplete = TPR_FALSE;
	m_iSize = 0;
	m_bContinue = TPR_TRUE;
}



