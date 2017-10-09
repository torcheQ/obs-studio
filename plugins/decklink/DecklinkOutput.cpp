#include "DecklinkOutput.hpp"
#include "decklink-device-discovery.hpp"
#include "decklink-device-instance.hpp"
#include "decklink-device-mode.hpp"

#include <util/threading.h>

DeckLinkOutput::DeckLinkOutput(obs_output_t *output, DeckLinkDeviceDiscovery *discovery_) :
		discovery(discovery_), output(output)
{
	discovery->AddCallback(DeckLinkOutput::DevicesChanged, this);
}

DeckLinkOutput::~DeckLinkOutput(void)
{
	discovery->RemoveCallback(DeckLinkOutput::DevicesChanged, this);
	Deactivate();
}

DeckLinkDevice *DeckLinkOutput::GetDevice() const
{
	return instance ? instance->GetDevice() : nullptr;
}

void DeckLinkOutput::DevicesChanged(void *param, DeckLinkDevice *device, bool added)
{
	DeckLinkOutput *decklink = reinterpret_cast<DeckLinkOutput*>(param);
	std::lock_guard<std::recursive_mutex> lock(decklink->deviceMutex);

	blog(LOG_DEBUG, "%s", device->GetHash().c_str());

	/*obs_source_update_properties(decklink->output);

	if (added && !decklink->instance) {
		const char *hash;
		long long mode;
		obs_data_t *settings;

		settings = obs_source_get_settings(decklink->source);
		hash = obs_data_get_string(settings, "device_hash");
		mode = obs_data_get_int(settings, "mode_id");
		obs_data_release(settings);

		if (device->GetHash().compare(hash) == 0) {
			if (!decklink->activateRefs)
				return;
			if (decklink->Activate(device, mode))
				os_atomic_dec_long(&decklink->activateRefs);
		}

	} else if (!added && decklink->instance) {
		if (decklink->instance->GetDevice() == device) {
			os_atomic_inc_long(&decklink->activateRefs);
			decklink->Deactivate();
		}
	}*/
}

bool DeckLinkOutput::Activate(DeckLinkDevice *device, long long modeId)
{
	std::lock_guard<std::recursive_mutex> lock(deviceMutex);
	DeckLinkDevice *curDevice = GetDevice();
	const bool same = device == curDevice;
	const bool isActive = instance != nullptr;

	if (same) {
		if (!isActive)
			return false;
		if (instance->GetActiveModeId() == modeId &&
			instance->GetActivePixelFormat() == pixelFormat &&
			instance->GetActiveColorSpace() == colorSpace &&
			instance->GetActiveColorRange() == colorRange &&
			instance->GetActiveChannelFormat() == channelFormat)
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

	if (!instance->StartOutput(nullptr)) { //TODO: pass mode here
		instance = nullptr;
		return false;
	}

	os_atomic_inc_long(&activateRefs);
	return true;
}

void DeckLinkOutput::Deactivate(void)
{
	std::lock_guard<std::recursive_mutex> lock(deviceMutex);
	if (instance)
		instance->StopOutput();
	instance = nullptr;

	os_atomic_dec_long(&activateRefs);
}

obs_output_t *DeckLinkOutput::GetOutput(void) const
{
	return output;
}

void DeckLinkOutput::DisplayVideoFrame(video_data *frame) {
	instance->DisplayVideoFrame(scaler, frame);
}
