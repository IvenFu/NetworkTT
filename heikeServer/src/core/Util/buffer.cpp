/**	@buffer.cpp
*	@note 111.
*	@brief Qos Bufferʵ��
*
*	@author		222
*	@date		2016/10/14
*
*	@note 
*
*	@warning 
*/

#include "Util.h"
#include "buffer.h"
#include "NPQos.h"
#include "NPQosImpl.h"
#include "TPR_Guard.h"
#include "timeUtil.h"
#define new(x)  new(x,NPQ_NEW_FLAG)

/**	@fn DataBuffer::DataBuffer
*	@brief ���캯��
*	@return 
*/
DataBuffer::DataBuffer()
:m_buf(NULL)
,m_pOneRtpbuf(NULL)
,m_nFree(0)
,m_nBufSize(0)
,m_bRtpBufferUsed(TPR_FALSE)
{
	m_list.clear();
}

/**	@fn DataBuffer::~DataBuffer
*	@brief ��������
*	@return 
*/
DataBuffer::~DataBuffer()
{

}

int DataBuffer::ExtenBuffer(int iNewLen)
{
	int iNewBufferLen = 0;

	if(iNewLen<= m_nBufSize)
	{
		return NPQ_OK;
	}

	iNewBufferLen = m_nBufSize;

	while (iNewBufferLen < iNewLen)
	{
		iNewBufferLen += DATA_BUFFER_STEP_LEN;
	}

	if(iNewBufferLen > DATA_BUFFER_MAX_LEN)
	{
		NPQ_ERROR("data buffer len reach max overflow %d iNewLen %d",iNewBufferLen, iNewLen);
		return NPQERR_I_OVERFLOW;
	}

	return ExtenBufferStep(iNewBufferLen);
}

int DataBuffer::ExtenBufferStep(int iNewBufferLen)
{	
	unsigned char* pbufNew = NULL;

	if(iNewBufferLen<= m_nBufSize)
	{
		return NPQ_OK;
	}

	TPR_Guard guard(&m_lock);
	
	pbufNew = (unsigned char*)new(std::nothrow) char[iNewBufferLen];
	if(!pbufNew)
	{
		NPQ_ERROR("new pbufNew err");
		return NPQERR_SYSTEM;
	}

	memcpy(pbufNew,m_buf,m_nBufSize);
	
	list<CRtpPacket>::iterator it = m_list.begin();
	for (; it != m_list.end(); ++it)
	{
		(*it).m_pData = pbufNew + ((*it).m_pData - m_buf);
	}
	
	SAFE_ARR_DEL(m_buf);

	m_buf = pbufNew;
	m_nBufSize = iNewBufferLen;
	return NPQ_OK;
}


/**	@fn DataBuffer::InitBuffer
*	@brief ��ʼ��������
*	@return ��ͷ�ļ������붨��
*/
int DataBuffer::InitBuffer()
{	
	if(!m_buf)
	{
		m_buf = (unsigned char*)new(std::nothrow)char[DATA_BUFFER_INIT_LEN];
		if(!m_buf)
		{
			return NPQERR_SYSTEM;
		}
		m_nBufSize = DATA_BUFFER_INIT_LEN;
	}

	if(!m_pOneRtpbuf)
	{
		m_pOneRtpbuf = (unsigned char*)new(std::nothrow)char[RTP_BUFFER_LEN];
		if(!m_pOneRtpbuf)
		{
			SAFE_DEL(m_buf);
			return NPQERR_SYSTEM;
		}
	}

	ClearBuffer();
	return NPQ_OK;
}

/**	@fn DataBuffer::FiniBuffer
*	@brief ����ʼ��������
*	@return ��ͷ�ļ������붨��
*/
void DataBuffer::FiniBuffer()
{
	TPR_Guard guard(&m_lock);
	ClearBuffer();

	SAFE_ARR_DEL(m_buf);
	m_nBufSize = 0;
	SAFE_ARR_DEL(m_pOneRtpbuf);
	m_bRtpBufferUsed = TPR_FALSE;
}

/**	@fn DataBuffer::ClearBuffer
*	@brief ��ջ�����
*	@return 
*/
void DataBuffer::ClearBuffer()
{
	TPR_Guard guard(&m_lock);
	m_list.clear();
	m_nFree = 0;
}

