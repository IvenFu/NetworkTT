#include "FecEncoder.h"


CHikFecEncoder::CHikFecEncoder(HikFecProtectionParams *p)
{
	m_stParam.aKeyPrtotectionFactor = p->aKeyPrtotectionFactor;
	m_stParam.aDeltaPrtotectionFactor = p->aDeltaPrtotectionFactor;
	m_pcFecCorrection = NULL;
	m_cbFecBack = NULL;
	m_pUser = NULL;
    m_pNode = NULL;
	m_sSeqRecord = 0;
	m_bFirstPacketInGroup = 1;
}

CHikFecEncoder::~CHikFecEncoder()
{
    SAFE_DEL(m_pcFecCorrection);

	while (!media_packets_fec_.empty())
	{
		delete media_packets_fec_.front();
		media_packets_fec_.pop_front();
	}
    
    if(m_pNode)
    {
        SAFE_ARR_DEL(m_pNode->pBuf);
        SAFE_DEL(m_pNode);
    }
}

int CHikFecEncoder::RegisterOutputFecCallBack(void(*OutputDataCallBack)(DATA_PACK_VEC* pstFecList, void *pUser), void *pUser)
{
	m_cbFecBack = OutputDataCallBack;
	m_pUser = pUser;

	return HIK_FEC_LIB_S_OK;
}

//每次送入1个或多个数据包，建议送一个
int CHikFecEncoder::GenerateFEC(DATA_PACK_VEC *pstDataVec,DATA_INFO* pstruInfo)
{
	int nRet = 0;
	
	if (NULL == pstDataVec)
	{
		return HIK_FEC_LIB_E_PARA_NULL;
	}

    if (NULL == m_pNode)
    {
        m_pNode = new DATA_PACK_VEC;
        if (NULL == m_pNode)
        {
            return HIK_FEC_LIB_E_MEM_OVER;
        }

        m_pNode->pBuf = new unsigned char[2048];
        if (NULL == m_pNode->pBuf)
        {
            SAFE_DEL(m_pNode);
            return HIK_FEC_LIB_E_MEM_OVER;
        }
    }

	if (NULL == m_pcFecCorrection)
	{
		m_pcFecCorrection = new(std::nothrow) ForwardErrorCorrection;
		if (NULL == m_pcFecCorrection)
		{
			NPQ_ERROR("GenerateFEC err");
            SAFE_DEL(m_pNode->pBuf);
            SAFE_DEL(m_pNode);
			return HIK_FEC_LIB_E_MEM_OVER;
		}
	}

	DATA_PACK_VEC *pNode = pstDataVec;
	
	while(pNode)
	{
		Packet* packet = new(std::nothrow) Packet;
		if(packet == NULL)
		{
			CleanList(&media_packets_fec_);
			return HIK_FEC_LIB_E_MEM_OVER;
		}

		packet->length = pNode->nDataLen;
		
		if(packet->length > IP_PACKET_SIZE)
		{
			SAFE_DEL(packet);
			CleanList(&media_packets_fec_);

			NPQ_ERROR("GenerateFEC length err %d",packet->length);
			return HIK_FEC_LIB_E_FAIL;
		}
		
		memcpy(packet->data, pNode->pBuf, packet->length);
		media_packets_fec_.push_back(packet);

		//检查序号是否连续
		unsigned short sSeqCur = (packet->data[2]<<8) + (packet->data[3]);
		//第一个包不检查
		if(!m_bFirstPacketInGroup)
		{
			if(!IsContinuSequenceNumberIn(sSeqCur,m_sSeqRecord))
			{
				NPQ_ERROR("seq err %d %d",sSeqCur,m_sSeqRecord);
				CleanList(&media_packets_fec_);
				m_bFirstPacketInGroup = 1;
				return HIK_FEC_LIB_E_ORDER;
			}
		}
		m_bFirstPacketInGroup = 0;
		m_sSeqRecord = sSeqCur;

		pNode = pNode->pNext;
	}
	
	//不够一组数据，先存着
	//按帧产生FEC或者 48个包产生FEC
	if(!pstruInfo->bMarkbit && media_packets_fec_.size()<48)
	{
		return HIK_FEC_LIB_S_OK;
	}

    nRet = CheckData(media_packets_fec_);
	if(-1== nRet)
	{
		CleanList(&media_packets_fec_);
		NPQ_ERROR("GenerateFEC CheckData  error nRet %d",nRet);
		m_bFirstPacketInGroup = 1;
        return HIK_FEC_LIB_E_FAIL;
	}

	HPR_UINT8 factor = 0;
	factor = pstruInfo->bKeyFrame ? m_stParam.aKeyPrtotectionFactor :m_stParam.aDeltaPrtotectionFactor;
	
	//HPR_UINT64 t1= HPR_GetTimeTick64();
	nRet = m_pcFecCorrection->GenerateFEC(media_packets_fec_, factor, 0, 0, kFecMaskRandom, &m_fec_packets_list);
	if (0 != nRet)
	{
		NPQ_ERROR("GenerateFEC error nRet %d",nRet);
		//TBD  是否应该清理内存？
        return HIK_FEC_LIB_E_FAIL;
	}

	DATA_PACK_VEC *pstOutFecList = NULL;

    NPQ_DEBUG("Out FEC start!");
	while (!m_fec_packets_list.empty())
	{
        memset(m_pNode->pBuf, 0, 2048);
        Packet* fecPacket = m_fec_packets_list.front();
		m_pNode->nDataLen = fecPacket->length;
        //m_pNode->pBuf = fecPacket->data;
		memcpy(m_pNode->pBuf, fecPacket->data, fecPacket->length);
		m_pNode->pNext = NULL;

		if (pstOutFecList == NULL)
		{	
			pstOutFecList = m_pNode;
		}
        //回调FEC包
        if (pstOutFecList)
        {
            m_cbFecBack(pstOutFecList, m_pUser);
        }
		m_fec_packets_list.pop_front();//跳向下一个FEC包
	}
    //SAFE_ARR_DEL(pNode1->pBuf);
    //SAFE_DEL(pNode1);

	CleanList(&media_packets_fec_);
	m_bFirstPacketInGroup = 1;
	return HIK_FEC_LIB_S_OK;
}

int CHikFecEncoder::CheckData(const PacketList& m_packList)
{
	Packet* pPack=NULL;
	unsigned short sSeqFirst = 0,sSeqLast = 0;
	
	pPack = m_packList.front();
		
	sSeqFirst = (pPack->data[2]<<8) + pPack->data[3];

	pPack = m_packList.back();
	sSeqLast = (pPack->data[2]<<8) + pPack->data[3];

	//目前最大支持一组管控48个包
	if( (unsigned short)(sSeqLast - sSeqFirst) > 48)
	{
		NPQ_DEBUG("sSeqLast=%d sSeqFirst=%d",sSeqLast, sSeqFirst);
        return -1;
	}

	return 0;
}

int CHikFecEncoder::SetFecProtectionParms( HikFecProtectionParams* pParam )
{
	m_stParam.aKeyPrtotectionFactor = pParam->aKeyPrtotectionFactor;
	m_stParam.aDeltaPrtotectionFactor = pParam->aDeltaPrtotectionFactor;
	return HIK_FEC_LIB_S_OK;
}

void CHikFecEncoder::CleanList( std::list<Packet*>* pList )
{
	Packet* pPacket = NULL;

	if(pList)
	{
		while(!pList->empty())
		{
			pPacket = pList->front();

			if(pPacket)
			{
				delete pPacket;
			}
			pList->pop_front();
		}
	}
}
