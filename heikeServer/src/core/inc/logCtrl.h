/**	@logCtrl.h
*	@note 111.
*	@brief 日志按模块开关控制
*
*	@author		222
*	@date		2017/12/07
*
*	@note  本头文件重载了new和delete,用于内存统计
*
*	@warning 
*/
#ifndef _LOGCTRL_H_
#define _LOGCTRL_H_

#include <string>

#define LOG_FUNC_JITTER   0  
#define LOG_FUNC_NACK   0  
#define LOG_FUNC_FEC   0  
#define LOG_FUNC_BW   0 
#define LOG_FUNC_PACING   0 
#define LOG_FUNC_PLI   0  
#define LOG_FUNC_NETEQ   0  

//重载的new操作符，区别系统的版本，记录函数名和行号
void* operator new(size_t size, const char* func, const size_t line);
void* operator new(size_t size, const std::nothrow_t& no, const char* func, const size_t line);
void* operator new[](size_t size, const char* func, const size_t line);
void* operator new[](size_t size, const std::nothrow_t& no,const char* func, const size_t line);

//以下重载函数只用于系统自动调用，不能主动调用，用于new类时初始化失败，自动调用delete释放内存，否则会有泄漏风险
void operator delete(void* ptr,const char* func, const size_t line);
void operator delete[](void* ptr,const char* func, const size_t line);
void operator delete(void* ptr,const std::nothrow_t& no,const char* func, const size_t line);
void operator delete[](void* ptr,const std::nothrow_t& no,const char* func, const size_t line);

/*获取当前使用的内存大小*/
TPR_INT64 MemoryUsed();

/*删除释放的内存*/
void MemoryDelete(void* ptr);

/*安全释放内存宏定义*/
#define SAFE_DEL(ptr)           \
	if(ptr != NULL)             \
{                               \
	delete ptr;                 \
	MemoryDelete(ptr);			\
	ptr = NULL;                 \
}

/*安全释放内存宏定义*/
#define SAFE_ARR_DEL(ptr)       \
	if (ptr != NULL)            \
{                               \
	delete[] ptr;               \
	MemoryDelete(ptr);			\
	ptr = NULL;                 \
}

/*安全释放内存宏定义*/
#define SAFE_ARR_FREE(ptr)      \
	if (ptr != NULL)            \
{                               \
	free(ptr);                  \
	ptr = NULL;                 \
}

#define NPQ_FUCNTION __FUNCTION__				//函数名系统宏
#define NPQ_LINE __LINE__						//行号系统宏
#define NPQ_NEW_FLAG __FUNCTION__,__LINE__		//调用重载的new接口标识
#endif
