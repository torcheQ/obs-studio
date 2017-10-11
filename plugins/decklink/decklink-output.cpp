#include <obs-module.h>
#include <obs-avc.h>

#include "const.h"

#include "DecklinkOutput.hpp"
#include "decklink-device.hpp"
#include "decklink-device-discovery.hpp"
#include "decklink-devices.hpp"

#include "../../libobs/media-io/video-scaler.h"

static void decklink_output_destroy(void *data)
{
	DeckLinkOutput *decklink = (DeckLinkOutput *)data;
	delete decklink;
}

static void *decklink_output_create(obs_data_t *settings, obs_output_t *output)
{
	DeckLinkOutput *decklinkOutput = new DeckLinkOutput(output, deviceEnum);

	obs_output_update(output, settings);


	return decklinkOutput;
}

static bool decklink_output_start(void *data)
{
	DeckLinkOutput *decklink = (DeckLinkOutput *)data;

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

	video_scaler_create(&decklink->scaler, &to, &from, VIDEO_SCALE_FAST_BILINEAR);

	ComPtr<DeckLinkDevice> device;

	device.Set(deviceEnum->FindByHash("4123904_UltraStudio Mini Monitor"));

	decklink->Activate(device, -1);

	if (!obs_output_begin_data_capture(decklink->GetOutput(), 0)) {
		return false;
	}


	return true;
}

static void decklink_output_stop(void *data, uint64_t ts)
{
	DeckLinkOutput *decklink = (DeckLinkOutput *)data;
}

static void decklink_output_raw_video(void *data, struct video_data *frame){
	DeckLinkOutput *decklink = (DeckLinkOutput *)data;

	decklink->DisplayVideoFrame(frame);
}

static void decklink_output_raw_audio(void *data, struct audio_data *frames)
{

}

static obs_properties_t *decklink_output_properties(void *unused)
{
	UNUSED_PARAMETER(unused);

	obs_properties_t *props = obs_properties_create();

	obs_property_t *list = obs_properties_add_list(props, DEVICE_HASH,
												   TEXT_DEVICE, OBS_COMBO_TYPE_LIST, OBS_COMBO_FORMAT_STRING);
//	obs_property_set_modified_callback(list, decklink_device_changed);

	fill_out_devices(list);


	return props;
}

static const char *decklink_output_get_name(void*)
{
	return obs_module_text("BlackmagicDevice");
}

struct obs_output_info create_decklink_output_info() {
	struct obs_output_info decklink_output_info = {};

	decklink_output_info.id             = "decklink_output";
	decklink_output_info.flags          = OBS_OUTPUT_AV;
	decklink_output_info.get_name       = decklink_output_get_name;
	decklink_output_info.create         = decklink_output_create;
	decklink_output_info.destroy        = decklink_output_destroy;
	decklink_output_info.start          = decklink_output_start;
	decklink_output_info.stop           = decklink_output_stop;
	decklink_output_info.get_properties = decklink_output_properties;
	decklink_output_info.raw_video      = decklink_output_raw_video;
	decklink_output_info.raw_audio      = decklink_output_raw_audio;

	return decklink_output_info;
}