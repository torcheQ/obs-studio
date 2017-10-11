#include <obs-module.h>
#include <obs-avc.h>

#include "const.h"

#include "DecklinkOutput.hpp"
#include "decklink-device.hpp"
#include "decklink-device-discovery.hpp"
#include "decklink-devices.hpp"

#include "../../libobs/media-io/video-scaler.h"

static void decklink_filter_destroy(void *data)
{
	DeckLinkOutput *decklink = (DeckLinkOutput *)data;
	delete decklink;
}

static void* decklink_filter_create(obs_data_t* settings, obs_source_t* source)
{
	DeckLinkOutput *decklinkOutput = new DeckLinkOutput(nullptr, deviceEnum);

	struct obs_video_info ovi;

	obs_get_video_info(&ovi);

	struct video_scale_info from = {
			.format = ovi.output_format,
			.width  = ovi.output_width,
			.height = ovi.output_height,
	};

	struct video_scale_info to = {
			.format = VIDEO_FORMAT_UYVY,
			.width  = 1920,
			.height = 1080,
	};

	//video_scaler_create(&decklink->scaler, &to, &from, VIDEO_SCALE_FAST_BILINEAR);

	ComPtr<DeckLinkDevice> device;

	device.Set(deviceEnum->FindByHash("4123904_UltraStudio Mini Monitor"));

	/*decklink->Activate(device, -1);

	if (!obs_output_begin_data_capture(decklink->GetOutput(), 0)) {
		return false;
	}*/

	return decklinkOutput;
}

static void decklink_filter_stop(void *data, uint64_t ts)
{
	DeckLinkOutput *decklink = (DeckLinkOutput *)data;
}

static void decklink_filter_raw_video(void *data, struct video_data *frame){
	DeckLinkOutput *decklink = (DeckLinkOutput *)data;

	decklink->DisplayVideoFrame(frame);
}

struct obs_audio_data* decklink_filter_audiofilter(void *data,
											  struct obs_audio_data* audio_data) {

}

static obs_properties_t *decklink_filter_properties(void *unused)
{
	UNUSED_PARAMETER(unused);

	obs_properties_t *props = obs_properties_create();

	obs_property_t *list = obs_properties_add_list(props, DEVICE_HASH,
												   TEXT_DEVICE, OBS_COMBO_TYPE_LIST, OBS_COMBO_FORMAT_STRING);
	//obs_property_set_modified_callback(list, decklink_device_changed);

	fill_out_devices(list);


	return props;
}

static const char *decklink_filter_get_name(void*)
{
	return obs_module_text("BlackmagicDevice");
}

struct obs_source_info create_decklink_filter_info() {
	struct obs_source_info decklink_filter_info = {};

	decklink_filter_info.id             = "decklink_filter";
	decklink_filter_info.type           = OBS_SOURCE_TYPE_FILTER;
	decklink_filter_info.output_flags   = OBS_SOURCE_VIDEO;
	decklink_filter_info.get_name       = decklink_filter_get_name;
	decklink_filter_info.create         = decklink_filter_create;
	decklink_filter_info.destroy        = decklink_filter_destroy;
	decklink_filter_info.get_properties = decklink_filter_properties;
	//decklink_filter_info.video_render   = decklink_filter_raw_video;
	decklink_filter_info.filter_audio   = decklink_filter_audiofilter;

	return decklink_filter_info;
}