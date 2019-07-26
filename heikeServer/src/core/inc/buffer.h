/**	@buffer.h
*	@note 111.
*	@brief Qos Buffer ��ض���
*
*	@author		222
*	@date		2016/10/14
*
*	@note 
*
*	@warning 
*/

#ifndef _BUFFER_H_
#define _BUFFER_H_
#include <list>
#include <map>
#include "TPR_tpr.h"
#include "TPR_Mutex.h"
#include "rtp.h"
#include "NPQosImpl.h"

using std::list;
using std::map;
using std::pair;
  
#define DATA_BUFFER_INIT_LEN    (2048)			//��ʼ����������С
#define DATA_BUFFER_STEP_LEN    (128*1024)		//��������������
#define DATA_BUFFER_MAX_LEN     (8*1024*1024)   //��󻺳�����С
#define RTP_BUFFER_LEN			(1500)			//һ��RTP�������С

/**	@class	 DataBuffer 
 *	@brief   ������������
 *	@note	 
 */
class DataBuffer
{
public:
	/*���캯��*/
	DataBuffer();

	/*��������*/
	virtual ~DataBuffer();
	
	/*��ʼ���ڴ�*/
	int InitBuffer();

	/*��ջ�����*/
	void ClearBuffer();
	
	/*�ͷ��ڴ�*/
	void FiniBuffer();
	
	/*���ݰ������Ƿ�Ϊ0*/
	TPR_BOOL IsNoPackets();

	int PacketsNumber();
	
	/*������������һ��RTP��*/
	int InsertPacket(const CRtpPacket& cRtpPacket, TPR_BOOL bBySeqNo);
	int InsertPacketV2(const CRtpPacket& cRtpPacket);

	/*���б�ͷ����ȡһ��RTP��*/
	int FrontPacket(CRtpPacket* pRtpPacket);

	/*���б�ͷ����ȡһ��RTP����ɾ���б��¼�����ǲ��ƶ��ڴ棬���ã�*/
	int PopFrontPacketNoMove(CRtpPacket* pRtpPacket);
	
	/*���б�β����ȡһ��RTP��*/
	int LastPacket(CRtpPacket* pRtpPacket);
	
	/*��ȡ�б����RTP�������ֵ*/
	int LastPacketSeq();
	
	/*�������ɾ��RTP��*/
	int EreasePacket(TPR_UINT16 sSeq);
	
	/*���ݹ���ʱ��ɾ��RTP��*/
	int EreasePacketsDue(TPR_TIME_T nTime);
	
	/*������Ų���RTP��*/
	int FindPacket(TPR_UINT16 sSeq,CRtpPacket* pRtpPacket);
	
	/*����RTP��ʱ��������Ч*/
	void InvalidRtpBuffer();
private:

	int ExtenBuffer(int iNewLen);

	int ExtenBufferStep(int iNewBufferLen);

	unsigned char* m_buf;			//�������׵�ַ
	unsigned char* m_pOneRtpbuf;	//RTP��ʱ�������׵�ַ
	TPR_BOOL m_bRtpBufferUsed;		//��ʱ�������Ƿ���Ч��־
	CRtpPackets m_list;				//RTP�б�
	unsigned int m_nFree;			//���õ�ַƫ��ֵ
	unsigned int m_nBufSize;        //��������С
	TPR_Mutex m_lock;				//������
};

class FrameBuffer: private DataBuffer
{
public:
	enum FrameStat
	{
		FRAME_EMPTY,
		FRAME_INCOMPLETE,
		FRAME_DECODEABLE,
	};

	FrameBuffer(OuterParam& struOuterParam);
	~FrameBuffer();

	int InsertPacketToFrame(CRtpPacket& RtpPacket);

	int OutputFrame();

	TPR_UINT32 Timestamp(){return m_nTimestamp;}
	
	TPR_UINT32 PayLoadFrequence(){return m_iPayLoadFrequence;}
	
	FrameStat FrameState(){return m_frameStat;}

    TPR_INT16 GetNackCount();

    int FrameLen(){return m_nFrameLen;}

    TPR_INT64 LatestPacketTimeMs(){return m_nlatestPacketTimeMs;}

	TPR_BOOL IsKeyFrame(){return m_bKeyFrame;}
private:
	TPR_UINT32 m_nTimestamp;
	TPR_BOOL m_bKeyFrame;
	TPR_UINT32 m_iPayLoadFrequence;
	FrameStat m_frameStat;
	TPR_BOOL m_bBufferInit;
	OuterParam& m_struOuterParam;
    TPR_UINT32 m_nNackCount;
    TPR_UINT32 m_nFrameLen;
    TPR_INT64  m_nlatestPacketTimeMs;
};

class FrameList
{
public:
	FrameList();
	~FrameList();

	int InsertFrame(FrameBuffer* pFrame);

	FrameBuffer* PopFrame();
private:
	typedef list<FrameBuffer*> FrameMap;
	FrameMap m_list;							//֡����
	TPR_Mutex m_lock;							//������
};

#endif

