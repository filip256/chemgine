#pragma once

#include "DataStoreAccessor.hpp"
#include "MolecularStructure.hpp"
#include "MoleculeId.hpp"

#include <optional>

class Reactable
{
private:
	static DataStoreAccessor dataAccessor;

	const bool isGeneric;
	const MoleculeId id;

	Reactable(const MoleculeId id, const bool isGeneric) noexcept;

	const DataStore& dataStore() const;

public:
	Reactable(const Reactable&) = default;

	const MoleculeId getId() const;
	const MolecularStructure& getStructure() const;

	std::unordered_map<c_size, c_size> matchWith(const MolecularStructure& structure) const;
	std::unordered_map<c_size, c_size> matchWith(const Reactable& other) const;
	bool matchesWith(const Reactable& other) const;

	bool operator==(const Reactable& other) const;
	bool operator!=(const Reactable& other) const;

	static std::optional<Reactable> get(MolecularStructure&& structure);
	static std::optional<Reactable> get(const std::string& smiles);

	static void setDataStore(const DataStore& dataStore);

	friend struct std::hash<Reactable>;
};


template<>
struct std::hash<Reactable>
{
	size_t operator() (const Reactable& reactable) const
	{
		return std::hash<MoleculeId>()(reactable.id);
	}
};

template<>
struct std::hash<std::pair<Reactable, uint8_t>>
{
	size_t operator() (const std::pair<Reactable, uint8_t>& reactable) const
	{
		return std::hash<Reactable>()(reactable.first);
	}
};
