#ifndef __HPR_NET_UTILS_H__
#define __HPR_NET_UTILS_H__

#include "HPR_Config.h"
#include "HPR_Types.h"

namespace hpr
{

	class HPR_DECLARE_CXX hpr_net_utils
	{
	public:
		static hpr_uint16 hpr_htons(hpr_uint16 hs); 
		static hpr_uint16 hpr_ntohs(hpr_uint16 ns);
		static hpr_uint32 hpr_htonl(hpr_uint32 hl);
		static hpr_uint32 hpr_ntohl(hpr_uint32 nl);

		static hpr_int32 get_host_name(char* hostname, hpr_int32 len);
		//static hpr_int32 get_mac_addr(const char* ip, unsigned char* mac, hpr_int32* len);
		static hostent* get_host_by_name(const char* name);

	public:
		hpr_net_utils();
		virtual ~hpr_net_utils();
	};

};

#endif


