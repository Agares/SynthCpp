#include "stdafx.h"
#define NOMINMAX
#include "audio_device.h"
#include <FunctionDiscoveryKeys_devpkey.h>
#include <audioclient.h>
#include "audio_stream.h"

audio_device::audio_device(CComPtr<IMMDevice> mm_device) : mm_device_(mm_device)
{
}

audio_stream audio_device::initialize() const
{
	return audio_stream(mm_device_);
}

std::wstring audio_device::name() const
{
	CComPtr<IPropertyStore> properties;
	if(FAILED(mm_device_->OpenPropertyStore(STGM_READ, &properties)))
	{
		throw std::runtime_error("Failed to open property store");
	}

	PROPVARIANT propertyValue;

	if(FAILED(properties->GetValue(PKEY_Device_FriendlyName, &propertyValue)))
	{
		throw std::runtime_error("Failed to get device name");
	}

	return propertyValue.bstrVal;
}
