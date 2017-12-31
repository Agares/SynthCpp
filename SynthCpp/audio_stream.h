#pragma once
#include <audioclient.h>
#include <atlmem.h>
#include <ATLComMem.h>
#include <mmdeviceapi.h>
#include "wave_format.h"
#include <utility>
#include <memory>

class sample_provider;

class audio_stream final
{
public:
	void initialize_audio_client(CComPtr<IMMDevice> mm_device, WAVEFORMATEXTENSIBLE desired_format,
	                             REFERENCE_TIME minimum_period);
	audio_stream(CComPtr<IMMDevice> mm_device);

	void play(sample_provider &sample_provider) const;
private:
	wave_format negotiate_format() const;

	CComPtr<IAudioClient> audio_client_;
	CComPtr<IAudioRenderClient> audio_render_client_;
	std::unique_ptr<wave_format> wave_format_{};

	HANDLE buffer_needs_filling_event_;
};

