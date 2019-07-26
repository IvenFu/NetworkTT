
#ifndef _HIK_FEC_INTERFACE_IMPL_H_
#define _HIK_FEC_INTERFACE_IMPL_H_

#include "HikFecInterface.h"

#define  IS_FEC(a)  ((a) == FEC_PACKET_VIDEO_PAYLOAD_TYPE || (a) == FEC_PACKET_AUDIO_PAYLOAD_TYPE)

class HikFecInterfaceImpl
{
public:
	HikFecInterfaceImpl(bool m_bEncoder);
	~HikFecInterfaceImpl();

	bool m_bEncoder;
	void* m_handle;
};

#endif


