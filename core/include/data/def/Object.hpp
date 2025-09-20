#pragma once

#include "data/def/Location.hpp"
#include "io/Log.hpp"
#include "structs/CountedRef.hpp"
#include "utils/STL.hpp"

#include <optional>
#include <unordered_map>
#include <unordered_set>

namespace def
{

enum class DefinitionType
{
    AUTO,
    DATA,
    ATOM,
    RADICAL,
    MOLECULE,
    REACTION,
    LABWARE,
};

class Object
{
private:
    const DefinitionType type;
    const std::string    identifier;
    const std::string    specifier;
    const def::Location  location;

    mutable std::unordered_set<std::string> accessedProperties;
    mutable std::unordered_set<std::string> accessedSubDefs;

    utils::StringMap<std::string>   properties;
    utils::StringMap<Object>        ilSubDefs;
    utils::StringMap<const Object*> outlineSubDefs;

public:
    Object(
        const DefinitionType              type,
        std::string&&                     identifier,
        std::string&&                     specifier,
        utils::StringMap<std::string>&&   properties,
        utils::StringMap<Object>&&        ilSubDefs,
        utils::StringMap<const Object*>&& outlineSubDefs,
        def::Location&&                   location) noexcept;

    Object(const Object&) = delete;
    Object(Object&&)      = default;

    DefinitionType     getType() const;
    const std::string& getIdentifier() const;
    const std::string& getSpecifier() const;

    const def::Location& getLocation() const;
    std::string          getLocationName() const;

    void logUnusedWarnings() const;

    /// <summary>
    /// Extracts and returns the property with the given key.
    /// If the property isn't found, an error message is logged.
    /// </summary>
    std::optional<std::string> getProperty(const std::string_view key) const;

    /// <summary>
    /// Extracts and returns the property with the given key (if found).
    /// </summary>
    std::optional<std::string> getOptionalProperty(const std::string_view key) const;

    /// <summary>
    /// Extracts and returns the property with the given key (if found).
    /// If the property isn't found the given default string is returned.
    /// </summary>
    std::string getDefaultProperty(const std::string_view key, std::string&& defaultValue) const;

    /// <summary>
    /// Extracts the property with the given key and returns its parsed value.
    /// If the property isn't found or parsing fails, an error message is logged.
    /// </summary>
    template <typename T, typename... Args>
    std::optional<T> getProperty(
        const std::string_view key,
        std::optional<T>       (*parser)(const std::string&, Args...),
        Args&&... parserArgs) const;

    /// <summary>
    /// Extracts the property with the given key (if found) and returns its parsed value.
    /// If the parsing fails, a warning message is logged.
    /// </summary>
    template <typename T, typename... Args>
    std::optional<T> getOptionalProperty(
        const std::string_view key,
        std::optional<T>       (*parser)(const std::string&, Args...),
        Args&&... parserArgs) const;

    /// <summary>
    /// Extracts the property with the given key (if found) and returns its parsed value.
    /// If the parsing fails, a warning message is logged.
    /// If the property isn't found or parsing fails, the given default value is returned.
    /// </summary>
    template <typename T, typename D, typename... Args>
    T getDefaultProperty(
        const std::string_view key,
        D&&                    defaultValue,
        std::optional<T>       (*parser)(const std::string&, Args...),
        Args&&... parserArgs) const;

    /// <summary>
    /// Extracts and returns the definition with the given key.
    /// If the definition isn't found, an error message is logged.
    /// </summary>
    const Object* getDefinition(const std::string_view key) const;

    /// <summary>
    /// Extracts and returns the definition with the given key (if found).
    /// </summary>
    const Object* getOptionalDefinition(const std::string_view key) const;

    /// <summary>
    /// Extracts the definition with the given key and returns its parsed value.
    /// If the definition isn't found or parsing fails, an error message is logged.
    /// </summary>
    template <typename T, typename... Args>
    std::optional<CountedRef<const T>> getDefinition(
        const std::string_view             key,
        std::optional<CountedRef<const T>> (*parser)(const Object&, Args...),
        Args&&... parserArgs) const;

