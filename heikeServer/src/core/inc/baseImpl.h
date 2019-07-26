
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

	unsigned int m_nRttUs;   //经过filter处理后的RTT值 单位us
	unsigned int m_nRealRttUs;  //实时RTT值
	NPQ_QOS_MAIN_TYPE m_enMainType; 
	unsigned char m_cLossFraction;  //丢包率   1/256
};

#endif