/**	@fn DataBuffer::IsNoPackets
*	@brief �Ƿ��������ж�
*	@return 1/0
*/
TPR_BOOL DataBuffer::IsNoPackets()
{
	TPR_Guard guard(&m_lock);
	return m_list.empty();
}

int DataBuffer::PacketsNumber()
{
	TPR_Guard guard(&m_lock);
	return m_list.size();
}

/**	@fn DataBuffer::InsertPacketV2
*	@brief ������������һ��RTP��
*	@param CRtpPacket& RtpPacket  RTP����Ϣ
*	@return ��ͷ�ļ������붨��
*/
int DataBuffer::InsertPacketV2(const CRtpPacket& cRtpPacket)
{
	TPR_Guard guard(&m_lock);
	TPR_BOOL bRepeat = TPR_FALSE;
	int iRet;

	CRtpPacket RtpPacket(cRtpPacket);

	unsigned char* pData = RtpPacket.m_pData;
	unsigned int nDataLen = RtpPacket.m_nLen;

	if(!pData || nDataLen == 0)
	{
		NPQ_ERROR("param err nDataLen = %d",nDataLen);
		return NPQERR_PARA;
	}

	if(!m_buf)
	{
		NPQ_ERROR("m_buf err NULL");
		return NPQERR_GENRAL;
	}
	
	if(m_nFree + nDataLen > m_nBufSize)
	{
		//���󻺳���
		iRet = ExtenBuffer(m_nFree + nDataLen);
		if(iRet!=NPQ_OK)
		{
			NPQ_ERROR("ExtenBuffer err");
			return iRet;
		}
	}

	list<CRtpPacket>::iterator it = m_list.begin();
	list<CRtpPacket>::iterator it2;

	unsigned int nPreLen = 0;
	//TBD �����޸ĳɴӺ���ǰ�ң����Ч��
	for (; it != m_list.end(); ++it)
	{
		if(RtpPacket.m_struHeader.sSeqNo == (*it).m_struHeader.sSeqNo)
		{
			bRepeat = TPR_TRUE;
			break;
		}

		if (!IsNewerSequenceNumber(RtpPacket.m_struHeader.sSeqNo, (*it).m_struHeader.sSeqNo))
		{
			break;
		}
		nPreLen += (*it).m_nLen;
	}

	if(bRepeat)
	{
		NPQ_WARN("!!!!!!packet is repeat, seq=%d",RtpPacket.m_struHeader.sSeqNo);
		return NPQERR_I_REPEATE;
	}
	
	//��������ش���
	if(it != m_list.end())
	{
		//RtpPacket.m_bOutOrder = TPR_TRUE;
	}

	it2 = it;
	if((m_nFree - nPreLen) > 0)
	{
		memmove(m_buf+ nPreLen + nDataLen , m_buf+nPreLen , m_nFree - nPreLen);

		//�ƶ�����ڵ�ĵ�ַ
		for (; it != m_list.end(); ++it)
		{
			(*it).m_pData += nDataLen;
			(*it).m_pRawData += nDataLen;
		}
	}
	
	int iRawOffset = RtpPacket.m_pRawData - RtpPacket.m_pData;
	RtpPacket.m_pData =(unsigned char*) m_buf+ nPreLen;
	RtpPacket.m_pRawData = RtpPacket.m_pData+iRawOffset;

	memcpy(m_buf+ nPreLen, pData, nDataLen);
	
	//����������������������
	RtpPacket.m_bNeedFree = TPR_FALSE;
	RtpPacket.m_bHaveCallback = TPR_FALSE;

	m_list.insert(it2,RtpPacket);
	m_nFree+= nDataLen;

	return NPQ_OK;
}

