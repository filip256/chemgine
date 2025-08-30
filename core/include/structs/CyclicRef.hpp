#pragma once

#include <type_traits>

template <typename T>
class CyclicRef
{
private:
    const T* container;

    using ElemT         = std::decay_t<decltype(std::declval<T>()[std::declval<std::size_t>()])>;
    using ConstElemRefT = std::conditional_t<std::is_fundamental_v<ElemT>, ElemT, const ElemT&>;

public:
    CyclicRef(const T& container) noexcept;

    size_t size() const;
    bool   empty() const;

    using Iterator = T::const_iterator;
    Iterator begin() const;
    Iterator end() const;
    Iterator rbegin() const;
    Iterator rend() const;

    size_t normalize(const int64_t idx) const;

    const T&      data() const;
    ConstElemRefT front() const;
    ConstElemRefT back() const;
    ConstElemRefT operator[](const int64_t idx) const;

    bool operator==(const CyclicRef other);
    bool operator!=(const CyclicRef other);
};

template <typename T>
CyclicRef<T>::CyclicRef(const T& container) noexcept :
    container(&container)
{}

template <typename T>
size_t CyclicRef<T>::size() const
{
    return container->size();
}

template <typename T>
bool CyclicRef<T>::empty() const
{
    return container->empty();
}

template <typename T>
CyclicRef<T>::Iterator CyclicRef<T>::begin() const
{
    return container->cbegin();
}

template <typename T>
CyclicRef<T>::Iterator CyclicRef<T>::end() const
{
    return container->cend();
}

template <typename T>
CyclicRef<T>::Iterator CyclicRef<T>::rbegin() const
{
    return container->rbegin();
}

template <typename T>
CyclicRef<T>::Iterator CyclicRef<T>::rend() const
{
    return container->rend();
}

template <typename T>
size_t CyclicRef<T>::normalize(const int64_t idx) const
{
    return static_cast<size_t>(idx % container->size() + container->size()) % container->size();
}

template <typename T>
const T& CyclicRef<T>::data() const
{
    return *container;
}

template <typename T>
CyclicRef<T>::ConstElemRefT CyclicRef<T>::front() const
{
    return container->front();
}

template <typename T>
CyclicRef<T>::ConstElemRefT CyclicRef<T>::back() const
{
    return container->back();
}

template <typename T>
CyclicRef<T>::ConstElemRefT CyclicRef<T>::operator[](const int64_t idx) const
{
    return (*container)[normalize(idx)];
}

template <typename T>
bool CyclicRef<T>::operator==(const CyclicRef other)
{
    return *this->container == *other.container;
}

template <typename T>
bool CyclicRef<T>::operator!=(const CyclicRef other)
{
    return *this->container != *other.container;
}
