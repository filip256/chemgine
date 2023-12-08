#pragma once

#include <cstdint>

enum class ComponentType : uint8_t
{
	NONE,
	ATOM,
	ATOM_ID,
	FUNCTIONAL,
	BACKBONE
};