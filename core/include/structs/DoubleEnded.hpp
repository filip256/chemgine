#pragma once

#include "utils/Meta.hpp"

#include <type_traits>

template <typename T>
class DoubleEnded
{
private:
    T forward;
    T backward;

    using ElemT         = std::decay_t<decltype(std::declval<T>()[std::declval<std::size_t>()])>;
    using ConstElemRefT = std::conditional_t<std::is_fundamental_v<ElemT>, ElemT, const ElemT&>;

public:
    using IndexT = std::conditional_t<sizeof(std::size_t) == 4, int32_t, int64_t>;

    DoubleEnded()                   = default;
    DoubleEnded(const DoubleEnded&) = default;
    DoubleEnded(DoubleEnded&&)      = default;
    template <typename CopyFn>
    DoubleEnded(const DoubleEnded& other, CopyFn makeCopy) noexcept;
    DoubleEnded(const IndexT begin, const IndexT end, const ConstElemRefT elem) noexcept;
    DoubleEnded(T&& init) noexcept;

    DoubleEnded& operator=(const DoubleEnded& other) = default;
    DoubleEnded& operator=(DoubleEnded&& other)      = default;

    bool   empty() const;
    size_t totalSize() const;

    IndexT beginIndex() const;
    IndexT endIndex() const;

    void reserve(const IndexT size);

    void resize(const IndexT size);
    void resize(const IndexT size, const ConstElemRefT elem);
    void expandTo(const IndexT idx);
    void expandTo(const IndexT idx, const ConstElemRefT elem);
    void expandBy(const IndexT size);
    void expandBy(const IndexT size, const ConstElemRefT elem);
    void shrinkToFit();

    void pushBack(const ElemT& elem);
    void pushFront(const ElemT& elem);

    template <typename... Args>
    void emplaceBack(Args&&... args);
    template <typename... Args>
    void emplaceFront(Args&&... args);

    template <typename It>
    void appendBack(It begin, It end);
    template <typename It>
    void appendFront(It begin, It end);
    void appendBack(const T& values);
    void appendFront(const T& values);
    void appendBack(const DoubleEnded& other);
    void appendFront(const DoubleEnded& other);

    void overwrite(IndexT idx, const T& values);

    T extract(const IndexT begin, const IndexT end) const;

    void popBack();
    void popFront();
    void clear();

    decltype(auto) operator[](const IndexT x) const;
    ElemT&         operator[](const IndexT x);

    decltype(auto) front() const;
    ElemT&         front();
    decltype(auto) back() const;
    ElemT&         back();

    const T& forwardData() const;
    T&       forwardData();
    const T& backwardData() const;
    T&       backwardData();

    void combine(T& output) const;
    T    combine() const;

    void dump(T& output, const IndexT beginIdx, const IndexT endIdx) const;
    void dump(std::ostream& os, const IndexT begin, const IndexT end) const;

    template <typename U>
    friend std::ostream& operator<<(std::ostream& os, const DoubleEnded<U>& obj);
};

template <typename T>
template <typename CopyFn>
DoubleEnded<T>::DoubleEnded(const DoubleEnded& other, CopyFn makeCopy) noexcept
{
    this->forward.reserve(other.forward.size());
    this->backward.reserve(other.backward.size());
    for (ConstElemRefT elem : other.forward) this->forward.emplace_back(makeCopy(elem));
    for (ConstElemRefT elem : other.backward) this->backward.emplace_back(makeCopy(elem));
}

template <typename T>
DoubleEnded<T>::DoubleEnded(const IndexT begin, const IndexT end, const ConstElemRefT elem) noexcept :
    forward(end > 0 ? end : 0, elem),
    backward(begin < 0 ? -begin : 0, elem)
{}

template <typename T>
DoubleEnded<T>::DoubleEnded(T&& init) noexcept :
    forward(std::move(init))
{}

template <typename T>
bool DoubleEnded<T>::empty() const
{
    return forward.empty() && backward.empty();
}

template <typename T>
size_t DoubleEnded<T>::totalSize() const
{
    return forward.size() + backward.size();
}

