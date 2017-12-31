#pragma once
#include <audioclient.h>
#include "wave_format.h"

class wasapi_wave_format_factory
{
public:
	static WAVEFORMATEXTENSIBLE convert(const wave_format& wave_format)
	{
		WAVEFORMATEXTENSIBLE format;
		ZeroMemory(&format, sizeof(WAVEFORMATEXTENSIBLE));

		format.Format.wFormatTag = WAVE_FORMAT_EXTENSIBLE;
		format.Format.nChannels = wave_format.number_of_channels();
		format.Format.nSamplesPerSec = wave_format.sample_rate();
		format.Format.wBitsPerSample = wave_format.number_of_bits() == 24 ? 32 : wave_format.number_of_bits();
		format.Format.cbSize = 22;
		format.Format.nBlockAlign = (format.Format.nChannels * format.Format.wBitsPerSample) / 8;
		format.Format.nAvgBytesPerSec = format.Format.nSamplesPerSec * format.Format.nBlockAlign;
		format.Samples.wValidBitsPerSample = wave_format.number_of_bits();
		format.SubFormat = KSDATAFORMAT_SUBTYPE_PCM;
		format.dwChannelMask = KSAUDIO_SPEAKER_STEREO;

		return format;
	}
};

