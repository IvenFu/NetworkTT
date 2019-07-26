/**	@callback.cpp
*	@note 111.
*	@brief ���ݻص�
*
*	@author		222
*	@date		2017/1/24
*
*	@note 
*
*	@warning 
*/
#include "TPR_Guard.h"
#include "Util.h"
#include "timeUtil.h"
#include "callback.h"

#define new(x)  new(x,NPQ_NEW_FLAG)

/**	@fn OuterParam:~OuterParam
*	@brief ��������
*	@return void
*/
OuterParam::~OuterParam()
{
	SAFE_DEL(m_pReceiverCtrl);
	SAFE_DEL(m_pSenderCtrl);
}


//FILE* g_debugFile = fopen("video_len.rtp","wb+");

/**	@fn OuterParam:DoCallback
*	@brief ���ûص�����
*	@param  CRtpPacket& rtpPacket   rtp���ݰ�
*	@return ��ȷ����NPQ_OK������������붨��
*/
int OuterParam::DoCallback(CRtpPacket& rtpPacket)
{
	if(!m_bInit)
	{
		if(NPQ_OK == InitCtrl())
		{
			m_bInit = TPR_TRUE;
		}
	}

	if(rtpPacket.m_bHaveCallback)
	{
		//NPQ_WARN("packet have already callback seq=%d",rtpPacket.m_struHeader.sSeqNo);
		return NPQ_OK;
	}

	TPR_Guard gurad(&m_lock);

	if(enType == NPQ_QOS_SENDER)
	{
		int iDataType = rtpPacket.CovertType(rtpPacket.m_enType);
		DoCallback(iDataType ,rtpPacket.m_pData,rtpPacket.m_nLen);
		rtpPacket.m_bHaveCallback = TPR_TRUE;

		//����ͳ��
		m_pSenderCtrl->UpdateRateStatistics(rtpPacket,OneTime::OneNowMs());
		return NPQ_OK;
	}
	else
	{
		if(!rtpPacket.m_bHaveCallback)
		{
			int iDataType = rtpPacket.CovertType(rtpPacket.m_enType);
			DoCallback(iDataType ,rtpPacket.m_pData,rtpPacket.m_nLen);
			
			//debug ��֡���һ��RTP������֡��ʱ
			if(rtpPacket.m_struHeader.bMarkerBit)
			{
				//NPQ_DEBUG("[key]frameDelayUutR=%lld",TPR_TimeNow()-rtpPacket.m_nArriveTimeUs);
			}
			
			rtpPacket.m_bHaveCallback = TPR_TRUE;

			m_pReceiverCtrl->UpdateVideoStatistics(rtpPacket);
			//if(rtpPacket.m_enType == RTP_VIDEO || rtpPacket.m_enType == RTP_PRIVATE)
			//{
				//fwrite(&rtpPacket.m_nLen,1,4,g_debugFile);
				//fwrite(rtpPacket.m_pData,1,rtpPacket.m_nLen,g_debugFile);
			//}
		}
	}

	return NPQERR_GENRAL;
}

/**	@fn OuterParam:DoCallback
*	@brief ���ûص�����
*	@param  int iDataType  ��������
*	@param  unsigned char* pData  ���ݵ�ַ
*	@param  unsigned int nDataLen  ���ݳ���
*	@return ��ȷ����NPQ_OK������������붨��
*/
int OuterParam::DoCallback(int iDataType, unsigned char* pData, unsigned int nDataLen)
{
	TPR_Guard gurad(&m_lock);
	
	if(pCallback)
	{
		pCallback(id, iDataType ,pData,nDataLen,pUser);
		return NPQ_OK;
	}
	return NPQERR_GENRAL;
}

/**	@fn OuterParam:DoCallback
*	@brief ���ûص�����
*	@param  NPQ_CMD& cmd   ������Ϣ
*	@return ��ȷ����NPQ_OK������������붨��
*/
int OuterParam::DoCallback(NPQ_CMD& cmd)
{
	//����ص�����Ҫ������һ�����

	if(pCallback)
	{
		pCallback(id, NPQ_DATA_CMD ,(unsigned char*)&cmd,sizeof(NPQ_CMD),pUser);
		return NPQ_OK;
	}
	return NPQERR_GENRAL;
}

/**	@fn OuterParam:GetRate
*	@brief ��ȡ����
*	@param TPR_UINT32* pVideo     ��Ƶ��������
*	@param TPR_UINT32* pTetrans	  �ش���������
*	@param TPR_UINT32* pFec       FEC��������
*	@param  TPR_INT64 nowMs  ��ǰʱ��
*	@return void
*/
void OuterParam::GetRate(TPR_UINT32* pVideo, TPR_UINT32* pTetrans,TPR_UINT32* pFec,TPR_UINT32* pPadding,TPR_INT64 nowMs)
{
	if(m_pSenderCtrl)
	{
		m_pSenderCtrl->GetRate(pVideo,pTetrans,pFec,pPadding,nowMs);
	}
}

unsigned char OuterParam::GetVideoLossRate()
{
	if(m_pReceiverCtrl)
	{
		return m_pReceiverCtrl->GetVideoLossRate();
	}
	else
	{
		return 0;
	}
}

TPR_UINT32 OuterParam::GetVideoBitrate()
{
	if(m_pReceiverCtrl)
	{
		return m_pReceiverCtrl->GetVideoBitrate();
	}
	else
	{
		return 0;
	}
}


int OuterParam::InitCtrl()
{
	int iRet;

	if(enType == NPQ_QOS_SENDER)
	{
		m_pSenderCtrl = new (std::nothrow)OuterCtrlSender(*this);
		if(m_pSenderCtrl == NULL)
		{
			return NPQERR_SYSTEM;
		}

		iRet = m_pSenderCtrl->Init();
		if(iRet!=NPQ_OK)
		{
			SAFE_DEL(m_pSenderCtrl);
			return NPQERR_SYSTEM;
		}
	}
	else
	{
		m_pReceiverCtrl = new (std::nothrow)OuterCtrlReceiver(*this);
		if(m_pReceiverCtrl == NULL)
		{
			return NPQERR_SYSTEM;
		}
	}
	return NPQ_OK;
}


