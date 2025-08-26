#include "molecules/ASCIIStructurePrinter.hpp"

#include "global/Charset.hpp"
#include "io/Log.hpp"
#include "molecules/MolecularStructure.hpp"
#include "structs/AverageStream.hpp"

#include <queue>

namespace ASCII
{

//
// Node
//

Node::Node(const BondedAtomBase& atom, ColoredString&& symbol) noexcept :
    atom(&atom),
    symbol(std::move(symbol))
{}

const BondedAtomBase& Node::getAtom() const { return *atom; }

const ColoredString& Node::getSymbol() const { return symbol; }

Symbol::SizeT Node::getSymbolSize() const { return static_cast<Symbol::SizeT>(symbol.size()); }

OS::BasicColor Node::getSymbolColor() const
{
    // The whole symbol has a single color.
    return symbol.front().color;
}

PositionLine Node::getPosition() const { return HLine(position, getSymbolSize()); }

bool Node::visited() const { return not utils::isNPos(position); }

void Node::unvisit() { position = utils::npos<Position>; }

std::ranges::subrange<Node::ConstCycleIterator> Node::getContainingCycles() const
{
    return std::ranges::subrange<Node::ConstCycleIterator>(containingCycles.cbegin(), containingCycles.cend());
}

std::ranges::subrange<Node::CycleIterator> Node::getContainingCycles()
{
    return std::ranges::subrange<Node::CycleIterator>(containingCycles.begin(), containingCycles.end());
}

Point<float> Node::getPrintAwayPoint() const
{
    AverageStream<Point<float>> avgCentroid(Point(0.0f, 0.0f));
    for (const auto c : containingCycles) {
        if (not c->visited())
            continue;

        avgCentroid.add(c->centroid);
    }

    return avgCentroid.count() ? avgCentroid.avg() : static_cast<Point<float>>(position);
}

bool Node::isPartOfCycle(const Cycle& cycle) const
{
    return std::ranges::any_of(containingCycles, [&cycle](const Cycle* c) { return c == &cycle; });
}

bool Node::isPartOfSameCycle(const Node& other) const
{
    // O(n * m), pretty bad but in practice an atom is part of at most 4 cycles.
    return std::ranges::any_of(this->containingCycles, [&other](const Cycle* c) { return other.isPartOfCycle(*c); });
}

void Node::addContainingCycle(Cycle& cycle) { containingCycles.emplace_back(&cycle); }

//
// Edge
//

Edge::Edge(const Bond& bond, const OS::BasicColor color) noexcept :
    color(color),
    bond(&bond)
{}

OS::BasicColor Edge::getColor() const { return color; }

const Bond& Edge::getBond() const { return *bond; }

//
// Cycle
//

Cycle::Cycle(std::vector<const BondedAtomBase*>&& cycle) noexcept :
    cycle(std::move(cycle))
{}

const std::vector<const BondedAtomBase*>& Cycle::getCycle() const { return cycle; }

const BondedAtomBase& Cycle::getAtom(const c_size idx) const { return *cycle[idx]; }

c_size Cycle::size() const { return static_cast<c_size>(cycle.size()); }

bool Cycle::visited() const { return not utils::isNPos(centroid); }

void Cycle::unvisit() { centroid = utils::npos<Point<float>>; }

std::optional<c_size> Cycle::getMemberIdx(const BondedAtomBase& atom) const
{
    const auto it = std::ranges::find(cycle, &atom);
    return it != cycle.end() ? std::optional(static_cast<c_size>(std::distance(cycle.begin(), it))) : std::nullopt;
}

c_size Cycle::countVisitedAtoms(const std::vector<Node>& nodes) const
{
    return static_cast<c_size>(std::ranges::count_if(cycle, [&](const auto* atom) { return nodes[atom->index].visited(); }));
}

void Cycle::computeAndSetCentroid(const std::vector<Node>& nodes)
{
    // Compute the center of the cycle.
    centroid       = Point(0.0f, 0.0f);
    int32_t sdArea = 0;
    for (size_t i = 0; i < cycle.size() - 1; ++i) {
        const auto currentPos  = nodes[cycle[i]->index].position;
        const auto nextPos     = nodes[cycle[i + 1]->index].position;
        const auto cross       = currentPos.cross(nextPos);
        centroid              += (currentPos + nextPos) * cross;
        sdArea                += cross;
    }

    const auto currentPos  = nodes[cycle.back()->index].position;
    const auto nextPos     = nodes[cycle.front()->index].position;
    const auto cross       = currentPos.cross(nextPos);
    centroid              += (currentPos + nextPos) * cross;
    sdArea                += cross;

    centroid /= (3.0f * sdArea);
}

//
// PrintOptions
//

const PrintOptions PrintOptions::Default = PrintFlags::DISABLE_ALL;

PrintOptions::PrintOptions(const FlagField<PrintFlags> field) noexcept :
    FlagField<PrintFlags>(field)
{}

//
// Impl
//

namespace
{

bool isAngleAllowed(const size_t size, const Angle currentAngle, const Angle prevAngle)
{
    if (size < 4)
        return true;
    // Prune acute-acute and acute-perpendicular adjacent angles, starts losing cyclic layouts from
    // C8.
    if (prevAngle == Angle::ACUTE && (currentAngle == Angle::ACUTE || currentAngle == Angle::PERPENDICULAR))
        return false;

    if (size < 5)
        return true;
    // Prune perpendicular-acute adjacent angles, starts losing cyclic layouts from C6.
    if (prevAngle == Angle::PERPENDICULAR && currentAngle == Angle::ACUTE)
        return false;

    if (size < 7)
        return true;
    // Prune perpendicular-perpendicular adjacent angles.
    if (prevAngle == Angle::PERPENDICULAR && currentAngle == Angle::PERPENDICULAR)
        return false;

    if (size < 9)
        return true;
    if (currentAngle == Angle::ACUTE)
        return false;

    if (size < 11)
        return true;
    // Prune perpendicular-parallel and parallel-perpendicular adjacent angles.
    if ((prevAngle == Angle::PERPENDICULAR && currentAngle == Angle::PARALLEL) ||
        (prevAngle == Angle::PARALLEL && currentAngle == Angle::PERPENDICULAR))
        return false;

    if (size < 15)
        return true;
    if (currentAngle == Angle::PERPENDICULAR)
        return false;

    return true;
}

float combineScores(const float angleScore, const float bondScore) { return angleScore * 0.8f + bondScore * 0.2f; }

float getPrintAwayScalingFactor(const Point<float> printAwayPoint, const Position otherPosition)
{
    // Integral distance is used when printing away from a point. This gives nicer prints than float
    // distance, but, if the point (O) is too close to the current atom (C), the distance difference
    // between two directions may be lost during rounding. This effect is countered by applying a
    // scaling factor to the direction vector.
    //   P-P       .
    //  /   \      .
    // P  O~~C-N   .
    //  \   /      .
    //   P-P       .
    static constexpr utils::LinearQuantization quant(0.5f, 4.0f, 1.6f, 1.3f);

    const auto distance = printAwayPoint.squaredDistance(otherPosition);
    return quant[distance];
}

const std::array<Direction, 8>& getLinearPreferredDirections()
{
    // Directions, sorted by preference.
    static const std::array<Direction, 8> directions{
        Direction::Right,
        Direction::Left,
        Direction::Down,
        Direction::Up,
        Direction::DownRight,
        Direction::DownLeft,
        Direction::UpRight,
        Direction::UpLeft};

    return directions;
}

const std::vector<Direction>& getDirectionsByEnteringDirection(const Direction enteringDirection)
{
    static std::unordered_map<uint8_t, std::vector<Direction>> storage;
    storage.reserve(8);

    const auto it = storage.find(enteringDirection.getIdx());
    if (it != storage.end())
        return it->second;

    // Obtuse angles are preferred:
    //     C   .
    //    /    .
    // P-C     .
    //    \    .
    //     C   .
    auto directionsCopy = Direction::AllDirectionsVector;
    std::ranges::sort(directionsCopy, [enteringDirection](const auto lhs, const auto rhs) {
        const auto lhsScore = enteringDirection.getCyclicEnteringAngleScore(lhs);
        const auto rhsScore = enteringDirection.getCyclicEnteringAngleScore(rhs);
        return lhsScore > rhsScore;
    });

    return storage.emplace(enteringDirection.getIdx(), std::move(directionsCopy)).first->second;
}

class CyclePrintState
{
public:
    float                     scoreSum;
    std::vector<PositionLine> positions;

    CyclePrintState(float scoreSum, std::vector<PositionLine>&& positions) noexcept :
        scoreSum(scoreSum),
        positions(std::move(positions))
    {}

