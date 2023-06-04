#pragma once

#include "Component.hpp"
#include "GraphEdge.hpp"

#include <vector>

class GraphStructure
{
    std::vector<std::reference_wrapper<Component>> nodes;
    std::vector<std::reference_wrapper<GraphEdge>> edges;


};