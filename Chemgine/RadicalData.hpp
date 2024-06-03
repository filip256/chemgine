#pragma once

#include "AtomData.hpp"

#include <unordered_set>

class AtomRepository;

class RadicalData : public AtomData
{
public:
	const std::unordered_set<AtomId> matchables;

	RadicalData(
		const AtomId id,
		const Symbol symbol,
		const std::string& name,
		std::unordered_set<AtomId>&& matchables
	) noexcept;

	RadicalData(
		const AtomId id,
		const Symbol symbol,
		const std::string& name,
		const std::unordered_set<Symbol>& matchables,
		const AtomRepository& repository
	) noexcept;

	static const std::unordered_set<AtomId> MatchAny;
};
