
#ifndef _BASE_IMPL_H_
#define _BASE_IMPL_H_

#include "NPQosImpl.h"

class InnerParam
{
public:
	InnerParam(NPQ_QOS_MAIN_TYPE enMainType)
		:m_nRttUs(NPQ_DEFAULT_RTT)
		,m_nRealRttUs(0)
		,m_enMainType(enMainType)
		,m_cLossFraction(0)
	{	
	}

	~InnerParam(){}

	unsigned int m_nRttUs;   //����filter������RTTֵ ��λus
	unsigned int m_nRealRttUs;  //ʵʱRTTֵ
	NPQ_QOS_MAIN_TYPE m_enMainType; 
	unsigned char m_cLossFraction;  //������   1/256
};

#endif
