#ifndef __HPR_SOCK_UTILS_H__
#define __HPR_SOCK_UTILS_H__

#include <HPR_Config.h>
#include <HPR_Types.h>
#include <HPR_Net_Addr.h>


//////////////////////////////////////////////////////////////////////////
///windows vista or later support WSAPoll 
#ifndef POLLRDNORM
#define POLLRDNORM 0x0100
#endif
#ifndef POLLWRNORM
#define POLLWRNORM 0x0010
#endif
//////////////////////////////////////////////////////////////////////////

#define HPR_FD_RD POLLRDNORM
#define HPR_FD_WR POLLWRNORM
#define HPR_FD_RDWR (POLLRDNORM|POLLWRNORM)

#define HPR_NONBLOCK_YES (1)
#define HPR_NONBLOCK_NO (0)

#define HPR_REUSEADDR_YES (1)
#define HPR_REUSEADDR_NO (0)

#define HPR_RDWR_ERR (-1)

#if (defined _WIN32 || defined _WIN64)
	#define SHUT_WR	SD_SEND
	#define SHUT_RD	SD_RECEIVE
	#define SHUT_RDWR SD_BOTH
	typedef SOCKET hpr_sock_t; 
#else
	#include <poll.h>
	typedef int hpr_sock_t;
#endif


namespace hpr
{

	class HPR_DECLARE_CLASS hpr_sock_utils
	{
	public:
		static hpr_int32 is_rd(hpr_sock_t fd, struct timeval* tv);
		static hpr_int32 is_wr(hpr_sock_t fd, struct timeval* tv);
		static hpr_int32 is_rdwr(hpr_sock_t fd, struct timeval* tv, hpr_int32 ev);

		static hpr_int32 select_ctime(hpr_sock_t fd, fd_set* rd, fd_set* wr, fd_set* ex, timeval* tv);
		static hpr_int32 select_vtime(hpr_sock_t fd, fd_set* rd, fd_set* wr, fd_set* ex, timeval* tv);

		static hpr_int32 set_sock_opt(hpr_sock_t fd, hpr_int32 level, hpr_int32 optname, const char* optval, hpr_int32 optlen);
		static hpr_int32 get_sock_opt(hpr_sock_t fd, hpr_int32 level, hpr_int32 optname, char* optval, hpr_int32* optlen);
		static hpr_int32 set_nonblock(hpr_sock_t fd, hpr_int32 nonblock);
		static hpr_int32 set_reuseaddr(hpr_sock_t fd, hpr_int32 reuse);
		static hpr_int32 set_tcp_dontlinger(hpr_sock_t fd);
		static hpr_int32 set_tcp_nodelay(hpr_sock_t fd);
		static hpr_int32 set_ttl(hpr_sock_t fd, int ttl);
		static hpr_int32 set_buffsize(hpr_sock_t fd, hpr_int32 rcvbuf, hpr_int32 sndbuf);

		static hpr_int32 send(hpr_sock_t fd, const char* buf, hpr_int32 len, hpr_int32 timeout = -1, hpr_int32 flags = 0);
		static hpr_int32 sendn(hpr_sock_t fd, const char* buf, hpr_int32 len, hpr_int32 timeout = -1, hpr_int32 flags = 0);

		static hpr_int32 recv(hpr_sock_t fd, char* buf, hpr_int32 len, hpr_int32 timeout = -1, hpr_int32 flags = 0);
		static hpr_int32 recvn(hpr_sock_t fd, char* buf, hpr_int32 len, hpr_int32 timeout = -1, hpr_int32 flags = 0);

		static hpr_int32 getaddrbyfd(hpr_sock_t fd, hpr::hpr_net_addr& remote, hpr::hpr_net_addr& local);
		
		static hpr_int32 sendto(hpr_sock_t fd, const char* buf, hpr_int32 len, const hpr::hpr_net_addr& remote, hpr_int32 timeout = -1, hpr_int32 flags = 0);
		static hpr_int32 recvfrom(hpr_sock_t fd, char* buf, hpr_int32 len, hpr::hpr_net_addr& remote, hpr_int32 timeout = -1, hpr_int32 flags = 0);

		static hpr_sock_t socket(hpr_int32 af, hpr_int32 type, hpr_int32 protocol);
		static hpr_int32 closesocket(hpr_sock_t fd);
		static hpr_int32 shutdown(hpr_sock_t fd, hpr_int32 rdwr);
		static hpr_int32 listen(hpr_sock_t fd, hpr_int32 backlog);
		static hpr_int32 bind(hpr_sock_t fd, const hpr::hpr_net_addr& addr);
		static hpr_int32 connect(hpr_sock_t fd, const hpr::hpr_net_addr& remote, hpr_int32 timeout = -1);
		static hpr_sock_t accept(hpr_sock_t fd, hpr::hpr_net_addr& client, hpr_int32 timeout = -1);
		
		static hpr_int32 join_multicast_group(hpr_sock_t fd, const hpr::hpr_net_addr& local, const hpr::hpr_net_addr& mcast);
		static hpr_int32 leave_multicast_group(hpr_sock_t fd, const hpr::hpr_net_addr& local, const hpr::hpr_net_addr& mcast);

		static hpr_int32 get_lasterror();

		
#if defined(__linux__)
		static hpr_int32 poll_vtime(struct pollfd *fds, nfds_t nfds, struct timeval* tv);
#endif

	public:
		hpr_sock_utils();
		virtual ~hpr_sock_utils();
	};

};

#endif

