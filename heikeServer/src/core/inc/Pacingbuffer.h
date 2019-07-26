/**	@Pacingbuffer.cpp
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
#ifndef _PACINGBUFFER_H_
#define _PACINGBUFFER_H_
#include <list>
#include "TPR_tpr.h"
#include "TPR_Mutex.h"

using std::list;


#define RTP_BUFFER_LEN			(1500)			//一个RTP包缓冲大小
#define RTP_PKT_NUM             (20)            //RTP包的个数

/** @struct    PacedPktBuf
 *  @brief    存放Pacing数据和长度
 */
struct PacedPktBuf
{
    unsigned int   nLen;
    unsigned char* pBuf;
	TPR_BOOL       bUsed;
};

typedef list<PacedPktBuf*> PacedPackets;

/**	@class	 PacingBuffer 
 *	@brief   缓冲区管理类
 *	@note	 
 */
class PacingBuffer
{
public:
	/*构造函数*/
	PacingBuffer();

	/*析构函数*/
	~PacingBuffer();

	/*往缓冲区插入一个RTP包*/
	int InsertPacket( unsigned char* pPacedBuff, unsigned int nPacedLen);

    /*提取最后一个数据包*/
    int FindFittingPacket(unsigned char** pPacedBuff, unsigned int* pnPacedLen, unsigned int nFindLen);

    /*初始化缓冲区*/
    int InitBuffer();

    /*反初始化缓冲区*/
    void FInitBuffer();

private:
    PacedPktBuf     m_szStPacedPktBuf[RTP_PKT_NUM];//存放pacing数据
	PacedPackets    m_list;				//RTP列表
	TPR_Mutex       m_lock;				//保护锁
};
#endif