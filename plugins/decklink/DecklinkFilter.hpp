#include <obs-module.h>
#include <media-io/video-io.h>
#include <media-io/video-frame.h>

#include "platform.hpp"

#include "decklink-device-discovery.hpp"
#include "decklink-device-instance.hpp"
#include "decklink-device-mode.hpp"

#include <map>
#include <vector>
#include <mutex>

#include <util/threading.h>

class DecklinkFilter {

public:
	ComPtr<DeckLinkDeviceInstance> instance;

	volatile long                         activateRefs = 0;
	std::recursive_mutex                  deviceMutex;

	obs_source_t* context;
	video_t* video_output;
	obs_display_t* renderer;
	gs_texrender_t* texrender;
	gs_stagesurf_t* stagesurface;
	uint32_t video_linesize;
	uint8_t* video_data;
	uint32_t known_width;
	uint32_t known_height;

	struct obs_video_info ovi;
	struct obs_audio_info oai;

	video_scaler_t *scaler;

	bool audio_initialized;

	virtual ~DecklinkFilter(void);

	DeckLinkDevice *GetDevice() const;

	void DisplayVideoFrame(struct video_data *frame);

	bool Activate(DeckLinkDevice *device, long long modeId);
	void Deactivate();
};
