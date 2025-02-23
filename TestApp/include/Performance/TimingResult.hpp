#pragma once

#include <chrono>

class TimingResult
{
public:
	std::chrono::nanoseconds averageTime;
	std::chrono::nanoseconds medianTime;

	TimingResult(
		std::chrono::nanoseconds averageTime,
		std::chrono::nanoseconds medianTime
	) noexcept;

	TimingResult& operator+=(const TimingResult& other);
};
