#pragma once

#include "structs/Point.hpp"

#include <array>

namespace ASCII
{
    //
    // Rotation
    //

    enum class Rotation : uint8_t
    {
        NONE,

        CLOCKWISE,
        COUNTER_CLOCKWISE
    };

    //
    // Angle
    //

    enum class Angle : uint8_t
    {
        NONE,

        ACUTE,
        PERPENDICULAR,
        OBTUSE,
        PARALLEL
    };

    //
    // Direction
    //

    class Direction
    {
    private:
        template<typename T>
        static constexpr int8_t normalize(const T val);
        static uint8_t findIdx(const int8_t x, const int8_t y);

        uint8_t idx = utils::npos<uint8_t>;

        static const std::array<Point<int8_t>, 8> _allDirections;

        constexpr Direction(
            const uint8_t idx = utils::npos<uint8_t>
        ) noexcept :
            idx(idx)
        {}

    public:
        Direction(
            const int8_t x,
            const int8_t y
        ) noexcept;
        template<typename T>
        Direction(const Point<T>& point) noexcept;

        constexpr Point<int8_t> get() const;
        constexpr uint8_t getIdx() const;

        constexpr bool isDiagonal() const;

        char getSymbol() const;
        Angle getAngle(const Direction other) const;

        template<Rotation R>
        Direction turn(const Angle angle, const bool parallelIsBackward = false) const;

        float_s getCyclicAngleScore(const Direction other) const;
        float_s getCyclicEnteringAngleScore(const Direction other) const;

        constexpr Direction& operator++();
        constexpr Direction& operator--();

        constexpr bool operator==(const Direction other) const;
        constexpr bool operator!=(const Direction other) const;

        static const Direction UpLeft;
        static const Direction Up;
        static const Direction UpRight;
        static const Direction Right;
        static const Direction DownRight;
        static const Direction Down;
        static const Direction DownLeft;
        static const Direction Left;
        static const std::array<Direction, 8> AllDirections;
        static const std::vector<Direction> AllDirectionsVector;

        friend class utils::NPos<Direction>;
    };

    inline constexpr Direction Direction::UpLeft = Direction(0);
    inline constexpr Direction Direction::Up = Direction(1);
    inline constexpr Direction Direction::UpRight = Direction(2);
    inline constexpr Direction Direction::Right = Direction(3);
    inline constexpr Direction Direction::DownRight = Direction(4);
    inline constexpr Direction Direction::Down = Direction(5);
    inline constexpr Direction Direction::DownLeft = Direction(6);
    inline constexpr Direction Direction::Left = Direction(7);

    inline constexpr std::array<Point<int8_t>, 8> Direction::_allDirections
    {
        Point<int8_t>(-1, -1), // 0 1 2 
        Point<int8_t>(0, -1),  //  \|/
        Point<int8_t>(1, -1),  // 7- -3
        Point<int8_t>(1, 0),   //  /|\ 
        Point<int8_t>(1, 1),   // 6 5 4
        Point<int8_t>(0, 1),
        Point<int8_t>(-1, 1),
        Point<int8_t>(-1, 0),
    };

    inline constexpr std::array<Direction, 8> Direction::AllDirections
    {
        UpLeft, Up, UpRight, Right,
        DownRight, Down, DownLeft, Left
    };

    template<typename T>
    Direction::Direction(
        const Point<T>& point
    ) noexcept :
        idx(findIdx(normalize(point.x), normalize(point.y)))
    {}

    template<typename T>
    constexpr int8_t Direction::normalize(const T val)
    {
        return
            val < 0 ? -1 :
            val > 0 ? 1 :
            0;
    }

    constexpr uint8_t Direction::getIdx() const
    {
        return idx;
    }

    constexpr Point<int8_t> Direction::get() const
    {
        return _allDirections[idx];
    }

    constexpr bool Direction::isDiagonal() const
    {
        return idx % 2 == 0;
    }

    constexpr Direction& Direction::operator++()
    {
        idx = static_cast<uint8_t>((idx + 1) % _allDirections.size());
        return *this;
    }

    constexpr Direction& Direction::operator--()
    {
        idx = static_cast<uint8_t>((idx + _allDirections.size() - 1) % _allDirections.size());
        return *this;
    }

    constexpr bool Direction::operator==(const Direction other) const
    {
        return this->idx == other.idx;
    }

    constexpr bool Direction::operator!=(const Direction other) const
    {
        return this->idx != other.idx;
    }

} // namespace ASCII

template <>
class utils::NPos<ASCII::Direction>
{
public:
    using ObjT = ASCII::Direction;

    static constexpr ObjT value;
    static bool isNPos(const ObjT& direction)
    {
        return utils::isNPos(direction.idx);
    }
};
