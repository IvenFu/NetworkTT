/**	@ProbeControler.cpp
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

#include "TPR_Guard.h"
#include "ProbeControler.h"

const TPR_INT64 kMaxWaitingTimeForProbingResultMs = 1000;	//探测超时时间
const int kExponentialProbingDisabled = 0;					//继续探测参数无效值
const TPR_INT64 kDefaultMaxProbingBitrateBps = 5000000;		//默认的最大码率
const int kRepeatedProbeMinPercentage = 70;					//继续探测最小百分比
const double kBitrateDropThreshold = 0.66;					//判断是否需要记录突变码率的比例

/**	@fn ProbeController::ProbeController
*	@brief 构造函数
*	@param PacedSender& pacer  pacing对象
*	@return void
*/
ProbeController::ProbeController(PacedSender& pacer)
	: m_pacer(pacer)
{
	Reset();
}

/**	@fn ProbeController::Reset
*	@brief 重置参数
*	@return void
*/
void ProbeController::Reset() 
{
	m_state = kInit;
	m_llMinBitrateToProbeFurtherbps = kExponentialProbingDisabled;
	m_llTimeLastProbingInitiatedMs = 0;
	m_llEstimatedBitratebps = 0;
	m_llStartBitratebps = 0;
	m_llMaxBitratebps = 0;

	TPR_INT64 llNowMs = OneTime::OneNowMs();
	llTimeOfLastLargeDropMs = llNowMs;
	llBitrateBeforeLastLargeDropbps = 0;
}

/**	@fn BitrateProber::SetBitrates
*	@brief 设置码率上下限以及初始码率
*	@param TPR_INT64 llMinBitratebps 最下码率
*	@param TPR_INT64 llStartBitratebps  初始码率
*	@param TPR_INT64 llMaxBitratebps 最大码率
*	@return void
*/
void ProbeController::SetBitrates(TPR_INT64 llMinBitratebps,TPR_INT64 llStartBitratebps,TPR_INT64 llMaxBitratebps) 
{
	TPR_Guard gurad(&m_lock);

	if (llStartBitratebps > 0)  
	{
		m_llStartBitratebps = llStartBitratebps;
		m_llEstimatedBitratebps = llStartBitratebps;
	} 
	else if (llStartBitratebps == 0)
	{
		m_llStartBitratebps = llMinBitratebps;
	}

	TPR_INT64 llOldMaxBitratebps = llMaxBitratebps;
	m_llMaxBitratebps = llMaxBitratebps;

	switch (m_state) 
	{
	case kInit:
		{
			InitiateExponentialProbing();
			break;
		}
	case kWaitingForProbingResult:
		{
			break;
		}
    case kProbingComplete:
		{
			if (m_llEstimatedBitratebps != 0 &&llOldMaxBitratebps < m_llMaxBitratebps &&m_llEstimatedBitratebps < m_llMaxBitratebps) 
			{
				list<TPR_INT64> lBitrates;
				lBitrates.push_back(m_llMaxBitratebps);
				InitiateProbing(OneTime::OneNowMs(), &lBitrates, TPR_FALSE);
			}
		}
	default:
		break;
	}
}

/**	@fn BitrateProber::InitiateExponentialProbing
*	@brief 初始化探测
*	@return 见状态码
*/
int ProbeController::InitiateExponentialProbing() 
{
	if(m_state != kInit)
	{
		return NPQERR_PARA;
	}

	if(m_llStartBitratebps <=0)
	{
		return NPQERR_PARA;
	}

	TPR_INT64 llNowMs = OneTime::OneNowMs();
	list<TPR_INT64> lBitrates;

	lBitrates.push_back(1 * m_llStartBitratebps);
	//lBitrates.push_back(2 * m_llStartBitratebps);
	return InitiateProbing(llNowMs,&lBitrates , TPR_TRUE);
}

