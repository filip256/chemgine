#include "mixtures/ContentInitializer.hpp"

ContentInitializer::ContentInitializer(std::initializer_list<std::pair<Molecule, Amount<Unit::MOLE>>> content) noexcept
{
    this->content.reserve(content.size());
    for (const auto& i : content)
        this->content.emplace(i);
}

ContentInitializer::ContentInitializer(const ReactantSet& content) noexcept
{
    this->content.reserve(content.size());
    for (const auto& [_, i] : content)
        this->content.emplace(i.molecule, i.amount);
}

size_t ContentInitializer::size() const { return content.size(); }

void ContentInitializer::add(const Molecule& molecule, Amount<Unit::MOLE> amount) { content.emplace(molecule, amount); }

ContentInitializer::const_iterator ContentInitializer::begin() const { return content.begin(); }

ContentInitializer::const_iterator ContentInitializer::end() const { return content.end(); }
