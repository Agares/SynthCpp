#pragma once
#include <mmdeviceapi.h>
#include <atlmem.h>
#include <ATLComMem.h>
#include <string>
#include "audio_stream.h"

class audio_device final
{
public:
	explicit audio_device(CComPtr<IMMDevice> mm_device);
	audio_stream initialize() const;
	std::wstring name() const;
private:
	CComPtr<IMMDevice> mm_device_;
};
