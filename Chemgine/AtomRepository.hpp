#pragma once

#include "AtomData.hpp"
#include "RadicalData.hpp"
#include "DefinitionObject.hpp"

class AtomRepository
{
private:
	std::unordered_map<Symbol, std::unique_ptr<const AtomData>> atomTable;
	std::unordered_map<Symbol, std::unique_ptr<const RadicalData>> radicalTable;

public:
	AtomRepository() = default;
	AtomRepository(const AtomRepository&) = delete;

	template <typename AtomT>
	bool add(DefinitionObject&& definition);

	bool contains(const Symbol& symbol) const;

	const AtomData& at(const Symbol& symbol) const;

	using AtomIterator = std::unordered_map<Symbol, std::unique_ptr<const AtomData>>::const_iterator;
	AtomIterator atomsBegin() const;
	AtomIterator atomsEnd() const;

	using RadicalIterator = std::unordered_map<Symbol, std::unique_ptr<const RadicalData>>::const_iterator;
	RadicalIterator radicalsBegin() const;
	RadicalIterator radicalsEnd() const;
	
	void clear();

	static constexpr size_t npos = static_cast<size_t>(-1);
};
