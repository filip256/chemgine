#pragma once

#include "Parsers.hpp"

class DefinitionObject;

namespace Def
{
	template <typename T, typename... Args>
	static std::optional<T> parse(const DefinitionObject& definition, Args&&... args);
}

template <typename T, typename... Args>
inline std::optional<T> Def::parse(const DefinitionObject& definition, Args&&... args)
{
	return Def::Parser<T>::parse(definition, std::forward<Args>(args)...);
}
