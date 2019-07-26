/**	@BitRateProber.h
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

#ifndef _BITRATE_PROBER_H_
#define _BITRATE_PROBER_H_

#include "TPR_Time.h"
#include <queue>

using std::queue;

/**	@struct	 PacedPacketInfo
 *	@brief  ̽��Ŀ����Ϣ
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

	static const int kNotAProbe = -1;	//��̽���־
	int iSendBitratebps;				//̽��Ŀ������
	int iProbeClusterId;				//̽��Ψһid
	int iProbeClusterMinProbes;			//̽��������Ĵ���
	int iProbeClusterMinBytes;			//̽����������ֽ���
};

/**	@struct	 ProbeCluster
 *	@brief  ̽�����Ϣ
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

	PacedPacketInfo paceInfo;	//̽��Ŀ����Ϣ
	int iSentProbes;			//�ѷ���̽��Ĵ���
	int iSentBytes;				//�ѷ���̽����ֽ���
	TPR_INT64 llTimeCreatedMs;	//����probingʱ��
	TPR_INT64 llTimeStartedMs;	//�������͵�һ��̽�����ݵ�ʱ��
	int iRetries;				//�쳣���ô���
};

/**	@enum EN_PROBING_STATE
 *	@brief  ����̽�⹦�ܵ�״̬
 *	@note	 
 */
enum EN_PROBING_STATE 
{
	kDisabled,		//����
	kInactive,		//������
	kActive,		//����
	kSuspended,		//����ע��̽�ⷢ����Ϻ󣬽������״̬
};

/**	@class	BitrateProber
 *	@brief  ̽�⹦��ģ��
 *	@note	 
 */
class BitrateProber
{
public:
	/*���캯��*/
	BitrateProber();

	/*��������*/
	~BitrateProber();
	
	/*��������*/
	void SetEnabled(TPR_BOOL bEnable);
	
	/*�Ƿ���̽��״̬*/
	TPR_BOOL IsProbing() const;
	
	/*pacingģ���������ݲ�ͬ��֪ͨprobingģ�飬������Ӧ̽�⹦��*/
	void OnIncomingPacket(int iPacketSize);
	
	/*����̽�����Ϣ*/
	void CreateProbeCluster(int iBitrateBps, TPR_INT64 llNowMs);
	
	/*������һ�η���̽�����ݵ�ʱ��*/
	int TimeUntilNextProbe(TPR_INT64 llNowMs);
	
	/*��ȡ��ǰ����̽���id*/
	int CurrentClusterId() const;
	
	/*��ȡ���������������*/
	int RecommendedMinProbeSize() const;
	
	/*��������ͳ��*/
	void ProbeSent(TPR_INT64 llNowMs, int iBytes);

private:
	/*����̽����Ϣ������ĳЩ�쳣����ָ�*/
	void ResetState(TPR_INT64 llNowMs);
	
	/*�����ѷ��͵�������������һ�η���̽�����ʱ���*/
	TPR_INT64 GetNextProbeTime(const ProbeCluster& cluster);

	EN_PROBING_STATE m_enProbingState;		//̽��״̬
	queue<ProbeCluster> m_clusters;			//̽��ض���
	TPR_INT64 m_llNextProbeTimeMs;			//��һ�η���̽�����ݵ�ʱ��
	int m_iNextClusterId;					//̽��id������¼
};
#endif 