    /// <summary>
    /// Extracts the definition with the given key (if found) and returns its parsed value.
    /// If the parsing fails, a warning message is logged.
    /// </summary>
    template <typename T, typename... Args>
    std::optional<CountedRef<const T>> getOptionalDefinition(
        const std::string_view             key,
        std::optional<CountedRef<const T>> (*parser)(const Object&, Args...),
        Args&&... parserArgs) const;

    /// <summary>
    /// Extracts the definition with the given key (if found) and returns its parsed value.
    /// If the parsing fails, a warning message is logged.
    /// If the property isn't found or parsing fails, the given default value is returned.
    /// </summary>
    template <typename T, typename D, typename... Args>
    CountedRef<const T> getDefaultDefinition(
        const std::string_view             key,
        D&&                                defaultValue,
        std::optional<CountedRef<const T>> (*parser)(const Object&, Args...),
        Args&&... parserArgs) const;
};

template <typename T, typename... Args>
std::optional<T> Object::getProperty(
    const std::string_view key, std::optional<T> (*parser)(const std::string&, Args...), Args&&... parserArgs) const
{
    const auto strProp = getProperty(key);
    if (not strProp)
        return std::nullopt;

    const auto parsed = parser(*strProp, std::forward<Args>(parserArgs)...);
    if (not parsed)
        Log(this).error("Failed to parse property: '{} : {}', at: {}.", key, *strProp, location.toString());

    return parsed;
}

template <typename T, typename... Args>
std::optional<T> Object::getOptionalProperty(
    const std::string_view key, std::optional<T> (*parser)(const std::string&, Args...), Args&&... parserArgs) const
{
    const auto strProp = getOptionalProperty(key);
    if (not strProp)
        return std::nullopt;

    const auto parsed = parser(*strProp, std::forward<Args>(parserArgs)...);
    if (not parsed)
        Log(this).warn("Failed to parse optional property: '{} : {}', at: {}.", key, *strProp, location.toString());

    return parsed;
}

template <typename T, typename D, typename... Args>
T Object::getDefaultProperty(
    const std::string_view key,
    D&&                    defaultValue,
    std::optional<T>       (*parser)(const std::string&, Args...),
    Args&&... parserArgs) const
{
    const auto prop = getOptionalProperty<T>(key, parser, std::forward<Args>(parserArgs)...);
    return prop ? *prop : std::move(defaultValue);
}

template <typename T, typename... Args>
std::optional<CountedRef<const T>> Object::getOptionalDefinition(
    const std::string_view             key,
    std::optional<CountedRef<const T>> (*parser)(const Object&, Args...),
    Args&&... parserArgs) const
{
    const auto* def = getOptionalDefinition(key);
    if (def == nullptr)
        return std::nullopt;

    auto parsed = parser(*def, std::forward<Args>(parserArgs)...);
    if (not parsed)
        Log(this).warn("Failed to parse optional sub-definition for: '{}', at: {}.", key, location.toString());

    return parsed;
}

template <typename T, typename... Args>
std::optional<CountedRef<const T>> Object::getDefinition(
    const std::string_view             key,
    std::optional<CountedRef<const T>> (*parser)(const Object&, Args...),
    Args&&... parserArgs) const
{
    const auto* def = getDefinition(key);
    if (def == nullptr)
        return std::nullopt;

    auto parsed = parser(*def, std::forward<Args>(parserArgs)...);
    if (not parsed)
        Log(this).error("Failed to parse sub-definition for: '{}', at: {}.", key, location.toString());

    return parsed;
}

template <typename T, typename D, typename... Args>
CountedRef<const T> Object::getDefaultDefinition(
    const std::string_view             key,
    D&&                                defaultValue,
    std::optional<CountedRef<const T>> (*parser)(const Object&, Args...),
    Args&&... parserArgs) const
{
    const auto def = getOptionalDefinition<T>(key, parser, std::forward<Args>(parserArgs)...);
    return def ? *def : std::move(defaultValue);
}

}  // namespace def
