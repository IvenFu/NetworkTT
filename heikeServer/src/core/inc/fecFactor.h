#ifndef _FEC_FACTOR_H_
#define _FEC_FACTOR_H_

#include "TPR_tpr.h"

// Number of time periods used for (max) window filter for packet loss
// TODO(marpan): set reasonable window size for filtered packet loss,
// adjustment should be based on logged/real data of loss stats/correlation.
#define  kLossPrHistorySize 10 

// 1000 ms, total filter length is (kLossPrHistorySize * 1000) ms
#define kLossPrShortFilterWinMs  1000

class ExpFilter 
{
public:
	static const float kValueUndefined;

	explicit ExpFilter(float alpha, float max = kValueUndefined)
		: max_(max) 
	{
		Reset(alpha);
	}

	// Resets the filter to its initial state, and resets filter factor base to
	// the given value |alpha|.
	void Reset(float alpha);

	// Applies the filter with a given exponent on the provided sample:
	// y(k) = min(alpha_^ exp * y(k-1) + (1 - alpha_^ exp) * sample, max_).
	float Apply(float exp, float sample);

	// Returns current filtered value.
	float filtered() const { return filtered_; }

	// Changes the filter factor base to the given value |alpha|.
	void UpdateBase(float alpha);

private:
	float alpha_;  // Filter factor base.
	float filtered_;  // Current filter output.
	const float max_;
};

class VCMLossProbabilitySample 
{
public:
	VCMLossProbabilitySample() : lossPr255(0), timeMs(-1) {}

	TPR_UINT8 lossPr255;
	TPR_INT64 timeMs;
};

struct VCMProtectionParameters 
{
	VCMProtectionParameters()
		: rtt(0),
		lossPr(0.0f),
		bitRate(0.0f),
		packetsPerFrame(0.0f),
		packetsPerFrameKey(0.0f),
		frameRate(0.0f),
		keyFrameSize(0.0f),
		fecRateDelta(0),
		fecRateKey(0),
		codecWidth(0),
		codecHeight(0),
		numLayers(1) {}

	TPR_UINT8 rtt;
	float lossPr;
	float bitRate;
	float packetsPerFrame;
	float packetsPerFrameKey;
	float frameRate;
	float keyFrameSize;
	TPR_UINT8 fecRateDelta;
	TPR_UINT8 fecRateKey;
	TPR_UINT16 codecWidth;
	TPR_UINT16 codecHeight;
	int numLayers;
};

enum VCMProtectionMethodEnum 
{ 
	kNack, 
	kFec, 
	kNackFec, 
	kNone 
};

// The type of filter used on the received packet loss reports.
enum FilterPacketLossMode 
{
	kNoFilter,   // No filtering on received loss.
	kAvgFilter,  // Recursive average filter.
	kMaxFilter   // Max-window filter, over the time interval of:
	// (kLossPrHistorySize * kLossPrShortFilterWinMs) ms.
};


class VCMProtectionMethod 
{
public:
	VCMProtectionMethod();
	virtual ~VCMProtectionMethod();

	// Updates the efficiency of the method using the parameters provided
	//
	// Input:
	//         - parameters         : Parameters used to calculate efficiency
	//
	// Return value                 : True if this method is recommended in
	//                                the given conditions.
	virtual TPR_BOOL UpdateParameters(const VCMProtectionParameters* parameters) = 0;

	// Returns the protection type
	//
	// Return value                 : The protection type
	enum VCMProtectionMethodEnum Type() const { return _type; }

	// Extracts the FEC protection factor for Key frame, required by this
	// protection method
	//
	// Return value                 : Required protectionFactor for Key frame
	virtual TPR_UINT8 RequiredProtectionFactorK() { return _protectionFactorK; }

	// Extracts the FEC protection factor for Delta frame, required by this
	// protection method
	//
	// Return value                 : Required protectionFactor for delta frame
	virtual TPR_UINT8 RequiredProtectionFactorD() { return _protectionFactorD; }

	virtual int MaxFramesFec() const { return 1; }

protected:
	TPR_UINT8 _protectionFactorK;
	TPR_UINT8 _protectionFactorD;
	// Estimation of residual loss after the FEC
	float _scaleProtKey;
	TPR_INT32 _maxPayloadSize;

	float _corrFecCost;
	VCMProtectionMethodEnum _type;
};

class VCMFecMethod : public VCMProtectionMethod 
{
public:
	VCMFecMethod();
	virtual ~VCMFecMethod();
	virtual TPR_BOOL UpdateParameters(const VCMProtectionParameters* parameters);
	// Get the FEC protection factors
	TPR_BOOL ProtectionFactor(const VCMProtectionParameters* parameters);
	// Get the boost for key frame protection
	TPR_UINT8 BoostCodeRateKey(TPR_UINT8 packetFrameDelta, TPR_UINT8 packetFrameKey) const;
	// Convert the rates: defined relative to total# packets or source# packets
	TPR_UINT8 ConvertFECRate(TPR_UINT8 codeRate) const;
	// Update FEC with protectionFactorD
	void UpdateProtectionFactorD(TPR_UINT8 protectionFactorD);
	// Update FEC with protectionFactorK
	void UpdateProtectionFactorK(TPR_UINT8 protectionFactorK);
	// Compute the bits per frame. Account for temporal layers when applicable.
	int BitsPerFrame(const VCMProtectionParameters* parameters);

protected:
	enum { kUpperLimitFramesFec = 6 };
	// Thresholds values for the bytes/frame and round trip time, below which we
	// may turn off FEC, depending on |_numLayers| and |_maxFramesFec|.
	// Max bytes/frame for VGA, corresponds to ~140k at 25fps.
	enum { kMaxBytesPerFrameForFec = 700 };
	// Max bytes/frame for CIF and lower: corresponds to ~80k at 25fps.
	enum { kMaxBytesPerFrameForFecLow = 400 };
	// Max bytes/frame for frame size larger than VGA, ~200k at 25fps.
	enum { kMaxBytesPerFrameForFecHigh = 1000 };
};


