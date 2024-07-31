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

	static const std::unordered_set<AtomId> MatchAny;
};
