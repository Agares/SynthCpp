#pragma once

class wave_format final
{
public:
	wave_format(const int sample_rate, const int number_of_channels, const int number_of_bits) noexcept
		: sample_rate_(sample_rate), number_of_channels_(number_of_channels), number_of_bits_(number_of_bits)
	{
		// todo validate arguments
	}

	int sample_rate() const noexcept
	{
		return sample_rate_;
	}

	int number_of_channels() const noexcept
	{
		return number_of_channels_;
	}

	int number_of_bits() const noexcept
	{
		return number_of_bits_;
	}
private:
	int sample_rate_;
	int number_of_channels_;
	int number_of_bits_;
};