/**	@fn BitrateProber::SetEstimatedBitrate
*	@brief 设置拥塞模块估算的码率
*	@param TPR_INT64 llBitratebps 码率
*	@return void
*/
void ProbeController::SetEstimatedBitrate(TPR_INT64 llBitratebps) 
{
	TPR_Guard gurad(&m_lock);

	NPQ_DEBUG("[PROBING]llBitratebps = %lld,m_state=%d",llBitratebps,m_state);

	TPR_INT64 llNowMs = OneTime::OneNowMs();

	if (m_state == kWaitingForProbingResult) 
	{
		//1秒超时时，状态从waiting置成complete
		if (llNowMs - m_llTimeLastProbingInitiatedMs > kMaxWaitingTimeForProbingResultMs) 
		{
			NPQ_DEBUG("[PROBING] probing time out");
			if (m_state == kWaitingForProbingResult) 
			{
				NPQ_DEBUG("[PROBING] probing kProbingComplete");
				m_state = kProbingComplete;
				m_llMinBitrateToProbeFurtherbps = kExponentialProbingDisabled;
			}
		}
		else
		{
			if (m_llMinBitrateToProbeFurtherbps != kExponentialProbingDisabled && llBitratebps >m_llMinBitrateToProbeFurtherbps) 
			{
				//实际码率比 min_bitrate_to_probe_further_bps_(70% 目标码率) 还要大，以2倍值继续探测
				list<TPR_INT64> lBitrates;

				NPQ_DEBUG("[PROBING] increacing probing %lld",2 * llBitratebps);
				lBitrates.push_back(2 * llBitratebps);
				InitiateProbing(llNowMs, &lBitrates, TPR_TRUE);
			}
		}
	}

	//如果估算出来下跌到0.66以下，那么记录下跌前的码率和时间 
						//0.66
	if (llBitratebps < kBitrateDropThreshold * m_llEstimatedBitratebps) 
	{
		llTimeOfLastLargeDropMs = llNowMs;
		llBitrateBeforeLastLargeDropbps = m_llEstimatedBitratebps;
	}
	m_llEstimatedBitratebps = llBitratebps;
}

/**	@fn BitrateProber::InitiateProbing
*	@brief 初始化探测
*	@param TPR_INT64 llNowMs 当前时间
*	@param list<TPR_INT64>* plBitratesToprobes 待探测的目标码率集合
*	@param TPR_BOOL bProbeFurther  是否继续探测
*	@return 见状态码
*/
int ProbeController::InitiateProbing(TPR_INT64 llNowMs,list<TPR_INT64>* plBitratesToprobes, TPR_BOOL bProbeFurther) 
{
	if(!plBitratesToprobes || plBitratesToprobes->empty())
	{
		return NPQERR_PARA;
	}
	
	list<TPR_INT64>::iterator it = plBitratesToprobes->begin();
	TPR_INT64 llBitrate = 0;
	for (;it!=plBitratesToprobes->end();++it)
	{
		llBitrate = *it;
		if(llBitrate<=0)
		{
			return NPQERR_PARA;
		}

		TPR_INT64 llMaxProbeBitratebps = m_llMaxBitratebps > 0 ? m_llMaxBitratebps : kDefaultMaxProbingBitrateBps;

		if (llBitrate > llMaxProbeBitratebps) 
		{
			llBitrate = llMaxProbeBitratebps;
			bProbeFurther = TPR_FALSE;
		}

		m_pacer.CreateProbeCluster(llBitrate);
	}

	m_llTimeLastProbingInitiatedMs = llNowMs;
  
	if (bProbeFurther) 
	{
		m_state = kWaitingForProbingResult;
		m_llMinBitrateToProbeFurtherbps = (plBitratesToprobes->back()) * kRepeatedProbeMinPercentage / 100;  //70%
	}
	else
	{
		NPQ_DEBUG("[PROBING]kProbingComplete");
		m_state = kProbingComplete;
		m_llMinBitrateToProbeFurtherbps = kExponentialProbingDisabled;  //0
	}
	return NPQ_OK;
}

