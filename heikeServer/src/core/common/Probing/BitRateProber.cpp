/**	@BitRateProber.cpp
*	@note 111.
*	@brief ����̽����ģ��
*
*	@author		222
*	@date		2018/4/18
*
*	@note 
*
*	@warning 
*/

#include "Util.h"
#include "BitRateProber.h"

const int kMinProbeDeltaMs = 1;						//���̽��ʱ����
const int kMinProbePacketsSent = 5;					//һ��̽�����С�������ݴ���
const int kMinProbeDurationMs = 15;					//һ��̽�����С��������������������ת��Ϊʱ�䣩
const int kMaxProbeDelayMs = 3;						//�����쳣��������ͺ�ʱ�䣬�������reset
const int kMaxRetryAttempts = 3;					//�쳣�ָ�����
const int kMinProbePacketSize = 200;				//һ�η���������С������200�ֽ�
const TPR_INT64 kProbeClusterTimeoutMs = 5000;		//�����ʱʱ�������̽���

/**	@fn BitrateProber::BitrateProber
*	@brief ���캯��
*	@return void
*/
BitrateProber::BitrateProber()
    : m_enProbingState(kDisabled),
      m_llNextProbeTimeMs(-1),
      m_iNextClusterId(0)
{
	SetEnabled(TPR_TRUE);
}

/**	@fn BitrateProber::~BitrateProber
*	@brief ��������
*	@return void
*/
BitrateProber::~BitrateProber()
{

}

/**	@fn BitrateProber::SetEnabled
*	@brief ����̽�⹦��
*	@param TPR_BOOL bEnable  0/1
*	@return void
*/
void BitrateProber::SetEnabled(TPR_BOOL bEnable) 
{
	if (bEnable) 
	{
		if (m_enProbingState == kDisabled) 
		{
			m_enProbingState = kInactive;
			NPQ_DEBUG("[PROBING]kInactive");
		}
	} 
	else 
	{
		m_enProbingState = kDisabled;
		NPQ_DEBUG("[PROBING]kDisabled");
	}
}

/**	@fn BitrateProber::IsProbing
*	@brief �Ƿ���̽��״̬
*	@return TRUE/FALSE
*/
TPR_BOOL BitrateProber::IsProbing() const 
{
  return m_enProbingState == kActive;
}

/**	@fn BitrateProber::OnIncomingPacket
*	@brief pacingģ���������ݲ�ͬ��֪ͨprobingģ�飬������Ӧ̽�⹦��
*	@param int iBitrateBps  ̽���Ŀ������
*	@param TPR_INT64 llNowMs ��ǰʱ��
*	@return void
*/
void BitrateProber::OnIncomingPacket(int iPacketSize) 
{
	if (m_enProbingState == kInactive && !m_clusters.empty() && iPacketSize >=npq_min(RecommendedMinProbeSize(), kMinProbePacketSize)) 
	{
		m_llNextProbeTimeMs = -1;
		m_enProbingState = kActive;
		NPQ_DEBUG("[PROBING]kActive");
    }
}

/**	@fn BitrateProber::CreateProbeCluster
*	@brief ����̽�����Ϣ
*	@param int iBitrateBps  ̽���Ŀ������
*	@param TPR_INT64 llNowMs ��ǰʱ��
*	@return void
*/
void BitrateProber::CreateProbeCluster(int iBitrateBps, TPR_INT64 llNowMs) 
{
	if(m_enProbingState == kDisabled)
	{
		return;
	}

	if(iBitrateBps ==0)
	{
		return;
	}

	//ɾ��5�볬ʱ��cluster
	while (!m_clusters.empty() && llNowMs - m_clusters.front().llTimeCreatedMs > kProbeClusterTimeoutMs) 
	{
		m_clusters.pop();
	}

	ProbeCluster cluster;
	cluster.llTimeCreatedMs = llNowMs;
	cluster.paceInfo.iProbeClusterMinProbes = kMinProbePacketsSent; //��С5����
	cluster.paceInfo.iProbeClusterMinBytes = iBitrateBps * kMinProbeDurationMs / 8000;  //15ms�ֽ���
	
	NPQ_DEBUG("[PROBING]m_clusters create %d",iBitrateBps);
	cluster.paceInfo.iSendBitratebps = iBitrateBps;
	cluster.paceInfo.iProbeClusterId = m_iNextClusterId++;
	m_clusters.push(cluster);
  
	if (m_enProbingState != kActive)
	{
		m_enProbingState = kInactive;
	}
}

/**	@fn BitrateProber::ResetState
*	@brief ����̽����Ϣ������ĳЩ�쳣����ָ�
*	@param TPR_INT64 llNowMs ��ǰʱ��
*	@return void
*/
void BitrateProber::ResetState(TPR_INT64 llNowMs) 
{
	if(m_enProbingState != kActive)
	{
		return;
	}

	queue<ProbeCluster> clusters;
	swap(clusters,m_clusters);

	while (!clusters.empty()) 
	{
		if (clusters.front().iRetries < kMaxRetryAttempts) 
		{
			CreateProbeCluster(clusters.front().paceInfo.iSendBitratebps, llNowMs);
			m_clusters.back().iRetries = clusters.front().iRetries + 1;
		}
		clusters.pop();
	}

	m_enProbingState = kInactive;
}

