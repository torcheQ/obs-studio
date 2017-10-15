#include <obs-module.h>
#include <obs-frontend-api.h>
#include <obs-avc.h>
#include <util/platform.h>
#include <media-io/video-io.h>
#include <media-io/video-frame.h>

#include "const.h"

#include "DecklinkFilter.hpp"
#include "decklink-device.hpp"
#include "decklink-device-discovery.hpp"
#include "decklink-devices.hpp"

#include "../../libobs/media-io/video-scaler.h"

#define TEXFORMAT GS_BGRA

static void decklink_filter_destroy(void *data)
{
	DecklinkFilter *decklink = (DecklinkFilter *)data;

	decklink->Deactivate();

	delete decklink;
}

void decklink_filter_update(void* data, obs_data_t* settings) {
	UNUSED_PARAMETER(settings);
	DecklinkFilter *decklink = (DecklinkFilter *)data;

	decklink->Deactivate();

	const char *hash = obs_data_get_string(settings, DEVICE_HASH);
	ComPtr<DeckLinkDevice> device;
	device.Set(deviceEnum->FindByHash(hash));

	decklink->Activate(device, -1);
}

static void decklink_filter_raw_video(void *data, struct video_data *frame){
	DecklinkFilter *decklink = (DecklinkFilter *)data;

	if (!frame || !frame->data[0])
		return;

	decklink->DisplayVideoFrame(frame);
}

void decklink_filter_offscreen_render(void* data, uint32_t cx, uint32_t cy) {
	DecklinkFilter *decklink = (DecklinkFilter *)data;

	obs_source_t* target = obs_filter_get_target(decklink->context);
	uint32_t width = obs_source_get_base_width(target);
	uint32_t height = obs_source_get_base_height(target);

	gs_texrender_reset(decklink->texrender);

	gs_blend_state_push();
	gs_blend_function(GS_BLEND_ONE, GS_BLEND_ZERO);

	if (gs_texrender_begin(decklink->texrender, width, height)) {
		struct vec4 background;
		vec4_zero(&background);

		gs_clear(GS_CLEAR_COLOR, &background, 0.0f, 0);
		gs_ortho(0.0f, (float)width, 0.0f, (float)height, -100.0f, 100.0f);

		obs_source_video_render(target);
		gs_texrender_end(decklink->texrender);

		if (decklink->known_width != width || decklink->known_height != height) {
			gs_stagesurface_unmap(decklink->stagesurface);
			gs_stagesurface_destroy(decklink->stagesurface);

			decklink->stagesurface =
					gs_stagesurface_create(width, height, TEXFORMAT);
			gs_stagesurface_map(decklink->stagesurface,
								&decklink->video_data, &decklink->video_linesize);

			video_output_info vi;
			vi.format = VIDEO_FORMAT_BGRA;
			vi.width = width;
			vi.height = height;
			vi.fps_den = decklink->ovi.fps_den;
			vi.fps_num = decklink->ovi.fps_num;
			vi.cache_size = 16;
			vi.colorspace = VIDEO_CS_DEFAULT;
			vi.range = VIDEO_RANGE_DEFAULT;
			vi.name = obs_source_get_name(decklink->context);

			video_output_close(decklink->video_output);
			video_output_open(&decklink->video_output, &vi);
			video_output_connect(decklink->video_output,
								 nullptr, decklink_filter_raw_video, decklink);

			decklink->known_width = width;
			decklink->known_height = height;
		}

		struct video_frame output_frame;
		if (video_output_lock_frame(decklink->video_output,
									&output_frame, 1, os_gettime_ns()))
		{
			gs_stage_texture(decklink->stagesurface,
							 gs_texrender_get_texture(decklink->texrender));

			memcpy(output_frame.data[0], decklink->video_data,
				   decklink->video_linesize * decklink->known_height);
			output_frame.linesize[0] = decklink->video_linesize;

			video_output_unlock_frame(decklink->video_output);
		}
	}

	gs_blend_state_pop();
}

static void* decklink_filter_create(obs_data_t* settings, obs_source_t* source)
{
	DecklinkFilter *decklink = new DecklinkFilter();

	decklink->context = source;
	decklink->texrender = gs_texrender_create(TEXFORMAT, GS_ZS_NONE);
	decklink->audio_initialized = false;

	gs_init_data display_desc = {};
	display_desc.adapter = 0;
	display_desc.format = TEXFORMAT;
	display_desc.zsformat = GS_ZS_NONE;
	display_desc.cx = 0;
	display_desc.cy = 0;

#ifdef _WIN32
	display_desc.window.hwnd = obs_frontend_get_main_window_handle();
#elif __APPLE__
	//display_desc.window.view = (id) obs_frontend_get_main_window_handle();
#endif

	obs_get_video_info(&decklink->ovi);
	obs_get_audio_info(&decklink->oai);

	decklink->renderer = obs_display_create(&display_desc);
	obs_display_add_draw_callback(decklink->renderer, decklink_filter_offscreen_render, decklink);

	decklink_filter_update(decklink, settings);

	return decklink;
}

void decklink_filter_video_render(void* data, gs_effect_t* effect) {
	UNUSED_PARAMETER(effect);
	DecklinkFilter *decklink = (DecklinkFilter *)data;
	obs_source_skip_video_filter(decklink->context);
}


struct obs_audio_data* decklink_filter_audiofilter(void *data,
											  struct obs_audio_data* audio_data) {
	return audio_data;
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
	decklink_filter_info.update         = decklink_filter_update;
	decklink_filter_info.get_properties = decklink_filter_properties;
	decklink_filter_info.video_render   = decklink_filter_video_render;
	decklink_filter_info.filter_audio   = decklink_filter_audiofilter;

	return decklink_filter_info;
}
