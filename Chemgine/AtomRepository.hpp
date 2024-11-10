#pragma once

#include "AtomData.hpp"
#include "RadicalData.hpp"
#include "Object.hpp"

class AtomRepository
{
private:
	std::unordered_map<Symbol, std::unique_ptr<const AtomData>> atoms;
	std::unordered_map<Symbol, std::unique_ptr<const RadicalData>> radicals;

public:
	AtomRepository() = default;
	AtomRepository(const AtomRepository&) = delete;
	AtomRepository(AtomRepository&&) = default;

	template <typename AtomT>
	bool add(const Def::Object& definition);

	bool contains(const Symbol& symbol) const;
	const AtomData& at(const Symbol& symbol) const;

	size_t totalDefinitionCount() const;

	using AtomIterator = std::unordered_map<Symbol, std::unique_ptr<const AtomData>>::const_iterator;
	AtomIterator atomsBegin() const;
	AtomIterator atomsEnd() const;

	using RadicalIterator = std::unordered_map<Symbol, std::unique_ptr<const RadicalData>>::const_iterator;
	RadicalIterator radicalsBegin() const;
	RadicalIterator radicalsEnd() const;
	
	void clear();

	static constexpr size_t npos = static_cast<size_t>(-1);
};
