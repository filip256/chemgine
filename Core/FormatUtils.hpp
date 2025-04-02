#pragma once

#include <string>
#include <chrono>

namespace Utils
{
	void pluralize(std::string& str);
	std::string pluralize(const std::string& str);

	void capitalize(std::string& str);
	std::string capitalize(const std::string& str);

	void formatFloatingPoint(std::string& str, const uint8_t maxDigits = 255);
	std::string formatFloatingPoint(const std::string& str, const uint8_t maxDigits = 255);

	template<typename T, typename = std::enable_if<std::is_floating_point_v<T>>>
	std::string formatFloatingPoint(const T& value, const uint8_t maxDigits = 255);

	void padFront(std::string& str, const size_t minLength, const char padding);
	std::string padFront(const std::string& str, const size_t minLength, const char padding);

	enum class TimeFormat
	{
		DIGITAL_HH_MM_SS,
		HUMAN_HH_MM_SS,
	};

	template<typename TimeT>
	std::string formatTime(TimeT time, const TimeFormat format);

	std::string toHex(const uint64_t value);
	std::string toHex(const void* ptr);
};

template<typename T, typename>
std::string Utils::formatFloatingPoint(const T& value, const uint8_t maxDigits)
{
	return formatFloatingPoint(std::to_string(value), maxDigits);
}

template<typename TimeT>
std::string Utils::formatTime(TimeT time, const TimeFormat format)
{
	const auto h = std::chrono::duration_cast<std::chrono::hours>(time);
	time -= h;
	const auto m = std::chrono::duration_cast<std::chrono::minutes>(time);
	time -= m;
	const auto s = std::chrono::duration_cast<std::chrono::seconds>(time);

	switch (format)
	{
	case TimeFormat::DIGITAL_HH_MM_SS:
		return std::to_string(h.count()) + ':' +
			padFront(std::to_string(m.count()), 2, '0') + ':' +
			padFront(std::to_string(s.count()), 2, '0');

	case TimeFormat::HUMAN_HH_MM_SS:
	{
		std::string result;
		if (h.count() > 0) {
			result += std::to_string(h.count()) + 'h';
		}
		if (m.count() > 0) {
			result += std::to_string(m.count()) + 'm';
		}
		if (s.count() > 0) {
			result += std::to_string(s.count()) + 's';
		}

		return result.size() ? result : "0s";
	}

	default:
		throw std::runtime_error("Unknown time format.");
		return "";
	}
}