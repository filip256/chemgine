#include "TextBlock.hpp"

TextBlock::TextLine::TextLine(const size_t size) noexcept :
	text(size, ' ')
{}

TextBlock::TextLine::TextLine(const std::string& str) noexcept :
	text(str)
{}

TextBlock::TextLine::TextLine(std::string&& str) noexcept :
	text(std::move(str))
{}

size_t TextBlock::TextLine::size() const
{
	return text.size();
}

size_t TextBlock::TextLine::getFirst() const
{
	return text.find_first_not_of(' ');
}

size_t TextBlock::TextLine::getLast() const
{
	return text.find_last_not_of(' ');
}

const std::string& TextBlock::TextLine::getString() const
{
	return text;
}

bool TextBlock::TextLine::isWhiteSpace() const
{
	return getFirst() == std::string::npos;
}

void TextBlock::TextLine::trim(const size_t left, const size_t right)
{
	if (text.empty())
		return;

	if (left > text.size())
		text = "";

	text = text.substr(left, right - left + 1);
	text.shrink_to_fit();
}

void TextBlock::TextLine::trim()
{
	trim(getFirst(), getLast());
}

void TextBlock::TextLine::clear()
{
	text.clear();
}

void TextBlock::TextLine::expandTo(const size_t size)
{
	if (size > text.size())
		text.resize(size, ' ');
}

void TextBlock::TextLine::expandWith(const size_t size)
{
	text += std::string(size, ' ');
}

void TextBlock::TextLine::expandWith(const std::string& str)
{
	text += str;
}

void TextBlock::TextLine::expandWith(const TextLine& other)
{
	this->expandWith(other.getString());
}

void TextBlock::TextLine::insert(const size_t x, const char c)
{
	expandTo(x);
	text += c;
}

void TextBlock::TextLine::insert(const size_t x, const char* str)
{
	for (size_t i = 0; str[i]; ++i)
		(*this)[x + i] = str[i];
}

void TextBlock::TextLine::insert(const size_t x, const std::string& str)
{
	insert(x, str.c_str());
}

char TextBlock::TextLine::operator[](const size_t x) const
{
	return text[x];
}

char& TextBlock::TextLine::operator[](const size_t x)
{
	expandTo(x + 1);
	return text[x];
}

TextBlock::TextLine& TextBlock::TextLine::operator+=(const char c)
{
	(*this)[getLast() + 1] = c;
	return *this;
}

TextBlock::TextLine& TextBlock::TextLine::operator+=(const char* str)
{
	const size_t last = getLast();
	for (size_t i = 0; str[i]; ++i)
		(*this)[last + i + 1] = str[i];
	return *this;
}

TextBlock::TextLine& TextBlock::TextLine::operator+=(const std::string& str)
{
	return *this += str.c_str();
}

TextBlock::TextBlock(const size_t witdh, const size_t height) noexcept
{
	block.reserve(height);
	expandTo(witdh, height);
}

TextBlock::TextBlock(const std::string& str) noexcept
{
	size_t lastSep = -1;
	for (size_t i = 0; i < str.size(); ++i)
		if (str[i] == '\n')
		{
			block.emplace_back(str.substr(lastSep + 1, i - lastSep - 1));
			lastSep = i;
		}

	block.emplace_back(std::move(str.substr(lastSep + 1)));
}

TextBlock::TextBlock(const char* str) noexcept:
	TextBlock(std::string(str))
{}

void TextBlock::expandTo(const size_t height)
{
	for (size_t i = getHeight(); i < height; ++i)
		block.emplace_back(0);
}

void TextBlock::expandAndCenterVerticallyTo(const size_t witdh, const size_t height)
{
	if (getHeight() > height)
		return;

	if (empty())
	{
		expandTo(height);
		return;
	}

	const auto diff = height - getHeight();
	const auto centerOffset = diff / 2;

	size_t i = 0;
	while (i < centerOffset)
	{
		block.insert(block.begin(), std::string(witdh, ' '));
		++i;
	}
	while (i < diff)
	{
		block.emplace_back(witdh);
		++i;
	}
}

void TextBlock::expandTo(const size_t witdh, const size_t height)
{
	for (size_t i = 0; i < getHeight(); ++i)
		block[i].expandTo(witdh);
	for (size_t i = getHeight(); i < height; ++i)
		block.emplace_back(witdh);
}

TextBlock& TextBlock::appendRight(const TextBlock& other, const std::string& padding)
{
	if (other.empty())
		return *this;

	this->expandAndCenterVerticallyTo(this->getWidth(), other.getHeight());
	
	const auto diff = this->getHeight() - other.getHeight();
	const auto centerOffset = diff / 2;

	size_t i = 0;
	while (i < centerOffset)
	{
		this->block[i].expandWith(padding);
		this->block[i].expandWith(other.getWidth());
		++i;
	}
	while (i < other.getHeight() + centerOffset)
	{
		this->block[i].expandWith(padding);
		this->block[i].expandWith(other[i - centerOffset]);
		++i;
	}
	while (i < this->getHeight())
	{
		this->block[i].expandWith(padding);
		this->block[i].expandWith(other.getWidth());
		++i;
	}

	return *this;
}

void TextBlock::trim()
{
	size_t wStart = std::string::npos, wEnd = 0;
	size_t hStart = 0, hEnd = 0;

	size_t idx = 0;
	while (idx < getHeight() && block[idx].isWhiteSpace()) ++idx;
	hStart = idx;

	while (idx < getHeight())
	{
		const auto first = block[idx].getFirst();
		if (first != std::string::npos)
		{
			hEnd = idx;
			wStart = std::min(wStart, first);
			wEnd = std::max(wEnd, block[idx].getLast());
		}
		++idx;
	}

	if (hStart <= hEnd)
	{
		block.erase(block.begin() + hEnd + 1, block.end());
		block.erase(block.begin(), block.begin() + hStart);
	}
	else
		block.clear();

	for (size_t i = 0; i < getHeight(); ++i)
		block[i].trim(wStart, wEnd);

	block.shrink_to_fit();
}

void TextBlock::clear()
{
	for (size_t i = 0; i < getHeight(); ++i)
		block[i].clear();
}

size_t TextBlock::getHeight() const
{
	return block.size();
}

size_t TextBlock::getWidth() const
{
	return getHeight() ? block.front().size() : 0;
}

bool TextBlock::empty() const
{
	return block.empty();
}

std::string TextBlock::toString() const
{
	if (block.empty())  // prevent size_t = -1 later
		return "";

	std::string result;

	for (size_t i = 0; i < getHeight() - 1; ++i)
		result += block[i].getString() + '\n';
	result += block.back().getString();

	return result;
}

const TextBlock::TextLine& TextBlock::operator[](const size_t y) const
{
	return block[y];
}

TextBlock::TextLine& TextBlock::operator[](const size_t y)
{
	expandTo(0, y + 1);
	return block[y];
}

std::ostream& operator<<(std::ostream& os, const TextBlock& block)
{
	for (size_t i = 0; i < block.getHeight(); ++i)
		os << block[i].getString() + '\n';
	return os;
}
