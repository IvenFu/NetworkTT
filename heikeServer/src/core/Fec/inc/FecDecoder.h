

#ifndef _W_FEC_DECODER_H_
#define _W_FEC_DECODER_H_


#include "HikFecInterface.h"
#include "public.h"
#include "Decoder.h"
#include "forward_error_correction.h"

using std::list;


class ForwardErrorCorrection;
class CHikFecDecoder
{
public:
	CHikFecDecoder();
	~CHikFecDecoder();

public:
	int RegisterOutputFecCallBack(HIKFEC_OutputDataCallBack pCb, void *pUser);
	int DecodeFec(DATA_PACK_VEC *pstStream);

private:
	ForwardErrorCorrection *m_pcFecCorrection;

	ReceivedPacketList received_packet_list_;
	RecoveredPacketList recovered_packet_list_;

	HIKFEC_OutputDataCallBack m_cbFecBack;
	void*              m_pUser;

	HPR_BOOL m_bTurn;
	int m_count;
	HPR_UINT32 m_nMediaSsrc; 
};
#endif






