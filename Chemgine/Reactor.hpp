#pragma once

#include <unordered_set>

#include "DataStoreAccessor.hpp"
#include "Molecule.hpp"
#include "LayerType.hpp"
#include "ConcreteReaction.hpp"

class Reactor
{
	class Reactant
	{
	public:
		const Molecule molecule;
		const LayerType layer;
		mutable double amount;
		mutable bool isNew;

		Reactant(
			const Molecule& molecule,
			const LayerType layer,
			const double amount
		) noexcept;

		Reactant(const Reactant&) = default;
		Reactant(Reactant&&) = default;

		bool operator==(const Reactant& other) const;
		bool operator!=(const Reactant& other) const;
	};

	class ReactantHash
	{
	public:
		size_t operator() (const Reactant& reactant) const;
	};

private:
	std::vector<ConcreteReaction> reactions;
	std::unordered_set<Reactant, ReactantHash> content;

	static DataStoreAccessor dataAccessor;

	void r() const;
	void generateMappingForReaction(const Reaction& reaction) const;

	void removeNegligibles();
	void checkReactions();

public:
	Reactor() noexcept;
	Reactor(const Reactor&) = delete;
	Reactor(Reactor&&) = default;

	void add(Reactor& other);
	void add(Reactor& other, const double ratio);

	void tick();

	static void setDataStore(const DataStore& dataStore);
};