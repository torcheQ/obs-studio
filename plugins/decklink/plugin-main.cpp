#include <obs-module.h>

#include "decklink-devices.hpp"
#include "decklink-source.hpp"
#include "decklink-output.hpp"

OBS_DECLARE_MODULE()
OBS_MODULE_USE_DEFAULT_LOCALE("decklink", "en-US")

bool obs_module_load(void)
{
	deviceEnum = new DeckLinkDeviceDiscovery();
	if (!deviceEnum->Init())
		return true;

	obs_register_source(&decklink_source_info);

	obs_register_output(&decklink_output_info);

	return true;
}

void obs_module_unload(void)
{
	delete deviceEnum;
}
