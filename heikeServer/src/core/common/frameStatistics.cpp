
#include "TPR_Guard.h"
#include "timeUtil.h"
#include "Util.h"
#include "frameStatistics.h"


#define FRAME_COUNTER_WINDOW_MS     1000


FrameStatistics::FrameStatistics()
{

}

FrameStatistics::~FrameStatistics()
{

}

int FrameStatistics::UpdateFrame(TPR_INT64 llNowMs)
{
	TPR_Guard gurad(&m_lock);

	EraseOld(llNowMs);

	m_frameCounter.push_back(llNowMs);
	return m_frameCounter.size();
}

int FrameStatistics::FrameRate()
{
	TPR_Guard gurad(&m_lock);

	EraseOld(OneTime::OneNowMs());

	return m_frameCounter.size();
}

void FrameStatistics::EraseOld(TPR_INT64 llNowMs)
{
	TPR_INT64 llTimeOut = llNowMs - FRAME_COUNTER_WINDOW_MS;
	while(!m_frameCounter.empty() && m_frameCounter.front()<llTimeOut)
	{
		m_frameCounter.pop_front();
	}
}
