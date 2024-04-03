#pragma once

#include "Repository.hpp"
#include "AtomData.hpp"

class AtomRepository : public Repository<ComponentId, Symbol, AtomData>
{
private:
	void addPredefined();

public:
	AtomRepository() = default;
	AtomRepository(const AtomRepository&) = delete;

	bool loadFromFile(const std::string& path);
};
