/**	@PacingBuffer.cpp
*	@note 111.
*	@brief Qos PacingBufferʵ��
*
*	@author		333
*	@date		2017/08/24
*
*	@note 
*
*	@warning 
*/
#include <math.h>
#include "Util.h"
#include "Pacingbuffer.h"
#include "NPQos.h"
#include "NPQosImpl.h"
#include "TPR_Guard.h"

#define new(x)  new(x,NPQ_NEW_FLAG)

/**	@fn PacingBuffer
*	@brief ���캯��
*	@return 
*/
PacingBuffer::PacingBuffer()
{
    m_list.clear();
    for(int i = 0; i < RTP_PKT_NUM; i++)//��ʼ��
    {
        m_szStPacedPktBuf[i].bUsed = TPR_FALSE;
        m_szStPacedPktBuf[i].nLen  = 0;
        m_szStPacedPktBuf[i].pBuf  = NULL;
    }
}

/**	@fn PacingBuffer::~PacingBuffer()
*	@brief ��������
*	@return 
*/
PacingBuffer::~PacingBuffer()
{

}

/**	@fn PacingBuffer::InitBuffer()
*	@brief ��ʼ��������
*	@return 
*/
int PacingBuffer::InitBuffer()
{
    for(int i = 0; i < RTP_PKT_NUM; i++)
    {
        PacedPktBuf* pPacedBuf = &m_szStPacedPktBuf[i];
        memset(pPacedBuf, 0 , sizeof(PacedPktBuf));
        pPacedBuf->pBuf = new(std::nothrow) unsigned char[RTP_BUFFER_LEN];
        if (NULL == pPacedBuf->pBuf)
        {
            return NPQERR_SYSTEM;
        }
    }
    return NPQ_OK;
}

/**	@fn PacingBuffer::FInitBuffer()
*	@brief ����ʼ�������������ڴ�
*	@return 
*/
void PacingBuffer::FInitBuffer()
{
    for(int i = 0; i < RTP_PKT_NUM; i++)
    {
        PacedPktBuf* pPacedBuf = &m_szStPacedPktBuf[i];
        SAFE_ARR_DEL(pPacedBuf->pBuf);
    }
}

/**	@fn InsertPacket
*	@brief ������������һ��RTP��
*	@param pPacedBuff  Pacing���ݻ���
*	@param nPacedLen  Pacing���ݳ���
*	@return int ��ͷ�ļ������붨��
*/
int PacingBuffer::InsertPacket( unsigned char* pPacedBuff, unsigned int nPacedLen)
{
    if (RTP_BUFFER_LEN < nPacedLen || NULL == pPacedBuff)
    {
        return NPQERR_PARA;
    }

    TPR_Guard guard(&m_lock);
	
	if(m_list.size()<RTP_PKT_NUM)
	{
		PacedPktBuf* pPacedPkt = NULL;
		for(int i = 0; i < RTP_PKT_NUM; i++)
		{
			if (TPR_FALSE == m_szStPacedPktBuf[i].bUsed)
			{
				pPacedPkt = &m_szStPacedPktBuf[i];
				break;
			} 
		}

		if(!pPacedPkt)
		{
			NPQ_ERROR("never seen111");
			return NPQERR_GENRAL;
		}

		memset(pPacedPkt->pBuf, 0 ,RTP_BUFFER_LEN);
		memcpy(pPacedPkt->pBuf, pPacedBuff, nPacedLen);
		pPacedPkt->nLen = nPacedLen;
		pPacedPkt->bUsed = TPR_TRUE;
		m_list.push_back(pPacedPkt);
	}
	else
	{
		PacedPktBuf* pPacedPket = m_list.front();
		m_list.pop_front();
		memset(pPacedPket->pBuf, 0 ,RTP_BUFFER_LEN);
		memcpy(pPacedPket->pBuf, pPacedBuff, nPacedLen);
		pPacedPket->nLen = nPacedLen;
		m_list.push_back(pPacedPket);
	}

    return NPQ_OK;
}

/**	@fn PacingBuffer::FindFittingPacket
*	@brief ���Һ��ʵ����ݰ�
*	@param unsigned char** pPacedBuff  ���ݰ�����
*	@param unsigned int* pnPacedLen  ָ��ָ�����ݰ�����
*	@param unsigned int nFindLen Ҫ���ҵ����ݰ�����
*	@return ��ͷ�ļ������붨��
*/
int PacingBuffer::FindFittingPacket(unsigned char** pPacedBuff, unsigned int* pnPacedLen, unsigned int nFindLen)
{
    TPR_Guard guard(&m_lock);
    if(!pPacedBuff || !pnPacedLen)
    {
        return NPQERR_PARA;
    }

    if(m_list.empty())
    {
        NPQ_ERROR("packet list is empty");
        return NPQERR_I_NOTEXIST;
    }
    PacedPktBuf* pPacedBuffer = NULL;

	//������nFindLen��ӽ������ݰ�
    int nMinDiff = static_cast<int>(pow(2.0,16) - 1);//��ʼ�� �ֽ�����ֵ����Сֵ
    list<PacedPktBuf*>::iterator it = m_list.begin();
    for (; it != m_list.end(); ++it)
    {        
        unsigned int nPacketSize = (*it)->nLen;
        int nDiff = (nPacketSize > nFindLen) ? (nPacketSize - nFindLen) : (nFindLen - nPacketSize);
        if (nDiff < nMinDiff)
        {
            nMinDiff = nDiff;
            pPacedBuffer = (*it);
        }
    }

    if (NULL == pPacedBuffer)
    {
		NPQ_ERROR("FindFittingPacket failed nFindLen=%d,m_list.size=%d",nFindLen,m_list.size());
        return NPQERR_GENRAL;
    }

    *pPacedBuff = pPacedBuffer->pBuf;//�ݲ�֧�ֶ��߳�  TBD
    *pnPacedLen = pPacedBuffer->nLen;

    return NPQ_OK;
}


