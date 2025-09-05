#include "data/OutlineDefRepository.hpp"

#include "data/def/Object.hpp"

const def::Object* OutlineDefRepository::add(def::Object&& definition)
{
    const auto existing = definitions.find(definition.getIdentifier());
    if (existing != definitions.end()) {
        Log(this).error(
            "Clash between existing OOL identifier: '{0}' (defined at: {1}) and new definition at: "
            "{2}.",
            existing->first,
            existing->second->getLocationName(),
            definition.getLocationName());
        return nullptr;
    }

    auto       tempId = definition.getIdentifier();
    const auto it = definitions.emplace(std::move(tempId), std::make_unique<const def::Object>(std::move(definition)));

    return it.first->second.get();
}

const def::Object* OutlineDefRepository::getDefinition(const std::string& identifier) const
{
    const auto it = definitions.find(identifier);
    return it != definitions.end() ? it->second.get() : nullptr;
}

size_t OutlineDefRepository::totalDefinitionCount() const { return definitions.size(); }

void OutlineDefRepository::clear()
{
    for (const auto& [_, d] : definitions)
        d->logUnusedWarnings();
    definitions.clear();
}
