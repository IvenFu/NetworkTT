

#ifndef _PUBLIC_H_
#define _PUBLIC_H_

#include <list>
#include "utilInner.h"

#define IP_PACKET_SIZE 1500  

enum FecMaskType 
{
	kFecMaskRandom,
	kFecMaskBursty,
};

class Packet 
{
public:
	Packet():length(0)
	{
		memset(data,0,IP_PACKET_SIZE);
	}

	virtual ~Packet() {}

	HPR_INT32 length;         
	HPR_UINT8 data[IP_PACKET_SIZE]; 

	void operator=(const Packet&);
};

typedef std::list<Packet*> PacketList;

class SortablePacket 
{
public:
	// True if first is <= than second.
	static HPR_BOOL LessThan(const SortablePacket* first,
		const SortablePacket* second)
	{
		return IsNewerSequenceNumberIn(second->seq_num, first->seq_num);
	}

	HPR_UINT16 seq_num;
};



#endif






