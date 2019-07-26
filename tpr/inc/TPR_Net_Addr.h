#ifndef _TPR_NET_ADDR_H_
#define _TPR_NET_ADDR_H_

#include "TPR_Config.h"
#include "TPR_Types.h"


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
#endif

#endif
