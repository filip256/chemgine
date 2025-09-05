#pragma once

#include "estimators/EstimationMode.hpp"
#include "global/Precision.hpp"

#include <optional>
#include <tuple>
#include <vector>

class Regressor3DBase
{
public:
    Regressor3DBase()                       = default;
    Regressor3DBase(const Regressor3DBase&) = default;
    virtual ~Regressor3DBase()              = default;

    virtual float_s get(const float_s input1, const float_s input2) const = 0;

    virtual std::vector<float_s> getParams() const = 0;

    virtual bool
    isEquivalent(const Regressor3DBase& other, const float_s epsilon = std::numeric_limits<float_s>::epsilon()) const;
};

class LinearRegressor3D : public Regressor3DBase
{
public:
    const float_s paramX;
    const float_s paramY;
    const float_s shift;

    LinearRegressor3D(const float_s paramX, const float_s paramY, const float_s shift) noexcept;
    LinearRegressor3D(const LinearRegressor3D&) = default;

    float_s get(const float_s input1, const float_s input2) const override final;

    std::vector<float_s> getParams() const override final;

    bool isEquivalent(const Regressor3DBase& other, const float_s epsilon = std::numeric_limits<float_s>::epsilon())
        const override final;

    static LinearRegressor3D fit(const std::vector<std::tuple<float_s, float_s, float_s>>& points);

    static constexpr EstimationMode Mode = EstimationMode::LINEAR;
};
