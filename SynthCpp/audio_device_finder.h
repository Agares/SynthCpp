#pragma once
#include "audio_device.h"
#include <mmdeviceapi.h>
#include <atlmem.h>
#include <ATLComMem.h>

class audio_device_finder final
{
public:
	audio_device_finder();
	audio_device find_default_device() const;
private:
	CComPtr<IMMDeviceEnumerator> enumerator_;
};
