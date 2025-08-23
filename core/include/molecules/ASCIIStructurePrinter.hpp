#pragma once

#include "atomics/BondedAtom.hpp"
#include "structs/Buffer2D.hpp"
#include "structs/FlagField.hpp"
#include "structs/HLine.hpp"
#include "structs/UndirectedEdge.hpp"

#include <ranges>
#include <source_location>

namespace ASCII
{

using Position     = Point<int16_t>;
using PositionLine = HLine<int16_t, Symbol::SizeT>;

//
// Node
//

class Cycle;

class Node
{
public:
    Position position = utils::npos<Position>;

private:
    const BondedAtomBase* atom;
    std::vector<Cycle*>   containingCycles;
    ColoredString         symbol;

public:
    Node(const BondedAtomBase& atom, ColoredString&& symbol) noexcept;
    Node(const Node&) = delete;
    Node(Node&&)      = default;

    const BondedAtomBase& getAtom() const;
    const ColoredString&  getSymbol() const;
    Symbol::SizeT         getSymbolSize() const;
    OS::BasicColor        getSymbolColor() const;
    PositionLine          getPosition() const;

    bool visited() const;
    void unvisit();

    using ConstCycleIterator = utils::DerefIterator<std::vector<Cycle*>::const_iterator>;
    std::ranges::subrange<ConstCycleIterator> getContainingCycles() const;
    using CycleIterator = utils::DerefIterator<std::vector<Cycle*>::iterator>;
    std::ranges::subrange<CycleIterator> getContainingCycles();

    Point<float> getPrintAwayPoint() const;

    bool isPartOfCycle(const Cycle& cycle) const;
    bool isPartOfSameCycle(const Node& other) const;

    void addContainingCycle(Cycle& cycle);
};

//
// Edge
//

class Edge
{
public:
    bool visited = false;

private:
    OS::BasicColor color;
    const Bond*    bond;

public:
    Edge(const Bond& bond, const OS::BasicColor color) noexcept;

    OS::BasicColor getColor() const;
    const Bond&    getBond() const;
};

//
// Cycle
//

class Cycle
{
public:
    Point<float> centroid = utils::npos<Point<float>>;

private:
    std::vector<const BondedAtomBase*> cycle;

public:
    Cycle(std::vector<const BondedAtomBase*>&& cycle) noexcept;
    Cycle(const Cycle&) = delete;
    Cycle(Cycle&&)      = default;

    const std::vector<const BondedAtomBase*>& getCycle() const;
    const BondedAtomBase&                     getAtom(const c_size idx) const;
    c_size                                    size() const;

    bool visited() const;
    void unvisit();

    std::optional<c_size> getMemberIdx(const BondedAtomBase& atom) const;
    c_size                countVisitedAtoms(const std::vector<Node>& nodes) const;
    void                  computeAndSetCentroid(const std::vector<Node>& nodes);
};

//
// PrintOptions
//

enum class PrintFlags : uint8_t
{
    DISABLE_ALL = 0,

    PRINT_IMPLIED_NON_CARBON_HYDROGENS = 1 << 0,
    PRINT_IMPLIED_CARBON_HYDROGENS     = 1 << 1,

    PRINT_ATOM_INDICES = 1 << 2,
    PRINT_CYCLE_ORDER  = 1 << 3,

    HIGHLIGHT_ATOM_ORIGIN = 1 << 4,

    ENABLE_ALL = static_cast<uint8_t>(-1),
};

class PrintOptions : public FlagField<PrintFlags>
{
public:
    using FlagField<PrintFlags>::FlagField;

    PrintOptions(const FlagField<PrintFlags> field) noexcept;

    static const PrintOptions Default;
};

//
// StructurePrinter
//

class StructurePrinter
{
private:
    c_size       expandedCycleCount = 0;
    c_size       errorCount         = 0;
    PrintOptions options;

    std::vector<Node>                                nodes;
    std::vector<Cycle>                               cycles;
    std::unordered_map<UndirectedEdge<c_size>, Edge> edges;

    ColoredTextBlock buffer;

    Edge& getEdge(const BondedAtomBase& from, const BondedAtomBase& to);

    ColoredString getNewClosureSymbol();

    static std::optional<std::pair<Position, Direction>>
    inferEdgePosition(const Position from, const Position to);
    static std::optional<std::pair<Position, Direction>>
    inferEdgePosition(const PositionLine from, const PositionLine to);

    bool isSymbol(const Point<int32_t> position) const;
    bool isAmbiguousBondPlacement(
        const Position position, const Direction direction, const BondType bondType) const;
    bool isAmbiguousBondPlacement(
        const Position                   position,
        const Direction                  direction,
        const BondType                   bondType,
        const std::vector<PositionLine>& lookaheadSymbols) const;
    bool isClutteredAtomPlacement(const PositionLine position) const;
    bool isAmbiguousAtomPlacement(const PositionLine position) const;

    std::tuple<Position, PositionLine, ASCII::Direction> getFreeCycleClosurePosition(
        const PositionLine              prevNodePos,
        const BondType                  bondType,
        const Symbol::SizeT             closureSymbolSize,
        const std::array<Direction, 8>& directions) const;
    std::tuple<Position, PositionLine, ASCII::Direction> getFreeCycleClosurePositionLookahead(
        const PositionLine              prevNodePos,
        const BondType                  bondType,
        const Symbol::SizeT             closureSymbolSize,
        const Position                  prevBondPos,
        const Direction                 prevBondDirection,
        const BondType                  prevBondType,
        const std::array<Direction, 8>& directions) const;

    std::vector<PositionLine> generateOptimalCycleLayout(
        const std::vector<const BondedAtomBase*>& cycle,
        const c_size                              firstCycleIdx,
        const c_size                              secondCycleIdx,
        const c_size                              lastCycleIdx,
        const bool                                noConstraints,
        const Direction                           enteringDirection) const;
    std::vector<PositionLine> generateOneShotCycleLayout(
        const std::vector<const BondedAtomBase*>& cycle,
        const c_size                              firstCycleIdx,
        const c_size                              secondCycleIdx,
        const Direction                           enteringDirection) const;
    void expandCycleLinearly(
        const Cycle&          cycle,
        const BondedAtomBase& firstAtom,
        const c_size          secondCycleIdx,
        const c_size          lastCycleIdx);

    void printError(
        const Position         point,
        const std::string&     message,
        std::source_location&& location = std::source_location::current());
    void printImpliedHydrogens(const Node& node);
    void printEdge(
        const Position        position,
        const Direction       direction,
        const BondedAtomBase& from,
        const BondedAtomBase& to);
    void printUnmaterializedEdges();
    void printNeighbors(const Node& node, const Direction prevDirection, const Cycle* cycle);
    void printCycle(const c_size startCycleIdx, Cycle& cycle, const Direction enteringDirection);

public:
    StructurePrinter(
        std::vector<Node>&&                                nodes,
        std::vector<Cycle>&&                               cycles,
        std::unordered_map<UndirectedEdge<c_size>, Edge>&& edges,
        const PrintOptions                                 options) noexcept;

    void print();
    void reset();

    ColoredTextBlock& getBlock();
    ColoredString     toString() const;

    static Position getNextEdgePosition(const PositionLine position, const Direction direction);
    static std::pair<Position, PositionLine> getNextPosition(
        const PositionLine position, const Direction direction, const Symbol::SizeT nextSymbolSize);

public:
    static std::vector<std::pair<ASCII::Direction, Position>>
    getPossibleNextDirections(const PositionLine origin);
};

}  // namespace ASCII
