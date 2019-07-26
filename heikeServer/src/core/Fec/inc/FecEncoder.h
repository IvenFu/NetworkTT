#ifndef _HIK_FEC_ENCODER_H_
#define _HIK_FEC_ENCODER_H_

#include "public.h"
#include "forward_error_correction.h"
#include "HikFecInterface.h"



typedef void(*OutputDataCallBack)(DATA_PACK_VEC* pstFecList, void *pUser);

class ForwardErrorCorrection;
class CHikFecEncoder
{
public:
	CHikFecEncoder(HikFecProtectionParams *p);
	~CHikFecEncoder();
public:
	int RegisterOutputFecCallBack(void(*OutputDataCallBack)(DATA_PACK_VEC* pstFecList, void *pUser), void *pUser);
	int GenerateFEC(DATA_PACK_VEC *pstStream,DATA_INFO* pstruInfo);
	int SetFecProtectionParms(HikFecProtectionParams* pParam);

private:
	int CheckData(const PacketList& packList);

	ForwardErrorCorrection *m_pcFecCorrection;
	HikFecProtectionParams m_stParam;
    

	std::list<Packet*> media_packets_fec_;
	std::list<Packet*> m_fec_packets_list;

	void CleanList(std::list<Packet*>* list);
	
	OutputDataCallBack m_cbFecBack;
	void*              m_pUser;
    DATA_PACK_VEC *    m_pNode;
	unsigned short     m_sSeqRecord;
	bool               m_bFirstPacketInGroup;
};

#endif