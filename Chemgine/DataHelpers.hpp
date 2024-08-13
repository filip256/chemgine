#pragma once

#include "DynamicAmount.hpp"
#include "Spline.hpp"
#include "Utils.hpp"
#include "StringUtils.hpp"
#include "Symbol.hpp"
#include "Color.hpp"
#include "LabwarePort.hpp"
#include "Molecule.hpp"
#include "EstimatorSpecifier.hpp"
#include "ReactionSpecifier.hpp"

#include <vector>
#include <string>
#include <optional>
#include <stdexcept>

class DataHelpers
{
public:
	template <typename T>
	static std::optional<T> parse(const std::string& str);
	template <typename T>
	static std::optional<T> parseUnsigned(const std::string& str);

	template <Unit U>
	static std::optional<Amount<U>> parse(const std::string& str);
	template <Unit U>
	static std::optional<Amount<U>> parseUnsigned(const std::string& str);

	template <typename E, typename = std::enable_if_t<std::is_enum_v<E>>>
	static std::optional<E> parseEnum(const std::string& str);

	template <typename T>
	static std::optional<T> parseId(const std::string& str);

	template <typename T>
	static std::optional<std::vector<T>> parseList(
		const std::string& line,
		const char sep,
		const bool ignoreEmpty = false);

	template <typename T1, typename T2>
	static std::optional<std::pair<T1, T2>> parsePair(
		const std::string& str,
		const char sep = '@');
	template <Unit U1, Unit U2>

	static std::optional<std::pair<Amount<U1>, Amount<U2>>> parsePair(
		const std::string& str,
		const char sep = '@');
	template <typename T, Unit U2>

	static std::optional< std::pair<T, Amount<U2>>> parsePair(
		const std::string& str,
		const char sep = '@');


	template <typename T, typename = void>
	class Parser
	{
		static_assert(std::is_arithmetic_v<T>, "Parser: Unsupported non-arithmetic type");

	public:
		static std::optional<T> parse(const std::string& str)
		{
			const auto r = DataHelpers::parse<int64_t>(str);

			if (r.has_value() == false ||
				*r < std::numeric_limits<T>::min() ||
				*r > std::numeric_limits<T>::max())
				return std::nullopt;


			return std::optional<T>(*r);
		}
	};

	template <>
	class Parser<std::string>
	{
	public:
		static std::optional<std::string> parse(const std::string& str)
		{
			return Utils::strip(str);
		}
	};

	template <>
	class Parser<std::vector<std::string>>
	{
	public:
		static std::optional<std::vector<std::string>> parse(const std::string& str)
		{
			auto list = Utils::split(str, ',', true);
			std::for_each(list.begin(), list.end(), [](auto& str) { Utils::strip(str); });
			return list;
		}
	};

	template <>
	class Parser<int64_t>
	{
	public:
		static std::optional<int64_t> parse(const std::string& str)
		{
			if (str.empty())
				return std::nullopt;

			try
			{
				return std::optional<int64_t>(std::stoll(str));
			}
			catch (const std::invalid_argument&)
			{
				return std::nullopt;
			}
			catch (const std::out_of_range&)
			{
				return std::nullopt;
			}
		}
	};

	template <>
	class Parser<uint64_t>
	{
	public:
		static std::optional<uint64_t> parse(const std::string& str)
		{
			if (str.empty())
				return std::nullopt;

			try
			{
				return std::optional<int64_t>(std::stoull(str));
			}
			catch (const std::invalid_argument&)
			{
				return std::nullopt;
			}
			catch (const std::out_of_range&)
			{
				return std::nullopt;
			}
		}
	};

	template <>
	class Parser<float>
	{
	public:
		static std::optional<float> parse(const std::string& str)
		{
			if (str.empty())
				return std::nullopt;

			try
			{
				return std::optional<float>(std::stof(str));
			}
			catch (const std::invalid_argument&)
			{
				return std::nullopt;
			}
			catch (const std::out_of_range&)
			{
				return std::nullopt;
			}
		}
	};

	template <>
	class Parser<double>
	{
	public:
		static std::optional<double> parse(const std::string& str)
		{
			if (str.empty())
				return std::nullopt;

			try
			{
				return std::optional<double>(std::stod(str));
			}
			catch (const std::invalid_argument&)
			{
				return std::nullopt;
			}
			catch (const std::out_of_range&)
			{
				return std::nullopt;
			}
		}
	};

	template <>
	class Parser<bool>
	{
	public:
		static std::optional<int64_t> parse(const std::string& str)
		{
			if (str.empty())
				return std::nullopt;

			if (str == "1" || str == "T" || str == "t" || str == "TRUE" || str == "true")
				return std::optional(true);
			if (str == "0" || str == "F" || str == "f" || str == "FALSE" || str == "false")
				return std::optional(false);

			return std::nullopt;
		}
	};

