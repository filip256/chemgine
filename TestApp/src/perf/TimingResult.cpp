#include "perf/TimingResult.hpp"

TimingResult::TimingResult(
	std::chrono::nanoseconds averageTime,
	std::chrono::nanoseconds medianTime
) noexcept :
	averageTime(averageTime),
	medianTime(medianTime)
{}

TimingResult& TimingResult::operator+=(const TimingResult& other)
{
	this->averageTime += other.averageTime;
	this->medianTime += other.medianTime;
	return *this;
}
