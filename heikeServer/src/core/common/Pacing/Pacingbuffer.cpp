/**	@PacingBuffer.cpp
*	@note 111.
*	@brief Qos PacingBuffer实现
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
*	@brief 构造函数
*	@return 
*/
PacingBuffer::PacingBuffer()
{
    m_list.clear();
    for(int i = 0; i < RTP_PKT_NUM; i++)//初始化
    {
        m_szStPacedPktBuf[i].bUsed = TPR_FALSE;
        m_szStPacedPktBuf[i].nLen  = 0;
        m_szStPacedPktBuf[i].pBuf  = NULL;
    }
}

/**	@fn PacingBuffer::~PacingBuffer()
*	@brief 析构函数
*	@return 
*/
PacingBuffer::~PacingBuffer()
{

}

/**	@fn PacingBuffer::InitBuffer()
*	@brief 初始化缓冲区
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
*	@brief 反初始化缓冲区回收内存
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
*	@brief 往缓冲区插入一个RTP包
*	@param pPacedBuff  Pacing数据缓冲
*	@param nPacedLen  Pacing数据长度
*	@return int 见头文件错误码定义
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
*	@brief 查找合适的数据包
*	@param unsigned char** pPacedBuff  数据包缓冲
*	@param unsigned int* pnPacedLen  指针指向数据包长度
*	@param unsigned int nFindLen 要查找的数据包长度
*	@return 见头文件错误码定义
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

	//查找与nFindLen最接近的数据包
    int nMinDiff = static_cast<int>(pow(2.0,16) - 1);//初始化 字节数差值的最小值
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

    *pPacedBuff = pPacedBuffer->pBuf;//暂不支持多线程  TBD
    *pnPacedLen = pPacedBuffer->nLen;

    return NPQ_OK;
}


