#include <obs-module.h>

#include <obs-frontend-api.h>
#include <QAction>
#include <QMainWindow>

#include "DecklinkOutputUI.h"

#include "decklink-devices.hpp"

OBS_DECLARE_MODULE()
OBS_MODULE_USE_DEFAULT_LOCALE("decklink", "en-US")

extern struct obs_source_info create_decklink_source_info();
struct obs_source_info decklink_source_info;

extern struct obs_output_info create_decklink_output_info();
struct obs_output_info decklink_output_info;

DecklinkOutputUI *doUI;

void addOutputUI(void) {
    QAction *action = (QAction*)obs_frontend_add_tools_menu_qaction(
            obs_module_text("Decklink Outputs"));

	QMainWindow *window = (QMainWindow*)obs_frontend_get_main_window();

    doUI = new DecklinkOutputUI(window);

    auto cb = [] ()
    {
        doUI->ShowHideDialog();
    };

    action->connect(action, &QAction::triggered, cb);
}

bool obs_module_load(void)
{
	deviceEnum = new DeckLinkDeviceDiscovery();
	if (!deviceEnum->Init())
		return true;

	decklink_source_info = create_decklink_source_info();
	obs_register_source(&decklink_source_info);

	decklink_output_info = create_decklink_output_info();
	obs_register_output(&decklink_output_info);

    addOutputUI();

	return true;
}

void obs_module_unload(void)
{
	delete deviceEnum;
}
