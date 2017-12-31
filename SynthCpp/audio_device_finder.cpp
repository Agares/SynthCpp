#include "stdafx.h"
#include "audio_device_finder.h"
#include <mmdeviceapi.h>
#include <stdexcept>

audio_device_finder::audio_device_finder()
{
	const auto enumerator_creation_result = enumerator_.CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_ALL);

	if(FAILED(enumerator_creation_result))
	{
		throw std::runtime_error("Cannot create IMMDeviceEnumerator");
	}
}

audio_device audio_device_finder::find_default_device() const
{
	CComPtr<IMMDevice> mm_device;
	const auto get_default_audio_endpoint_result = enumerator_->GetDefaultAudioEndpoint(eRender, eMultimedia, &mm_device);

	if(FAILED(get_default_audio_endpoint_result))
	{
		throw std::runtime_error("Cannot get default audio endpoint");
	}

	return audio_device(mm_device);
}