	template <Unit U>
	class Parser<Amount<U>>
	{
	public:
		static std::optional<Amount<U>> parse(const std::string& str)
		{
			const auto pair = Utils::split(str, '_', true);
			if (pair.empty())
				return std::nullopt;

			const auto val = DataHelpers::parse<Amount<>::StorageType>(pair.front());
			if (val.has_value() == false)
				return std::nullopt;

			if (pair.size() == 1)
				return Amount<U>(*val);

			if (pair.size() == 2)
				return DynamicAmount::get<U>(*val, pair.back());

			return std::nullopt;
		}
	};

	template <>
	class Parser<DynamicAmount>
	{
	public:
		static std::optional<DynamicAmount> parse(const std::string& str)
		{
			const auto pair = Utils::split(str, '_', true);
			if (pair.empty())
				return std::nullopt;

			const auto val = DataHelpers::parse<Amount<>::StorageType>(pair.front());
			if (val.has_value() == false)
				return std::nullopt;

			if (pair.size() == 1)
				return DynamicAmount(*val, Unit::ANY);

			if (pair.size() == 2)
				return DynamicAmount::get(*val, pair.back());

			return std::nullopt;
		}
	};

	template <>
	class Parser<Symbol>
	{
	public:
		static std::optional<Symbol> parse(const std::string& str)
		{
			if (str.size() != 1 && str.size() != 2)
				return std::nullopt;

			return Symbol(str);
		}
	};

	template <>
	class Parser<Color>
	{
	public:
		static std::optional<Color> parse(const std::string& str)
		{
			if (str.empty())
				return std::nullopt;

			const auto rgba = parseList<uint8_t>(str, ':', true);
			if (rgba.has_value() == false || rgba->size() != 4)
				return std::nullopt;

			return Color((*rgba)[0], (*rgba)[1], (*rgba)[2], (*rgba)[3]);
		}
	};

	template <typename T>
	class Parser<Spline<T>>
	{
	public:
		static std::optional<Spline<T>> parse(const std::string& str)
		{
			const auto& pointsStr = Utils::split(str, ';', true);

			std::vector<std::pair<float, float>> points;
			points.reserve(pointsStr.size());

			for (size_t i = 0; i < pointsStr.size(); ++i)
			{
				const auto p = parsePair<double, double>(pointsStr[i]);
				if (p.has_value() == false)
					return std::nullopt;
				points.emplace_back(Utils::reversePair(*p));
			}

			return points.empty() ? std::nullopt : std::optional<Spline<float>>(Spline(std::move(points)));
		}
	};

	template <>
	class Parser<MolecularStructure>
	{
	public:
		static std::optional<MolecularStructure> parse(const std::string& str)
		{
			MolecularStructure molecule(str);
			return molecule.isEmpty() ?
				std::nullopt :
				std::optional(std::move(molecule));
		}
	};

	template <>
	class Parser<Molecule>
	{
	public:
		static std::optional<Molecule> parse(const std::string& str)
		{
			return DataHelpers::parse<MolecularStructure>(str);
		}
	};

	template <>
	class Parser<LabwarePort>
	{
	public:
		static std::optional<LabwarePort> parse(const std::string& str)
		{
			const auto port = Utils::split(str, ':', true);

			if (port.size() != 4)
				return std::nullopt;

			const auto type = DataHelpers::parseEnum<PortType>(port[0]);
			const auto x = DataHelpers::parse<uint32_t>(port[1]);
			const auto y = DataHelpers::parse<uint32_t>(port[2]);
			const auto angle = DataHelpers::parse<Unit::DEGREE>(port[3]);

			if (type.has_value() == false || x.has_value() == false || y.has_value() == false || angle.has_value() == false)
				return std::nullopt;

			return LabwarePort(*type, *x, *y, *angle);
		}
	};

	template <typename T>
	class Parser<std::vector<T>>
	{
	public:
		static std::optional<std::vector<T>> parse(const std::string& str, const char sep = ',')
		{
			return parseList<T>(str, sep, true);
		}
	};

	template <typename T1, typename T2>
	class Parser<std::pair<T1, T2>>
	{
	public:
		static std::optional<std::pair<T1, T2>> parse(const std::string& str, const char sep = '@')
		{
			const auto& pairStr = Utils::split(str, sep, true);
			if (pairStr.size() != 2)
				return std::nullopt;

			const auto val1 = DataHelpers::parse<T1>(pairStr.front());
			if (val1.has_value() == false)
				return std::nullopt;

			const auto val2 = DataHelpers::parse<T2>(pairStr.back());
			if (val2.has_value() == false)
				return std::nullopt;

			return std::optional<std::pair<T1, T2>>(std::make_pair(*val1, *val2));
		}
	};