/**	@fn DataBuffer::InsertPacket
*	@brief ������������һ��RTP������Ƶ������������У���Ƶ��ʱ�����������
*	@param CRtpPacket& RtpPacket  RTP����Ϣ
*	@param TPR_BOOL bBySeqNo  �Ƿ�������б�־
*	@return ��ͷ�ļ������붨��
*/
int DataBuffer::InsertPacket(const CRtpPacket& cRtpPacket, TPR_BOOL bBySeqNo)
{
	TPR_Guard guard(&m_lock);
	TPR_BOOL bRepeat = TPR_FALSE;
	int iRet;

	CRtpPacket RtpPacket(cRtpPacket);

	unsigned char* pData = RtpPacket.m_pData;
	unsigned int nDataLen = RtpPacket.m_nLen;

	if(!pData || nDataLen == 0)
	{
		NPQ_ERROR("param err nDataLen = %d",nDataLen);
		return NPQERR_PARA;
	}

	if(!m_buf)
	{
		NPQ_ERROR("m_buf err NULL");
		return NPQERR_GENRAL;
	}

	if(m_nFree + nDataLen > m_nBufSize)
	{
		//���󻺳���
		iRet = ExtenBuffer(m_nFree + nDataLen);
		if(iRet!=NPQ_OK)
		{
			NPQ_ERROR("ExtenBuffer err");
			return iRet;
		}
	}

	list<CRtpPacket>::reverse_iterator it = m_list.rbegin();
	list<CRtpPacket>::iterator it2;

	unsigned int nPreLen = 0;
	unsigned int nPreLen2 = 0;
	
	//֧�ֵ�����룬���Ч��
	if(bBySeqNo)
	{
		for (; it != m_list.rend(); ++it)
		{
			if(RtpPacket.m_struHeader.sSeqNo == (*it).m_struHeader.sSeqNo)
			{
				bRepeat = TPR_TRUE;
				break;
			}

			if (IsNewerSequenceNumber(RtpPacket.m_struHeader.sSeqNo, (*it).m_struHeader.sSeqNo))
			{
				break;
			}
			nPreLen += (*it).m_nLen;
		}
	}
	else
	{
		for (; it != m_list.rend(); ++it)
		{
			if(RtpPacket.m_struHeader.nTimestamp == (*it).m_struHeader.nTimestamp)
			{
				bRepeat = TPR_TRUE;
				break;
			}

			if (IsNewerTimestamp(RtpPacket.m_struHeader.nTimestamp, (*it).m_struHeader.nTimestamp))
			{
				break;
			}
			nPreLen += (*it).m_nLen;
		}
	}

	if(bRepeat || nPreLen> m_nFree)
	{
		NPQ_WARN("!!!!!!packet is repeat %d, RtpPacket.m_struHeader.nTimestamp=%u",bRepeat,RtpPacket.m_struHeader.nTimestamp);
		return NPQERR_I_REPEATE;
	}

	it2 = it.base(); //it2 = it + 1;
	nPreLen2 = m_nFree - nPreLen;
	if(nPreLen > 0)
	{
		memmove(m_buf+ nPreLen2 + nDataLen , m_buf+ nPreLen2 , nPreLen);

		//�ƶ�����ڵ�ĵ�ַ
		for (; it2 != m_list.end(); ++it2)
		{
			(*it2).m_pData += nDataLen;
			(*it2).m_pRawData += nDataLen;
		}
	}
	
	int iRawOffset = RtpPacket.m_pRawData - RtpPacket.m_pData;
	RtpPacket.m_pData =(unsigned char*) m_buf+ nPreLen2;
	RtpPacket.m_pRawData = RtpPacket.m_pData + iRawOffset; 
	memcpy(m_buf+ nPreLen2, pData, nDataLen);

	//����������������������
	RtpPacket.m_bNeedFree = TPR_FALSE;
	RtpPacket.m_bHaveCallback = TPR_FALSE;
	
	m_list.insert(it.base(),RtpPacket);
	m_nFree+= nDataLen;

	return NPQ_OK;
}

/**	@fn DataBuffer::InvalidRtpBuffer
*	@brief ������ʱ��������Ч
*	@return void
*/
void DataBuffer::InvalidRtpBuffer()
{
	m_bRtpBufferUsed = TPR_FALSE;
}

/**	@fn DataBuffer::FrontPacket
*	@brief ��ͷ����ȡһ��RTP��
*	@param CRtpPacket* pRtpPacket  RTP����Ϣ
*	@return ��ͷ�ļ������붨��
*/
int DataBuffer::FrontPacket( CRtpPacket* pRtpPacket )
{
	TPR_Guard guard(&m_lock);
	if(!pRtpPacket)
	{
		return NPQERR_PARA;
	}

	if(m_list.empty())
	{
		return NPQERR_I_NOTEXIST;
	}

	if(m_bRtpBufferUsed || !m_pOneRtpbuf)
	{
		return NPQERR_I_RTPBUFFER;
	}

	CRtpPacket rtpPacket;
	rtpPacket = m_list.front();
	
	memcpy(m_pOneRtpbuf,rtpPacket.m_pData,rtpPacket.m_nLen);

	*pRtpPacket = rtpPacket;
	pRtpPacket->m_pData = m_pOneRtpbuf;

	int iRawOffset = rtpPacket.m_pRawData - rtpPacket.m_pData;
	pRtpPacket->m_pRawData = m_pOneRtpbuf + iRawOffset;
	m_bRtpBufferUsed =TPR_TRUE;

	return NPQ_OK;
}	

