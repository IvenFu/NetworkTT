#ifndef _TPR_SOCK_UTILS_H_
#define _TPR_SOCK_UTILS_H_

#include "TPR_Config.h"
#include "TPR_Types.h"

#ifndef POLLRDNORM
#define POLLRDNORM 0x0100
#endif

#ifndef POLLWRNORM
#define POLLWRNORM 0x0010
#endif

#define TPR_FD_RD_POLLRDNORM
#define TPR_FD_WR_POLLWRNORM
#define TPR_FD_RDWR (POLLRDNORM|POLLWRNORM)

#define TPR_NOBLOCK_YES 1
#define TPR_NOBLOCK_NO 0
#define TPR_REUSEADDR_YES 1
#define TPR_REUSEADDR_NO 0

#define TPR_RDWR_ERR (-1)

#if (defined _WIN32 || defined _WIN64)
	#define SHUT_WR_SD_SEND
	#define SHUT_RD_SD_RECEIVE
	#define SHUT_RDWR_SD_BOTH
	typedef SOCKET tpr_sock_t;
#else
	#include <poll.h>
	typedef int tpr_sock_t;
#endif


#endif