    CyclePrintState(const CyclePrintState&) = delete;
    CyclePrintState(CyclePrintState&&)      = default;

    CyclePrintState& operator=(const CyclePrintState&) = delete;
    CyclePrintState& operator=(CyclePrintState&&)      = default;

    float getScore() const { return scoreSum / positions.size(); }

    bool operator<(const CyclePrintState& other) const
    {
        const auto thisScore  = this->getScore();
        const auto otherScore = other.getScore();
        if (thisScore < otherScore)
            return true;
        if (thisScore > otherScore)
            return false;

        // States closer to the end have higher priority if the score is the same.
        return this->positions.size() < other.positions.size();
    }

private:
    static bool areOverlapping(const PositionLine& position, const std::vector<PositionLine>& newPositions)
    {
        return std::any_of(
            newPositions.begin(), newPositions.end(), [position](const auto pos) { return position.intersects(pos); });
    }

    static bool areOverlapping(
        const PositionLine& positionA, const PositionLine& positionB, const std::vector<PositionLine>& newPositions)
    {
        // Only positionB is checked since positionA was checked on the previous call (check uses).
        if (areOverlapping(positionB, newPositions))
            return true;

        // Edges aren't stored in the state so their position must be inferred from the node
        // positions.
        const auto edgePos =
            StructurePrinter::getNextEdgePosition(positionA, Direction(positionB.origin - positionA.origin));
        return areOverlapping(HLine(edgePos, 1), newPositions);
    }

public:
    // Checks if the new positions overlap with existing positions or the edges between them.
    bool arePositionsTaken(const PositionLine& firstPosition, const std::vector<PositionLine>& newPositions) const
    {
        if (positions.empty() || newPositions.empty())
            return false;

        // There is a generated edge between the first (fixed) position and second generated
        // position but the fixed position is not stored in the state to save memory.
        if (areOverlapping(firstPosition, positions.front(), newPositions))
            return true;

        for (size_t i = 1; i < positions.size(); ++i)
            if (areOverlapping(positions[i - 1], positions[i], newPositions))
                return true;

        return false;
    }
};

}  // namespace

//
// StructurePrinter
//

StructurePrinter::StructurePrinter(
    std::vector<Node>&&                                nodes,
    std::vector<Cycle>&&                               cycles,
    std::unordered_map<UndirectedEdge<c_size>, Edge>&& edges,
    const PrintOptions                                 options) noexcept :
    options(options),
    nodes(std::move(nodes)),
    cycles(std::move(cycles)),
    edges(std::move(edges))
{}

Position StructurePrinter::getNextEdgePosition(const PositionLine position, const Direction direction)
{
    // Computes the next edge position, taking into account the symbol length:
    // \|  /   .
    // -Uut-   .
    // /|  \   .
    const auto vector           = direction.get();
    auto       nextEdgePosition = static_cast<Position>(position.origin + vector);
    if (vector.x > 0)
        nextEdgePosition.x += position.length - 1;

    return nextEdgePosition;
}

std::pair<Position, PositionLine>
StructurePrinter::getNextPosition(const PositionLine position, const Direction direction, const Symbol::SizeT nextSymbolSize)
{
    // Computes the next edge and node position, taking into account the symbol lengths:
    // Si Si  Si   .
    //   \|  /     .
    // Si-Uut-Si   .
    //   /|  \     .
    // Si Si  Si   .
    const auto vector           = direction.get();
    auto       nextEdgePosition = static_cast<Position>(position.origin + vector);
    auto       nextNodePosition = static_cast<Position>(nextEdgePosition + vector);

    if (vector.x > 0) {
        nextEdgePosition.x += position.length - 1;
        nextNodePosition.x += position.length - 1;
    }
    if (vector.x < 0)
        nextNodePosition.x -= nextSymbolSize - 1;

    return std::make_pair(nextEdgePosition, HLine(nextNodePosition, nextSymbolSize));
};

std::vector<std::pair<ASCII::Direction, Position>> StructurePrinter::getPossibleNextDirections(const PositionLine origin)
{
    // Populate the next possible directions:
    // \|/|  ||/   .
    // -Xxx..xx-   .
    // /|\|  ||\   .
    std::vector<std::pair<ASCII::Direction, Position>> directions;
    directions.reserve(8 + 2 * (origin.length > 1 ? origin.length - 1 : 0));

    if (origin.length == 1) {
        for (const auto dir : ASCII::Direction::AllDirections) directions.emplace_back(dir, origin.origin);
    }
    else {
        for (auto dir = ASCII::Direction::DownRight; dir != ASCII::Direction::Right; ++dir)
            directions.emplace_back(dir, origin.origin);
        for (Symbol::SizeT i = 2; i < origin.length; ++i) {
            directions.emplace_back(ASCII::Direction::Up, origin.origin + Point<Symbol::SizeT>(i, 0));
            directions.emplace_back(ASCII::Direction::Down, origin.origin + Point<Symbol::SizeT>(i, 0));
        }
        for (auto dir = ASCII::Direction::UpRight; dir != ASCII::Direction::Down; ++dir)
            directions.emplace_back(dir, origin.origin + Point<Symbol::SizeT>(origin.length - 1, 0));
    }

    return directions;
}

void StructurePrinter::print()
{
    if (cycles.empty()) {
        // Non-cyclic:
        nodes.front().position = Position(0, 0);
        buffer.insert(Position(0, 0), nodes.front().getSymbol());

        printNeighbors(nodes.front(), utils::npos<Direction>, /*cycle=*/nullptr);
        return;
    }

    // Cyclic:
    for (auto& c : cycles)
        for (auto* a : c.getCycle()) nodes[a->index].addContainingCycle(c);

    const auto largestCycleIt =
        std::ranges::max_element(cycles, [](const auto& lhs, const auto& rhs) { return lhs.size() < rhs.size(); });

    nodes[largestCycleIt->getCycle().front()->index].position = Position(0, 0);

    printCycle(0, *largestCycleIt, utils::npos<Direction>);
    printUnmaterializedEdges();
}

void StructurePrinter::reset()
{
    buffer.clear();
    expandedCycleCount = 0;
    errorCount         = 0;

    for (auto& c : cycles) c.unvisit();
    for (auto& [_, e] : edges) e.visited = false;
    for (auto& n : nodes) n.unvisit();
}

ColoredTextBlock& StructurePrinter::getBlock() { return buffer; }

ColoredString StructurePrinter::toString() const { return buffer.toString(); }

Edge& StructurePrinter::getEdge(const BondedAtomBase& from, const BondedAtomBase& to)
{
    // TODO: Could replace getBondTo() uses with getEdge() to get O(1).
    const auto it = edges.find(UndirectedEdge(from.index, to.index));
    if (it == edges.end())
        Log(this).fatal("Unregistered bond between nodes {0} and {1}.", from.index, to.index);

    return it->second;
}

ColoredString StructurePrinter::getNewClosureSymbol()
{
    ++expandedCycleCount;
    return ColoredString('%' + std::to_string(expandedCycleCount), OS::BasicColor::DARK_YELLOW_BG);
}

std::optional<std::pair<Position, Direction>> StructurePrinter::inferEdgePosition(const Position from, const Position to)
{
    const auto diff               = to - from;
    const auto absDiff            = diff.abs();
    const auto [minDiff, maxDiff] = absDiff.x <= absDiff.y ? absDiff : absDiff.transpose();

    // Chebyshev distance must be 2, but also prevent against situations like:
    // C    .
    //  \   .
    //  C   .
    if (maxDiff != 2 || minDiff == 1)
        return std::nullopt;

    const auto dir     = Direction(diff);
    const auto edgePos = getNextEdgePosition(HLine(from, 1), dir);
    return std::make_pair(edgePos, dir);
}

std::optional<std::pair<Position, Direction>>
StructurePrinter::inferEdgePosition(const PositionLine from, const PositionLine to)
{
    const auto [fromPoint, toPoint] = from.closestTo(to);
    return inferEdgePosition(fromPoint, toPoint);
}

bool StructurePrinter::isSymbol(const Point<int32_t> position) const
{
    if (buffer.isWhiteSpace(position))
        return false;

    const auto chr = buffer[position].chr;
    return chr != 'H' && Bond::fromASCII(chr) == BondType::NONE;
}

bool
StructurePrinter::isAmbiguousBondPlacement(const Position position, const Direction direction, const BondType bondType) const
{
    // Bond types with incomplete 8-direction ASCII representation can produce ambiguity in some
    // cases: A B   .
    //  =    .
    // C D   .
    // If the bond goes from A to D, ambiguity occurs when both B and C are symbols.
    return not Bond::hasCompleteASCIIRepresentation(bondType) &&
           direction.isDiagonal() &&
           (isSymbol(position + direction.turn<Rotation::CLOCKWISE>(Angle::PERPENDICULAR).get()) &&
            isSymbol(position + direction.turn<Rotation::COUNTER_CLOCKWISE>(Angle::PERPENDICULAR).get()));
}

bool StructurePrinter::isAmbiguousBondPlacement(
    const Position                   position,
    const Direction                  direction,
    const BondType                   bondType,
    const std::vector<PositionLine>& lookaheadSymbols) const
{
    const auto isSymbolCheck = [&](const auto pos) {
        return isSymbol(pos) || std::ranges::any_of(lookaheadSymbols, [pos](const auto s) { return s.contains(pos); });
    };
    return not Bond::hasCompleteASCIIRepresentation(bondType) &&
           direction.isDiagonal() &&
           (isSymbolCheck(position + direction.turn<Rotation::CLOCKWISE>(Angle::PERPENDICULAR).get()) &&
            isSymbolCheck(position + direction.turn<Rotation::COUNTER_CLOCKWISE>(Angle::PERPENDICULAR).get()));
}

bool StructurePrinter::isClutteredAtomPlacement(const PositionLine position) const
{
    // Avoid over-cluttered atoms:
    //  **    .
    // *Si*   .
    //  **    .
    if (isSymbol(position.origin + Direction::Left.get()) || isSymbol(position.endPoint() + Direction::Right.get()))
        return true;

    for (Symbol::SizeT i = 0; i < position.length; ++i)
        if (isSymbol(position.origin + Direction::Up.get()) || isSymbol(position.endPoint() + Direction::Down.get()))
            return true;

    return false;
}

bool StructurePrinter::isAmbiguousAtomPlacement(const PositionLine position) const
{
    // Same as ambiguous bond placements, N cannot be placed in this configuration:
    // C C C   .
    //  = =    .
    // C N C   .
    //  = =    .
    // C C C   .
    static constexpr std::array<Direction, 4> diagonals{
        Direction::UpLeft, Direction::UpRight, Direction::DownRight, Direction::DownLeft};
    for (const auto dir : diagonals) {
        const auto edgePos = getNextEdgePosition(position, dir);
        if (buffer.isWhiteSpace(edgePos))
            continue;

        const auto bondType = Bond::fromASCII(buffer[edgePos].chr);
        if (bondType == BondType::NONE || Bond::hasCompleteASCIIRepresentation(bondType))
            continue;

        if (isSymbol(edgePos + dir.get()) &&
            isSymbol(edgePos + dir.turn<Rotation::CLOCKWISE>(Angle::PERPENDICULAR).get()) &&
            isSymbol(edgePos + dir.turn<Rotation::COUNTER_CLOCKWISE>(Angle::PERPENDICULAR).get()))
            return true;
    }

    return false;
}

std::tuple<Position, PositionLine, ASCII::Direction> StructurePrinter::getFreeCycleClosurePosition(
    const PositionLine              prevNodePos,
    const BondType                  bondType,
    const Symbol::SizeT             closureSymbolSize,
    const std::array<Direction, 8>& directions) const
{
    for (const auto dir : directions) {
        const auto [edgePos, closurePos] = getNextPosition(prevNodePos, dir, closureSymbolSize);
        if (buffer.isWhiteSpace(edgePos) &&
            buffer.isWhiteSpace(closurePos) &&
            not isAmbiguousBondPlacement(edgePos, dir, bondType) &&
            not isAmbiguousAtomPlacement(closurePos))
            return std::make_tuple(edgePos, closurePos, dir);
    }

    return utils::npos<std::tuple<Position, PositionLine, ASCII::Direction>>;
}

std::tuple<Position, PositionLine, ASCII::Direction> StructurePrinter::getFreeCycleClosurePositionLookahead(
    const PositionLine              prevNodePos,
    const BondType                  bondType,
    const Symbol::SizeT             closureSymbolSize,
    const Position                  prevBondPos,
    const Direction                 prevBondDirection,
    const BondType                  prevBondType,
    const std::array<Direction, 8>& directions) const
{
    for (const auto dir : directions) {
        const auto [edgePos, closurePos] = getNextPosition(prevNodePos, dir, closureSymbolSize);
        if (buffer.isWhiteSpace(edgePos) &&
            buffer.isWhiteSpace(closurePos) &&
            not isAmbiguousBondPlacement(edgePos, dir, bondType) &&
            not isAmbiguousAtomPlacement(closurePos) &&
            not isAmbiguousBondPlacement(prevBondPos, prevBondDirection, prevBondType, {prevNodePos, closurePos}))
            return std::make_tuple(edgePos, closurePos, dir);
    }

    return utils::npos<std::tuple<Position, PositionLine, ASCII::Direction>>;
}

std::vector<PositionLine> StructurePrinter::generateOptimalCycleLayout(
    const std::vector<const BondedAtomBase*>& cycle,
    const c_size                              firstCycleIdx,
    const c_size                              secondCycleIdx,
    const c_size                              lastCycleIdx,
    const bool                                noConstraints,
    const Direction                           enteringDirection) const
{
    const auto directions = utils::isNPos(enteringDirection) ? Direction::AllDirectionsVector
                                                             : getDirectionsByEnteringDirection(enteringDirection);

    const auto& firstAtom  = *cycle[firstCycleIdx];
    const auto& secondAtom = *cycle[secondCycleIdx];
    const auto& lastAtom   = *cycle[lastCycleIdx];

    const auto firstSymbolSize  = nodes[firstAtom.index].getSymbolSize();
    const auto secondSymbolSize = nodes[secondAtom.index].getSymbolSize();
    const auto lastSymbolSize   = nodes[lastAtom.index].getSymbolSize();

    const auto firstPosition = HLine(nodes[firstAtom.index].position, firstSymbolSize);
    const auto lastPosition  = HLine(nodes[lastAtom.index].position, lastSymbolSize);

    const auto [firstPrevDirection, lastDirection] =
        noConstraints
            ? utils::npos<std::pair<Direction, Direction>>
            : std::make_pair(
                  Direction(
                      firstPosition.origin -
                      nodes[cycle[(firstCycleIdx + cycle.size() - 1) % cycle.size()]->index].position),
                  Direction(nodes[cycle[(lastCycleIdx + 1) % cycle.size()]->index].position - lastPosition.origin));

    const auto penultimateCycleIdx = (lastCycleIdx + cycle.size() - 2) % cycle.size();
    const auto totalAtomsToPrint =
        secondCycleIdx < lastCycleIdx ? lastCycleIdx - secondCycleIdx + 1 :  // current->end
            cycle.size() - secondCycleIdx + lastCycleIdx + 1;                // current->last_in_cycle + first_in_cycle->end

    const auto& firstToSecondBond = *firstAtom.getBondTo(secondAtom);

    const utils::LinearQuantization dirOrderModifierQuant(uint8_t(0), static_cast<uint8_t>(directions.size()), 1.0f, 0.9f);

    std::priority_queue<CyclePrintState> stack;
    if (totalAtomsToPrint == 2 && not noConstraints) {
        // Treat the case when only one atom (N) is missing from the cycle layout. This can only
        // occur in polycycles:
        //   C-C     .
        //  /#/ \    .
        // C#C   N   .
        //  \#\ /    .
        //   C-C     .
        const auto& lastToFirstBond = *secondAtom.getBondTo(lastAtom);

        // There are at most 4 directions which can reach E from P through another atom:
        //   E     E     E-2   1-E-3   .
        //  /|\    |\    | |    X|X    .
        // 1 2 3   1 2   1-P   2-P-4   .
        //  \|/     \|                 .
        //   P       P                 .
        uint8_t foundDirs = 0;
        for (uint8_t dirIdx = 0; dirIdx < directions.size(); ++dirIdx) {
            const auto dir                      = directions[dirIdx];
            const auto [newEdgePos, newNodePos] = getNextPosition(firstPosition, dir.get(), secondSymbolSize);

            if (not buffer.isWhiteSpace(newEdgePos) ||
                not buffer.isWhiteSpace(newNodePos) ||
                isAmbiguousBondPlacement(newEdgePos, dir, firstToSecondBond.getType()) ||
                isAmbiguousAtomPlacement(newNodePos) ||
                isClutteredAtomPlacement(newNodePos))
                continue;  // Position already taken.

            // Even if a layout can't reach the last position, it might reach the end of the symbol:
            // C-Si     .
            // | | \    .
            // C-Si-N   .
            if (lastPosition.origin.chebyshevDistance(newNodePos.origin) > 2 &&
                (lastSymbolSize == 1 ||
                 newNodePos.origin.x <= lastPosition.origin.x ||
                 newNodePos.origin.x - lastPosition.origin.x - lastSymbolSize > 1))
                continue;  // Failed to close a cycle, skip.

            // There can be at most one direction which returns to the first node.
            const auto dirToFirst  = Direction(lastPosition.origin - newNodePos.origin);
            const auto lastEdgePos = static_cast<Position>(newNodePos.origin + dirToFirst.get());
            if (not buffer.isWhiteSpace(lastEdgePos) ||
                isAmbiguousBondPlacement(lastEdgePos, dirToFirst, lastToFirstBond.getType()))
                continue;  // Non-planar cycle, skip.

            // The first directions have slightly higher score.
            const auto directionOrderModifier = dirOrderModifierQuant(dirIdx);
            const auto score =
                (combineScores(firstPrevDirection.getCyclicAngleScore(dir), firstToSecondBond.getASCIIScore(dir)) *
                     directionOrderModifier +
                 combineScores(dir.getCyclicAngleScore(dirToFirst), lastToFirstBond.getASCIIScore(dirToFirst)) +
                 combineScores(dirToFirst.getCyclicAngleScore(lastDirection), 1.0f)) /
                3.0f;

            // Add the complete layout to the stack ensuring all results remain sorted by score.
            stack.emplace(score, std::vector<PositionLine>{newNodePos});

            if (foundDirs == 3)
                break;
            ++foundDirs;
        }
    }
    else {
        // Regular case where 2 or more nodes need to be printed.
        for (uint8_t dirIdx = 0; dirIdx < directions.size(); ++dirIdx) {
            const auto dir                      = directions[dirIdx];
            const auto [newEdgePos, newNodePos] = getNextPosition(firstPosition, dir.get(), secondSymbolSize);

            if (not buffer.isWhiteSpace(newEdgePos) ||
                not buffer.isWhiteSpace(newNodePos) ||
                isAmbiguousBondPlacement(newEdgePos, dir, firstToSecondBond.getType()) ||
                isAmbiguousAtomPlacement(newNodePos) ||
                isClutteredAtomPlacement(newNodePos))
                continue;  // Position already taken.

            // The first directions have slightly higher score.
            const auto directionOrderModifier = dirOrderModifierQuant(dirIdx);
            const auto initialScore           = combineScores(
                                          noConstraints ? 1.0f : firstPrevDirection.getCyclicAngleScore(dir),
                                          firstToSecondBond.getASCIIScore(dir)) *
                                      directionOrderModifier;

            stack.emplace(initialScore, std::vector<PositionLine>{newNodePos});
        }
    }

    while (stack.size()) {
        // Trick to move the top element, it will be popped right after so it's safe.
        auto current = std::move(const_cast<CyclePrintState&>(stack.top()));
        stack.pop();

        const auto positionCount = current.positions.size();

        if (positionCount == totalAtomsToPrint - 1)  // Complete layout.
        {
            Log(this).trace(
                "Generated optimal layout of size {0} (score: {1}, stack size: {2}).",
                totalAtomsToPrint - (not noConstraints),
                current.getScore(),
                stack.size() + 1);
            return std::move(current.positions);
        }

        const auto firstDirection = Direction(current.positions.front().origin - firstPosition.origin);
        const auto prevDirection =
            positionCount >= 2 ? Direction(current.positions.back().origin - current.positions[positionCount - 2].origin)
                               : firstDirection;
        const auto prevAngle =
            positionCount >= 3
                ? Direction(current.positions[positionCount - 2].origin - current.positions[positionCount - 3].origin)
                      .getAngle(prevDirection)
            : positionCount == 2 ? firstDirection.getAngle(prevDirection)
                                 : Angle::NONE;

        const auto  currentIdx     = (positionCount + secondCycleIdx - 1) % cycle.size();
        const auto  nextIdx        = (currentIdx + 1) % cycle.size();
        const auto& currentAtom    = *cycle[currentIdx];
        const auto& nextAtom       = *cycle[nextIdx];
        const auto  nextSymbolSize = nodes[nextAtom.index].getSymbolSize();

        if (currentIdx == penultimateCycleIdx)  // Penultimate node.
        {
            const auto& currentToLastBond = *currentAtom.getBondTo(nextAtom);
            const auto& lastToFirstBond   = *nextAtom.getBondTo(lastAtom);

            uint8_t foundDirs = 0;
            for (const auto& dirToLast : Direction::AllDirections) {
                const auto [newEdgePos, newNodePos] =
                    getNextPosition(current.positions.back(), dirToLast.get(), nextSymbolSize);

                if (not buffer.isWhiteSpace(newEdgePos) ||
                    not buffer.isWhiteSpace(newNodePos) ||
                    isAmbiguousBondPlacement(newEdgePos, dirToLast, currentToLastBond.getType()) ||
                    isAmbiguousAtomPlacement(newNodePos) ||
                    isClutteredAtomPlacement(newNodePos) ||
                    current.arePositionsTaken(firstPosition, {newNodePos, HLine(newEdgePos, 1)}))
                    continue;  // Position already taken.

                const auto maybeLastEdgePos = inferEdgePosition(newNodePos, lastPosition);
                if (not maybeLastEdgePos)
                    continue;  // Failed to close cycle.

                const auto [lastEdgePos, dirToFirst] = *maybeLastEdgePos;
                if (not buffer.isWhiteSpace(lastEdgePos) ||
                    isAmbiguousBondPlacement(lastEdgePos, dirToFirst, lastToFirstBond.getType()) ||
                    current.arePositionsTaken(firstPosition, {HLine(lastEdgePos, 1)}))
                    continue;  // Non-planar cycle, skip.

                auto newPositions = current.positions;
                newPositions.emplace_back(newNodePos);

                // Since we add the score of 2 new edges but only add 1 node, a scaling of N/(N+1)
                // is applied to obtain (sum*N/(N+1))/N = sum/(N+1) during averaging.
                auto newScoreSum =
                    noConstraints
                        ? (current.scoreSum -
                           combineScores(1.0f, firstToSecondBond.getASCIIScore(firstDirection)) +
                           combineScores(
                               prevDirection.getCyclicAngleScore(dirToLast), currentToLastBond.getASCIIScore(dirToLast)) +
                           combineScores(
                               dirToLast.getCyclicAngleScore(dirToFirst), lastToFirstBond.getASCIIScore(dirToFirst)) +
                           combineScores(
                               dirToFirst.getCyclicAngleScore(firstDirection),
                               firstToSecondBond.getASCIIScore(firstDirection))) *
                              newPositions.size() /
                              (newPositions.size() + 1)
                        : (current.scoreSum +
                           combineScores(
                               prevDirection.getCyclicAngleScore(dirToLast), currentToLastBond.getASCIIScore(dirToLast)) +
                           combineScores(
                               dirToLast.getCyclicAngleScore(dirToFirst), lastToFirstBond.getASCIIScore(dirToFirst)) +
                           combineScores(dirToFirst.getCyclicAngleScore(lastDirection), 1.0f)) *
                              newPositions.size() /
                              (newPositions.size() + 2);

                // Add the complete layout to the stack ensuring all results remain sorted by score.
                stack.emplace(newScoreSum, std::move(newPositions));

                if (foundDirs == 3)
                    break;
                ++foundDirs;
            }

            continue;
        }

        const auto& bondToNext = *currentAtom.getBondTo(nextAtom);

        for (const auto& dir : Direction::AllDirections) {
            const auto [newEdgePos, newNodePos] = getNextPosition(current.positions.back(), dir.get(), nextSymbolSize);

            if (not buffer.isWhiteSpace(newEdgePos) ||
                not buffer.isWhiteSpace(newNodePos) ||
                isAmbiguousBondPlacement(newEdgePos, dir, bondToNext.getType()) ||
                isAmbiguousAtomPlacement(newNodePos) ||
                isClutteredAtomPlacement(newNodePos) ||
                current.arePositionsTaken(firstPosition, {newNodePos, HLine(newEdgePos, 1)}))
                continue;  // Position already taken.

            // If there aren't enough nodes to get back to the first node, the branch can be pruned.
            // This vastly improves hit-rate on larger cycle sizes.
            const auto distanceToFirst = newNodePos.origin.chebyshevDistance(lastPosition.origin);
            const auto remainingAtoms =
                currentIdx < lastCycleIdx ? lastCycleIdx - currentIdx - 2 :  // current->end
                    cycle.size() - currentIdx + lastCycleIdx - 1;            // current->last_in_cycle + first_in_cycle->end
            // One bond from last to first (+1), then each atom comes with a bond (+nodes*2) and
            // we should reach an adjacent position to start, not start itself (+1).
            if (remainingAtoms * 2 + 2 < distanceToFirst)
                continue;

            const auto currentAngle = prevDirection.getAngle(dir);
            if (not isAngleAllowed(cycle.size(), currentAngle, prevAngle))
                continue;

            auto newPositions = current.positions;
            newPositions.emplace_back(newNodePos);

            const auto newEdgeScore = combineScores(prevDirection.getCyclicAngleScore(dir), bondToNext.getASCIIScore(dir));
            const auto newScoreSum  = current.scoreSum + newEdgeScore;

            stack.emplace(newScoreSum, std::move(newPositions));
        }
    }

    return {};
}

std::vector<PositionLine> StructurePrinter::generateOneShotCycleLayout(
    const std::vector<const BondedAtomBase*>& cycle,
    const c_size                              firstCycleIdx,
    const c_size                              secondCycleIdx,
    const Direction                           enteringDirection) const
{
    const auto& firstAtom = *cycle[firstCycleIdx];

    std::vector<std::pair<c_size, Angle>> commands;
    commands.reserve(8);
    const auto sideLength = static_cast<c_size>(cycle.size() / 4);
    switch (cycle.size() % 4) {
    case 0:
        // Circle:
        //   C<S     .
        //  /   \    .
        // C     C   .
        // |     |   .
        // C     C   .
        //  \   /    .
        //   C-C     .
        commands.emplace_back(sideLength - 1, Angle::OBTUSE);
        commands.emplace_back(1, Angle::OBTUSE);
        commands.emplace_back(sideLength - 1, Angle::OBTUSE);
        commands.emplace_back(1, Angle::OBTUSE);
        commands.emplace_back(sideLength - 1, Angle::OBTUSE);
        commands.emplace_back(1, Angle::OBTUSE);
        commands.emplace_back(sideLength - 1, Angle::NONE);
        break;
    case 1:
        // Oval with cut corner:
        //   C<S       .
        //  /   \      .
        // C     C     .
        // |      \    .
        // C       C   .
        //  \     /    .
        //   C-C-C     .
        commands.emplace_back(sideLength - 1, Angle::OBTUSE);
        commands.emplace_back(1, Angle::OBTUSE);
        commands.emplace_back(sideLength - 1, Angle::OBTUSE);
        commands.emplace_back(1, Angle::OBTUSE);
        commands.emplace_back(sideLength, Angle::OBTUSE);
        commands.emplace_back(1, Angle::OBTUSE);
        commands.emplace_back(sideLength - 2, Angle::OBTUSE);
        commands.emplace_back(1, Angle::NONE);
        break;
    case 2:
        // Oval:
        //   C-C<S     .
        //  /     \    .
        // C       C   .
        // |       |   .
        // C       C   .
        //  \     /    .
        //   C-C-C     .
        commands.emplace_back(sideLength, Angle::OBTUSE);
        commands.emplace_back(1, Angle::OBTUSE);
        commands.emplace_back(sideLength - 1, Angle::OBTUSE);
        commands.emplace_back(1, Angle::OBTUSE);
        commands.emplace_back(sideLength, Angle::OBTUSE);
        commands.emplace_back(1, Angle::OBTUSE);
        commands.emplace_back(sideLength - 1, Angle::NONE);
        break;
    case 3:
    default:
        // Circle with cut corner:
        //   C<S       .
        //  /   \      .
        // C     C     .
        // |      \    .
        // C       C   .
        // |       |   .
        // C       C   .
        //  \     /    .
        //   C-C-C     .
        commands.emplace_back(sideLength - 1, Angle::OBTUSE);
        commands.emplace_back(1, Angle::OBTUSE);
        commands.emplace_back(sideLength, Angle::OBTUSE);
        commands.emplace_back(1, Angle::OBTUSE);
        commands.emplace_back(sideLength, Angle::OBTUSE);
        commands.emplace_back(1, Angle::OBTUSE);
        commands.emplace_back(sideLength - 1, Angle::OBTUSE);
        commands.emplace_back(1, Angle::NONE);
    }

    // If this is the first cycle to be printed (no entering direction), the first direction is Left
    // from the starting atom (S).
    //       P   .
    //      /    .
    //   C<S     .
    //  /   \    .
    // C     C   .
    // |     |   .
    // C     C   .
    //  \   /    .
    //   C-S     .
    const auto directions = utils::isNPos(enteringDirection) ? std::vector<Direction>{Direction::Left}
                                                             : getDirectionsByEnteringDirection(enteringDirection);

    for (const auto dir : directions) {
        auto prevIdx       = firstCycleIdx;
        auto currentIdx    = secondCycleIdx;
        auto prevPosition  = nodes[firstAtom.index].getPosition();
        auto prevDirection = dir;

        bool                      failed = false;
        std::vector<PositionLine> positions;
        positions.reserve(cycle.size() - 1);

        // Apply commands.
        for (const auto& command : commands) {
            for (c_size i = 0; i < command.first; ++i) {
                const auto& prevAtom          = *cycle[prevIdx];
                const auto& currentAtom       = *cycle[currentIdx];
                const auto  currentSymbolSize = nodes[currentAtom.index].getSymbolSize();

                const auto [edgePos, nodePos] = getNextPosition(prevPosition, prevDirection, currentSymbolSize);
                if (not buffer.isWhiteSpace(edgePos) ||
                    not buffer.isWhiteSpace(nodePos) ||
                    isAmbiguousBondPlacement(edgePos, prevDirection, prevAtom.getBondTo(currentAtom)->getType()) ||
                    isAmbiguousAtomPlacement(nodePos) ||
                    isClutteredAtomPlacement(nodePos)) {
                    failed = true;
                    break;
                }

                prevPosition = nodePos;
                positions.emplace_back(prevPosition);
                prevIdx    = currentIdx;
                currentIdx = (currentIdx + 1) % cycle.size();
            }

            if (command.second != Angle::NONE)
                prevDirection = prevDirection.turn<Rotation::COUNTER_CLOCKWISE>(command.second);
        }

        if (failed)
            continue;
        const auto firstPosition = nodes[firstAtom.index].position;
        if (firstPosition.chebyshevDistance(prevPosition.origin) > 2)
            continue;  // Failed to close a cycle.

        // There can be at most one direction which returns to the first node.
        const auto dirToFirst = Direction(firstPosition - prevPosition.origin);

        // Check last edge.
        const auto edgePos = getNextEdgePosition(prevPosition, prevDirection);
        if (not buffer.isWhiteSpace(edgePos) ||
            isAmbiguousBondPlacement(edgePos, dirToFirst, cycle[prevIdx]->getBondTo(firstAtom)->getType()))
            return {};

        Log(this).trace("Generated one-shot layout of size {0}.", cycle.size());
        return positions;
    }

    return {};
}

void StructurePrinter::expandCycleLinearly(
    const Cycle& cycle, const BondedAtomBase& firstAtom, const c_size secondCycleIdx, const c_size lastCycleIdx)
{
    const auto closureSymbol = getNewClosureSymbol();

    // The cycle can be linearly expanded in 2 directions starting from both the first and last atom
    // in the constraint sequence (C). Ideally we would try to print the whole cycle on one side (A)
    // checking that there is always enough space to also print the cycle head (%x). If any nodes
    // remain they can be printed starting from the other side (B).
    //  B-%1           .
    //  |              .
    //  C-C-C-A-A-%1   .
    // /     \         .

    auto totalAtomsToPrint =
        secondCycleIdx < lastCycleIdx ? lastCycleIdx - secondCycleIdx + 1 :  // current->end
            cycle.size() - secondCycleIdx + lastCycleIdx + 1;                // current->last_in_cycle + first_in_cycle->end
    std::vector<std::pair<const Node*, ASCII::Direction>> printedAtoms;
    printedAtoms.reserve(totalAtomsToPrint);

    // 1. Forward printing:
    auto        idx           = secondCycleIdx;
    auto        prevPosition  = nodes[firstAtom.index].getPosition();
    const auto* prevAtom      = &firstAtom;
    const auto* currentAtom   = &cycle.getAtom(secondCycleIdx);
    const auto* bondToNext    = prevAtom->getBondTo(*currentAtom);
    const auto* currentSymbol = &nodes[currentAtom->index].getSymbol();

    auto directionsCopy = getLinearPreferredDirections();

    const auto linearExpand = [&](const c_size endIdx, const bool backward) {
        const auto printAwayPoint = nodes[prevAtom->index].getPrintAwayPoint();
        const auto scalingFactor  = getPrintAwayScalingFactor(printAwayPoint, prevPosition.origin);
        std::ranges::sort(directionsCopy, [&](const auto lhs, const auto rhs) {
            const auto lhsScore = utils::isqrt(
                round_cast<uint16_t>(printAwayPoint.squaredDistance(prevPosition.origin + lhs.get() * scalingFactor)));
            const auto rhsScore = utils::isqrt(
                round_cast<uint16_t>(printAwayPoint.squaredDistance(prevPosition.origin + rhs.get() * scalingFactor)));
            return lhsScore > rhsScore;
        });

        auto tempClosurePos = getFreeCycleClosurePosition(
            prevPosition, bondToNext->getType(), static_cast<Symbol::SizeT>(closureSymbol.size()), directionsCopy);
        if (utils::isNPos(tempClosurePos)) {
            printError(prevPosition.origin, "Insufficient space for cycle closure: '" + closureSymbol.toString() + "'.");
            return;
        }

        auto [closureEdgePos, closureNodePos, closureDir] = tempClosurePos;

        while (idx != endIdx) {
            const auto nextIdx = backward ? (idx + cycle.size() - 1) % cycle.size() : (idx + 1) % cycle.size();
            bondToNext         = prevAtom->getBondTo(*currentAtom);

            for (const auto dir : directionsCopy) {
                // Check that after inserting the current node (C) enough space remains for the
                // closure symbol:
                //   p     .
                //  / \    .
                // P-C P   .
                // | | |   .
                // P %xP   .
                //  \ /    .
                //   P     .
                const auto [edgePos, nodePos] =
                    getNextPosition(prevPosition, dir, static_cast<Symbol::SizeT>(currentSymbol->size()));
                if (not buffer.isWhiteSpace(edgePos) ||
                    not buffer.isWhiteSpace(nodePos) ||
                    isAmbiguousAtomPlacement(nodePos))
                    continue;

                // The lookahead also checks that the bond to current isn't ambiguous, so no need to
                // check above.
                const auto bondToClosure = currentAtom->getBondTo(cycle.getAtom(nextIdx));
                tempClosurePos           = getFreeCycleClosurePositionLookahead(
                    nodePos,
                    bondToClosure->getType(),
                    static_cast<Symbol::SizeT>(closureSymbol.size()),
                    edgePos,
                    dir,
                    bondToNext->getType(),
                    directionsCopy);
                if (utils::isNPos(tempClosurePos))
                    continue;

                std::tie(closureEdgePos, closureNodePos, closureDir) = tempClosurePos;

                printEdge(edgePos, dir, *prevAtom, *currentAtom);
                buffer.insert(nodePos.origin, *currentSymbol);

                prevPosition         = nodePos;
                auto& currentNode    = nodes[currentAtom->index];
                currentNode.position = nodePos.origin;
                printedAtoms.emplace_back(&currentNode, dir);
                break;
            }

            if (utils::isNPos(nodes[currentAtom->index].position))
                break;  // No space left to print.

            prevAtom      = &cycle.getAtom(idx);
            currentAtom   = &cycle.getAtom(nextIdx);
            currentSymbol = &nodes[currentAtom->index].getSymbol();
            idx           = nextIdx;
        }

        // Print closure.
        printEdge(closureEdgePos, closureDir, *prevAtom, *currentAtom);
        buffer.insert(closureNodePos.origin, closureSymbol);
    };

    linearExpand(lastCycleIdx, false);

    // 2. Backward printing:
    const auto lastPrintedIdx = idx;

    idx           = (lastCycleIdx + cycle.size() - 1) % cycle.size();
    prevAtom      = &cycle.getAtom(lastCycleIdx);
    prevPosition  = nodes[prevAtom->index].getPosition();
    currentAtom   = &cycle.getAtom(idx);
    currentSymbol = &nodes[currentAtom->index].getSymbol();

    linearExpand((lastPrintedIdx + cycle.size() - 1) % cycle.size(), true);

    // The neighbors are printed after all cyclic atoms are printed.
    for (const auto& [node, prevDir] : printedAtoms) printNeighbors(*node, prevDir, &cycle);
}

void StructurePrinter::printError(const Position point, const std::string& message, std::source_location&& location)
{
    ++errorCount;
    const auto errorSymbolStr = ASCII::AshUppercase + std::to_string(errorCount);
    const auto errorSymbol    = ColoredString(errorSymbolStr, OS::BasicColor::RED_BG);

    Log(this).error(LogFormat("[{0}] {1}", std::move(location)), errorSymbolStr, message);

    if (buffer.isWhiteSpace(point, errorSymbol.size())) {
        buffer.insert(point, errorSymbol);
        return;
    }

    for (const auto d : getLinearPreferredDirections()) {
        const auto pos = point + d.get();
        if (buffer.isWhiteSpace(pos, errorSymbol.size())) {
            buffer.insert(pos, errorSymbol);
            return;
        }
    }

    // The error is always printed, even if it overwrites something.
    buffer.insert(point, errorSymbol);
}

void StructurePrinter::printImpliedHydrogens(const Node& node)
{
    static constexpr std::array<Direction, 4> directions{Direction::Right, Direction::Left, Direction::Down, Direction::Up};

    const auto hCount = MolecularStructure::getImpliedHydrogenCount(node.getAtom());
    if (hCount <= 0)
        return;

    uint8_t directionIdx = 0;
    while (directionIdx < directions.size()) {
        const auto dir = directions[directionIdx];
        ++directionIdx;

        const auto [hPos, countPos] = getNextPosition(node.getPosition(), dir, 1);
        if (not buffer.isWhiteSpace(hPos) || (hCount > 1 && not buffer.isWhiteSpace(countPos)))
            continue;

        const auto color = OS::darken(node.getSymbolColor());
        buffer[hPos]     = ColoredChar('H', color);
        if (hCount > 1)
            buffer[countPos.origin] = ColoredChar('0' + hCount, color);

        return;
    }

    printError(node.position, "Insufficient space for implied hydrogen atom.");
}

void StructurePrinter::printEdge(
    const Position position, const Direction direction, const BondedAtomBase& from, const BondedAtomBase& to)
{
    auto& edge       = getEdge(from, to);
    edge.visited     = true;
    buffer[position] = ColoredChar(edge.getBond().getASCII(direction), edge.getColor());
}

void StructurePrinter::printUnmaterializedEdges()
{
    // There are cases where all the atoms of one cycle are already printed by adjacent cycles,
    // but the cycle-enclosing bond (N-C) remains unprinted:
    //   P   P     .
    //  / \ / \    .
    // P 1 P 2 P   .
    //  \ /3\ /    .
    //   N~~~C     .
    for (const auto& [indexes, edge] : edges) {
        if (edge.visited)
            continue;

        const auto& nodeA = nodes[indexes.getIdxA()];
        const auto& nodeB = nodes[indexes.getIdxB()];

        // Adjacent positions: bond can be drawn.
        if (const auto edgePos = inferEdgePosition(nodeA.getPosition(), nodeB.getPosition())) {
            printEdge(edgePos->first, edgePos->second, nodeA.getAtom(), nodeB.getAtom());
            continue;
        }

        // Further away: linear expansion is needed.
        const auto closureSymbol = getNewClosureSymbol();

        const auto printClosure = [&](const Node& nodeA, const Node& nodeB) {
            const auto [edgePos, closurePos, dir] = getFreeCycleClosurePosition(
                nodeA.getPosition(),
                edge.getBond().getType(),
                static_cast<Symbol::SizeT>(closureSymbol.size()),
                getLinearPreferredDirections());
            if (utils::isNPos(edgePos)) {
                printError(nodeA.position, "Insufficient space for cycle closure: '" + closureSymbol.toString() + "'.");
                return;
            }

            printEdge(edgePos, dir, nodeA.getAtom(), nodeB.getAtom());
            buffer.insert(closurePos.origin, closureSymbol);
        };

        // Only add closures to nodes that were printed (some may not be printed due to insufficient
        // space).
        if (nodeA.visited())
            printClosure(nodeA, nodeB);
        if (nodeB.visited())
            printClosure(nodeB, nodeA);
    }
}

void StructurePrinter::printNeighbors(const Node& node, const Direction prevDirection, const Cycle* cycle)
{
    class Neighbor
    {
    public:
        c_size constraintCount;
        Cycle* cycle;

    private:
        const Bond* bond;

        Neighbor(const Bond& bond, Cycle* cycle, const c_size constraintCount) noexcept :
            constraintCount(constraintCount),
            cycle(cycle),
            bond(&bond)
        {}

    public:
        const Bond& getBond() const { return *bond; }

        const BondedAtomBase& getAtom() const { return bond->getOther(); }

        static Neighbor create(const Bond& bond, std::vector<Node>& nodes)
        {
            const auto& atom = bond.getOther();

            Cycle* largestCycle = nullptr;
            for (auto& cycle : nodes[atom.index].getContainingCycles()) {
                if (cycle.visited())
                    continue;

                if (largestCycle == nullptr || cycle.size() > largestCycle->size())
                    largestCycle = &cycle;
            }

            // Find the constraint count of the largest cycle.
            const auto constraintCount = largestCycle ? largestCycle->countVisitedAtoms(nodes) : c_size(0);

            return Neighbor(bond, largestCycle, constraintCount);
        }
    };

    const auto& atom = node.getAtom();

    std::vector<Neighbor> neighbors;
    neighbors.reserve(atom.bonds.size());
    for (const auto& b : atom.bonds) {
        const auto& other = b.getOther();
        if (nodes[other.index].visited())
            continue;

        neighbors.emplace_back(Neighbor::create(b, nodes));
    }

    // Neighbors with multiple constraints and larger cycles should be printed first.
    std::ranges::sort(neighbors, [](const auto& lhs, const auto& rhs) {
        if (lhs.constraintCount != rhs.constraintCount)
            return lhs.constraintCount > rhs.constraintCount;

        const auto lhsSize = lhs.cycle ? lhs.cycle->size() : 0;
        const auto rhsSize = rhs.cycle ? rhs.cycle->size() : 0;
        return lhsSize > rhsSize;
    });

    for (const auto& n : neighbors) {
        const auto& bond      = n.getBond();
        const auto& other     = bond.getOther();
        auto&       otherNode = nodes[other.index];
        if (otherNode.visited())
            continue;

        // We might not maintain the previous direction due to an unprintable bond. In this case the
        // previous direction is propagated instead of the current direction, non-parallel
        // direction.
        //   \         .
        //    P=C      .
        //       \     .
        //        N    .
        //         \   .
        // The same can occur in the cyclic case:
        //          /   .
        //         N    .
        //        /     .
        //   P-P=C      .
        //  /  |        .
        // P O P        .
        // |  /         .
        // P-P          .
        ASCII::Direction propagatedDir = utils::npos<ASCII::Direction>;

        auto directionsCopy = getLinearPreferredDirections();
        if (not utils::isNPos(prevDirection)) {
            // Non-cyclic prev atom:
            // Maintaining the same direction as the previous one increases the spread of the nodes
            // which leads to less space conflicts.
            //    4 3 2   .
            //     \|/    .
            // ..-P-C-1   .
            //     /|\    .
            //    4 3 2   .
            // (1-best, 4-worst)
            std::ranges::sort(directionsCopy, [prevDirection, &bond](const auto lhs, const auto rhs) {
                const auto lhsScore = underlying_cast(prevDirection.getAngle(lhs)) * bond.getASCIIScore(lhs);
                const auto rhsScore = underlying_cast(prevDirection.getAngle(rhs)) * bond.getASCIIScore(rhs);
                return lhsScore > rhsScore;
            });
            propagatedDir = prevDirection;
        }
        else if (cycle != nullptr) {
            // Cyclic prev atom:
            // Reverse-sorting directions by Euclidian distance to cycle center (O) ensures inner
            // pointing branching occurs only as a last resort. Using the integral distance ensures
            // straight edges (1) are preferred over diagonals (2), while also maintaining "good"
            // diagonals (3). 3   1 2     .
            //  \  |/      .
            //   C-C-C     .
            //  /     \    .
            // C   O   C   .
            const auto printAwayPoint = node.getPrintAwayPoint();
            const auto scalingFactor  = getPrintAwayScalingFactor(printAwayPoint, node.position);
            std::ranges::sort(directionsCopy, [&](const auto lhs, const auto rhs) {
                const auto lhsScore =
                    utils::isqrt(
                        round_cast<uint16_t>(printAwayPoint.squaredDistance(node.position + lhs.get() * scalingFactor))) *
                    bond.getASCIIScore(lhs);
                const auto rhsScore =
                    utils::isqrt(
                        round_cast<uint16_t>(printAwayPoint.squaredDistance(node.position + rhs.get() * scalingFactor))) *
                    bond.getASCIIScore(rhs);
                return lhsScore > rhsScore;
            });

            propagatedDir = *std::ranges::max_element(getLinearPreferredDirections(), [&](const auto lhs, const auto rhs) {
                const auto lhsScore = utils::isqrt(
                    round_cast<uint16_t>(printAwayPoint.squaredDistance(node.position + lhs.get() * scalingFactor)));
                const auto rhsScore = utils::isqrt(
                    round_cast<uint16_t>(printAwayPoint.squaredDistance(node.position + rhs.get() * scalingFactor)));
                return lhsScore < rhsScore;
            });
        }

        auto dirFound = false;
        for (uint8_t dirIdx = 0; dirIdx < directionsCopy.size(); ++dirIdx) {
            const auto dir = directionsCopy[dirIdx];

            const auto [edgePos, nodePos] = getNextPosition(node.getPosition(), dir, otherNode.getSymbolSize());
            if (not buffer.isWhiteSpace(edgePos) ||
                not buffer.isWhiteSpace(nodePos) ||
                isAmbiguousBondPlacement(edgePos, dir, n.getBond().getType()) ||
                isAmbiguousAtomPlacement(nodePos) ||
                isClutteredAtomPlacement(nodePos))
                continue;

            if (n.cycle != nullptr) {
                const auto cycleStartIdx = n.cycle->getMemberIdx(n.getBond().getOther());

                // Cyclic atom.
                if (n.constraintCount == 0) {
                    // If there are no existing constraints, the first atom is placed as if it were
                    // non-cyclic, forming an initial constraint for the layout generator.
                    printEdge(edgePos, dir, atom, n.getAtom());
                    otherNode.position = nodePos.origin;

                    printCycle(*cycleStartIdx, *n.cycle, dir);
                }
                else
                    printCycle(*cycleStartIdx, *n.cycle, utils::npos<Direction>);
            }
            else {
                // Non-cyclic atom.
                otherNode.position = nodePos.origin;

                printEdge(edgePos, dir, atom, n.getAtom());
                buffer.insert(nodePos.origin, otherNode.getSymbol());

                // The propagation change is only applied if the best-score direction was used for
                // printing.
                const auto pDir = dirIdx != 0 || utils::isNPos(propagatedDir) ? dir : propagatedDir;
                printNeighbors(otherNode, pDir, /*cycle=*/nullptr);
            }

            dirFound = true;
            break;
        }

        if (not dirFound)
            printError(node.position, "Insufficient space for atom: '" + other.getAtom().getSymbol().str() + "'.");
    }

    static const auto carbon = Atom("C");
    if (not options.has(PrintFlags::PRINT_ATOM_INDICES) &&
        ((options.has(PrintFlags::PRINT_IMPLIED_CARBON_HYDROGENS) && atom.getAtom() == carbon) ||
         (options.has(PrintFlags::PRINT_IMPLIED_NON_CARBON_HYDROGENS) && atom.getAtom() != carbon)))
        printImpliedHydrogens(node);
}

void StructurePrinter::printCycle(const c_size startCycleIdx, Cycle& cycle, const Direction enteringDirection)
{
    // If another atom of the cycle has already been printed, it's position is considered a
    // constraint. This should only occur if this cycle is part of a poly-cycle and at least one of
    // its adjacent cycles has already been printed. Because we use the minimal cycle basis and we
    // print cycles one-by-one the constraints should always be a single contiguous sequence nodes
    // containing the starting atom.

    // If the states of the stack are ordered by a score (priority_queue) then the first layout
    // which matches all requirements should also be the one with the highest possible score. When
    // such a layout is found it is copied to the original buffer and the printing recursion
    // continues for all non-visited members of the cycle. If no suitable layout is found the cycle
    // is printed linearly starting from the first atom and cycle closures are added at both ends of
    // the cycle.

    const auto& startAtom = cycle.getAtom(startCycleIdx);

    // C - start/current atom
    auto firstCycleIdx  = startCycleIdx;  // F
    auto secondCycleIdx = startCycleIdx;  // S
    auto lastCycleIdx   = startCycleIdx;  // L
    auto noConstraints  = true;
    if (nodes[cycle.getAtom((startCycleIdx + cycle.size() - 1) % cycle.size()).index].visited()) {
        // Case 1: Constraint sequence ends in C. L iterates backwards from C until the first
        // unvisited atom, S=C, F=S-1.
        // ..-L-C..C-F-S-..
        do {
            lastCycleIdx = (lastCycleIdx + cycle.size() - 1) % cycle.size();
        } while (nodes[cycle.getAtom((lastCycleIdx + cycle.size() - 1) % cycle.size()).index].visited());

        secondCycleIdx = startCycleIdx;
        firstCycleIdx  = (secondCycleIdx + cycle.size() - 1) % cycle.size();
        noConstraints  = false;
    }
    else if (nodes[cycle.getAtom((startCycleIdx + 1) % cycle.size()).index].visited()) {
        // Case 2: Constraint sequence starts with C. S iterates forward from C until the first
        // unvisited atom, F=S-1, E=C+1.
        // ..-S-F-C..C-L-.. -> ..-L-C..C-F-S-..
        do {
            secondCycleIdx = (secondCycleIdx + 1) % cycle.size();
        } while (nodes[cycle.getAtom(secondCycleIdx).index].visited());

        lastCycleIdx  = (startCycleIdx + 1) % cycle.size();
        firstCycleIdx = (secondCycleIdx + cycle.size() - 1) % cycle.size();
        noConstraints = false;
    }
    else {
        // Case 3: There are no constraints. The C is printed at the assigned position, F=L=C, S=F+1
        // ..-F/L-S-..
        const auto startPosition = nodes[startAtom.index].position;
        buffer.insert(startPosition, nodes[startAtom.index].getSymbol());

        firstCycleIdx  = startCycleIdx;
        secondCycleIdx = (startCycleIdx + 1) % cycle.size();
        lastCycleIdx   = startCycleIdx;
        noConstraints  = true;
    }

    const auto& firstAtom = cycle.getAtom(firstCycleIdx);

    const auto totalAtomsToPrint = secondCycleIdx < lastCycleIdx
                                       ? lastCycleIdx - secondCycleIdx + 1 - (not noConstraints)
                                       : cycle.size() - secondCycleIdx + lastCycleIdx + 1 - (not noConstraints);

    std::vector<PositionLine> positions;
    if (noConstraints && cycle.size() > 12)
        positions = generateOneShotCycleLayout(cycle.getCycle(), firstCycleIdx, secondCycleIdx, enteringDirection);
    if (positions.empty() && totalAtomsToPrint <= 18)
        positions = generateOptimalCycleLayout(
            cycle.getCycle(), firstCycleIdx, secondCycleIdx, lastCycleIdx, noConstraints, enteringDirection);
    if (positions.empty()) {
        Log(this).trace("Failed to print ASCII cycle, reverting to linear printing.");
        expandCycleLinearly(cycle, noConstraints ? startAtom : firstAtom, secondCycleIdx, lastCycleIdx);
        return;
    }

    class Member
    {
    private:
        const BondedAtomBase* atom;

    public:
        c_size totalConstraintCount;
        c_size totalCycleSize;
        c_size unprintedNeighbourCount;

    private:
        Member(
            const BondedAtomBase& atom,
            const c_size          totalConstraintCount,
            const c_size          totalCycleSize,
            const c_size          unprintedNeighbourCount) noexcept :
            atom(&atom),
            totalConstraintCount(totalConstraintCount),
            totalCycleSize(totalCycleSize),
            unprintedNeighbourCount(unprintedNeighbourCount)
        {}

    public:
        const BondedAtomBase& getAtom() const { return *atom; }

        static Member create(const BondedAtomBase& atom, const std::vector<Node>& nodes)
        {
            c_size totalConstraintCount = 0;
            c_size totalCycleSize       = 0;

            for (const auto& cycle : nodes[atom.index].getContainingCycles()) {
                if (cycle.visited())
                    continue;

                totalCycleSize       += cycle.size();
                totalConstraintCount += cycle.countVisitedAtoms(nodes);
            }

            const auto unprintedNeighbourCount = static_cast<c_size>(std::ranges::count_if(
                atom.bonds, [&](const auto& bond) { return not nodes[bond.getOther().index].visited(); }));

            return Member(atom, totalConstraintCount, totalCycleSize, unprintedNeighbourCount);
        }
    };

    const auto& secondAtom    = cycle.getAtom(secondCycleIdx);
    const auto  startPosition = noConstraints ? nodes[startAtom.index].position : nodes[firstAtom.index].position;

    // Print the first bond.
    if (noConstraints) {
        const auto dirToNext = Direction(positions.front().origin - startPosition);
        const auto edgePos   = getNextEdgePosition(nodes[startAtom.index].getPosition(), dirToNext);
        printEdge(edgePos, dirToNext, startAtom, secondAtom);
    }
    else {
        const auto dirToNext = Direction(positions.front().origin - startPosition);
        const auto edgePos   = getNextEdgePosition(nodes[firstAtom.index].getPosition(), dirToNext);
        printEdge(edgePos, dirToNext, firstAtom, secondAtom);
    }

    // Print the cycle.
    auto orderedIdx = secondCycleIdx;
    for (size_t i = 0; i < positions.size() - 1; ++i) {
        const auto& currentAtom = cycle.getAtom(orderedIdx);
        const auto  currentPos  = positions[i];

        orderedIdx           = (orderedIdx + 1) % cycle.size();
        const auto& nextAtom = cycle.getAtom(orderedIdx);
        const auto  nextPos  = positions[i + 1];

        nodes[currentAtom.index].position = currentPos.origin;

        const auto dirToNext = Direction(nextPos.origin - currentPos.origin);
        const auto edgePos   = getNextEdgePosition(currentPos, dirToNext);

        printEdge(edgePos, dirToNext, currentAtom, nextAtom);
        buffer.insert(currentPos.origin, nodes[currentAtom.index].getSymbol());
    }
    {
        const auto& currentAtom = cycle.getAtom(orderedIdx);
        const auto  currentPos  = positions.back();

        orderedIdx           = (orderedIdx + 1) % cycle.size();
        const auto& nextAtom = cycle.getAtom(orderedIdx);

        nodes[currentAtom.index].position = currentPos.origin;

        // Last bond uses edge position inference to match with the generator logic.
        const auto& lastNode = nodes[cycle.getAtom(lastCycleIdx).index];
        const auto  edgePos  = inferEdgePosition(currentPos, lastNode.getPosition());
        printEdge(edgePos->first, edgePos->second, currentAtom, nextAtom);
        buffer.insert(currentPos.origin, nodes[currentAtom.index].getSymbol());
    }

    cycle.computeAndSetCentroid(nodes);

    if (options.has(PrintFlags::PRINT_CYCLE_ORDER) && cycle.size() >= 4) {
        const auto centerPos = round_cast<ColoredTextBlock::IndexT>(cycle.centroid);

        const ColoredString label(
            std::to_string(std::ranges::count_if(cycles, [](const auto& c) { return c.visited(); })),
            OS::BasicColor::DARK_GREY);
        if (buffer.isWhiteSpace(centerPos, label.size()))
            buffer.insert(centerPos, label);
    }

    // Populate printed nodes (must be done after the center is computed and the cycle is marked as
    // visited).
    std::vector<Member> printedAtoms;
    printedAtoms.reserve(positions.size() + noConstraints);

    if (noConstraints)
        printedAtoms.emplace_back(Member::create(startAtom, nodes));

    orderedIdx = secondCycleIdx;
    for (size_t i = 0; i < positions.size(); ++i) {
        const auto& currentAtom = cycle.getAtom(orderedIdx);
        printedAtoms.emplace_back(Member::create(currentAtom, nodes));
        orderedIdx = (orderedIdx + 1) % cycle.size();
    }

    // Cycles with the highest number of constraints should be printed first.
    //     C-C       .
    //    /   \      .
    //   C  2  X     .
    //  / \   / \    .
    // C 1 C-C 3 C   .
    //  \ /   \ /    .
    //   C     C     .
    // If cycle 1 is printed first followed by cycle 3, atom X might be printed downwards due to the
    // extra degree of freedom, resulting in an impossible layout for cycle 2.
    std::ranges::sort(printedAtoms, [&](const auto& lhs, const auto& rhs) {
        return lhs.totalConstraintCount > rhs.totalConstraintCount   ? true
               : lhs.totalConstraintCount < rhs.totalConstraintCount ? false
               : lhs.totalCycleSize > rhs.totalCycleSize             ? true
               : lhs.totalCycleSize < rhs.totalCycleSize             ? false
                                                         : lhs.unprintedNeighbourCount > rhs.unprintedNeighbourCount;
    });

    for (const auto& atom : printedAtoms) {
        // For nodes which are part of 2 or more cycles, the preferred directions should point away
        // from both centers in order to avoid printing inside the second cycle:
        //     C 1 C     .
        //    / \|/ \    .
        //   C 2-N-2 C   .
        //   |  /|\  |   .
        //   C 2 C 2 C   .
        //    \ / \ /    .
        //     C   C     .
        // (1-best, 2-worst)
        printNeighbors(nodes[atom.getAtom().index], utils::npos<Direction>, &cycle);
    }
}

}  // namespace ASCII
