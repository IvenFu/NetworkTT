#ifndef __HPR_NET_ADDR_H__
#define __HPR_NET_ADDR_H__

#include <HPR_Config.h>
#include <HPR_Types.h>

//////////////////////////////////////////////////////////////////////////
#if defined (OS_POSIX)

#ifndef SOCKADDR_IN
#define SOCKADDR_IN sockaddr_in
#endif

#ifndef SOCKADDR
#define SOCKADDR sockaddr
#endif

#ifndef PSOCKADDR
#define PSOCKADDR sockaddr*
#endif

#ifndef IN_ADDR
#define IN_ADDR in_addr
#endif

#endif///(OS_POSIX)
//////////////////////////////////////////////////////////////////////////

namespace hpr
{
	class HPR_DECLARE_CLASS hpr_net_addr
	{
		friend HPR_DECLARE_CLASS bool operator==(const hpr_net_addr& addr1, const hpr_net_addr& addr2);
		friend HPR_DECLARE_CLASS bool operator!=(const hpr_net_addr& addr1, const hpr_net_addr& addr2);

	public:
		hpr_net_addr();
		hpr_net_addr(const hpr_net_addr &);
		hpr_net_addr(const hpr_int32 port, const char* host = NULL);
		hpr_net_addr(const char* url); //ip:port
		hpr_net_addr(const SOCKADDR_IN & addr);
		//hpr_net_addr(const SOCKADDR_IN6 & addr);

		virtual ~hpr_net_addr();

	public:
		hpr_net_addr& operator=(const hpr_net_addr & addr);
		hpr_int32 get_port() const;
		const char* get_host() const;
		const char* get_addr(char* host, hpr_int32 & addrlen) const;//url ip:port
		hpr_int32 get_addr(SOCKADDR_IN& addr) const;
		hpr_int32 get_addr(IN_ADDR& addr) const;
		//int GetAddr(SOCKADDR_IN6& addr);

		hpr_int32 set_host(const char* host);
		hpr_int32 set_port(const hpr_int32 port);
		hpr_int32 set_addr(const hpr_int32 port, const char* host = NULL);
		hpr_int32 set_addr(const char* url);
		hpr_int32 set_addr(const SOCKADDR_IN addr);

	private:
		hpr_int32 parse_url(const char* url);
		hpr_bool is_valid_ipv4(const char* host) const;

	private:
		SOCKADDR_IN m_stSockAddr;
		//SOCKADDR_IN6 m_stSockAddr6;
	};

	HPR_DECLARE_CLASS bool operator==(const hpr_net_addr& addr1, const hpr_net_addr& addr2);
	HPR_DECLARE_CLASS bool operator!=(const hpr_net_addr& addr1, const hpr_net_addr& addr2);
		
};

#endif
