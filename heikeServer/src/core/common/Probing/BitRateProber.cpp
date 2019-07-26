/**	@BitRateProber.cpp
*	@note 111.
*	@brief 带宽探测子模块
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

const int kMinProbeDeltaMs = 1;						//最短探测时间间隔
const int kMinProbePacketsSent = 5;					//一个探测簇最小发送数据次数
const int kMinProbeDurationMs = 15;					//一个探测簇最小发送数据量（根据码率转化为时间）
const int kMaxProbeDelayMs = 3;						//数据异常发送最大滞后时间，否则进行reset
const int kMaxRetryAttempts = 3;					//异常恢复次数
const int kMinProbePacketSize = 200;				//一次发送数据最小不超过200字节
const TPR_INT64 kProbeClusterTimeoutMs = 5000;		//清除超时时间以外的探测簇

/**	@fn BitrateProber::BitrateProber
*	@brief 构造函数
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
*	@brief 析构函数
*	@return void
*/
BitrateProber::~BitrateProber()
{

}

/**	@fn BitrateProber::SetEnabled
*	@brief 开启探测功能
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
*	@brief 是否是探测状态
*	@return TRUE/FALSE
*/
TPR_BOOL BitrateProber::IsProbing() const 
{
  return m_enProbingState == kActive;
}

/**	@fn BitrateProber::OnIncomingPacket
*	@brief pacing模块输入数据并同步通知probing模块，激活相应探测功能
*	@param int iBitrateBps  探测的目标码率
*	@param TPR_INT64 llNowMs 当前时间
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
*	@brief 创建探测簇信息
*	@param int iBitrateBps  探测的目标码率
*	@param TPR_INT64 llNowMs 当前时间
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

	//删除5秒超时的cluster
	while (!m_clusters.empty() && llNowMs - m_clusters.front().llTimeCreatedMs > kProbeClusterTimeoutMs) 
	{
		m_clusters.pop();
	}

	ProbeCluster cluster;
	cluster.llTimeCreatedMs = llNowMs;
	cluster.paceInfo.iProbeClusterMinProbes = kMinProbePacketsSent; //最小5个包
	cluster.paceInfo.iProbeClusterMinBytes = iBitrateBps * kMinProbeDurationMs / 8000;  //15ms字节数
	
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
*	@brief 重置探测信息，用于某些异常情况恢复
*	@param TPR_INT64 llNowMs 当前时间
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
*	@brief 计算probing两波探测包之间的时间间隔, 每2 * kMinProbeDeltaMs 间隔
*	@param TPR_INT64 llNowMs 当前时间
*	@return 时间间隔，异常返回-1
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

		//iTimeUntilProbeMs 一般情况下等于0~2ms
		//iTimeUntilProbeMs如果是负数，说明发送一波数据耗时太久，超过了2ms，小于-3，说明超过了5ms，reset
		if (iTimeUntilProbeMs < -kMaxProbeDelayMs) 
		{ 
			//-3ms 属于异常情况
			ResetState(llNowMs);
			NPQ_ERROR("ResetState iTimeUntilProbeMs=%d",iTimeUntilProbeMs);
			return -1;
		}
	}
	return npq_max(iTimeUntilProbeMs, 0);
}

/**	@fn BitrateProber::CurrentClusterId
*	@brief 获取当前正在探测的Cluster Id
*	@param 无
*	@return 当前正在探测的Cluster Id
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

	return m_clusters.front().paceInfo.iProbeClusterId; //返回第一个cluster
}

/**	@fn BitrateProber::RecommendedMinProbeSize
*	@brief 获取建议待发送数据量
*	@param 无
*	@return 数据大小
*/
int BitrateProber::RecommendedMinProbeSize() const 
{
	if(m_clusters.empty())
	{
		NPQ_ERROR("m_clusters 2 empty");
		return -1;
	}
  
	return m_clusters.front().paceInfo.iSendBitratebps * 2 * kMinProbeDeltaMs /(8 * 1000);  //2ms 字节数
}

/**	@fn BitrateProber::ProbeSent
*	@brief 发送数据统计
*	@param TPR_INT64 llNowMs 当前时间
*	@param int iBytes 已发送数据字节数
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

		//15ms的数据量 且 5次  5*2ms=10ms 
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
*	@brief 根据已发送的数据量计算下一次发送探测包的时间点
*	@param const ProbeCluster& cluster  
*	@return 成功返回时间，错误返回-1
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

	//注意：
	//根据iSentBytes 和 iSendBitratebps 算出已发送数据代表的时间，但是不代表发送数据消耗的时间。
	//比如需要发送当前码率2ms的数据量，有可能在1ms内就发送完了
	TPR_INT64 llDeltaMs =(8000ll * cluster.iSentBytes + cluster.paceInfo.iSendBitratebps / 2) /cluster.paceInfo.iSendBitratebps;   
	
	return cluster.llTimeStartedMs + llDeltaMs;
}

