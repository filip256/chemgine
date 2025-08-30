#pragma once

#include "utils/Terminal.hpp"

#include <string>
#include <vector>

//
// ColoredChar
//

class ColoredChar
{
public:
    char           chr;
    OS::BasicColor color;

    constexpr ColoredChar(const char chr, const OS::BasicColor color = OS::BasicColor::NONE) noexcept;
    ColoredChar(const ColoredChar&) = default;

    bool operator==(const ColoredChar other) const;
    bool operator!=(const ColoredChar other) const;
};

constexpr ColoredChar::ColoredChar(const char chr, const OS::BasicColor color) noexcept :
    chr(chr),
    color(color)
{}

//
// ColoredString
//

class ColoredString
{
private:
    // Storing the color of each char requires a simpler implementation and gives better
    // performance, but it takes more memory than a color-change set implementation. The former is
    // preferred since ColoredString's aren't usually going to be stored for long.
    using ContainerType = std::vector<ColoredChar>;
    ContainerType str;

    ColoredString(ContainerType&& str) noexcept;

    static std::vector<ColoredChar> bind(const char* str, const OS::BasicColor color);
    static std::vector<ColoredChar> bind(const std::string& str, const OS::BasicColor color);

public:
    using value_type = ColoredChar;

    static constexpr size_t npos = std::string::npos;

    ColoredString() = default;
    ColoredString(const char* str, const OS::BasicColor color = OS::BasicColor::NONE) noexcept;
    ColoredString(const std::string& str, const OS::BasicColor color = OS::BasicColor::NONE) noexcept;
    ColoredString(const size_t size, const ColoredChar chr) noexcept;
    template <typename It>
    ColoredString(const It first, const It last) noexcept;
    ColoredString(const ColoredString&) = default;
    ColoredString(ColoredString&&)      = default;

    ColoredString& operator=(ColoredString&&) = default;

    std::string toString() const;

    bool   empty() const;
    size_t size() const;

    void clear();
    void reserve(const size_t size);
    void resize(const size_t size, const ColoredChar coloredChr);
    void shrink_to_fit();

    using Iterator      = ContainerType::iterator;
    using ConstIterator = ContainerType::const_iterator;
    Iterator      begin();
    Iterator      end();
    ConstIterator begin() const;
    ConstIterator end() const;
    ConstIterator cbegin() const;
    ConstIterator cend() const;
    using ReverseIterator      = ContainerType::reverse_iterator;
    using ConstReverseIterator = ContainerType::const_reverse_iterator;
    ReverseIterator      rbegin();
    ReverseIterator      rend();
    ConstReverseIterator rbegin() const;
    ConstReverseIterator rend() const;
    ConstReverseIterator crbegin() const;
    ConstReverseIterator crend() const;

    ColoredChar  front() const;
    ColoredChar& front();
    ColoredChar  back() const;
    ColoredChar& back();

    void push_back(const ColoredChar coloredChr);
    void append(const ColoredString& other);
    void append(const std::string& str, const OS::BasicColor color = OS::BasicColor::NONE);
    void insert(const size_t idx, const ColoredChar coloredChr);
    void insert(const size_t idx, const ColoredString& other);
    template <typename It>
    void insert(const Iterator pos, const It first, const It last);

    ColoredChar  operator[](const size_t idx) const;
    ColoredChar& operator[](const size_t idx);

    ColoredString& operator+=(const ColoredChar chr);
    ColoredString& operator+=(const ColoredString& other);
    ColoredString& operator+=(const std::string& str);
    ColoredString& operator<<(const ColoredChar chr);
    ColoredString& operator<<(const ColoredString& other);
    ColoredString& operator<<(const std::string& str);
    ColoredString  operator+(const ColoredChar chr);
    ColoredString  operator+(const ColoredString& other) const;
    ColoredString  operator+(const std::string& str) const;

    ColoredString substr(const size_t pos = 0, const size_t count = npos) const;
    size_t        find_first_not_of(const ColoredChar chr, const size_t pos = 0) const;
    size_t        find_last_not_of(const ColoredChar chr, const size_t pos = 0) const;

    friend std::ostream& operator<<(std::ostream& os, const ColoredString& coloredString);
};

template <typename It>
ColoredString::ColoredString(const It first, const It last) noexcept :
    str(first, last)
{}

template <typename It>
void ColoredString::insert(const Iterator pos, const It first, const It last)
{
    str.insert(pos, first, last);
}
