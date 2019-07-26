

#ifndef _FRAME_STATISTICS_H_
#define _FRAME_STATISTICS_H_
#include <list>
#include "TPR_Mutex.h"

using std::list;

class FrameStatistics
{
public:
	FrameStatistics();
	~FrameStatistics();

	int UpdateFrame(TPR_INT64 llNowMs);
	
	int FrameRate();

private:
	void EraseOld(TPR_INT64 llNowMs);

	list<TPR_INT64> m_frameCounter;
	TPR_Mutex m_lock;
};


#endif

