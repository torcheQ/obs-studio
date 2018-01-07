#pragma once

#include "DecklinkBase.h"

#include "../../libobs/media-io/video-scaler.h"

class DeckLinkOutput : public DecklinkBase {
protected:
	obs_output_t                          *output;

	int width;
	int height;

	static void DevicesChanged(void *param, DeckLinkDevice *device,
							   bool added);

public:
	video_scaler_t *scaler;
	const char                            *deviceHash;
	long long                             modeID;

	DeckLinkOutput(obs_output_t *output, DeckLinkDeviceDiscovery *discovery);
	virtual ~DeckLinkOutput(void);

	obs_output_t *GetOutput(void) const;

	bool Activate(DeckLinkDevice *device, long long modeId) override;
	void Deactivate() override;

	void DisplayVideoFrame(video_data *pData);

	void SetSize(int width, int height);

	int GetWidth();

	int GetHeight();
};
