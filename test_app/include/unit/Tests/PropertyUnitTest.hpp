#pragma once

#include "unit/UnitTest.hpp"
#include "DataPoint.hpp"
#include "ImmutableSet.hpp"

template<typename ObjT, Unit OutU, Unit... InUs>
class PropertyUnitTest : public UnitTest
{
public:
	using PropertyGetter = Amount<OutU>(ObjT::*)(const Amount<InUs>...) const;

private:
	const ObjT& object;
	const PropertyGetter getterCb;
	const ImmutableSet<DataPoint<OutU, InUs...>> reference;
	const float_h threshold;

public:
	PropertyUnitTest(
		std::string&& name,
		const ObjT& object,
		const PropertyGetter getterCb,
		ImmutableSet<DataPoint<OutU, InUs...>>&& reference,
		const float_h threshold
	) noexcept;

	bool run() override final;
};

template<typename ObjT, Unit OutU, Unit... InUs>
PropertyUnitTest<ObjT, OutU, InUs...>::PropertyUnitTest(
	std::string&& name,
	const ObjT& object,
	const PropertyGetter getterCb,
	ImmutableSet<DataPoint<OutU, InUs...>>&& reference,
	const float_h threshold
) noexcept :
	UnitTest(std::move(name)),
	object(object),
	getterCb(getterCb),
	reference(std::move(reference)),
	threshold(threshold)
{}

template<typename ObjT, Unit OutU, Unit... InUs>
bool PropertyUnitTest<ObjT, OutU, InUs...>::run()
{
	const auto instancedCb = [this](const Amount<InUs>... inputs) -> Amount<OutU>
		{
			return (this->object.*(this->getterCb))(inputs...);
		};

	bool success = true;
	for (size_t i = 0; i < reference.size(); ++i)
	{
		const auto ref = reference[i].output;
		const auto act = std::apply(instancedCb, reference[i].inputs);
		const auto error = std::abs((ref - act).asStd());
		if (error > threshold)
		{
			Log(this).error("Error: {0} (act= {1}, ref= {2}) exceeded the test threshold: {3}.",
				std::format("{:e}", error), act.toString(), ref.toString(), std::format("{:e}", threshold));
			success = false;
		}
	}

	return success;
}
