// SynthCpp.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "audio_device_finder.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <objbase.h>
#include <stdexcept>
#include <mmdeviceapi.h>
#include <iostream>

#include <atlmem.h>
#include <ATLComMem.h>
#include <thread>
#include "sine_sample_provider.h"

int main()
{
	std::thread thread{
		[]()
		{
			const auto com_initialize_result = CoInitializeEx(
				nullptr, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE | COINIT_APARTMENTTHREADED);
			if (FAILED(com_initialize_result))
			{
				throw std::runtime_error("Cannot initialize COM");
			}

			try
			{
				audio_device_finder finder;

				auto device = finder.find_default_device();

				std::wcout << device.name() << std::endl;

				audio_stream audio_stream = device.initialize();
				auto sample_provider = sine_sample_provider{};
				audio_stream.play(sample_provider);
			}
			catch (std::runtime_error& error)
			{
				std::wcerr << error.what() << std::endl;
			}

			CoUninitialize();
		}
	};
	thread.join();

	return 0;
}