class VCMNackFecMethod : public VCMFecMethod 
{
public:
	VCMNackFecMethod(TPR_INT64 lowRttNackThresholdMs, TPR_INT64 highRttNackThresholdMs);
	virtual ~VCMNackFecMethod();
	virtual TPR_BOOL UpdateParameters(const VCMProtectionParameters* parameters);
	// Get the protection factors
	TPR_BOOL ProtectionFactor(const VCMProtectionParameters* parameters);
	// Get the max number of frames the FEC is allowed to be based on.
	int MaxFramesFec() const;
	// Turn off the FEC based on low bitrate and other factors.
	TPR_BOOL BitRateTooLowForFec(const VCMProtectionParameters* parameters);

private:
	int ComputeMaxFramesFec(const VCMProtectionParameters* parameters);

	TPR_INT64 _lowRttNackMs;
	TPR_INT64 _highRttNackMs;
	int _maxFramesFec;
};

class VCMLossProtectionLogic 
{
public:
	explicit VCMLossProtectionLogic(TPR_INT64 nowMs);
	~VCMLossProtectionLogic();

	// Set the protection method to be used
	//
	// Input:
	//        - newMethodType    : New requested protection method type. If one
	//                           is already set, it will be deleted and replaced
	void SetMethod(VCMProtectionMethodEnum newMethodType);

	void UpdateRtt(TPR_INT64 rtt);

	// Update the filtered packet loss.
	//
	// Input:
	//          - packetLossEnc :  The reported packet loss filtered
	//                             (max window or average)
	void UpdateFilteredLossPr(TPR_UINT8 packetLossEnc);

	// Update the current target bit rate.
	//
	// Input:
	//          - bitRate          : The current target bit rate in kbits/s
	void UpdateBitRate(float bitRate);

	// Update the number of packets per frame estimate, for delta frames
	//
	// Input:
	//          - nPackets         : Number of packets in the latest sent frame.
	void UpdatePacketsPerFrame(float nPackets, TPR_INT64 nowMs);

	// Update the number of packets per frame estimate, for key frames
	//
	// Input:
	//          - nPackets         : umber of packets in the latest sent frame.
	void UpdatePacketsPerFrameKey(float nPackets, TPR_INT64 nowMs);

	// Update the keyFrameSize estimate
	//
	// Input:
	//          - keyFrameSize     : The size of the latest sent key frame.
	void UpdateKeyFrameSize(float keyFrameSize);

	// Update the frame rate
	//
	// Input:
	//          - frameRate        : The current target frame rate.
	void UpdateFrameRate(float frameRate) { _frameRate = frameRate; }

	// Update the frame size
	//
	// Input:
	//          - width        : The codec frame width.
	//          - height       : The codec frame height.
	void UpdateFrameSize(int width, int height);

	// Update the number of active layers
	//
	// Input:
	//          - numLayers    : Number of layers used.
	void UpdateNumLayers(int numLayers);

	// The amount of packet loss to cover for with FEC.
	//
	// Input:
	//          - fecRateKey      : Packet loss to cover for with FEC when
	//                              sending key frames.
	//          - fecRateDelta    : Packet loss to cover for with FEC when
	//                              sending delta frames.
	void UpdateFECRates(TPR_UINT8 fecRateKey, TPR_UINT8 fecRateDelta) 
	{
		_fecRateKey = fecRateKey;
		_fecRateDelta = fecRateDelta;
	}

	// Update the protection methods with the current VCMProtectionParameters
	// and set the requested protection settings.
	// Return value     : Returns true on update
	TPR_BOOL UpdateMethod();

	// Returns the method currently selected.
	//
	// Return value                 : The protection method currently selected.
	VCMProtectionMethod* SelectedMethod() const;

	// Return the protection type of the currently selected method
	VCMProtectionMethodEnum SelectedType() const;

	// Updates the filtered loss for the average and max window packet loss,
	// and returns the filtered loss probability in the interval [0, 255].
	// The returned filtered loss value depends on the parameter |filter_mode|.
	// The input parameter |lossPr255| is the received packet loss.

	// Return value                 : The filtered loss probability
	TPR_UINT8 FilteredLoss(TPR_INT64 nowMs,FilterPacketLossMode filter_mode,TPR_UINT8 lossPr255);

	void Reset(TPR_INT64 nowMs);

	void Release();

private:
	// Sets the available loss protection methods.
	void UpdateMaxLossHistory(TPR_UINT8 lossPr255, TPR_INT64 now);
	TPR_UINT8 MaxFilteredLossPr(TPR_INT64 nowMs) const;
	VCMProtectionMethod* _selectedMethod;
	VCMProtectionParameters _currentParameters;
	TPR_INT64 _rtt;
	float _lossPr;
	float _bitRate;
	float _frameRate;
	float _keyFrameSize;
	TPR_UINT8 _fecRateKey;
	TPR_UINT8 _fecRateDelta;
	TPR_INT64 _lastPrUpdateT;
	TPR_INT64 _lastPacketPerFrameUpdateT;
	TPR_INT64 _lastPacketPerFrameUpdateTKey;
	ExpFilter _lossPr255;
	VCMLossProbabilitySample _lossPrHistory[kLossPrHistorySize];
	TPR_UINT8 _shortMaxLossPr255;
	ExpFilter _packetsPerFrame;
	ExpFilter _packetsPerFrameKey;
	int _codecWidth;
	int _codecHeight;
	int _numLayers;
};













#endif
