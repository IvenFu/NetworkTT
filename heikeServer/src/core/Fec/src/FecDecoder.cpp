

#include "HikFecInterfaceImpl.h"
#include "FecDecoder.h"
#include "rtpInner.h"


CHikFecDecoder::CHikFecDecoder()
{
	m_pcFecCorrection = NULL;
	m_cbFecBack = NULL;
	m_pUser = NULL;
	m_bTurn = 0;
	m_count = 0;
	m_nMediaSsrc = 0;
}

CHikFecDecoder::~CHikFecDecoder()
{
	SAFE_DEL(m_pcFecCorrection);

	while (!received_packet_list_.empty()) 
	{
		delete received_packet_list_.front();
		received_packet_list_.pop_front();
	}

	while (!recovered_packet_list_.empty())
	{
		delete recovered_packet_list_.front();
		recovered_packet_list_.pop_front();
	}	
}

int CHikFecDecoder::RegisterOutputFecCallBack(HIKFEC_OutputDataCallBack pCb, void *pUser)
{
	m_cbFecBack = pCb;
	m_pUser = pUser;

	return HIK_FEC_LIB_S_OK;
}

//ÿ������һ��RTP����FEC����ý�����Ҫ���RTP��װ
int CHikFecDecoder::DecodeFec(DATA_PACK_VEC *pstDataVec)
{
	RecoveredPacketList::iterator it;
	int nRet = 0;
	int bRecoverFlag = 0;

	if (NULL == pstDataVec)
	{
		return HIK_FEC_LIB_E_PARA_NULL;
	}

	if (NULL == m_pcFecCorrection)
	{
		m_pcFecCorrection = new (std::nothrow)ForwardErrorCorrection;
		if (NULL == m_pcFecCorrection)
		{
			NPQ_ERROR("new ForwardErrorCorrection err");
			return HIK_FEC_LIB_E_MEM_OVER;
		}
	}

	ReceivedPacket* received_packet = new(std::nothrow) ReceivedPacket;	
	if(received_packet == NULL)
	{
		NPQ_ERROR("new received_packet err");
		return HIK_FEC_LIB_E_MEM_OVER;
	}

	received_packet->pkt = new(std::nothrow) Packet;
	if(received_packet->pkt == NULL)
	{
		NPQ_ERROR("new Packet err");
		SAFE_DEL(received_packet);
		return HIK_FEC_LIB_E_MEM_OVER;
	}

	CRtpPacketIn rtpPacket;
	if(0 != rtpPacket.Parse(pstDataVec->pBuf,pstDataVec->nDataLen))
	{
		NPQ_ERROR("fec rtpPacket err");
		SAFE_DEL(received_packet->pkt);
		SAFE_DEL(received_packet);
		return HIK_FEC_LIB_E_FAIL;
	}

	//FEC��ȥ��RTP��װ
	//ý��������������RTP��
	if(rtpPacket.IsFecPacket())
	{	
		received_packet->pkt->length = rtpPacket.m_nRawLen;
		memcpy(&received_packet->pkt->data, rtpPacket.m_pRawData, rtpPacket.m_nRawLen);
		received_packet->is_fec = 1;
	}
	else
	{
		received_packet->pkt->length = pstDataVec->nDataLen;
		memcpy(&received_packet->pkt->data, pstDataVec->pBuf, pstDataVec->nDataLen);
		received_packet->is_fec = 0;
		m_nMediaSsrc = rtpPacket.m_struHeader.nSsrc;
	}
	
	/*
	DDDD FFFF DDDDDD FFFF  ֡����   һ��D��FҪ�������� 

	...
	������������򣬿��������
	*/

	if(m_bTurn != received_packet->is_fec && !received_packet->is_fec)
	{
		//���չ�����һ����һ�ε���Դ�����Ч��
		m_pcFecCorrection->ResetState(&recovered_packet_list_);
	}

	m_bTurn = received_packet->is_fec;

	received_packet->ssrc = received_packet->is_fec? m_nMediaSsrc :rtpPacket.m_struHeader.nSsrc;  //�޸�Fec��ssrcΪý���ssrc����֤�ָ��õ�������ssrcһ��
	received_packet->seq_num = rtpPacket.m_struHeader.sSeqNo;
	received_packet_list_.push_back(received_packet);
	
	nRet = m_pcFecCorrection->DecodeFEC(&received_packet_list_, &recovered_packet_list_);
	if (nRet != 0)
	{
		return HIK_FEC_LIB_E_FAIL;
	}

	// Send any recovered media packets to VCM.
	it = recovered_packet_list_.begin();
	for (; it != recovered_packet_list_.end(); ++it) 
	{
		if ((*it)->returned)  // Already sent to the VCM and the jitter buffer.
		{
			continue;
		}

		Packet* packet = (*it)->pkt;

		//ÿ�λص�һ�����ݰ�
		if (m_cbFecBack)
		{
			DATA_PACK_VEC stVec;
			stVec.pBuf = packet->data;
			stVec.nDataLen = packet->length;
			stVec.pNext = NULL;

			m_cbFecBack(&stVec, m_pUser);
		}
		(*it)->returned = 1;
	}

	return HIK_FEC_LIB_S_OK;

}
