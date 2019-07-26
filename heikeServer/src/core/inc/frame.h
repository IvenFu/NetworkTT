
#ifndef _FRAME_H_
#define _FRAME_H_

#include "rtp.h"

class CFrame
{
public:
	CFrame();
	~CFrame();

	int Length(){return m_iSize;}

	TPR_BOOL bKeyFrame(){return m_bKeyFrame;}

	TPR_BOOL bFrameComplete(){return m_bFrameComplete;}

	TPR_BOOL bConitnue(){return m_bContinue;}

	int InputRtp(CRtpPacket& RtpPacket,TPR_BOOL bConitnue);
	
	void Reset();
private:

	TPR_BOOL m_bKeyFrame;
	TPR_BOOL m_bFrameComplete;
	TPR_BOOL m_bContinue;
	int m_iSize;		//Ö¡³¤¶È
};


class FrameAudioPCM
{
public:
	FrameAudioPCM():m_iDataLen(0){}
	~FrameAudioPCM(){}


	unsigned char m_psData[2048];
	int m_iDataLen;
};


#endif
