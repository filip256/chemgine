#pragma once

#include "DataStoreAccessor.hpp"
#include "MolecularStructure.hpp"
#include "MoleculeId.hpp"

#include <optional>

class ReactableFactory;

class Reactable
{
private:
	static DataStoreAccessor dataAccessor;

	const bool isGeneric;
	const MoleculeId id;

	Reactable(const MoleculeId id, const bool isGeneric) noexcept;

	const DataStore& dataStore() const;

public:
	const MoleculeId getId() const;
	const MolecularStructure& getStructure() const;

	std::unordered_map<c_size, c_size> matchWith(const MolecularStructure& structure) const;
	std::unordered_map<c_size, c_size> matchWith(const Reactable& other) const;
	bool matchesWith(const Reactable& other) const;

	bool operator==(const Reactable& other) const;
	bool operator!=(const Reactable& other) const;

	static std::optional<Reactable> get(const std::string& smiles);

	static void setDataStore(const DataStore& dataStore);
};
