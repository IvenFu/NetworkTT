#ifndef _TPR_SELECT_H_
#define _TPR_SELECT_H_


#include "TPR_Config.h"
#include "TPR_Types.h"

#if defined(OS_POSIX)
#if defined(OS_S60)
#include <select.h>
#else
#include <poll.h>
#endif
#endif



TPR_DECLARE TPR_INT32 CALLBACK TPR_FdIsSet(TPR_INT32 fd, fd_set* set);
TPR_DECLARE TPR_INT32 CALLBACK TPR_Select(TPR_INT32 iMaxFd, fd_set* fdset_r, fd_set* fdset_w, fd_set* fdset_e,timeval* tv);
TPR_DECLARE TPR_INT32 CALLBACK TPR_SelectEx(TPR_INT32 iMaxFd, fd_set* fdset_r, fd_set* fdset_w, fd_set* fdset_e,timeval* tv);
TPR_DECLARE TPR_INT32 CALLBACK TPR_Poll(struct pollfd* fds, TPR_INT32 iFds, TPR_INT32 iTimeOut);
TPR_DECLARE TPR_INT32 CALLBACK TPR_PollEx(struct pollfd* fds, TPR_INT32 iFds, TPR_INT32* iTimeO);


#endif
