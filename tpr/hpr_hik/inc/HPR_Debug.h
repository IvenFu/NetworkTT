/*
 * CopyRight:	HangZhou Hikvision System Technology Co., Ltd. All	Right Reserved.
 * FileName:	hpr2_debug.h
 * Desc:		hpr2 debug.
 * Author:		zhaoyiji
 * Date:		2012-11-12
 * Contact:		zhaoyiji@hikvision.com.cn
 * History:		Created By Zhaoyiji 2012-11-12 (V0.1)
 * */

#ifndef __HPR2_DEBUG_H__
#define __HPR2_DEBUG_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define HPR2_LOG_LEVEL_DEBUG 0
#define HPR2_LOG_LEVEL_INFO  1
#define HPR2_LOG_LEVEL_WARN  2
#define HPR2_LOG_LEVEL_ERROR 3

void hpr2_log(int level, const char* format, ...);


#if (defined _WIN32 || defined _WIN64)
#if (_MSC_VER >= 1500)
#define HPR2_LOG_ERROR(fmt, ...)         hpr2_log(HPR2_LOG_LEVEL_ERROR, "<%s>|<%d>|<%s>,"fmt, __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__)
#define HPR2_LOG_WARN(fmt, ...)          hpr2_log(HPR2_LOG_LEVEL_WARN, "<%s>|<%d>|<%s>,"fmt, __FILE__,  __LINE__, __FUNCTION__, ##__VA_ARGS__)    
#define HPR2_LOG_INFO(fmt, ...)          hpr2_log(HPR2_LOG_LEVEL_INFO, "<%s>|<%d>|<%s>,"fmt,  __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__)    
#define HPR2_LOG_DEBUG(fmt, ...)         hpr2_log(HPR2_LOG_LEVEL_DEBUG, "<%s>|<%d>|<%s>,"fmt, __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__)
#else
#define HPR2_LOG_ERROR         hpr2_log
#define HPR2_LOG_WARN          hpr2_log   
#define HPR2_LOG_INFO          hpr2_log    
#define HPR2_LOG_DEBUG         hpr2_log
#endif
#else
#define HPR2_LOG_ERROR(fmt, args...)     hpr2_log(HPR2_LOG_LEVEL_ERROR, "<%s>|<%d>|<%s>,"fmt, __FILE__, __LINE__, __FUNCTION__, ##args)
#define HPR2_LOG_WARN(fmt, args...)     hpr2_log(HPR2_LOG_LEVEL_WARN, "<%s>|<%d>|<%s>,"fmt, __FILE__, __LINE__, __FUNCTION__, ##args)
#define HPR2_LOG_INFO(fmt, args...)     hpr2_log(HPR2_LOG_LEVEL_INFO, "<%s>|<%d>|<%s>,"fmt, __FILE__, __LINE__, __FUNCTION__, ##args)
#define HPR2_LOG_DEBUG(fmt, args...)     hpr2_log(HPR2_LOG_LEVEL_DEBUG, "<%s>|<%d>|<%s>,"fmt, __FILE__, __LINE__, __FUNCTION__, ##args)
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
