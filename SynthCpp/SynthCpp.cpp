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
	sine_sample_provider sample_provider;

	std::thread thread{
		[&sample_provider]()
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
				audio_stream.play(sample_provider);
			}
			catch (std::runtime_error& error)
			{
				std::wcerr << error.what() << std::endl;
			}

			CoUninitialize();
		}
	};

	const float semitone_multiplier = float(pow(2.0, 1.0 / 12.0));

	while(true)
	{

		const char c = getchar();

		if(c == 'z')
		{
			sample_provider.frequency(sample_provider.frequency() / semitone_multiplier);
		} else if(c == 'x')
		{
			sample_provider.frequency(sample_provider.frequency() * semitone_multiplier);
		}
	}

	return 0;
}
