#pragma once

#include "Repository.hpp"
#include "AtomData.hpp"
#include "RadicalData.hpp"

class AtomRepository
{
private:
	MultiIndexMap<AtomId, Symbol, const AtomData*> table;

	bool add(const AtomData* data);

	void loadBuiltins();

public:
	AtomRepository() = default;
	AtomRepository(const AtomRepository&) = delete;
	~AtomRepository() noexcept;

	template <typename A, typename... Args, typename = std::enable_if_t<
		(std::is_base_of_v<AtomData, A> || std::is_same_v<AtomData, A>) &&
		(std::is_constructible_v<A, Args...> || std::is_constructible_v<A, Args..., const AtomRepository&>)>>
	bool add(Args&&... args);

	bool contains(const AtomId id) const;
	bool contains(const Symbol symbol) const;

	const AtomData& at(const AtomId id) const;
	const AtomData& at(const Symbol symbol) const;

	/// <summary>
	/// Returns the ids of the atoms with the given symbols.
	/// Missing symbols are skipped.
	/// </summary>
	std::unordered_set<AtomId> getIds(const std::unordered_set<Symbol>& symbols) const;

	bool loadFromFile(const std::string& path);

	static constexpr size_t npos = static_cast<size_t>(-1);
};

template <typename A, typename... Args, typename>
bool AtomRepository::add(Args&&... args)
{
	const AtomData* temp;
	if constexpr (std::is_constructible_v<A, Args...>)
		temp = new A(std::forward<Args>(args)...);
	else
		temp = new A(std::forward<Args>(args)..., *this);

	return add(temp);
}
