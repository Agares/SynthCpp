#include "stdafx.h"
#define NOMINMAX
#include "audio_stream.h"
#include <stdexcept>
#include "wave_format.h"
#include <thread>
#include "wasapi_wave_format_factory.h"
#include <vector>
#include <avrt.h>
#include "com_caller.h"
#include "audio_buffer.h"
#include "sample_provider.h"

#pragma comment (lib, "avrt.lib")

static const AUDCLNT_SHAREMODE share_mode = AUDCLNT_SHAREMODE_EXCLUSIVE;

audio_stream::audio_stream(CComPtr<IMMDevice> mm_device)
	: wave_format_(nullptr), buffer_needs_filling_event_(CreateEvent(nullptr, false, false, nullptr))
{
	com_caller::call_method(mm_device, &IMMDevice::Activate, __uuidof(IAudioClient), CLSCTX_ALL, nullptr, reinterpret_cast<void**>(&audio_client_));

	wave_format_ = std::make_unique<wave_format>(negotiate_format());
	const auto desired_format = wasapi_wave_format_factory::convert(*wave_format_);

	REFERENCE_TIME minimum_period;
	com_caller::call_method(audio_client_, &IAudioClient::GetDevicePeriod, nullptr, &minimum_period);

	initialize_audio_client(mm_device, desired_format, minimum_period);

	com_caller::call_method(audio_client_, &IAudioClient::SetEventHandle, buffer_needs_filling_event_);
}

void audio_stream::initialize_audio_client(CComPtr<IMMDevice> mm_device, WAVEFORMATEXTENSIBLE desired_format, REFERENCE_TIME minimum_period)
{
	const auto audio_client_initialize_result = audio_client_->Initialize(share_mode, AUDCLNT_STREAMFLAGS_EVENTCALLBACK, minimum_period, minimum_period, reinterpret_cast<WAVEFORMATEX*>(&desired_format), nullptr);

	if (!(FAILED(audio_client_initialize_result))) {
		return;
	}

	if (audio_client_initialize_result != AUDCLNT_E_BUFFER_SIZE_NOT_ALIGNED) {
		throw std::runtime_error("Failed to initialize audio client");
	}

	UINT32 buffer_size;

	com_caller::call_method(audio_client_, &IAudioClient::GetBufferSize, &buffer_size);

	const auto hns_requested_duration = static_cast<REFERENCE_TIME>(round(10000.0 * 1000 / desired_format.Format.nSamplesPerSec * buffer_size));

	audio_client_.Release();
	com_caller::call_method(mm_device, &IMMDevice::Activate, __uuidof(IAudioClient), CLSCTX_ALL, nullptr, reinterpret_cast<void**>(&audio_client_));
	com_caller::call_method(audio_client_, &IAudioClient::Initialize, share_mode, AUDCLNT_STREAMFLAGS_EVENTCALLBACK, hns_requested_duration, hns_requested_duration, reinterpret_cast<WAVEFORMATEX*>(&desired_format), nullptr);
}

void audio_stream::play(sample_provider &sample_provider) const
{
	DWORD task_index = 0;
	if(FAILED(AvSetMmThreadCharacteristics(TEXT("Pro Audio"), &task_index)))
	{
		throw std::runtime_error("Cannot change thread priority");
	}

	UINT32 buffer_size;
	audio_client_->GetBufferSize(&buffer_size);

	CComPtr<IAudioRenderClient> audio_render_client;
	audio_client_->GetService(__uuidof(IAudioRenderClient), reinterpret_cast<void**>(&audio_render_client));

	audio_client_->Start();

	const audio_buffer buffer(buffer_size, *wave_format_);

	for (;;)
	{
		BYTE *raw_buffer;

		if(WaitForSingleObject(this->buffer_needs_filling_event_, 1000) != WAIT_OBJECT_0)
		{
			throw std::runtime_error("Wait for buffer event timed out");
		}

		sample_provider.fill_buffer(buffer); // todo run it in a thread?

		com_caller::call_method(audio_render_client, &IAudioRenderClient::GetBuffer, buffer_size, &raw_buffer);

		for(size_t i = 0; i < buffer_size * wave_format_->number_of_channels(); i++)
		{
			reinterpret_cast<int16_t*>(raw_buffer)[i] = int16_t(buffer[i] * static_cast<float>(std::numeric_limits<int16_t>::max()));
		}

		com_caller::call_method(audio_render_client, &IAudioRenderClient::ReleaseBuffer, buffer_size, 0);
	}
}

wave_format audio_stream::negotiate_format() const
{
	std::vector<wave_format> formats_to_try = {
		//{ wave_format(192000, 2), 32 },
		//{ wave_format(192000, 2), 24 },
		//wave_format(48000, 2, 32),
		//wave_format(48000, 2, 24),
		//{ wave_format(192000, 2), 16 },
		wave_format(48000, 2, 16),
		//wave_format(44100, 2, 32),
		//wave_format(44100, 2, 24),
		wave_format(44100, 2, 16),
	};

	for (const auto &format : formats_to_try) {
		auto wasapi_format = wasapi_wave_format_factory::convert(format);

		if (S_OK != audio_client_->IsFormatSupported(share_mode, reinterpret_cast<WAVEFORMATEX*>(&wasapi_format), nullptr))
		{
			continue;
		}

		return format;
	}

	throw std::runtime_error("Output format not supported");
}
