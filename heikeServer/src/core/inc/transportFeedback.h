#ifndef _TRANSPORTFEEDBACK_H_
#define _TRANSPORTFEEDBACK_H_

#include <map>
#include <vector>
#include <deque>
#include "TPR_Mutex.h"
#include "TPR_Time.h"
#include "rtp.h"
#include "receiverImpl.h"

using std::map;
using std::vector;
using std::deque;

enum StatusSymbol 
{
	kNotReceived,
	kReceivedSmallDelta,
	kReceivedLargeDelta,
};

class PacketStatusChunk;
class TransportFeedback
{
public:
	TransportFeedback();
	~TransportFeedback();
	
	void SetBase(TPR_INT64 llbaseSequence,TPR_INT64 llrefTimestampUs);

	void SetFeedbackSequenceNumber(TPR_UINT8 cbFeedbackSeq);

	TPR_UINT16 GetBaseSeq();

	TPR_UINT32 GetBaseTime();

	TPR_UINT16 GetCount();

	TPR_UINT8 GetFeedbackSequenceNumber();

	TPR_BOOL AddReceivedPacket(TPR_INT64 llSeq,TPR_INT64 llTimestamp);

	int GetLength();

	vector<TPR_INT16>* GetDeltas();

    vector<TPR_INT64> GetReceiveDeltasUs();

	vector<PacketStatusChunk*>* GetChuncks();

	void EmitRemaining();

    PacketStatusChunk* ParseChunk(const unsigned char* buffer, unsigned int max_size) ;
    vector<StatusSymbol> GetStatusVector();
    bool Parse(unsigned char* pData, unsigned int len);
private:
	TPR_BOOL AddSymbol(StatusSymbol Symbol, TPR_INT64 llSeq);

	TPR_BOOL Encode(StatusSymbol Symbol);

	void EmitVectorChunk();

	void EmitRunLengthChunk();


#define kDeltaScaleFactor (250)
#define kBaseScaleFactor (kDeltaScaleFactor * (1 << 8))
	
	TPR_INT64 m_llBaseSequence;			//数据包基准序号
	TPR_INT64 m_llLastSequence;         //记录上一个包的序号
	TPR_INT64 m_llBaseTime;				//数据包基准达到时间
	TPR_UINT8 m_cbFeedbackSeq;			//反馈包本身序号
	TPR_INT64 m_llLastTimestamp;		//记录上一个包的达到时间

	vector<TPR_INT16> m_vdeltas;		//数据包达到时间差集合
	deque<StatusSymbol> m_vsymbol;		//符号集合
	TPR_UINT16 m_sSameCount;			//相同符合个数集合
	TPR_BOOL bVecNeedsTwoBitSymbols;
	vector<PacketStatusChunk*> m_vchunks;//chunck集合
	int m_size;
};

class PacketStatusChunk 
{
public:
	virtual ~PacketStatusChunk() {}
	virtual TPR_UINT16 NumSymbols() = 0;
	virtual void AppendSymbolsTo(vector<StatusSymbol>* vec) = 0;
	virtual void WriteTo(TPR_UINT8* buffer) = 0;
};

//  One Bit Status Vector Chunk
//
//  0                   1
//  0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5
//  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//  |T|S|       symbol list         |
//  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//
//  T = 1
//  S = 0
//  symbol list = 14 entries where 0 = not received, 1 = received

class OneBitVectorChunk : public PacketStatusChunk 
{
public:
#define ONEBITVECTOR_CAPACITY 14

	OneBitVectorChunk(deque<StatusSymbol>* symbols);

	~OneBitVectorChunk();

	TPR_UINT16 NumSymbols();

	void AppendSymbolsTo(vector<StatusSymbol>* vec);

	void WriteTo(TPR_UINT8* buffer); 

	static OneBitVectorChunk* ParseFrom(const TPR_UINT8* data);

private:
	OneBitVectorChunk() {}

	StatusSymbol symbols_[ONEBITVECTOR_CAPACITY];
};

//  Two Bit Status Vector Chunk
//
//  0                   1
//  0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5
//  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//  |T|S|       symbol list         |
//  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//
//  T = 1
//  S = 1
//  symbol list = 7 entries of two bits each, see (Encode|Decode)Symbol

class TwoBitVectorChunk : public PacketStatusChunk 
{
public:
#define TWOBITVECTOR_CAPACITY 7

	TwoBitVectorChunk(deque<StatusSymbol>* symbols);

	~TwoBitVectorChunk();

	TPR_UINT16 NumSymbols();

	void AppendSymbolsTo(vector<StatusSymbol>* vec);

	void WriteTo(TPR_UINT8* buffer);

	static TwoBitVectorChunk* ParseFrom(const TPR_UINT8* buffer);

private:
	TwoBitVectorChunk() {}

	StatusSymbol symbols_[TWOBITVECTOR_CAPACITY];
};

//  Two Bit Status Vector Chunk
//
//  0                   1
//  0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5
//  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//  |T| S |       Run Length        |
//  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//
//  T = 0
//  S = symbol, see (Encode|Decode)Symbol
//  Run Length = Unsigned integer denoting the run length of the symbol

class RunLengthChunk : public PacketStatusChunk 
{
public:
	RunLengthChunk(StatusSymbol symbol, int size);

	~RunLengthChunk();

	TPR_UINT16 NumSymbols();

	void AppendSymbolsTo(vector<StatusSymbol>* vec);

	void WriteTo(TPR_UINT8* buffer);

	static RunLengthChunk* ParseFrom(const TPR_UINT8* buffer);

private:
	StatusSymbol symbol_;
	const int size_;
};

class ReceiverImpl;
class TransportFeedBackStatistics
{
public:
	TransportFeedBackStatistics();
	~TransportFeedBackStatistics();

	void IncomingPacket(CRtpPacket& rtpPacket);

	void SetCallbackInterface(ReceiverImpl* pInterface);
private:
	int OnPacketArrival(TPR_UINT16 sSeqNo, TPR_TIME_T time,TPR_BOOL bFrameOver);
	
	static TPR_VOIDPTR __stdcall OutThread( TPR_VOIDPTR pUserData);
	
	int OutThreadRel();

	int BuildFeedback(TransportFeedback* pFeedback);

	SequenceNumberUnwrapper m_sepTool;
	
	typedef map<TPR_INT64, TPR_TIME_T> PackTimeMap;
	PackTimeMap m_packetTimes;

	TPR_INT32 m_iLastTimestamp;

	TPR_UINT8 m_cbFeedbackSeq;

	ReceiverImpl* m_pOutInterface;

	TPR_BOOL m_bStart;
	TPR_HANDLE m_hThread;

	TPR_Mutex m_lock;
};
#endif
