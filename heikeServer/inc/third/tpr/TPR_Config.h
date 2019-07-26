#ifndef _TPR_CONFIG_H_
#define _TPR_CONFIG_H_

#if defined(__cplusplus)
#define CXXCOMPLIE  1
#endif


#if (defined(_WIN32) || defined(_WIN32_WCE) || defined(__WIN64))
	#pragma warning(disable: 4819)

	#if (_MSC_VER >= 1700)
		#pragma warning(disable:4996)
		#define MSVC_2012
	#elif (_MSC_VER >= 1600)
		#pragma warning(disable:4996)
		#define MSVC_2010
	#elif (_MSC_VER >= 1500)
		#pragma warning(disable:4996)
		#define MSVC_2008
	#elif (_MSC_VER >= 1400)
		#pragma warning(disable:4996)
		#define MSVC_2005
	#elif (_MSC_VER >= 1310)
		#pragma warning(disable:4996)
		#define MSVC_2003
	#else                     //vs98
		#define _WIN32_WINNT  0x0500
		#pragma warning(disable:4710)
		#pragma warning(disable:4786)
		#define MSVC_98
	#endif

	#if defined(_WIN32_WCE)
	#define OS_WINCE 1
	#endif

	#if defined(_WIN64)
	#define OS_WINDOWS64 1
	#endif

	#pragma warning(disable:4100)
	#pragma warning(disable:4127)

	#ifndef CALLBACK
	#define CALLBACK __stdcall
	#endif

	#ifdef MSVC_2012
	#pragma comment(lib, "ws2_32.lib")
	#pragma comment(lib, "Rpcrt4.lib")
	#pragma comment(lib, "winmm.lib")
	#endif

	#include <winsock2.h>
	#include <ws2tcpip.h>
	#include <stdio.h>
	#include <stdlib.h>

	#ifndef OS_WINPHONE
		#include <Mswsock.h>
	#endif

	#define OS_WINDOWS 1
	#define TPR_EXPORTS 1
	#define TPR_INFINITE INFINITE

	#ifdef TPR_STATIC
		#define TPR_DECLARE
		#define TPR_DECLARE_CLASS
		#define TPR_DECLARE_CXX
	#else
		#if defined(TPR_EXPORTS)
			#define TPR_DECLARE  extern "C" __declspec(dllexport)
			#define TPR_DECLARE_CLASS __declspec(dllexport)
			#define TPR_DECLARE_CXX __declspec(dllexport)
		#else
			#define TPR_DECLARE  extern "C" __declspec(dllimport)
			#define TPR_DECLARE_CLASS __declspec(dllimport)
			#define TPR_DECLARE_CXX __declspec(dllimport)
		#endif
	#endif

#elif (defined(__linux__) || defined(__APPLE__) || defined(__SYMBIAN32__) || defined(ANDROID))
	#include <unistd.h>
	#include <sys/socket.h>
	#include <sys/ioctl.h>
	#include <netinet/in.h>
	#include <arpa/inet.h>
	#include <sys/types.h>
	#include <netdb.h>
	#include <errno.h>
	#include <string.h>
	#include <stdio.h>
	#include <stdlib.h>
	#include <stdarg.h>
	#include <net/if.h>

	#if (defined(__SYMBIAN32__) || defined(ANDROID))
		#include <sys/stat.h>
	#endif

	#define OS_POSIX 1
	#define TPR_INFINITE 0xFFFFFFFF
	#define WAIT_TIMEOUT 250

	#if defined(__LP64__)
		#define OS_POSIX64 1
	#endif



	#if defined(__APPLE__)

		#include <TargetConditionals.h>
		#include <sys/stat.h>
		#include <sys/sockio.h>
		#define OS_APPLE 1
		
		#if (defined(TARGET_OS_IPHONE) && (TARGET_OS_IPHONE == 1))
			#define OS_IPHONE 1
		#elif (defined(TARGET_OS_MAC) && (TARGET_OS_MAC == 1))
			#define OS_MAC 1
		#endif
	#endif

	#if defined(__SYMBIAN32__)
		#define OS_S60   1
	#endif

	#if defined(ANDROID)
		#define OS_ANDROID 1
	#endif

	#if ((defined(__arm__) || defined(__ARMEL__)) && defined(__linux__))
		#define OS_ARMLINUX 1
	#endif

	#ifdef TPR_STATIC
		#define TPR_DECLARE
	#else
		#define TPR_DECLARE  extern "C" 
	#endif

	#define TPR_DECLARE_CLASS 
	#define TPR_DECLARE_CXX 

	#ifndef CALLBACK
		#define CALLBACK 
	#else
		#undef CALLBACK
		#define CALLBACK
	#endif

#else
	#error OS Not Implement Yet.
#endif


#define TPR_OK    (0)
#define TPR_ERROR    (-1)
#define TPR_NPTSUPPORT    (-2)
#define TPR_TIMEOUT (0)
#define TPR_FOREVER (-1)

#define OS_SUPPORT_IPV6  1
#endif



