#pragma once

#include "io/Log.hpp"
#include "utils/Meta.hpp"

template <typename AvgT>
class AverageStream
{
private:
    using CountT  = std::conditional_t<(sizeof(AvgT) <= 4), uint32_t, uint64_t>;
    using AvgRefT = utils::RefType<AvgT>;

    AvgT   _sum   = AvgT();
    CountT _count = 0;

public:
    AverageStream()                     = default;
    AverageStream(const AverageStream&) = default;
    AverageStream(AvgRefT zero) noexcept;

    AvgT   avg() const;
    CountT count() const;

    void add(AvgRefT value);
    void remove(AvgRefT value);
    void clear();

    AverageStream& operator<<(AvgRefT value);
};

template <typename AvgT>
AverageStream<AvgT>::AverageStream(AvgRefT zero) noexcept :
    _sum(zero),
    _count(0)
{}

template <typename AvgT>
AvgT AverageStream<AvgT>::avg() const
{
    if (_count == 0)
        Log(this).fatal("Division by 0.");

    return _sum / _count;
}

template <typename AvgT>
AverageStream<AvgT>::CountT AverageStream<AvgT>::count() const
{
    return _count;
}

template <typename AvgT>
void AverageStream<AvgT>::add(AvgRefT value)
{
    _sum += value;
    ++_count;
}

template <typename AvgT>
void AverageStream<AvgT>::remove(AvgRefT value)
{
    if (_count == 0)
        Log(this).fatal("Tried to remove value: {} from an empty stream.", value);

    _sum -= value;
    --_count;
}

template <typename AvgT>
void AverageStream<AvgT>::clear()
{
    _sum   = AvgT();
    _count = 0;
}

template <typename AvgT>
AverageStream<AvgT>& AverageStream<AvgT>::operator<<(AvgRefT value)
{
    add(value);
    return *this;
}
