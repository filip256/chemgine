#pragma once

#include "DataStoreAccessor.hpp"
#include "MolecularStructure.hpp"
#include "MoleculeIdType.hpp"

#include <optional>

class ReactableFactory;

class Reactable
{
private:
	static DataStoreAccessor dataAccessor;

	const bool isGeneric;
	const MoleculeIdType id;

	Reactable(const MoleculeIdType id, const bool isGeneric) noexcept;

	const DataStore& dataStore() const;

public:
	const MoleculeIdType getId() const;
	const MolecularStructure& getStructure() const;
	std::unordered_map<c_size, c_size> matchWith(const MolecularStructure& structure) const;

	static std::optional<Reactable> get(const std::string& smiles);

	static void setDataStore(const DataStore& dataStore);
};