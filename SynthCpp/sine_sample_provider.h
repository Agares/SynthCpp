#pragma once
#include "sample_provider.h"
#include "audio_buffer.h"
#include <atomic>

class sine_sample_provider final : public sample_provider
{
public:
	sine_sample_provider() = default;
	~sine_sample_provider() override = default;

	sine_sample_provider(const sine_sample_provider &) = delete;
	sine_sample_provider &operator=(const sine_sample_provider &other) = delete;

	sine_sample_provider(sine_sample_provider &&other) = default;

	sine_sample_provider& operator=(sine_sample_provider&& other) noexcept
	{
		std::swap(phase_, other.phase_);
		frequency_.exchange(other.frequency_);

		return *this;
	}

	void fill_buffer(const audio_buffer& buffer) override
	{
		assert(frequency >= 1.0f && frequency <= buffer.wave_format().sample_rate()/2.0f);

		for(size_t i = 0; i < buffer.number_of_frames(); i++)
		{
			phase_ += two_pi * frequency_ / float(buffer.wave_format().sample_rate());
			buffer.write_to_all_channels(i, sinf(phase_));

			if (phase_ > two_pi) phase_ -= two_pi;
		}
	}
private:
	float phase_ = 0.0f;
	std::atomic<float> frequency_ = 440.0f;
	inline static const float two_pi = 2.0f * 3.14159265358979323f;
};
