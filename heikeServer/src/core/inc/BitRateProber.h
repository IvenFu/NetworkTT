/**	@BitRateProber.h
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

#ifndef _BITRATE_PROBER_H_
#define _BITRATE_PROBER_H_

#include "TPR_Time.h"
#include <queue>

using std::queue;

/**	@struct	 PacedPacketInfo
 *	@brief  探测目标信息
 *	@note	 
 */
struct PacedPacketInfo 
{
	PacedPacketInfo() {}
	PacedPacketInfo(int iProbeClusterIdI,int iProbeClusterMinProbesI,int iProbeClusterMinBytesI)
		:iSendBitratebps(-1),
		iProbeClusterId(iProbeClusterIdI),
		iProbeClusterMinProbes(iProbeClusterMinProbesI),
		iProbeClusterMinBytes(iProbeClusterMinBytesI) 
	{
	}

	TPR_BOOL operator==(const PacedPacketInfo& rhs) const 
	{
		return iSendBitratebps == rhs.iSendBitratebps &&
			iProbeClusterId == rhs.iProbeClusterId &&
			iProbeClusterMinProbes == rhs.iProbeClusterMinProbes &&
			iProbeClusterMinBytes == rhs.iProbeClusterMinBytes;
	}

	static const int kNotAProbe = -1;	//非探测标志
	int iSendBitratebps;				//探测目标码率
	int iProbeClusterId;				//探测唯一id
	int iProbeClusterMinProbes;			//探测已满足的次数
	int iProbeClusterMinBytes;			//探测已满足的字节数
};

/**	@struct	 ProbeCluster
 *	@brief  探测簇信息
 *	@note	 
 */
struct ProbeCluster 
{
	ProbeCluster()
		:iSentProbes(0),
		iSentBytes(0),
		llTimeCreatedMs(-1),
		llTimeStartedMs(-1),
		iRetries(0)
	{
	}

	PacedPacketInfo paceInfo;	//探测目标信息
	int iSentProbes;			//已发送探测的次数
	int iSentBytes;				//已发送探测的字节数
	TPR_INT64 llTimeCreatedMs;	//创建probing时间
	TPR_INT64 llTimeStartedMs;	//开启发送第一波探测数据的时间
	int iRetries;				//异常重置次数
};

/**	@enum EN_PROBING_STATE
 *	@brief  整个探测功能的状态
 *	@note	 
 */
enum EN_PROBING_STATE 
{
	kDisabled,		//禁用
	kInactive,		//待激活
	kActive,		//激活
	kSuspended,		//挂起，注：探测发送完毕后，进入挂起状态
};

/**	@class	BitrateProber
 *	@brief  探测功能模块
 *	@note	 
 */
class BitrateProber
{
public:
	/*构造函数*/
	BitrateProber();

	/*析构函数*/
	~BitrateProber();
	
	/*开启功能*/
	void SetEnabled(TPR_BOOL bEnable);
	
	/*是否处于探测状态*/
	TPR_BOOL IsProbing() const;
	
	/*pacing模块输入数据并同步通知probing模块，激活相应探测功能*/
	void OnIncomingPacket(int iPacketSize);
	
	/*创建探测簇信息*/
	void CreateProbeCluster(int iBitrateBps, TPR_INT64 llNowMs);
	
	/*计算下一次发送探测数据的时间*/
	int TimeUntilNextProbe(TPR_INT64 llNowMs);
	
	/*获取当前正在探测的id*/
	int CurrentClusterId() const;
	
	/*获取建议待发送数据量*/
	int RecommendedMinProbeSize() const;
	
	/*发送数据统计*/
	void ProbeSent(TPR_INT64 llNowMs, int iBytes);

private:
	/*重置探测信息，用于某些异常情况恢复*/
	void ResetState(TPR_INT64 llNowMs);
	
	/*根据已发送的数据量计算下一次发送探测包的时间点*/
	TPR_INT64 GetNextProbeTime(const ProbeCluster& cluster);

	EN_PROBING_STATE m_enProbingState;		//探测状态
	queue<ProbeCluster> m_clusters;			//探测簇队列
	TPR_INT64 m_llNextProbeTimeMs;			//下一次发送探测数据的时间
	int m_iNextClusterId;					//探测id增长记录
};
#endif 
