#include "utils/ASCII.hpp"

#include "global/Charset.hpp"
#include "io/Log.hpp"

namespace ASCII
{

//
// Direction
//

const std::vector<Direction>
    Direction::AllDirectionsVector(AllDirections.begin(), AllDirections.end());

Direction::Direction(const int8_t x, const int8_t y) noexcept :
    idx(findIdx(normalize(x), normalize(y)))
{}

uint8_t Direction::findIdx(const int8_t x, const int8_t y)
{
    const auto norm = Point(x, y);
    const auto it   = std::ranges::find(_allDirections, norm);
    return it != _allDirections.end()
               ? static_cast<uint8_t>(std::distance(_allDirections.begin(), it))
               : utils::npos<uint8_t>;
}

char Direction::getSymbol() const
{
    const auto dir = get();
    return dir.x == 0                     ? ASCII::LineV
           : dir.y == 0                   ? ASCII::LineH
           : ((dir.x < 0) == (dir.y < 0)) ? '\\'
                                          : '/';
}

Angle Direction::getAngle(const Direction other) const
{
    if (*this == other)
        return Angle::PARALLEL;

    const auto thisDir  = this->get();
    const auto otherDir = other.get();

    const auto dot = thisDir.x * otherDir.x + thisDir.y * otherDir.y;
    return dot < 0 ? Angle::ACUTE : dot > 0 ? Angle::OBTUSE : Angle::PERPENDICULAR;
}

template <Rotation R>
Direction Direction::turn(const Angle angle, const bool parallelIsBackward) const
{
    const auto step = [this](const Angle angle, const bool parallelIsBackward) -> uint8_t {
        switch (angle) {
        case Angle::ACUTE:
            return 3;
        case Angle::PERPENDICULAR:
            return 2;
        case Angle::OBTUSE:
            return 1;
        case Angle::PARALLEL:
            return parallelIsBackward ? 4 : 0;
        default:
            Log(this).fatal("Got unknown angle.");
            return 0xFF;
        }
    }(angle, parallelIsBackward);

    if constexpr (R == Rotation::CLOCKWISE)
        return Direction(static_cast<uint8_t>((idx + step) % Direction::AllDirections.size()));
    else
        return Direction(
            static_cast<uint8_t>(
                (idx - step + Direction::AllDirections.size()) % Direction::AllDirections.size()));
}

template Direction
Direction::turn<Rotation::CLOCKWISE>(const Angle angle, const bool parallelIsBackward) const;
template Direction Direction::turn<Rotation::COUNTER_CLOCKWISE>(
    const Angle angle, const bool parallelIsBackward) const;

namespace
{

constexpr uint8_t getAngleIndex(const Direction d1, const Direction d2)
{
    // Relative angle index (example when d1 is (1, 0)):
    //  7 6 5   .
    //   \|/    .
    //  0- -4   .
    //   /|\    .
    //  1 2 3   .
    // The following always hold:
    //  0   = same direction
    //  4   = reverse direction
    //  1-3 = CCW
    //  5-7 = CW
    return static_cast<uint8_t>(
        (d1.getIdx() - d2.getIdx() + Direction::AllDirections.size()) %
        Direction::AllDirections.size());
}

}  // namespace

float_s Direction::getCyclicAngleScore(const Direction other) const
{
    if (utils::isNPos(*this) || utils::isNPos(other))
        Log(this).fatal("Cannot compute cyclic angle score with an undefined direction.");

    switch (getAngleIndex(*this, other)) {
    case 1:
        return 1.0f;  // CCW obtuse
    case 0:
        return 0.9f;  // parallel
    case 7:
        return 0.95f;  // CW obtuse
    case 2:
        return 0.55f;  // CCW perpendicular
    case 6:
        return 0.5f;  // CW perpendicular
    case 3:
        return -0.2f;  // CCW acute
    case 5:
        return -0.25f;  // CW acute
    default:
        return -1.0f;  // backward
    }
}

float_s Direction::getCyclicEnteringAngleScore(const Direction other) const
{
    if (utils::isNPos(*this) || utils::isNPos(other))
        Log(this).fatal("Cannot compute cyclic angle score with an undefined direction.");

    switch (getAngleIndex(*this, other)) {
    case 7:
        return 1.0f;  // CW obtuse
    case 6:
        return 0.9f;  // CW perpendicular
    case 1:
        return 0.8f;  // CCW obtuse
    case 0:
        return 0.7f;  // parallel
    case 2:
        return 0.6f;  // CCW perpendicular
    case 5:
        return 0.1f;  // CW acute
    case 3:
        return -0.5f;  // CCW acute
    default:
        return -1.0f;  // backward
    }
}

}  // namespace ASCII
