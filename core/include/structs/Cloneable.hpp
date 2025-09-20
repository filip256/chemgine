#pragma once

#include <memory>

template <typename T>
class CloneableBase
{
protected:
    CloneableBase() = default;

public:
    virtual ~CloneableBase() = default;

    virtual std::unique_ptr<T> clone() const = 0;
};

template <typename DerivedT, typename BaseT>
class Cloneable : public BaseT
{
public:
    using BaseT::BaseT;

    std::unique_ptr<BaseT> clone() const override final;
};

template <typename DerivedT, typename BaseT>
std::unique_ptr<BaseT> Cloneable<DerivedT, BaseT>::clone() const
{
    return std::make_unique<DerivedT>(static_cast<const DerivedT&>(*this));
}