	template <>
	class Parser<EstimatorSpecifier>
	{
	public:
		static std::optional<EstimatorSpecifier> parse(const std::string& str)
		{
			const auto sep = str.find("->");
			if (sep > str.size() - 3)
				return std::nullopt;

			const auto inStr = Utils::strip(str.substr(0, sep));
			const auto outStr = Utils::strip(str.substr(sep + 2));

			const auto inUnit = DynamicAmount::getUnitFromSymbol(inStr);
			const auto outUnit = DynamicAmount::getUnitFromSymbol(outStr);

			return inUnit.has_value() && outUnit.has_value() ?
				std::optional(EstimatorSpecifier(*inUnit, *outUnit)) :
				std::nullopt;
		}
	};

	template <>
	class Parser<ReactionSpecifier>
	{
	public:
		static std::optional<ReactionSpecifier> parse(const std::string& str)
		{
			const auto sep = str.find("->");
			if (sep > str.size() - 3)
				return std::nullopt;

			const auto reactantsStr = str.substr(0, sep);
			const auto productsStr = str.substr(sep + 2);

			auto reactants = Utils::split(reactantsStr, '+', false);
			auto products = Utils::split(productsStr, '+', false);

			if (reactants.empty() || products.empty())
				return std::nullopt;

			for (size_t i = 0; i < reactants.size(); ++i)
			{
				Utils::strip(reactants[i]);
				if (reactants[i].empty())
					return std::nullopt;
			}

			for (size_t i = 0; i < products.size(); ++i)
			{
				Utils::strip(products[i]);
				if (products[i].empty())
					return std::nullopt;
			}

			return std::optional<ReactionSpecifier>(std::in_place,
				std::move(reactants), std::move(products));
		}
	};
};


template <typename T>
inline std::optional<T> DataHelpers::parse(const std::string& str)
{
	return DataHelpers::Parser<T>::parse(str);
}

template <typename T>
inline std::optional<T> DataHelpers::parseUnsigned(const std::string& str)
{
	const auto r = DataHelpers::Parser<T>::parse(str);
	return r.has_value() == false || *r < 0 ?
		std::nullopt :
		std::optional(r);
}

template <Unit U>
static std::optional<Amount<U>> DataHelpers::parse(const std::string& str)
{
	return DataHelpers::Parser<Amount<U>>::parse(str);
}

template <Unit U>
static std::optional<Amount<U>> DataHelpers::parseUnsigned(const std::string& str)
{
	const auto r = DataHelpers::Parser<Amount<U>>::parse(str);
	return r.has_value() == false || *r < 0 ?
		std::nullopt :
		std::optional(r);
}

template <typename E, typename>
inline std::optional<E> DataHelpers::parseEnum(const std::string& str)
{
	const auto val = parse<std::underlying_type_t<E>>(str);
	return val.has_value() ? std::make_optional(static_cast<E>(*val)) : std::nullopt;
}

template <typename T>
static std::optional<T> DataHelpers::parseId(const std::string& str)
{
	if (str.empty() || str.front() != '#')
		return std::nullopt;
	
	return parseUnsigned<T>(str.substr(1));
}

template <typename T>
std::optional<std::vector<T>> DataHelpers::parseList(
	const std::string& line,
	const char sep,
	const bool ignoreEmpty)
{
	if (line.starts_with('{') && line.ends_with('}'))
		return parseList<T>(line.substr(1, line.size() - 2), sep, ignoreEmpty);

	std::vector<T> result;
	size_t lastSep = -1;

	for (size_t i = 0; i < line.size(); ++i)
		if (line[i] == sep)
		{
			if (ignoreEmpty == false || i - lastSep - 1 > 0)
			{
				auto r = parse<T>(Utils::strip(line.substr(lastSep + 1, i - lastSep - 1)));
				if (r.has_value())
					result.emplace_back(std::move(*r));
				else
					return std::nullopt;
			}
			lastSep = i;
		}
	if (ignoreEmpty == false || lastSep + 1 < line.size())
	{
		auto r = parse<T>(Utils::strip(line.substr(lastSep + 1)));
		if (r.has_value())
			result.emplace_back(std::move(*r));
		else
			return std::nullopt;
	}
	return result;
}

template <typename T1, typename T2>
inline std::optional<std::pair<T1, T2>> DataHelpers::parsePair(
	const std::string& str,
	const char sep)
{
	return Parser<std::pair<T1, T2>>::parse(str, sep);
}

template <Unit U1, Unit U2>
static std::optional<std::pair<Amount<U1>, Amount<U2>>> DataHelpers::parsePair(
	const std::string& str,
	const char sep)
{
	return parsePair<Amount<U1>, Amount<U2>>(str, sep);
}

template <typename T, Unit U>
static std::optional< std::pair<T, Amount<U>>> DataHelpers::parsePair(
	const std::string& str,
	const char sep)
{
	return parsePair<T, Amount<U>>(str, sep);
}