/**	@fn DataBuffer::PopFrontPacketNoMove
*	@brief ��ͷ����ȡһ��RTP��,��ɾ���б��¼�����ǲ��ƶ��ڴ�(ע��˽ӿڵ�ʹ��)
*	@param CRtpPacket* pRtpPacket  RTP����Ϣ
*	@return ��ͷ�ļ������붨��
*/
int DataBuffer::PopFrontPacketNoMove( CRtpPacket* pRtpPacket )
{
	int iRet;
	TPR_Guard guard(&m_lock);

	iRet = FrontPacket(pRtpPacket);
	if(iRet !=NPQ_OK)
	{
		return iRet;
	}

	m_list.pop_front();
	return NPQ_OK;
}

/**	@fn DataBuffer::LastPacket
*	@brief ��β����ȡһ��RTP��
*	@param CRtpPacket* pRtpPacket  RTP����Ϣ
*	@return ��ͷ�ļ������붨��
*/
int DataBuffer::LastPacket( CRtpPacket* pRtpPacket )
{
	TPR_Guard guard(&m_lock);
	if(!pRtpPacket)
	{
		return NPQERR_PARA;
	}

	if(m_list.empty())
	{
		NPQ_ERROR("packet list is empty");
		return NPQERR_I_NOTEXIST;
	}

	if(m_bRtpBufferUsed || !m_pOneRtpbuf)
	{
		NPQ_ERROR("m_bRtpBufferUsed err m_bRtpBufferUsed=%d",m_bRtpBufferUsed);
		return NPQERR_I_RTPBUFFER;
	}

	CRtpPacket rtpPacket;
	rtpPacket = m_list.back();


	memcpy(m_pOneRtpbuf,rtpPacket.m_pData,rtpPacket.m_nLen);

	*pRtpPacket = rtpPacket;
	pRtpPacket->m_pData = m_pOneRtpbuf;

	int iRawOffset = rtpPacket.m_pRawData - rtpPacket.m_pData;
	pRtpPacket->m_pRawData = m_pOneRtpbuf + iRawOffset;
	m_bRtpBufferUsed =TPR_TRUE;

	return NPQ_OK;
}

/**	@fn DataBuffer::LastPacketSeq
*	@brief ��β����ȡһ��RTP�����
*	@return ��ͷ�ļ������붨��
*/
int DataBuffer::LastPacketSeq()
{	
	int iRet;
	CRtpPacket temp;

	iRet = LastPacket(&temp);
	if(iRet!=NPQ_OK)
	{
		return -1;
	}

	InvalidRtpBuffer();

	return temp.m_struHeader.sSeqNo;
}

/**	@fn DataBuffer::EreasePacket
*	@brief �������ɾ��һ��RTP��
*	@param TPR_UINT16 sSeq  RTP�����
*	@return ��ͷ�ļ������붨��
*/
int DataBuffer::EreasePacket(TPR_UINT16 sSeq)
{
	TPR_Guard guard(&m_lock);

	unsigned int nPreLen = 0;//������Ҫɾ���İ��ĳ���

	list<CRtpPacket>::iterator it = m_list.begin();
	list<CRtpPacket>::iterator it2;
	for (; it != m_list.end(); ++it)
	{
		nPreLen+=(*it).m_nLen;
		if(sSeq == (*it).m_struHeader.sSeqNo)
		{
			break;
		}
	}

	if(it == m_list.end())
	{
		return NPQERR_I_NOTEXIST;
	}

	it2 = it;
	CRtpPacket& rtpPacket= *it;

	if(rtpPacket.m_pData+rtpPacket.m_nLen > m_buf + m_nFree
		|| nPreLen > m_nFree)
	{
		NPQ_ERROR("EreasePacket err");
		return NPQERR_GENRAL;
	}
	
	if(m_nFree-nPreLen > 0)
	{
		memmove(rtpPacket.m_pData, rtpPacket.m_pData + rtpPacket.m_nLen, m_nFree - nPreLen);
		//�ƶ�����ڵ��ַ
		for(;it!=m_list.end();it++)
		{
			(*it).m_pData-= rtpPacket.m_nLen;
			(*it).m_pRawData-= rtpPacket.m_nLen;
		}
	}
	
	m_nFree -= rtpPacket.m_nLen;
	m_list.erase(it2);

	return NPQ_OK;
}

