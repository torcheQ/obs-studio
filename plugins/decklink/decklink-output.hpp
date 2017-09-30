#include <obs-module.h>
#include <obs-avc.h>

#include "decklink.hpp"
#include "decklink-device.hpp"
#include "decklink-device-discovery.hpp"
#include "decklink-devices.hpp"

static void decklink_output_destroy(void *data)
{

}

static void *decklink_output_create(obs_data_t *settings, obs_output_t *output)
{

}

static bool decklink_output_start(void *data)
{

}

static void decklink_output_stop(void *data, uint64_t ts)
{

}

static void decklink_output_raw_video(void *data, struct video_data *frame){

}

static obs_properties_t *decklink_output_properties(void *unused)
{
	UNUSED_PARAMETER(unused);

	obs_properties_t *props = obs_properties_create();

	obs_property_t *list = obs_properties_add_list(props, DEVICE_HASH,
												   TEXT_DEVICE, OBS_COMBO_TYPE_LIST, OBS_COMBO_FORMAT_STRING);
	obs_property_set_modified_callback(list, decklink_device_changed);

	fill_out_devices(list);


	return props;
}

struct obs_output_info decklink_output_info = {
		.id             = "decklink-output",
		.create         = decklink_output_create,
		.destroy        = decklink_output_destroy,
		.start          = decklink_output_start,
		.stop           = decklink_output_stop,
		.get_properties = decklink_output_properties,
		.raw_video      = decklink_output_raw_video
};