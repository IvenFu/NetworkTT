#ifndef _VCMJITTER_H_
#define _VCMJITTER_H_

#include <list>
#include <map>
#include <memory>
#include <set>
#include <vector>
#include "TPR_Utils.h"
#include "Util.h"
#include "rtt.h"
#include "buffer.h"
#include "VCMTiming.h"
#include "baseImpl.h"

enum VCMNackMode { kNackMode, kNoNackMode };

struct VCMJitterSample 
{
    VCMJitterSample() : nTimestamp(0), nFrameSize(0), nLatestPacketTime(-1) {}
    TPR_UINT32 nTimestamp;
    TPR_UINT32 nFrameSize;
    TPR_INT64  nLatestPacketTime;
};


class VCMInterFrameDelay 
{
public:
    explicit VCMInterFrameDelay();

    // Resets the estimate. Zeros are given as parameters.
    void Reset();

    // Calculates the delay of a frame with the given timestamp.
    // This method is called when the frame is complete.
    //
    // Input:
    //          - timestamp         : RTP timestamp of a received frame
    //          - *delay            : Pointer to memory where the result should be
    //          stored
    //          - currentWallClock  : The current time in milliseconds.
    //                                Should be -1 for normal operation, only used
    //                                for testing.
    // Return value                 : true if OK, false when reordered timestamps
    bool CalculateDelay(TPR_UINT32 timestamp, TPR_INT64* delay, TPR_INT64 currentWallClock, float fscale);

    // Returns the current difference between incoming timestamps
    //
    // Return value                 : Wrap-around compensated difference between
    // incoming
    //                                timestamps.
    TPR_UINT32 CurrentTimeStampDiffMs() const;

private:
    // Controls if the RTP timestamp counter has had a wrap around
    // between the current and the previously received frame.
    //
    // Input:
    //          - timestmap         : RTP timestamp of the current frame.
    void CheckForWrapArounds(TPR_UINT32 timestamp);

    TPR_INT32 _wrapArounds;    // Number of wrapArounds detected
    // The previous timestamp passed to the delay estimate
    TPR_UINT32 _prevTimestamp;
    // The previous wall clock timestamp used by the delay estimate
    TPR_INT64 _prevWallClock;
    // Wrap-around compensated difference between incoming timestamps
    TPR_INT64 _dTS;
};


class VCMJitterEstimator 
{
public:
    VCMJitterEstimator(InnerParam& innerParam);
    virtual ~VCMJitterEstimator();

    // Resets the estimate to the initial state
    void Reset();
    void ResetNackCount();

    // Updates the jitter estimate with the new data.
    //
    // Input:
    //          - frameDelay      : Delay-delta calculated by UTILDelayEstimate in
    //          milliseconds
    //          - frameSize       : Frame size of the current frame.
    //          - incompleteFrame : Flags if the frame is used to update the
    //          estimate before it
    //                              was complete. Default is false.
    void UpdateEstimate(TPR_INT64 frameDelayMS, TPR_UINT32 frameSizeBytes, bool incompleteFrame = false);

    // Returns the current jitter estimate in milliseconds and adds
    // also adds an RTT dependent term in cases of retransmission.
    //  Input:
    //          - rttMultiplier  : RTT param multiplier (when applicable).
    //
    // Return value                   : Jitter estimate in milliseconds
    virtual int GetJitterEstimate(double rttMultiplier);

    // Updates the nack counter.
    void FrameNacked();

    // Updates the RTT filter.
    //
    // Input:
    //          - rttMs               : RTT in ms
    void UpdateRtt(TPR_INT64 rttMs);

    void UpdateMaxFrameSize(TPR_UINT32 frameSizeBytes);

    // A constant describing the delay from the jitter buffer
    // to the delay on the receiving side which is not accounted
    // for by the jitter buffer nor the decoding delay estimate.
    static const TPR_UINT32 OPERATING_SYSTEM_JITTER = 10;

protected:
    // These are protected for better testing possibilities
    double _theta[2];  // Estimated line parameters (slope, offset)
    double _varNoise;  // Variance of the time-deviation from the line

private:
    // Updates the Kalman filter for the line describing
    // the frame size dependent jitter.
    //
    // Input:
    //          - frameDelayMS    : Delay-delta calculated by UTILDelayEstimate in
    //          milliseconds
    //          - deltaFSBytes    : Frame size delta, i.e.
    //                            : frame size at time T minus frame size at time
    //                            T-1
    void KalmanEstimateChannel(TPR_INT64 frameDelayMS, TPR_INT32 deltaFSBytes);

    // Updates the random jitter estimate, i.e. the variance
    // of the time deviations from the line given by the Kalman filter.
    //
    // Input:
    //          - d_dT              : The deviation from the kalman estimate
    //          - incompleteFrame   : True if the frame used to update the
    //          estimate
    //                                with was incomplete
    void EstimateRandomJitter(double d_dT, bool incompleteFrame);

    double NoiseThreshold() const;

    // Calculates the current jitter estimate.
    //
    // Return value                 : The current jitter estimate in milliseconds
    double CalculateEstimate();

    // Post process the calculated estimate
    void PostProcessEstimate();

    // Calculates the difference in delay between a sample and the
    // expected delay estimated by the Kalman filter.
    //
    // Input:
    //          - frameDelayMS    : Delay-delta calculated by UTILDelayEstimate in
    //          milliseconds
    //          - deltaFS         : Frame size delta, i.e. frame size at time
    //                              T minus frame size at time T-1
    //
    // Return value                 : The difference in milliseconds
    double DeviationFromExpectedDelay(TPR_INT64 frameDelayMS,
        TPR_INT32 deltaFSBytes) const;