/**	@fn DataBuffer::EreasePacketsDue
*	@brief ���ݹ���ʱ��ɾ��RTP��
*	@param TPR_TIME_T nTime ʱ��
*	@return ��ͷ�ļ������붨��
*/
int DataBuffer::EreasePacketsDue( TPR_TIME_T nTime )
{
	TPR_BOOL bContinueDue = TPR_TRUE;
	TPR_BOOL bNotFoundDue = TPR_FALSE;
	unsigned int nPreLen = 0;//��Ҫɾ���İ��ĳ���
	unsigned int nMoveLen = 0;
	
	TPR_Guard guard(&m_lock);

	list<CRtpPacket>::iterator it = m_list.begin();
	list<CRtpPacket>::iterator it2 = m_list.end();

	if(m_list.empty())
	{
		return NPQ_OK;
	}

	for (; it != m_list.end(); ++it)
	{
		if((*it).m_nArriveTimeUs >= nTime)
		{
			bNotFoundDue =TPR_TRUE;
			if(it2 == m_list.end())
			{
				it2 = it;//�ٽ��
			}
			nMoveLen+=(*it).m_nLen;
		}
		else
		{
			nPreLen+=(*it).m_nLen;
			if(bNotFoundDue)
			{
				bContinueDue = TPR_FALSE;
			}
		}
	}

	if(!bContinueDue)
	{
		NPQ_ERROR("packets are not continue due err");
		return NPQERR_I_ORDER;
	}

	if(nPreLen+nMoveLen != m_nFree)
	{
		NPQ_ERROR("packets length err");
		return NPQERR_GENRAL;
	}
	
	//ɾ���б�ڵ�
	if(it2!=m_list.begin())
	{
		m_list.erase(m_list.begin(),it2);
	}

	if(nPreLen>0 && nMoveLen>0)
	{
		//�ƶ��ڴ�
		memmove(m_buf,m_buf+nPreLen,nMoveLen);

		//�ƶ�����ڵ��ַ
		for (it = m_list.begin(); it != m_list.end(); ++it)
		{
			(*it).m_pData-= nPreLen;
			(*it).m_pRawData-= nPreLen;
		}
	}

	m_nFree -=nPreLen;
	return NPQ_OK;
}

/**	@fn DataBuffer::EreasePacketsDue
*	@brief ������Ų���RTP��
*	@param TPR_UINT16 sSeq  ���
*	@param CRtpPacket* pRtpPacket  RTP����Ϣ
*	@return ��ͷ�ļ������붨��
*/
int DataBuffer::FindPacket( TPR_UINT16 sSeq,CRtpPacket* pRtpPacket )
{
	TPR_Guard guard(&m_lock);
	if(!pRtpPacket)
	{
		return NPQERR_PARA;
	}
	
	if(m_list.empty())
	{
		NPQ_ERROR("m_list empty err");
		return NPQERR_I_NOTEXIST;
	}

	if(m_bRtpBufferUsed || !m_pOneRtpbuf)
	{
		NPQ_ERROR("m_bRtpBufferUsed err %d",m_bRtpBufferUsed);
		return NPQERR_I_RTPBUFFER;
	}

	list<CRtpPacket>::iterator it = m_list.begin();
	for (; it != m_list.end(); ++it)
	{
		if(sSeq == (*it).m_struHeader.sSeqNo)
		{
			break;
		}
	}

	if(it == m_list.end())
	{
		NPQ_ERROR("sSeq not found err,sSeq=%d",sSeq);
		return NPQERR_I_NOTEXIST;
	}
	
	memcpy(m_pOneRtpbuf,(*it).m_pData,(*it).m_nLen);

	*pRtpPacket = *it;
	pRtpPacket->m_pData = m_pOneRtpbuf;

	int iRawOffset = (*it).m_pRawData - (*it).m_pData;
	pRtpPacket->m_pRawData = m_pOneRtpbuf + iRawOffset;

	m_bRtpBufferUsed =TPR_TRUE;
	return NPQ_OK;
}


