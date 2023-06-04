#pragma once

#include "Component.hpp"
#include "GraphStructure.hpp"

class Composite : public Component
{
    GraphStructure structure;
};