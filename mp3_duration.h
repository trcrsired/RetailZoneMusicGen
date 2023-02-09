#pragma once

#include<cstdint>

struct duration_result
{
	::std::uint_least64_t duration{};
	bool invalid{};
};

duration_result get_mp3_duration(void const*,void const*) noexcept;