/**	@fn BitrateProber::TimeUntilNextProbe
*	@brief ����probing����̽���֮���ʱ����, ÿ2 * kMinProbeDeltaMs ���
*	@param TPR_INT64 llNowMs ��ǰʱ��
*	@return ʱ�������쳣����-1
*/
int BitrateProber::TimeUntilNextProbe(TPR_INT64 llNowMs) 
{
	if (m_enProbingState != kActive || m_clusters.empty())
	{
		return -1;
	}

	int iTimeUntilProbeMs = 0;
	if (m_llNextProbeTimeMs >= 0) 
	{
		iTimeUntilProbeMs = m_llNextProbeTimeMs - llNowMs;

		//iTimeUntilProbeMs һ������µ���0~2ms
		//iTimeUntilProbeMs����Ǹ�����˵������һ�����ݺ�ʱ̫�ã�������2ms��С��-3��˵��������5ms��reset
		if (iTimeUntilProbeMs < -kMaxProbeDelayMs) 
		{ 
			//-3ms �����쳣���
			ResetState(llNowMs);
			NPQ_ERROR("ResetState iTimeUntilProbeMs=%d",iTimeUntilProbeMs);
			return -1;
		}
	}
	return npq_max(iTimeUntilProbeMs, 0);
}

/**	@fn BitrateProber::CurrentClusterId
*	@brief ��ȡ��ǰ����̽���Cluster Id
*	@param ��
*	@return ��ǰ����̽���Cluster Id
*/
int BitrateProber::CurrentClusterId() const 
{
	if(m_clusters.empty())
	{
		NPQ_ERROR("m_clusters 1 empty");
		return -1;
	}

	if(m_enProbingState != kActive)
	{
		NPQ_ERROR("m_enProbingState not kActive %d",m_enProbingState);
		return -1;
	}

	return m_clusters.front().paceInfo.iProbeClusterId; //���ص�һ��cluster
}

/**	@fn BitrateProber::RecommendedMinProbeSize
*	@brief ��ȡ���������������
*	@param ��
*	@return ���ݴ�С
*/
int BitrateProber::RecommendedMinProbeSize() const 
{
	if(m_clusters.empty())
	{
		NPQ_ERROR("m_clusters 2 empty");
		return -1;
	}
  
	return m_clusters.front().paceInfo.iSendBitratebps * 2 * kMinProbeDeltaMs /(8 * 1000);  //2ms �ֽ���
}

/**	@fn BitrateProber::ProbeSent
*	@brief ��������ͳ��
*	@param TPR_INT64 llNowMs ��ǰʱ��
*	@param int iBytes �ѷ��������ֽ���
*	@return void
*/
void BitrateProber::ProbeSent(TPR_INT64 llNowMs, int iBytes)
{
	if(m_enProbingState != kActive)
	{
		NPQ_ERROR("m_enProbingState = %d",m_enProbingState);
		return;
	}

	if(iBytes<=0)
	{
		NPQ_ERROR("iBytes error %d",iBytes);
		return ;
	}

	if (!m_clusters.empty()) 
	{
		ProbeCluster* pCluster = &m_clusters.front();
		if (pCluster->iSentProbes == 0)
		{
			if(pCluster->llTimeStartedMs != -1)
			{
				NPQ_ERROR("llTimeStartedMs err = %lld",pCluster->llTimeStartedMs);
				return;
			}
			pCluster->llTimeStartedMs = llNowMs;
		}

		pCluster->iSentBytes += iBytes;
		pCluster->iSentProbes += 1;
		m_llNextProbeTimeMs = GetNextProbeTime(*pCluster);

		//15ms�������� �� 5��  5*2ms=10ms 
		if (pCluster->iSentBytes >= pCluster->paceInfo.iProbeClusterMinBytes &&
			pCluster->iSentProbes >= pCluster->paceInfo.iProbeClusterMinProbes) 
		{
			NPQ_DEBUG("[PROBING]m_clusters pop %d",pCluster->paceInfo.iSendBitratebps);
			m_clusters.pop();
		}
	
		if (m_clusters.empty())
		{
			m_enProbingState = kSuspended;
			NPQ_DEBUG("[PROBING]kSuspended");
		}
	}
}

/**	@fn BitrateProber::GetNextProbeTime
*	@brief �����ѷ��͵�������������һ�η���̽�����ʱ���
*	@param const ProbeCluster& cluster  
*	@return �ɹ�����ʱ�䣬���󷵻�-1
*/
TPR_INT64 BitrateProber::GetNextProbeTime(const ProbeCluster& cluster) 
{
	if(cluster.paceInfo.iSendBitratebps <= 0)
	{
		NPQ_ERROR("iSendBitratebps is valid");
		return -1;
	}

	if(cluster.llTimeStartedMs < 0)
	{
		NPQ_ERROR("llTimeStartedMs is valid");
		return -1;
	}

	//ע�⣺
	//����iSentBytes �� iSendBitratebps ����ѷ������ݴ����ʱ�䣬���ǲ��������������ĵ�ʱ�䡣
	//������Ҫ���͵�ǰ����2ms�����������п�����1ms�ھͷ�������
	TPR_INT64 llDeltaMs =(8000ll * cluster.iSentBytes + cluster.paceInfo.iSendBitratebps / 2) /cluster.paceInfo.iSendBitratebps;   
	
	return cluster.llTimeStartedMs + llDeltaMs;
}

