#ifndef _VCMTIMING_H_
#define _VCMTIMING_H_

#include "TPR_Utils.h"
#include "Util.h"
#include "timeUtil.h"
#include "jitterEstimate.h"

class VCMTiming 
{
public:
    // The primary timing component should be passed
    // if this is the dual timing component.
    explicit VCMTiming();
    virtual ~VCMTiming();

    // Resets the timing to the initial state.
    void Reset();
    void ResetDecodeTime();

    // Set the amount of time needed to render an image. Defaults to 10 ms.
    void set_render_delay(int render_delay_ms);

    // Set the minimum time the video must be delayed on the receiver to
    // get the desired jitter buffer level.
    void SetJitterDelay(int required_delay_ms);

    // Set the minimum playout delay from capture to render in ms.
    void set_min_playout_delay(int min_playout_delay_ms);

    // Returns the minimum playout delay from capture to render in ms.
    int min_playout_delay();

    // Set the maximum playout delay from capture to render in ms.
    void set_max_playout_delay(int max_playout_delay_ms);

    // Returns the maximum playout delay from capture to render in ms.
    int max_playout_delay();

    // Increases or decreases the current delay to get closer to the target delay.
    // Calculates how long it has been since the previous call to this function,
    // and increases/decreases the delay in proportion to the time difference.
    void UpdateCurrentDelay(TPR_UINT32 frame_timestamp, float fScale);

    // Increases or decreases the current delay to get closer to the target delay.
    // Given the actual decode time in ms and the render time in ms for a frame,
    // this function calculates how late the frame is and increases the delay
    // accordingly.
    void UpdateCurrentDelay(TPR_INT64 render_time_ms,
        TPR_INT64 actual_decode_time_ms);

	int CurrentDelayMs(){return nCurrentDelayMs;}

    // Stops the decoder timer, should be called when the decoder returns a frame
    // or when the decoded frame callback is called.
    TPR_INT32 StopDecodeTimer(TPR_UINT32 time_stamp,
        TPR_INT32 decode_time_ms,
        TPR_INT64 now_ms,
        TPR_INT64 render_time_ms);

    // Used to report that a frame is passed to decoding. Updates the timestamp
    // filter which is used to map between timestamps and receiver system time.
    void IncomingTimestamp(TPR_UINT32 time_stamp, TPR_INT64 last_packet_time_ms, float fScale);
    // Returns the receiver system time when the frame with timestamp
    // frame_timestamp should be rendered, assuming that the system time currently
    // is now_ms.
    virtual TPR_INT64 RenderTimeMs(TPR_UINT32 frame_timestamp, TPR_INT64 now_ms, float fScale) ;

    // Returns the current target delay which is required delay + decode time +
    // render delay.
    int TargetVideoDelay() const;

    // Calculates whether or not there is enough time to decode a frame given a
    // certain amount of processing time.
    bool EnoughTimeToDecode(TPR_UINT32 available_processing_time_ms) const;

    // Return current timing information.
    void GetTimings(int* decode_ms,
        int* max_decode_ms,
        int* current_delay_ms,
        int* target_delay_ms,
        int* jitter_buffer_ms,
        int* min_playout_delay_ms,
        int* render_delay_ms) const;

    enum { kDefaultRenderDelayMs = 10 };
    enum { kDelayMaxChangeMsPerS = 100 };

protected:
    int RequiredDecodeTimeMs() const ;
    TPR_INT64 RenderTimeMsInternal(TPR_UINT32 frame_timestamp, TPR_INT64 now_ms, float fScale);
    int TargetDelayInternal() const ;

private:

    TPR_Mutex m_lock;				//保护锁

    TimestampExtrapolator m_timer;		//基准时间估计
    int render_delay_ms_;
    // Best-effort playout delay range for frames from capture to render.
    // The receiver tries to keep the delay between |min_playout_delay_ms_|
    // and |max_playout_delay_ms_| taking the network jitter into account.
    // A special case is where min_playout_delay_ms_ = max_playout_delay_ms_ = 0,
    // in which case the receiver tries to play the frames as they arrive.
    int min_playout_delay_ms_;
    int max_playout_delay_ms_;
    int jitter_delay_ms_;
    int nCurrentDelayMs;
    int last_decode_ms_;
    TPR_UINT32 nPrevFrameTimestamp;

    // Statistics.
    size_t num_decoded_frames_;
    size_t num_delayed_decoded_frames_;
    TPR_INT64 first_decoded_frame_ms_;
    TPR_UINT64 sum_missed_render_deadline_ms_;

    TPR_UINT64 m_nFirstRenderTime;
    TPR_UINT64 m_nFirstComTime;
    TPR_BOOL  m_bFirst;
};


#endif