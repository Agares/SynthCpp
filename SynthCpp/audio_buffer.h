#pragma once
#include "wave_format.h"
#include <cassert>
#include <memory>

class audio_buffer final
{
public:
	audio_buffer(const audio_buffer &other) = delete;
	audio_buffer &operator=(const audio_buffer &other) = delete;

	audio_buffer(const size_t number_of_frames, const wave_format wave_format)
		: buffer_size_(number_of_frames * wave_format.number_of_channels()), 
		  wave_format_(wave_format),
		  buffer_(new float[buffer_size_])
	{
	}

	audio_buffer(audio_buffer &&other) noexcept
		: buffer_size_(other.buffer_size_), wave_format_(other.wave_format_), buffer_(std::move(other.buffer_))
	{
	};

	audio_buffer &operator=(audio_buffer &&other) noexcept
	{
		std::swap(buffer_size_, other.buffer_size_);
		std::swap(wave_format_, other.wave_format_);
		std::swap(buffer_, other.buffer_);

		return *this;
	}

	~audio_buffer() = default;

	void write_to_all_channels(const size_t index, const float value) const noexcept
	{
		for (size_t channel_index = 0; channel_index < size_t(wave_format_.number_of_channels()); channel_index++) {
			buffer_[index * wave_format_.number_of_channels() + channel_index] = value;
		}
	}

	float &operator[](const size_t index) const noexcept
	{
		assert(index < buffer_size_);

		return buffer_[index];
	}

	size_t number_of_frames() const noexcept
	{
		return buffer_size_ / wave_format_.number_of_channels();
	}

	wave_format wave_format() const noexcept
	{
		return wave_format_;
	}
private:
	size_t buffer_size_;
	::wave_format wave_format_;

	std::unique_ptr<float[]> buffer_;
};
