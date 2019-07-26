
#ifndef _BASE_H_
#define _BASE_H_

#include "Util.h"

class OuterParam;

class Base
{
public:
	Base(OuterParam& struOuterParam);
	
	virtual ~Base();

	virtual int Start() = 0;

	virtual int Stop() = 0;
	
	virtual int InputData(int iDataType,unsigned char* pData, unsigned int nDataLen) = 0;

	virtual int GetStat (NPQ_MAIN_TYPE enMainType,NPQ_STAT* pStat) = 0;

protected:
	OuterParam& m_struOuterParam;
};

#endif
