#pragma once

#include "AtomData.hpp"
#include "RadicalData.hpp"
#include "DefinitionObject.hpp"

class AtomRepository
{
private:
	// TODO: remove id, use only symbol hash
	std::unordered_map<Symbol, std::unique_ptr<const AtomData>> table;

public:
	AtomRepository() = default;
	AtomRepository(const AtomRepository&) = delete;

	template <typename A, typename... Args>
	bool add(const Symbol& symbol, Args&&... args);

	template <typename AtomT>
	bool add(DefinitionObject&& definition);

	bool contains(const Symbol symbol) const;

	const AtomData& at(const Symbol symbol) const;

	using Iterator = std::unordered_map<Symbol, std::unique_ptr<const AtomData>>::const_iterator;
	Iterator begin() const;
	Iterator end() const;
	
	void clear();

	static constexpr size_t npos = static_cast<size_t>(-1);
};

template <typename A, typename... Args>
bool AtomRepository::add(const Symbol& symbol, Args&&... args)
{
	static_assert(std::is_base_of_v<AtomData, A> || std::is_same_v<AtomData, A>,
		"AtomRepository: A must be an AtomData type or an AtomData derived type.");
	static_assert(std::is_constructible_v<A, Symbol, Args... >,
		"AtomRepository: Unable to construct A from the given Args.");

	auto temp = std::make_unique<const A>(symbol, std::forward<Args>(args)...);
	return table.emplace(symbol, std::move(temp)).second;
}
