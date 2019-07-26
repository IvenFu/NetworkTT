/**	@buffer.h
*	@note 111.
*	@brief Qos Buffer 相关定义
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
  
#define DATA_BUFFER_INIT_LEN    (2048)			//初始化缓冲区大小
#define DATA_BUFFER_STEP_LEN    (128*1024)		//缓冲区步进长度
#define DATA_BUFFER_MAX_LEN     (8*1024*1024)   //最大缓冲区大小
#define RTP_BUFFER_LEN			(1500)			//一个RTP包缓冲大小

/**	@class	 DataBuffer 
 *	@brief   缓冲区管理类
 *	@note	 
 */
class DataBuffer
{
public:
	/*构造函数*/
	DataBuffer();

	/*析构函数*/
	virtual ~DataBuffer();
	
	/*初始化内存*/
	int InitBuffer();

	/*清空缓冲区*/
	void ClearBuffer();
	
	/*释放内存*/
	void FiniBuffer();
	
	/*数据包数量是否为0*/
	TPR_BOOL IsNoPackets();

	int PacketsNumber();
	
	/*往缓冲区插入一个RTP包*/
	int InsertPacket(const CRtpPacket& cRtpPacket, TPR_BOOL bBySeqNo);
	int InsertPacketV2(const CRtpPacket& cRtpPacket);

	/*从列表头部获取一个RTP包*/
	int FrontPacket(CRtpPacket* pRtpPacket);

	/*从列表头部获取一个RTP包并删除列表记录，但是不移动内存，慎用！*/
	int PopFrontPacketNoMove(CRtpPacket* pRtpPacket);
	
	/*从列表尾部获取一个RTP包*/
	int LastPacket(CRtpPacket* pRtpPacket);
	
	/*获取列表最大RTP包的序号值*/
	int LastPacketSeq();
	
	/*根据序号删除RTP包*/
	int EreasePacket(TPR_UINT16 sSeq);
	
	/*根据过期时间删除RTP包*/
	int EreasePacketsDue(TPR_TIME_T nTime);
	
	/*根据序号查找RTP包*/
	int FindPacket(TPR_UINT16 sSeq,CRtpPacket* pRtpPacket);
	
	/*设置RTP临时缓冲区无效*/
	void InvalidRtpBuffer();
private:

	int ExtenBuffer(int iNewLen);

	int ExtenBufferStep(int iNewBufferLen);

	unsigned char* m_buf;			//缓冲区首地址
	unsigned char* m_pOneRtpbuf;	//RTP临时缓冲区首地址
	TPR_BOOL m_bRtpBufferUsed;		//临时缓冲区是否有效标志
	CRtpPackets m_list;				//RTP列表
	unsigned int m_nFree;			//可用地址偏移值
	unsigned int m_nBufSize;        //缓冲区大小
	TPR_Mutex m_lock;				//保护锁
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
	FrameMap m_list;							//帧链表
	TPR_Mutex m_lock;							//保护锁
};

#endif