    double GetFrameRate() const;

    // Constants, filter parameters
    const double _phi;
    const double _psi;
    const TPR_UINT32 _alphaCountMax;
    const double _thetaLow;
    const TPR_UINT32 _nackLimit;
    const TPR_INT32 _numStdDevDelayOutlier;
    const TPR_INT32 _numStdDevFrameSizeOutlier;
    const double _noiseStdDevs;
    const double _noiseStdDevOffset;

    double _thetaCov[2][2];  // Estimate covariance
    double _Qcov[2][2];      // Process noise covariance
    double _avgFrameSize;    // Average frame size
    double _varFrameSize;    // Frame size variance
    double _maxFrameSize;    // Largest frame size received (descending
    // with a factor _psi)
    TPR_UINT32 _fsSum;
    TPR_UINT32 _fsCount;

    TPR_INT64 _lastUpdateT;
    double _prevEstimate;     // The previously returned jitter estimate
    TPR_UINT32 _prevFrameSize;  // Frame size of the previous frame
    double _avgNoise;         // Average of the random jitter
    TPR_UINT32 _alphaCount;
    double _filterJitterEstimate;  // The filtered sum of jitter estimates

    TPR_UINT32 _startupCount;

    TPR_INT64
        _latestNackTimestamp;  // Timestamp in ms when the latest nack was seen
    TPR_UINT32 _nackCount;       // Keeps track of the number of nacks received,
    // but never goes above _nackLimit
    VCMRttFilter _rttFilter;

    enum ExperimentFlag { kInit, kEnabled, kDisabled };
    InnerParam& m_innerParam;		    //内部参数
	TPR_INT64 m_lastRestNackTimeMs;		//重置nack参数时间
};



class VCMJitterBuffer 
{
public:
    VCMJitterBuffer(InnerParam& innerParam);

    ~VCMJitterBuffer();

    // Initializes and starts jitter buffer.
    void Start();

    // Signals all internal events and stops the jitter buffer.
    void Stop();

    // Returns true if the jitter buffer is running.
    bool Running() const;

    // Empty the jitter buffer of all its data.
    void Flush();

    // Gets number of packets received.
    int num_packets() const;

    // Gets number of duplicated packets received.
    int num_duplicated_packets() const;

    // Gets number of packets discarded by the jitter buffer.
    int num_discarded_packets() const;

    // Statistics, Calculate frame and bit rates.
    void IncomingRateStatistics(unsigned int* framerate, unsigned int* bitrate);

    // Returns the estimated jitter in milliseconds.
    TPR_UINT32 EstimatedJitterMs();

    TPR_INT64 LastPacketTime(FrameBuffer* frame, bool* retransmitted);

    // The following three functions update the jitter estimate with the
    // payload size, receive time and RTP timestamp of a frame.
    void UpdateJitterEstimate(const VCMJitterSample& sample, float fscale, bool incomplete_frame);

    void UpdateJitterEstimate(TPR_INT64 latest_packet_time_ms, TPR_UINT32 timestamp, unsigned int frame_size, float fscale, bool incomplete_frame);

    // Updates the nack counter.
    void FrameNacked();

    void Reset();


private:
    class SequenceNumberLessThan 
    {
    public:
        bool operator()(const TPR_UINT16& sequence_number1, const TPR_UINT16& sequence_number2) const
        {
             return IsNewerSequenceNumber(sequence_number2, sequence_number1);
        }
    };

    int NonContinuousOrIncompleteDuration();

    // If we are running (have started) or not.
    bool running_;
    TPR_Mutex m_lock;

    bool first_packet_since_reset_;

    // Latest calculated frame rates of incoming stream.
    unsigned int incoming_frame_rate_;
    unsigned int incoming_frame_count_;
    TPR_INT64 time_last_incoming_frame_count_;
    unsigned int incoming_bit_count_;
    unsigned int incoming_bit_rate_;
    // Number of packets in a row that have been too old.
    int num_consecutive_old_packets_;
    // Number of packets received.
    int num_packets_ ;
    // Number of duplicated packets received.
    int num_duplicated_packets_ ;
    // Number of packets discarded by the jitter buffer.
    int num_discarded_packets_ ;
    // Time when first packet is received.
    TPR_INT64 time_first_packet_ms_ ;

    // Jitter estimation.
    // Filter for estimating jitter.
    VCMJitterEstimator jitter_estimate_;
    // Calculates network delays used for jitter calculations.
    VCMInterFrameDelay inter_frame_delay_;
    VCMJitterSample waiting_for_completion_;

    // NACK and retransmissions.
    VCMNackMode nack_mode_;
    TPR_INT64 low_rtt_nack_threshold_ms_;
    TPR_INT64 high_rtt_nack_threshold_ms_;
    // Holds the internal NACK list (the missing sequence numbers).
    TPR_UINT16 latest_received_sequence_number_;
    size_t max_nack_list_size_;
    int max_packet_age_to_nack_;  // Measured in sequence numbers.
    int max_incomplete_time_ms_;

    // Estimated rolling average of packets per frame
    float average_packets_per_frame_;
    // average_packets_per_frame converges fast if we have fewer than this many
    // frames.
    int frame_counter_;
    InnerParam& m_innerParam;		    //内部参数

};


#endif