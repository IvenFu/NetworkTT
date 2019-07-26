/**	@ProbeControler.cpp
*	@note 111.
*	@brief ����̽�����
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

const TPR_INT64 kMaxWaitingTimeForProbingResultMs = 1000;	//̽�ⳬʱʱ��
const int kExponentialProbingDisabled = 0;					//����̽�������Чֵ
const TPR_INT64 kDefaultMaxProbingBitrateBps = 5000000;		//Ĭ�ϵ��������
const int kRepeatedProbeMinPercentage = 70;					//����̽����С�ٷֱ�
const double kBitrateDropThreshold = 0.66;					//�ж��Ƿ���Ҫ��¼ͻ�����ʵı���

/**	@fn ProbeController::ProbeController
*	@brief ���캯��
*	@param PacedSender& pacer  pacing����
*	@return void
*/
ProbeController::ProbeController(PacedSender& pacer)
	: m_pacer(pacer)
{
	Reset();
}

/**	@fn ProbeController::Reset
*	@brief ���ò���
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
*	@brief ���������������Լ���ʼ����
*	@param TPR_INT64 llMinBitratebps ��������
*	@param TPR_INT64 llStartBitratebps  ��ʼ����
*	@param TPR_INT64 llMaxBitratebps �������
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
*	@brief ��ʼ��̽��
*	@return ��״̬��
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
*	@brief ����ӵ��ģ����������
*	@param TPR_INT64 llBitratebps ����
*	@return void
*/
void ProbeController::SetEstimatedBitrate(TPR_INT64 llBitratebps) 
{
	TPR_Guard gurad(&m_lock);

	NPQ_DEBUG("[PROBING]llBitratebps = %lld,m_state=%d",llBitratebps,m_state);

	TPR_INT64 llNowMs = OneTime::OneNowMs();

	if (m_state == kWaitingForProbingResult) 
	{
		//1�볬ʱʱ��״̬��waiting�ó�complete
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
				//ʵ�����ʱ� min_bitrate_to_probe_further_bps_(70% Ŀ������) ��Ҫ����2��ֵ����̽��
				list<TPR_INT64> lBitrates;

				NPQ_DEBUG("[PROBING] increacing probing %lld",2 * llBitratebps);
				lBitrates.push_back(2 * llBitratebps);
				InitiateProbing(llNowMs, &lBitrates, TPR_TRUE);
			}
		}
	}

	//�����������µ���0.66���£���ô��¼�µ�ǰ�����ʺ�ʱ�� 
						//0.66
	if (llBitratebps < kBitrateDropThreshold * m_llEstimatedBitratebps) 
	{
		llTimeOfLastLargeDropMs = llNowMs;
		llBitrateBeforeLastLargeDropbps = m_llEstimatedBitratebps;
	}
	m_llEstimatedBitratebps = llBitratebps;
}

/**	@fn BitrateProber::InitiateProbing
*	@brief ��ʼ��̽��
*	@param TPR_INT64 llNowMs ��ǰʱ��
*	@param list<TPR_INT64>* plBitratesToprobes ��̽���Ŀ�����ʼ���
*	@param TPR_BOOL bProbeFurther  �Ƿ����̽��
*	@return ��״̬��
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

