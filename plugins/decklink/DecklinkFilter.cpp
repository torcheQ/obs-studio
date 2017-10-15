#include "DecklinkFilter.hpp"


DecklinkFilter::~DecklinkFilter(void)
{
	video_output_close(video_output);
	obs_display_destroy(renderer);

	gs_stagesurface_unmap(stagesurface);
	gs_stagesurface_destroy(stagesurface);
	gs_texrender_destroy(texrender);
}

DeckLinkDevice *DecklinkFilter::GetDevice() const
{
	return instance ? instance->GetDevice() : nullptr;
}

void DecklinkFilter::DisplayVideoFrame(struct video_data *frame) {
	instance->DisplayVideoFrame(scaler, frame);
}

bool DecklinkFilter::Activate(DeckLinkDevice *device, long long modeId)
{
	struct video_scale_info from = {};
	from.format = ovi.output_format;
	from.width = ovi.output_width;
	from.height = ovi.output_height;

	struct video_scale_info to = {};
	to.format = VIDEO_FORMAT_UYVY;
	to.width = 1920;
	to.height = 1080;

	video_scaler_create(&scaler, &to, &from, VIDEO_SCALE_FAST_BILINEAR);

	std::lock_guard<std::recursive_mutex> lock(deviceMutex);
	DeckLinkDevice *curDevice = GetDevice();
	const bool same = device == curDevice;
	const bool isActive = instance != nullptr;

	if (same) {
		if (!isActive)
			return false;
	}

	if (isActive)
		instance->StopOutput();

	if (!same)
		instance.Set(new DeckLinkDeviceInstance(nullptr, device)); // TODO: pass a real instance here (inheritance?)

	if (instance == nullptr)
		return false;

	if (modeId >= 0) {
		DeckLinkDeviceMode *mode = GetDevice()->FindOutputMode(modeId);
		if (mode == nullptr) {
			instance = nullptr;
			return false;
		}
	}

	if (!instance->StartOutput()) {
		instance = nullptr;
		return false;
	}

	os_atomic_inc_long(&activateRefs);

	return true;
}

void DecklinkFilter::Deactivate(void)
{
	std::lock_guard<std::recursive_mutex> lock(deviceMutex);
	if (instance)
		instance->StopOutput();
	instance = nullptr;

	os_atomic_dec_long(&activateRefs);
}
