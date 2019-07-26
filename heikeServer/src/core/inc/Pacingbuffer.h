/**	@Pacingbuffer.cpp
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
#ifndef _PACINGBUFFER_H_
#define _PACINGBUFFER_H_
#include <list>
#include "TPR_tpr.h"
#include "TPR_Mutex.h"

using std::list;


#define RTP_BUFFER_LEN			(1500)			//һ��RTP�������С
#define RTP_PKT_NUM             (20)            //RTP���ĸ���

/** @struct    PacedPktBuf
 *  @brief    ���Pacing���ݺͳ���
 */
struct PacedPktBuf
{
    unsigned int   nLen;
    unsigned char* pBuf;
	TPR_BOOL       bUsed;
};

typedef list<PacedPktBuf*> PacedPackets;

/**	@class	 PacingBuffer 
 *	@brief   ������������
 *	@note	 
 */
class PacingBuffer
{
public:
	/*���캯��*/
	PacingBuffer();

	/*��������*/
	~PacingBuffer();

	/*������������һ��RTP��*/
	int InsertPacket( unsigned char* pPacedBuff, unsigned int nPacedLen);

    /*��ȡ���һ�����ݰ�*/
    int FindFittingPacket(unsigned char** pPacedBuff, unsigned int* pnPacedLen, unsigned int nFindLen);

    /*��ʼ��������*/
    int InitBuffer();

    /*����ʼ��������*/
    void FInitBuffer();

private:
    PacedPktBuf     m_szStPacedPktBuf[RTP_PKT_NUM];//���pacing����
	PacedPackets    m_list;				//RTP�б�
	TPR_Mutex       m_lock;				//������
};
#endif