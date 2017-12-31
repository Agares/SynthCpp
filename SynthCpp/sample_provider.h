#pragma once

class audio_buffer;

class sample_provider
{
public:
	virtual ~sample_provider() = default;
	virtual void fill_buffer(const audio_buffer &buffer) = 0;
};
