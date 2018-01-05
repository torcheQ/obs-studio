#pragma once

#include "platform.hpp"

#include <obs-module.h>

#include <map>
#include <vector>
#include <mutex>

#include "../../libobs/media-io/video-scaler.h"

class DeckLinkDeviceDiscovery;
class DeckLinkDeviceInstance;
class DeckLinkDevice;
class DeckLinkDeviceMode;

class DeckLinkOutput {
	protected:
	ComPtr<DeckLinkDeviceInstance>        instance;
	DeckLinkDeviceDiscovery               *discovery;
	obs_output_t                          *output;
	volatile long                         activateRefs = 0;
	std::recursive_mutex                  deviceMutex;
	BMDPixelFormat                        pixelFormat = bmdFormat8BitYUV;
	video_colorspace                      colorSpace = VIDEO_CS_DEFAULT;
	video_range_type                      colorRange = VIDEO_RANGE_DEFAULT;
	speaker_layout                        channelFormat = SPEAKERS_STEREO;

	static void DevicesChanged(void *param, DeckLinkDevice *device,
							   bool added);

	public:
	video_scaler_t *scaler;
	const char                            *deviceHash;
	long long                             modeID;

	DeckLinkOutput(obs_output_t *output, DeckLinkDeviceDiscovery *discovery);
	virtual ~DeckLinkOutput(void);

	DeckLinkDevice *GetDevice() const;

	long long GetActiveModeId(void) const;
	obs_output_t *GetOutput(void) const;

	inline BMDPixelFormat GetPixelFormat() const {return pixelFormat;}
	inline void SetPixelFormat(BMDPixelFormat format)
	{
		pixelFormat = format;
	}
	inline video_colorspace GetColorSpace() const {return colorSpace;}
	inline void SetColorSpace(video_colorspace format)
	{
		colorSpace = format;
	}
	inline video_range_type GetColorRange() const {return colorRange;}
	inline void SetColorRange(video_range_type format)
	{
		colorRange = format;
	}
	inline speaker_layout GetChannelFormat() const {return channelFormat;}
	inline void SetChannelFormat(speaker_layout format)
	{
		channelFormat = format;
	}

	bool Activate(DeckLinkDevice *device, long long modeId);
	void Deactivate();

	void DisplayVideoFrame(video_data *pData);
};
