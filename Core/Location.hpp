#pragma once

#include <string>

namespace Def
{
	class Location
	{
	private:
		const static size_t eofLine = static_cast<size_t>(-1);

		std::string file;
		size_t line;

	public:
		Location(
			const std::string& file,
			const size_t line
		) noexcept;
		Location(const Location&) = default;
		Location(Location&&) = default;

		Location& operator=(Location&&) = default;

		const std::string& getFile() const;
		size_t getLine() const;

		bool isEOF() const;

		std::string toString() const;

		static Location createUnknown();
		static Location createEOF(const std::string& file);
	};
}