template <typename T>
DoubleEnded<T>::IndexT DoubleEnded<T>::beginIndex() const
{
    return -static_cast<IndexT>(backward.size());
}

template <typename T>
DoubleEnded<T>::IndexT DoubleEnded<T>::endIndex() const
{
    return static_cast<IndexT>(forward.size());
}

template <typename T>
void DoubleEnded<T>::reserve(const IndexT size)
{
    if (size >= 0) {
        forward.reserve(size);
        return;
    }

    backward.reserve(-size);
}

template <typename T>
void DoubleEnded<T>::pushBack(const ElemT& elem)
{
    forward.push_back(elem);
}

template <typename T>
void DoubleEnded<T>::pushFront(const ElemT& elem)
{
    backward.push_back(elem);
}

template <typename T>
template <typename... Args>
void DoubleEnded<T>::emplaceBack(Args&&... args)
{
    forward.emplace_back(std::forward<Args>(args)...);
}

template <typename T>
template <typename... Args>
void DoubleEnded<T>::emplaceFront(Args&&... args)
{
    backward.emplace_back(std::forward<Args>(args)...);
}

template <typename T>
template <typename It>
void DoubleEnded<T>::appendBack(It begin, It end)
{
    forward.insert(forward.end(), begin, end);
}

template <typename T>
template <typename It>
void DoubleEnded<T>::appendFront(It begin, It end)
{
    backward.insert(backward.end(), begin, end);
}

template <typename T>
void DoubleEnded<T>::appendBack(const T& values)
{
    appendBack(values.begin(), values.end());
}

template <typename T>
void DoubleEnded<T>::appendFront(const T& values)
{
    appendFront(values.begin(), values.end());
}

template <typename T>
void DoubleEnded<T>::appendBack(const DoubleEnded& other)
{
    appendBack(other.backward.rbegin(), other.backward.rend());
    appendBack(other.forward);
}

template <typename T>
void DoubleEnded<T>::appendFront(const DoubleEnded& other)
{
    appendFront(other.forward.rbegin(), other.forward.rend());
    appendFront(other.backward);
}

template <typename T>
void DoubleEnded<T>::overwrite(IndexT idx, const T& values)
{
    // -5    0    5
    //  -----------
    //    ^
    //   -3 overwrite "012345"

    if (values.empty())
        return;

    size_t i = 0;
    while (idx < 0 && i < values.size()) {
        backward[-idx - 1] = values[i];
        ++idx;
        ++i;
    }

    while (i < values.size()) {
        forward[idx] = values[i];
        ++idx;
        ++i;
    }
}

template <typename T>
T DoubleEnded<T>::extract(const IndexT begin, const IndexT end) const
{
    T temp;
    temp.reserve(end - begin);
    dump(temp, begin, end);
    return temp;
}

template <typename T>
void DoubleEnded<T>::popBack()
{
    forward.pop_back();
}

template <typename T>
void DoubleEnded<T>::popFront()
{
    backward.pop_back();
}

template <typename T>
void DoubleEnded<T>::clear()
{
    forward.clear();
    backward.clear();
}

template <typename T>
void DoubleEnded<T>::resize(const IndexT size)
{
    if (size >= 0)
        forward.resize(size);
    else
        backward.resize(-size);
}

template <typename T>
void DoubleEnded<T>::resize(const IndexT size, const ConstElemRefT elem)
{
    if (size >= 0)
        forward.resize(size, elem);
    else
        backward.resize(-size, elem);
}

template <typename T>
void DoubleEnded<T>::expandTo(const IndexT idx)
{
    if (idx >= 0) {
        const auto forwardSize = static_cast<size_t>(idx + 1);
        if (forwardSize > forward.size())
            forward.resize(forwardSize);
        return;
    }

    const auto backwardSize = static_cast<size_t>(-idx);
    if (backwardSize > backward.size())
        backward.resize(backwardSize);
}

