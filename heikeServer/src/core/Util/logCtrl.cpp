/**	@logCtrl.cpp
*	@note 111.
*	@brief 内存管理、日志处理等
*
*	@author		222
*	@date		2018/4/14
*
*	@note 
*
*	@warning 
*/
#include <string>
#include <map>
#include "TPR_Mutex.h"
#include "TPR_Guard.h"
#include "Util.h"

using std::map;
using std::string;

/**	@struct	 MEM_RECORD_T 
 *	@brief  内存节点记录
 *	@note	 
 */
struct MEM_RECORD_T
{
	string szFunc;  //分配内存的函数
	size_t iLine;	//分配内存的代码行号
	size_t iSize;	//分配的内存大小
};

//记录内存的map定义
typedef map<void* , MEM_RECORD_T> MEMOY_MAP;

static TPR_Mutex g_lock;			//控制内存记录操作的全局锁
static TPR_INT64 g_llMemoryLen = 0; //使用内存长度统计
static MEMOY_MAP g_mMemoryMap;		//记录内存使用节点列表

/**	@fn MemoryUsed
*	@brief 获取当前使用的内存大小
*	@return 内存大小
*/
TPR_INT64 MemoryUsed()
{
	TPR_Guard gurad(&g_lock);

	//MEMOY_MAP::iterator it = mMemoryMap.begin();
	//for(;it!=mMemoryMap.end();++it)
	//{
	//	NPQ_DEBUG("MemoryUsed size=%d,func=%s,line=%d",it->second.iSize,it->second.szFunc.c_str(),it->second.iLine);
	//}
	return g_llMemoryLen;
}

/**	@fn MemoryNew
*	@brief 记录新申请的内存
*	@param  void* ptr  内存起始地址
*	@param size_t size 内存长度
*	@param const char* func 申请内存的函数
*	@param const size_t lin 申请内存的函数所在行
*	@return void
*/
void MemoryNew(void* ptr,size_t size,const char* func, const size_t line)
{
	TPR_Guard gurad(&g_lock);

	MEM_RECORD_T struMem;
	struMem.szFunc.assign(func);
	struMem.iLine = line;
	struMem.iSize = size;

	g_llMemoryLen+=size;
	g_mMemoryMap[ptr] = struMem;

	//if(size > 1000)
	{
		//NPQ_DEBUG("MemoryNew size=%d,func=%s,line=%d",size,func,line);
	}
}

/**	@fn MemoryDelete
*	@brief 删除释放的内存
*	@param  void* ptr  内存起始地址
*	@return void
*/
void MemoryDelete(void* ptr)
{
	TPR_Guard gurad(&g_lock);

	MEMOY_MAP::iterator it;

	it = g_mMemoryMap.find(ptr);
	if(it == g_mMemoryMap.end())
	{
		return;
	}
	
	int iSize = it->second.iSize;
	//NPQ_DEBUG("MemoryDelete size=%d,func=%s,line=%d",iSize,it->second.szFunc.c_str(),it->second.iLine);

	g_mMemoryMap.erase(ptr);
	g_llMemoryLen -= iSize;
	g_llMemoryLen = npq_max(0,g_llMemoryLen);
}

/**	@fn operator new
*	@brief 重载new操作符
*	@param  size_t size 申请内存大小
*	@param  const char* fun 申请内存所在的函数
*	@param  const size_t line 申请内存所在的行号
*	@return 分配的内存地址
*/
void* operator new(size_t size, const char* func, const size_t line) 
{
	return ::operator new(size);
}

/**	@fn operator new
*	@brief 重载nothrow版本new操作符
*	@param  size_t size 申请内存大小
*	@param  onst std::nothrow_t& no  nothrow标识符
*	@param  const char* fun 申请内存所在的函数
*	@param  const size_t line 申请内存所在的行号
*	@return 分配的内存地址
*/
void* operator new(size_t size, const std::nothrow_t& no, const char* func, const size_t line) 
{
	void* ptr;

	ptr = ::operator new(size,no);
	if(ptr == NULL)
	{
		return NULL;
	}

	MemoryNew(ptr,size,func,line);
	return ptr;
}

/**	@fn operator new
*	@brief 重载数组版本new操作符
*	@param  size_t size 申请内存大小
*	@param  const char* fun 申请内存所在的函数
*	@param  const size_t line 申请内存所在的行号
*	@return 分配的内存地址
*/
void* operator new[](size_t size, const char* func, const size_t line) 
{
	return ::operator new[](size);
}

/**	@fn operator new
*	@brief 重载nothrow 数组版本new操作符
*	@param  size_t size 申请内存大小
*	@param  onst std::nothrow_t& no  nothrow标识符
*	@param  const char* fun 申请内存所在的函数
*	@param  const size_t line 申请内存所在的行号
*	@return 分配的内存地址
*/
void* operator new[](size_t size, const std::nothrow_t& no,const char* func, const size_t line) 
{
	void* ptr;

	ptr = ::operator new[](size,no);
	if(ptr == NULL)
	{
		return NULL;
	}

	MemoryNew(ptr,size,func,line);
	return ptr;
}

//以下重载函数只用于系统自动调用，不能主动调用，用于new类时初始化失败，自动调用delete释放内存，否则会有泄漏风险
/**	@fn operator delete
*	@brief 重载数组版本delete操作符
*	@param  void* ptr 内存首地址
*	@param  const char* fun 申请内存所在的函数
*	@param  const size_t line 申请内存所在的行号
*	@return 
*/
void operator delete(void* ptr,const char* func, const size_t line) 
{
	::operator delete(ptr);
}

/**	@fn operator delete
*	@brief 重载数组版本delete操作符
*	@param  void* ptr 内存首地址
*	@param  const char* fun 申请内存所在的函数
*	@param  const size_t line 申请内存所在的行号
*	@return 
*/
void operator delete[](void* ptr,const char* func, const size_t line) 
{
	::operator delete[](ptr);
}

/**	@fn operator delete
*	@brief 重载nothrow 版本delete操作符
*	@param  void* ptr 内存首地址
*	@param  onst std::nothrow_t& no  nothrow标识符
*	@param  const char* fun 申请内存所在的函数
*	@param  const size_t line 申请内存所在的行号
*	@return void
*/
void operator delete(void* ptr,const std::nothrow_t& no,const char* func, const size_t line) 
{
	::operator delete(ptr);
}

/**	@fn operator delete
*	@brief 重载nothrow 数组版本delete操作符
*	@param  void* ptr 内存首地址
*	@param  onst std::nothrow_t& no  nothrow标识符
*	@param  const char* fun 申请内存所在的函数
*	@param  const size_t line 申请内存所在的行号
*	@return void
*/
void operator delete[](void* ptr,const std::nothrow_t& no,const char* func, const size_t line) 
{
	::operator delete[](ptr);
}











