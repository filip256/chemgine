#pragma once

#include <memory>
#include <string>
#include <unordered_map>

namespace def
{

class Object;

}  // namespace def

class OutlineDefRepository
{
private:
    std::unordered_map<std::string, std::unique_ptr<const def::Object>> definitions;

public:
    OutlineDefRepository()                            = default;
    OutlineDefRepository(const OutlineDefRepository&) = delete;
    OutlineDefRepository(OutlineDefRepository&&)      = default;

    const def::Object* add(def::Object&& definition);

    const def::Object* getDefinition(const std::string& identifier) const;

    size_t totalDefinitionCount() const;

    void clear();
};
