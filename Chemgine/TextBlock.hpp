#pragma once

#include <vector>
#include <string>
#include <ostream>

/// <summary>
/// 2D block of text which expands as it's being written to, filling empty chars with spaces.
/// </summary>
class TextBlock
{
public:
	class TextLine
	{
	private:
		std::string text;

	public:
		TextLine() = default;
		TextLine(const size_t size) noexcept;
		TextLine(const TextLine&) = delete;
		TextLine(TextLine&&) = default;

		TextLine& operator=(TextLine&& other) = default;

		size_t size() const;
		size_t getFirst() const;
		size_t getLast() const;
		const std::string& getString() const;
		bool isWhiteSpace() const;

		void expandTo(const size_t size);
		void insert(const size_t x, const char c);
		void insert(const size_t x, const char* str);
		void insert(const size_t x, const std::string& str);
		void trim(const size_t left, const size_t right);
		void trim();
		void clear();

		char operator[](const size_t x) const;
		char& operator[](const size_t x);

		TextLine& operator+=(const char c);
		TextLine& operator+=(const char* str);
		TextLine& operator+=(const std::string& str);
	};

private:
	std::vector<TextLine> block;

public:
	TextBlock() = default;
	TextBlock(const size_t sizeX, const size_t sizeY) noexcept;
	TextBlock(const TextBlock&) = delete;
	TextBlock(TextBlock&&) = default;

	void expandTo(const size_t sizeY);
	void expandTo(const size_t sizeX, const size_t sizeY);

	/// <summary>
	/// Trims any excess white spaces, while maintaining the original printable shape.
	/// </summary>
	void trim();
	void clear();

	size_t size() const;
	std::string toString() const;

	const TextLine& operator[](const size_t x) const;
	TextLine& operator[](const size_t x);

	friend std::ostream& operator<<(std::ostream& os, const TextBlock& block);
};
