/**	@ProbeControler.h
*	@note 111.
*	@brief 带宽探测控制
*
*	@author		222
*	@date		2018/4/18
*
*	@note 
*
*	@warning 
*/

#ifndef _PROBER_CONTROLER_H_
#define _PROBER_CONTROLER_H_
#include <list>
#include "TPR_Mutex.h"
#include "PaceSender.h"

using std::list;

/**	@enum EN_PROBE_STATE
 *	@brief  探测状态
 *	@note	 
 */
enum EN_PROBE_STATE 
{
	kInit,						//初始化
	kWaitingForProbingResult,	//等待探测结果
	kProbingComplete,			//探测结束
};

class PacedSender;

/**	@class ProbeController
 *	@brief  带宽探测控制
 *	@note	 
 */
class ProbeController
{
public:
	/*构造函数*/
	ProbeController(PacedSender& pacer);
	
	/*设置码率上下限以及初始码率*/
	void SetBitrates(TPR_INT64 llMinBitratebps,TPR_INT64 llStartBitratebps,TPR_INT64 llMaxBitratebps);

	/*设置拥塞模块估算的码率*/
	void SetEstimatedBitrate(TPR_INT64 llBitratebps);
	
	/*重置参数*/
	void Reset();

private:
	/*初始化探测*/
	int InitiateExponentialProbing();
	
	/*初始化探测*/
	int InitiateProbing(TPR_INT64 llNowMs,list<TPR_INT64>* plBitratesToprobes, TPR_BOOL bProbeFurther);

	PacedSender&  m_pacer;						//pacing对象
	EN_PROBE_STATE m_state;						//探测控制状态
	TPR_INT64 m_llMinBitrateToProbeFurtherbps;	//继续探测预期最小码率
	TPR_INT64 m_llTimeLastProbingInitiatedMs;	//记录上一次初始化探测的时间
	TPR_INT64 m_llEstimatedBitratebps;			//设置的估算码率值
	TPR_INT64 m_llStartBitratebps;				//初始码率
	TPR_INT64 m_llMaxBitratebps;				//最大码率
	TPR_INT64 llTimeOfLastLargeDropMs;			//码率大幅下滑时的时间
	TPR_INT64 llBitrateBeforeLastLargeDropbps;	//码率大幅下滑前的的码率
	TPR_Mutex m_lock;							//锁
};

#endif  
