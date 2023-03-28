#pragma once

#include "../zaoly-wav-file/wav-file.hpp"
#include <vector>

namespace zaoly
{
	class general_wave
	{
	public:
		general_wave(uint32_t _sample_rate = {}, uint16_t _channels = {}, uint16_t _bits = {}) :
			sample_rate(_sample_rate),
			channels(_channels),
			bits(_bits)
		{}

		uint32_t sample_rate;
		uint16_t channels;
		uint16_t bits;
	};
}
