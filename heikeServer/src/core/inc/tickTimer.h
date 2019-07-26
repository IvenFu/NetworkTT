#ifndef _TICKTIMER_H_
#define _TICKTIMER_H_

#include "Util.h"

//每10ms计一次数
class TickTimer 
{
public:
	class Stopwatch 
	{
	public:
		explicit Stopwatch(const TickTimer& ticktimer);
		~Stopwatch(){}

		TPR_UINT64 ElapsedTicks() const { return ticktimer_.ticks() - starttick_; }

		TPR_UINT64 ElapsedMs() const 
		{
			const TPR_UINT64 elapsed_ticks = ticktimer_.ticks() - starttick_;
			const int ms_per_tick = ticktimer_.ms_per_tick();
			return elapsed_ticks * ms_per_tick;
		}

	private:
		const TickTimer& ticktimer_;
		const TPR_UINT64 starttick_;
	};

	class Countdown {
	public:
		Countdown(const TickTimer& ticktimer, TPR_UINT64 ticks_to_count);
		~Countdown();

		TPR_BOOL Finished() const 
		{
			return stopwatch_->ElapsedTicks() >= ticks_to_count_;
		}

	private:
		Stopwatch* stopwatch_;
		const TPR_UINT64 ticks_to_count_;
	};

	explicit TickTimer() : ms_per_tick_(10),ticks_(0){}
	~TickTimer(){}

	void Increment() { ++ticks_; }

	void Increment(TPR_UINT64 x) { ticks_ += x; }

	TPR_UINT64 ticks() const { return ticks_; }

	int ms_per_tick() const { return ms_per_tick_; }

	void GetNewStopwatch(Stopwatch** ppStopWatch) const ;

	void GetNewCountdown(TPR_UINT64 ticks_to_count, Countdown** ppCountdown) const ;

private:
	TPR_UINT64 ticks_;
	const int ms_per_tick_;
	DISALLOW_COPY_AND_ASSIGN(TickTimer);
};


typedef void (*EventTimerCallback)(void* pUser);

class EventTimer
{
public:
	EventTimer();
	~EventTimer();

	
	int StartTimer(TPR_UINT32 uTimeMs, EventTimerCallback pCallback, void* pUser);

	int StopTimer();
private:
	static TPR_VOIDPTR TimerHandle(TPR_INT32 hEvent, TPR_VOIDPTR pUsrData);

	void TimerHandleRel();
	
	int m_hEvent;
	EventTimerCallback m_pCallback;
	void* m_pUser;
	TPR_BOOL m_bStart;
};



#endif


