#include "RadicalData.hpp"
#include "AtomRepository.hpp"
#include "Log.hpp"
#include "Utils.hpp"

const std::unordered_set<AtomId> RadicalData::MatchAny = std::unordered_set<AtomId>{ 0 };

RadicalData::RadicalData(
	const AtomId id,
	const Symbol symbol,
	const std::string& name,
	std::unordered_set<AtomId>&& matchables
) noexcept :
	AtomData(id, symbol, name, 0.0_g, Utils::copy(AtomData::RadicalAnyValence)),
	matchables(std::move(matchables))
{}
