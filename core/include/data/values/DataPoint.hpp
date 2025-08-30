#pragma once

#include "data/def/Location.hpp"
#include "data/values/Amount.hpp"
#include "data/values/DynamicAmount.hpp"
#include "io/Log.hpp"

template <Unit OutU, Unit... InUs>
class DataPoint
{
public:
    const Amount<OutU>                output;
    const std::tuple<Amount<InUs>...> inputs;

    DataPoint(const Amount<OutU> output, const std::tuple<Amount<InUs>...>& inputs) noexcept;

    template <typename DummyT = void, typename = std::enable_if_t<(sizeof...(InUs) > 1), DummyT>>
    DataPoint(const Amount<OutU> output, const Amount<InUs>... inputs) noexcept;

    DataPoint(const DataPoint&) = default;

    bool operator==(const DataPoint& other) const;
    bool operator<(const DataPoint& other) const;
    bool operator>(const DataPoint& other) const;
};

template <Unit OutU, Unit... InUs>
DataPoint<OutU, InUs...>::DataPoint(const Amount<OutU> output, const std::tuple<Amount<InUs>...>& inputs) noexcept :
    output(output),
    inputs(inputs)
{}

template <Unit OutU, Unit... InUs>
template <typename DummyT, typename>
DataPoint<OutU, InUs...>::DataPoint(const Amount<OutU> output, const Amount<InUs>... inputs) noexcept :
    DataPoint<OutU, InUs...>(output, std::make_tuple(inputs...))
{}

template <Unit OutU, Unit... InUs>
bool DataPoint<OutU, InUs...>::operator==(const DataPoint& other) const
{
    return this->output == other.output && this->inputs == other.inputs;
}

template <Unit OutU, Unit... InUs>
bool DataPoint<OutU, InUs...>::operator<(const DataPoint& other) const
{
    return this->inputs < other.inputs;
}

template <Unit OutU, Unit... InUs>
bool DataPoint<OutU, InUs...>::operator>(const DataPoint& other) const
{
    return this->inputs > other.inputs;
}

template <Unit OutU, Unit... InUs>
class def::Parser<DataPoint<OutU, InUs...>>
{
private:
    template <size_t... Is>
    static std::optional<std::tuple<Amount<InUs>...>>
    convertInputs(const std::vector<DynamicAmount>& baseInputs, const def::Location& location, std::index_sequence<Is...>)
    {
        std::tuple<Amount<InUs>...> expectedInputs;
        auto                        failed = static_cast<uint8_t>(-1);

        ([&baseInputs, &expectedInputs, &failed]() {
            const auto expected = baseInputs[Is].to<InUs>();
            if (not expected) {
                failed = Is;
                return;
            }

            std::get<Is>(expectedInputs) = *expected;
        }(), ...);

        return failed == static_cast<uint8_t>(-1) ? std::optional(expectedInputs) : std::nullopt;
    }

public:
    static std::optional<DataPoint<OutU, InUs...>> parse(
        const std::string&       str,
        const Unit               outputBaseUnit,
        const std::vector<Unit>& inputBaseUnits,
        const def::Location&     location)
    {
        const Log<DataPoint<OutU, InUs...>> log;
        const auto                          inputCount = sizeof...(InUs);

        // parse values
        const auto pair = def::parse<std::pair<std::string, DynamicAmount>>(str, ':');
        if (not pair) {
            log.error("Malfomed data point: '{0}', at: {1}.", str, location.toString());
            return std::nullopt;
        }

        const auto rawInputs = def::parse<std::vector<DynamicAmount>>(pair->first, ',');
        if (not rawInputs) {
            log.error("Malfomed data point inputs list: '{0}', at: {1}.", pair->first, location.toString());
            return std::nullopt;
        }
        if (rawInputs->size() != inputCount) {
            log.error(
                "The number of data point inputs ({0}) in: '{1}' does not match the expected "
                "number of inputs ({2}), "
                "at: {3}.",
                rawInputs->size(),
                pair->first,
                inputCount,
                location.toString());
            return std::nullopt;
        }

        // convert output
        const auto baseOutput = pair->second.to(outputBaseUnit);
        if (not baseOutput) {
            log.error(
                "Failed to convert data point output from given unit: '{0}' to base unit: '{1}', "
                "at: {2}.",
                pair->second.getUnitSymbol(),
                DynamicAmount::getUnitSymbol(outputBaseUnit),
                location.toString());
            return std::nullopt;
        }

        const auto expectedOutput = baseOutput->to<OutU>();
        if (not expectedOutput) {
            log.error(
                "Failed to convert data point output from base unit: '{0}' to expected unit: "
                "'{1}', at: {2}.",
                baseOutput->getUnitSymbol(),
                DynamicAmount::getUnitSymbol(OutU),
                location.toString());
            return std::nullopt;
        }

        // convert inputs
        std::vector<DynamicAmount> baseInputs;
        baseInputs.reserve(inputCount);
        for (size_t i = 0; i < inputCount; i++) {
            const auto convert = (*rawInputs)[i].to(inputBaseUnits[i]);
            if (not convert) {
                log.error(
                    "Failed to convert data point input from given unit: '{0}' to base unit: "
                    "'{1}', at: {2}.",
                    (*rawInputs)[i].getUnitSymbol(),
                    DynamicAmount::getUnitSymbol(inputBaseUnits[i]),
                    location.toString());
                return std::nullopt;
            }

            baseInputs.emplace_back(*convert);
        }

        // try given input order
        const auto expectedInputs = convertInputs(baseInputs, location, std::make_index_sequence<inputCount>{});
        if (expectedInputs)
            return DataPoint(*expectedOutput, *expectedInputs);

        // try to permute input order (must sort before calling next_permutation)
        std::sort(
            baseInputs.begin(), baseInputs.end(), [](const auto& l, const auto& r) { return l.getUnit() < r.getUnit(); });
        do {
            const auto expectedInputs = convertInputs(baseInputs, location, std::make_index_sequence<inputCount>{});
            if (expectedInputs)
                return DataPoint(*expectedOutput, *expectedInputs);

        } while (std::next_permutation(
            baseInputs.begin(), baseInputs.end(), [](const auto& l, const auto& r) { return l.getUnit() < r.getUnit(); }));

        std::string baseUnitNames = DynamicAmount::getUnitSymbol(inputBaseUnits.front());
        for (size_t i = 1; i < inputBaseUnits.size(); ++i)
            baseUnitNames += ", " + DynamicAmount::getUnitSymbol(inputBaseUnits[i]);

        std::string expectedUnitNames;
        ((expectedUnitNames += Amount<InUs>::unitSymbol() + ", "), ...);
        expectedUnitNames.pop_back();
        expectedUnitNames.pop_back();

        log.error(
            "Failed to convert data point inputs from base units: '{0}' to expected units: '{1}', "
            "at: {2}.",
            baseUnitNames,
            expectedUnitNames,
            location.toString());
        return std::nullopt;
    }
};

template <Unit OutU, Unit... InUs>
class def::Printer<DataPoint<OutU, InUs...>>
{
public:
    static std::string print(const DataPoint<OutU, InUs...>& object)
    {
        return def::print(std::pair(object.inputs, object.output));
    }

    static std::string prettyPrint(const DataPoint<OutU, InUs...>& object)
    {
        return def::prettyPrint(std::pair(object.inputs, object.output));
    }
};
