project(mac-decklink)

if(DISABLE_DECKLINK)
	message(STATUS "decklink plugin disabled")
	return()
endif()

find_library(COREFOUNDATION CoreFoundation)
find_package(Qt5Widgets REQUIRED)

# This lets us find the generated UI header file
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(mac-decklink-sdk_HEADERS
		decklink-sdk/DeckLinkAPI.h
		decklink-sdk/DeckLinkAPIConfiguration.h
		decklink-sdk/DeckLinkAPIDeckControl.h
		decklink-sdk/DeckLinkAPIDiscovery.h
		decklink-sdk/DeckLinkAPIModes.h
		decklink-sdk/DeckLinkAPIStreaming.h
		decklink-sdk/DeckLinkAPITypes.h
		decklink-sdk/DeckLinkAPIVersion.h
	)

set(mac-decklink-sdk_SOURCES
		decklink-sdk/DeckLinkAPIDispatch.cpp
	)

set(mac-decklink_HEADERS
		../decklink-devices.hpp
		../const.h
		../DecklinkOutput.hpp
		../platform.hpp
		../decklink.hpp
		../decklink-device-instance.hpp
		../decklink-device-discovery.hpp
		../decklink-device.hpp
		../decklink-device-mode.hpp
		../audio-repack.h
		../audio-repack.hpp
		../DecklinkOutputUI.h)

set(mac-decklink_SOURCES
		../plugin-main.cpp
		../decklink-devices.cpp
		../decklink-output.cpp
		../decklink-source.cpp
		../DecklinkOutput.cpp
		../decklink.cpp
		../decklink-device-instance.cpp
		../decklink-device-discovery.cpp
		../decklink-device.cpp
		../decklink-device-mode.cpp
		../audio-repack.c
		platform.cpp
		../DecklinkOutputUI.cpp)

qt5_wrap_ui(decklink_UI_HEADERS
		../forms/output.ui)

include_directories(
		${COREFOUNDATION}
		"${CMAKE_SOURCE_DIR}/UI/obs-frontend-api")

list(APPEND decklink_HEADERS ${decklink_UI_HEADERS})

add_library(mac-decklink MODULE
	${mac-decklink_SOURCES}
	${mac-decklink_HEADERS}
	${mac-decklink-sdk_HEADERS}
	${mac-decklink-sdk_SOURCES})

target_link_libraries(mac-decklink
	libobs
	${COREFOUNDATION}
	obs-frontend-api
        Qt5::Widgets)

install_obs_plugin_with_data(mac-decklink ../data)