template <typename T>
void DoubleEnded<T>::expandTo(const IndexT idx, const ConstElemRefT elem)
{
    if (idx >= 0) {
        const auto forwardSize = static_cast<size_t>(idx + 1);
        if (forwardSize > forward.size())
            forward.resize(forwardSize, elem);
        return;
    }

    const auto backwardSize = static_cast<size_t>(-idx);
    if (backwardSize > backward.size())
        backward.resize(backwardSize, elem);
}

template <typename T>
void DoubleEnded<T>::expandBy(const IndexT size)
{
    if (size >= 0)
        forward.resize(forward.size() + size);
    else
        backward.resize(backward.size() - size);
}

template <typename T>
void DoubleEnded<T>::expandBy(const IndexT size, const ConstElemRefT elem)
{
    if (size >= 0)
        forward.resize(forward.size() + size, elem);
    else
        backward.resize(backward.size() - size, elem);
}

template <typename T>
void DoubleEnded<T>::shrinkToFit()
{
    forward.shrink_to_fit();
    backward.shrink_to_fit();
}

template <typename T>
decltype(auto) DoubleEnded<T>::operator[](const IndexT x) const
{
    return x >= 0 ? forward[x] : backward[-x - 1];
}

template <typename T>
typename DoubleEnded<T>::ElemT& DoubleEnded<T>::operator[](const IndexT x)
{
    return x >= 0 ? forward[x] : backward[-x - 1];
}

template <typename T>
decltype(auto) DoubleEnded<T>::front() const
{
    return backward.back();
}

template <typename T>
typename DoubleEnded<T>::ElemT& DoubleEnded<T>::front()
{
    return backward.back();
}

template <typename T>
decltype(auto) DoubleEnded<T>::back() const
{
    return forward.back();
}

template <typename T>
typename DoubleEnded<T>::ElemT& DoubleEnded<T>::back()
{
    return forward.back();
}

template <typename T>
const T& DoubleEnded<T>::forwardData() const
{
    return forward;
}

template <typename T>
T& DoubleEnded<T>::forwardData()
{
    return forward;
}

template <typename T>
const T& DoubleEnded<T>::backwardData() const
{
    return backward;
}

template <typename T>
T& DoubleEnded<T>::backwardData()
{
    return backward;
}

template <typename T>
void DoubleEnded<T>::combine(T& output) const
{
    output.insert(output.end(), backward.rbegin(), backward.rend());
    output.insert(output.end(), forward.begin(), forward.rend());
}

template <typename T>
T DoubleEnded<T>::combine() const
{
    T result;
    result.reserve(forward.size() + backward.size());
    combine(result);

    return result;
}

template <typename T>
void DoubleEnded<T>::dump(T& output, const IndexT beginIdx, const IndexT endIdx) const
{
    for (auto i = beginIdx; i < endIdx; ++i) output.push_back((*this)[i]);
}

template <typename T>
void DoubleEnded<T>::dump(std::ostream& os, const IndexT beginIdx, const IndexT endIdx) const
{
    if constexpr (utils::is_streamable_v<ElemT>) {
        for (auto i = beginIdx; i < endIdx; ++i) os << (*this)[i];
    }
    else if constexpr (utils::is_streamable_v<T>) {
        T temp;
        temp.reserve(endIdx - beginIdx);
        dump(temp, beginIdx, endIdx);
        os << temp;
    }
    else
        static_assert(utils::always_false<T>, "DoubleEnded: Undefined operator<< for T and ElemT.");
}

template <typename T>
std::ostream& operator<<(std::ostream& os, const DoubleEnded<T>& obj)
{
    if constexpr (utils::is_streamable_v<typename DoubleEnded<T>::ElemT>) {
        for (size_t i = obj.backward.size(); i-- > 0;) os << obj.backward[i];
        for (size_t i = 0; i < obj.forward.size(); ++i) os << obj.forward[i];
    }
    else if constexpr (utils::is_streamable_v<T>)
        os << obj.combine();
    else {
        static_assert(utils::always_false<T>, "DoubleEnded: Undefined operator<< for T and ElemT.");
    }

    return os;
}