FrameBuffer::FrameBuffer(OuterParam& struOuterParam)
:m_nTimestamp(0)
,m_bKeyFrame(TPR_FALSE)
,m_iPayLoadFrequence(0)
,m_frameStat(FRAME_EMPTY)
,m_bBufferInit(TPR_FALSE)
,m_struOuterParam(struOuterParam)
,DataBuffer()
,m_nNackCount(0)
,m_nFrameLen(0)
,m_nlatestPacketTimeMs(0)
{

}

FrameBuffer::~FrameBuffer()
{
	FiniBuffer();
}

int FrameBuffer::InsertPacketToFrame( CRtpPacket& RtpPacket)
{
	int iRet;
	
	//�����ڴ�
	if(!m_bBufferInit)
	{
		InitBuffer();
		m_bBufferInit = TPR_TRUE;
	}

	if(m_frameStat == FRAME_EMPTY)
	{
		m_nTimestamp = RtpPacket.m_struHeader.nTimestamp;
		m_iPayLoadFrequence = RtpPacket.m_struHeader.iPayLoadFrequence;
		
		if(RtpPacket.m_struHeader.bMarkerBit)
		{
			m_frameStat = FRAME_DECODEABLE;
		}
		else
		{
			m_frameStat = FRAME_INCOMPLETE;
		}
	}
	else
	{
		if(RtpPacket.m_struHeader.bMarkerBit)
		{
			m_frameStat = FRAME_DECODEABLE;
		}
		else if(m_nTimestamp!= RtpPacket.m_struHeader.nTimestamp)
		{
			m_frameStat = FRAME_DECODEABLE;
			return NPQERR_I_NEWFRAME;
		}
	}

	m_bKeyFrame = RtpPacket.m_unioCodecInfo.struVideo.bKeyFrame;
	
	//padding��Ҳ�ᱻͳ�ƣ���Ҫ�Ż���TBD
	if(RtpPacket.IsRetransPacket())
	{
		m_nNackCount++;
	}

	iRet = InsertPacket(RtpPacket,TPR_TRUE);
	if(iRet!=NPQ_OK)
	{
		NPQ_ERROR("InsertPacket err iRet=%x",iRet);
		return iRet;
	}

	m_nFrameLen += RtpPacket.m_nLen;
	m_nlatestPacketTimeMs = OneTime::OneNowMs();

	return NPQ_OK;
}

int FrameBuffer::OutputFrame()
{
	int iRet;
	CRtpPacket temp;

	if(m_frameStat != FRAME_DECODEABLE)
	{
		return NPQERR_GENRAL;
	}

	while(!IsNoPackets())
	{
		iRet = PopFrontPacketNoMove(&temp);
		if(iRet!=NPQ_OK)
		{
			break;
		}

		//NPQ_INFO("output data seq=%d",temp.m_struHeader.sSeqNo);
		m_struOuterParam.DoCallback(temp);

		InvalidRtpBuffer();
	}
	
	//����ڴ沢���ò����������ڴ���Ȼ���ڣ���δ�ͷ�
	ClearBuffer();
	m_frameStat = FRAME_EMPTY;
	m_nTimestamp = 0;
	m_bKeyFrame = TPR_FALSE;
	m_iPayLoadFrequence = 0;
	m_nNackCount =0;
	m_nFrameLen = 0;
	m_nlatestPacketTimeMs = 0;
	return NPQ_OK;
}

FrameList::FrameList()
{
	m_list.clear();
}

FrameList::~FrameList()
{
	m_list.clear();
}

int FrameList::InsertFrame(FrameBuffer* pFrame)
{
	if(!pFrame)
	{
		NPQ_ERROR("pFrame NULL");
		return NPQERR_PARA;
	}
	
	TPR_Guard guard(&m_lock);
	m_list.push_back(pFrame);
	return NPQ_OK;
}

FrameBuffer* FrameList::PopFrame()
{
	TPR_Guard guard(&m_lock);

	if(m_list.empty())
	{
		NPQ_ERROR("FrameList empty");
		return NULL;
	}

	FrameMap::iterator it = m_list.begin();
	if (it == m_list.end())
	{
		NPQ_ERROR("FrameList empty 2");
		return NULL;
	}

	FrameBuffer* frame = *it;
	m_list.pop_front();
	return frame;
}


TPR_INT16 FrameBuffer::GetNackCount()
{
    return m_nNackCount;
}
