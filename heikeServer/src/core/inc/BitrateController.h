#ifndef _BITRATECONTROL_H_
#define _BITRATECONTROL_H_

#if 0
class RateStatistics 
{
public:
    static const float kBpsScale = 8000.0f;

    // max_window_size_ms = Maximum window size in ms for the rate estimation.
    //                      Initial window size is set to this, but may be changed
    //                      to something lower by calling SetWindowSize().
    // scale = coefficient to convert counts/ms to desired unit
    //         ex: kBpsScale (8000) for bits/s if count represents bytes.
    RateStatistics(int64_t max_window_size_ms, float scale);
    ~RateStatistics();

    // Reset instance to original state.
    void Reset();

    // Update rate with a new data point, moving averaging window as needed.
    void Update(size_t count, int64_t now_ms);

    // Note that despite this being a const method, it still updates the internal
    // state (moves averaging window), but it doesn't make any alterations that
    // are observable from the other methods, as long as supplied timestamps are
    // from a monotonic clock. Ie, it doesn't matter if this call moves the
    // window, since any subsequent call to Update or Rate would still have moved
    // the window as much or more.
    uint32_t Rate(int64_t now_ms) ;

    // Update the size of the averaging window. The maximum allowed value for
    // window_size_ms is max_window_size_ms as supplied in the constructor.
    bool SetWindowSize(int64_t window_size_ms, int64_t now_ms);

private:
    void EraseOld(int64_t now_ms);
    bool IsInitialized() ;

    // Counters are kept in buckets (circular buffer), with one bucket
    // per millisecond.
    struct Bucket 
    {
        size_t sum;      // Sum of all samples in this bucket.
        size_t samples;  // Number of samples in this bucket.
    };
    Bucket buckets_;

    // Total count recorded in buckets.
    int accumulated_count_;

    // The total number of samples in the buckets.
    int num_samples_;

    // Oldest time recorded in buckets.
    int64_t oldest_time_;

    // Bucket index of oldest counter recorded in buckets.
    uint32_t oldest_index_;

    // To convert counts/ms to desired units
    const float scale_;

    // The window sizes, in ms, over which the rate is calculated.
    const int64_t max_window_size_ms_;
    int64_t current_window_size_ms_;
};


class BitrateController 
{
    // This class collects feedback from all streams sent to a peer (via
    // RTCPBandwidthObservers). It does one  aggregated send side bandwidth
    // estimation and divide the available bitrate between all its registered
    // BitrateObservers.
public:
    static const int kDefaultStartBitratebps = 300000;

    // Deprecated:
    // TODO(perkj): BitrateObserver has been deprecated and is not used in WebRTC.
    // Remove this method once other other projects does not use it.
    static BitrateController* CreateBitrateController(BitrateObserver* observer, RtcEventLog* event_log);

    static BitrateController* CreateBitrateController( RtcEventLog* event_log);

    virtual ~BitrateController() {}

    virtual RtcpBandwidthObserver* CreateRtcpBandwidthObserver() = 0;

    // Deprecated
    virtual void SetStartBitrate(int start_bitrate_bps) = 0;
    // Deprecated
    virtual void SetMinMaxBitrate(int min_bitrate_bps, int max_bitrate_bps) = 0;
    virtual void SetBitrates(int start_bitrate_bps,
        int min_bitrate_bps,
        int max_bitrate_bps) = 0;

    virtual void ResetBitrates(int bitrate_bps,
        int min_bitrate_bps,
        int max_bitrate_bps) = 0;

    virtual void OnDelayBasedBweResult(const Result& result) = 0;

    // Gets the available payload bandwidth in bits per second. Note that
    // this bandwidth excludes packet headers.
    virtual bool AvailableBandwidth(uint32_t* bandwidth) const = 0;

    virtual void SetReservedBitrate(uint32_t reserved_bitrate_bps) = 0;

    virtual bool GetNetworkParameters(uint32_t* bitrate,
        uint8_t* fraction_loss,
        int64_t* rtt) = 0;
};



class BitrateControllerImpl : public BitrateController 
{
public:
    // TODO(perkj): BitrateObserver has been deprecated and is not used in WebRTC.
    // |observer| is left for project that is not yet updated.
    BitrateControllerImpl(Clock* clock,BitrateObserver* observer, RtcEventLog* event_log);
    virtual ~BitrateControllerImpl() {}

    bool AvailableBandwidth(uint32_t* bandwidth) const override;

    RtcpBandwidthObserver* CreateRtcpBandwidthObserver() override;

    // Deprecated
    void SetStartBitrate(int start_bitrate_bps) override;
    // Deprecated
    void SetMinMaxBitrate(int min_bitrate_bps, int max_bitrate_bps) override;

    void SetBitrates(int start_bitrate_bps, int min_bitrate_bps, int max_bitrate_bps) override;

    void ResetBitrates(int bitrate_bps, int min_bitrate_bps, int max_bitrate_bps) override;


    void SetReservedBitrate(uint32_t reserved_bitrate_bps) override;

    // Returns true if the parameters have changed since the last call.
    bool GetNetworkParameters(uint32_t* bitrate, uint8_t* fraction_loss, int64_t* rtt) override;

    void OnDelayBasedBweResult(const Result& result) override;

    int64_t TimeUntilNextProcess() override;
    void Process() override;

private:
    class RtcpBandwidthObserverImpl;

    // Called by BitrateObserver's direct from the RTCP module.
    void OnReceiverEstimatedBitrate(uint32_t bitrate);

    void OnReceivedRtcpReceiverReport(uint8_t fraction_loss,
        int64_t rtt,
        int number_of_packets,
        int64_t now_ms);

    // Deprecated
    void MaybeTriggerOnNetworkChanged();

    void OnNetworkChanged(uint32_t bitrate,
        uint8_t fraction_loss,  // 0 - 255.
        int64_t rtt) EXCLUSIVE_LOCKS_REQUIRED(critsect_);

    // Used by process thread.

    BitrateObserver* const observer_;
    int64_t last_bitrate_update_ms_;
    RtcEventLog* const event_log_;

    rtc::CriticalSection critsect_;
    SendSideBandwidthEstimation bandwidth_estimation_ GUARDED_BY(critsect_);
    uint32_t reserved_bitrate_bps_ GUARDED_BY(critsect_);

    uint32_t last_bitrate_bps_ GUARDED_BY(critsect_);
    uint8_t last_fraction_loss_ GUARDED_BY(critsect_);
    int64_t last_rtt_ms_ GUARDED_BY(critsect_);
    uint32_t last_reserved_bitrate_bps_ GUARDED_BY(critsect_);

    RTC_DISALLOW_IMPLICIT_CONSTRUCTORS(BitrateControllerImpl);
};
#endif
#endif