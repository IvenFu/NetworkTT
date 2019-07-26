

#ifndef _DECODER_H_
#define _DECODER_H_

#include <list>
#include "public.h"

using std::list;


class ProtectedPacket: public SortablePacket
{
public:
	Packet* pkt;
};

typedef std::list<ProtectedPacket*> ProtectedPacketList;


class FecPacket : public SortablePacket
{
public:
	ProtectedPacketList protected_pkt_list;
	HPR_UINT32 ssrc;
	Packet* pkt;
};


class ReceivedPacket: public SortablePacket
{
public:
	ReceivedPacket()
		:ssrc(0)
	,is_fec(0)
	,pkt(NULL){}
	~ReceivedPacket(){}

	HPR_UINT32 ssrc;

	HPR_BOOL is_fec;

	Packet* pkt; 
};


typedef list<ReceivedPacket*> ReceivedPacketList;


class RecoveredPacket: public SortablePacket
{
public:
	RecoveredPacket()
		:was_recovered(0)
	,returned(0)
	,pkt(NULL){}
	~RecoveredPacket(){}

	HPR_BOOL was_recovered; 
	
	HPR_BOOL returned; 

	HPR_UINT8 length_recovery[2];  

	Packet* pkt; 
};

typedef list<RecoveredPacket*> RecoveredPacketList;

#endif






