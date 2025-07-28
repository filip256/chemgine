#include "ColoredString.hpp"

#include "Log.hpp"

#include <iostream>

//
// ColoredChar
//

constexpr ColoredChar::ColoredChar(
	const char chr,
	const OS::ColorType color
) noexcept :
	chr(chr),
	color(color)
{}

bool ColoredChar::operator==(const ColoredChar other) const
{
	return this->chr == other.chr && this->color == other.color;
}

bool ColoredChar::operator!=(const ColoredChar other) const
{
	return this->chr != other.chr || this->color != other.color;
}

//
// ColoredString
//

ColoredString::ColoredString(ContainerType&& str) noexcept :
	str(std::move(str))
{}

ColoredString::ColoredString(
	const char* str,
	const OS::ColorType color
) noexcept :
	ColoredString(bind(str, color))
{}

ColoredString::ColoredString(
	const std::string& str,
	const OS::ColorType color
) noexcept :
	ColoredString(bind(str, color))
{}

ColoredString::ColoredString(
	const size_t size,
	const ColoredChar chr
) noexcept :
	str(size, chr)
{}

std::vector<ColoredChar> ColoredString::bind(const char* str, const OS::ColorType color)
{
	std::vector<ColoredChar> temp;
	for (size_t i = 0; str[i] != '\0'; ++i)
		temp.emplace_back(str[i], color);
	
	return temp;
}

std::vector<ColoredChar> ColoredString::bind(const std::string& str, const OS::ColorType color)
{
	std::vector<ColoredChar> temp;
	temp.reserve(str.size());
	std::ranges::transform(str, std::back_inserter(temp),
		[color](const auto c) { return ColoredChar(c, color); });

	return temp;
}

std::string ColoredString::toString() const
{
	std::string temp;
	temp.reserve(str.size());
	std::ranges::transform(str, std::back_inserter(temp),
		[](const auto c) { return c.chr; });

	return temp;
}

bool ColoredString::empty() const
{
	return str.empty();
}

size_t ColoredString::size() const
{
	return str.size();
}

void ColoredString::clear()
{
	str.clear();
}

void ColoredString::reserve(const size_t size)
{
	str.reserve(size);
}

void ColoredString::resize(const size_t size, const ColoredChar coloredChr)
{
	str.resize(size, coloredChr);
}

void ColoredString::shrink_to_fit()
{
	str.shrink_to_fit();
}

ColoredString::Iterator ColoredString::begin()
{
	return str.begin();
}

ColoredString::Iterator ColoredString::end()
{
	return str.end();
}

ColoredString::ConstIterator ColoredString::begin() const
{
	return str.begin();
}

ColoredString::ConstIterator ColoredString::end() const
{
	return str.end();
}

ColoredString::ConstIterator ColoredString::cbegin() const
{
	return str.cbegin();
}

ColoredString::ConstIterator ColoredString::cend() const
{
	return str.cend();
}

ColoredString::ReverseIterator ColoredString::rbegin()
{
	return str.rbegin();
}

ColoredString::ReverseIterator ColoredString::rend()
{
	return str.rend();
}

ColoredString::ConstReverseIterator ColoredString::rbegin() const
{
	return str.rbegin();
}

ColoredString::ConstReverseIterator ColoredString::rend() const
{
	return str.rend();
}

ColoredString::ConstReverseIterator ColoredString::crbegin() const
{
	return str.crbegin();
}

ColoredString::ConstReverseIterator ColoredString::crend() const
{
	return str.crend();
}

ColoredChar ColoredString::front() const
{
	return str.front();
}

ColoredChar& ColoredString::front()
{
	return str.front();
}

ColoredChar ColoredString::back() const
{
	return str.back();
}

ColoredChar& ColoredString::back()
{
	return str.back();
}

void ColoredString::push_back(const ColoredChar coloredChr)
{
	str.push_back(coloredChr);
}

void ColoredString::append(const ColoredString& other)
{
	this->str.insert(this->str.end(), other.str.begin(), other.str.end());
}

void ColoredString::append(const std::string& str, const OS::ColorType color)
{
	for (const auto c : str)
		push_back(ColoredChar(c, color));
}

void ColoredString::insert(const size_t idx, const ColoredChar coloredChr)
{
	str.insert(str.begin() + idx, coloredChr);
}

void ColoredString::insert(const size_t idx, const ColoredString& other)
{
	this->str.insert(this->str.begin() + idx, other.str.begin(), other.str.end());
}

ColoredChar ColoredString::operator[](const size_t idx) const
{
	return str[idx];
}

ColoredChar& ColoredString::operator[](const size_t idx)
{
	return str[idx];
}

ColoredString& ColoredString::operator+=(const ColoredChar chr)
{
	push_back(chr);
	return *this;
}

ColoredString& ColoredString::operator+=(const ColoredString& other)
{
	append(other);
	return *this;
}

ColoredString& ColoredString::operator+=(const std::string& str)
{
	append(str);
	return *this;
}

ColoredString& ColoredString::operator<<(const ColoredChar chr)
{
	return operator+=(chr);
}

ColoredString& ColoredString::operator<<(const ColoredString& other)
{
	return operator+=(other);
}

ColoredString& ColoredString::operator<<(const std::string& str)
{
	return operator+=(str);
}

ColoredString ColoredString::operator+(const ColoredChar chr)
{
	ColoredString temp(*this);
	return temp += chr;
}

ColoredString ColoredString::operator+(const ColoredString& other) const
{
	ColoredString temp(*this);
	return temp += other;
}

ColoredString ColoredString::operator+(const std::string& str) const
{
	ColoredString temp(*this);
	return temp += str;
}

ColoredString ColoredString::substr(const size_t pos, const size_t count) const
{
	if (pos >= str.size())
		Log(this).fatal("substr(): Out-of-range starting position: {0} (size: {1})", pos, str.size());

	const auto maxCount = std::min(count, str.size() - pos);
	return std::vector<ColoredChar>(str.begin() + pos, str.begin() + pos + maxCount);
}

size_t ColoredString::find_first_not_of(const ColoredChar chr, const size_t pos) const
{
	for (size_t i = pos; i < str.size(); ++i)
		if (str[i] != chr)
			return i;
	return npos;
}

size_t ColoredString::find_last_not_of(const ColoredChar chr, const size_t pos) const
{
	for (size_t i = str.size(); i-- > pos; )
		if (str[i] != chr)
			return i;
	return npos;
}

std::ostream& operator<<(std::ostream& os, const ColoredString& coloredString)
{
	// Ignore colors if the output stream doesn't support colors.
	if (&os != &std::cout && &os != &std::cerr && &os != &std::clog)
	{
		for (const auto c : coloredString.str)
			os << c.chr;
		return os;
	}

	const auto initialColor = OS::getTextColor();
	for (const auto c : coloredString.str)
	{
		OS::setTextColor(c.color != OS::Color::None ? c.color : initialColor);
		os << c.chr;
	}

	OS::setTextColor(initialColor);
	return os;
}
