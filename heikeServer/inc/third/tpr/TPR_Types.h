#ifndef _TPR_TYPES_H_
#define _TPR_TYPES_H_

typedef signed char     TPR_INT8;
typedef unsigned char   TPR_UINT8;
typedef signed short    TPR_INT16;
typedef unsigned short  TPR_UINT16;
typedef signed int      TPR_INT32;
typedef unsigned int    TPR_UINT32;
typedef void*			TPR_VOIDPTR;
typedef signed long     TPR_LONG;
typedef unsigned long   TPR_ULONG;

#define TPR_VOID void


#if defined(_MSC_VER)
	typedef signed __int64		TPR_INT64;
	typedef unsigned __int64	TPR_UINT64;
	typedef HANDLE				TPR_HANDLE;

	#if (_MSC_VER >= 1310)
		#if defined _WIN32_WCE
			#include <ctrdefs.h>
		#else
			#include <stddef.h>
		#endif
		typedef uintptr_t			TPR_UINT;
		typedef intptr_t			TPR_INT;
	#endif

	#ifndef socklen_t
		typedef int socklen_t;
	#endif

	typedef int (CALLBACK *TPR_PROC)();

#elif defined(__GNUC__) || defined(__SYMBIAN32__)
	#if defined(__LP64__)
		typedef signed long TPR_INT64;
		typedef unsigned long TPR_UINT64;
	#else
		typedef signed long long TPR_INT64;
		typedef unsigned long long TPR_UINT64;
	#endif

	typedef void* TPR_HANDLE;
	#include <ctype.h>
	#ifdef __APPLE__
		#include <cstdint>
	#endif

	typedef uintptr_t	TPR_UINT;
	typedef intptr_t	TPR_INT;
	typedef void*		TPR_PROC;
#endif

#define TPR_SUPPORT_INT64 1

#ifndef TPR_BOOL
	#define TPR_BOOL TPR_INT32
	#define TPR_TRUE 1
	#define TPR_FALSE 0
#endif

#define TPR_INVALID_HANDLE  NULL

#endif


