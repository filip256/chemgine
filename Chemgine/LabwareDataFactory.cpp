#include "LabwareDataFactory.hpp"
#include "DataHelpers.hpp"

BaseLabwareData* LabwareDataFactory::get(const LabwareIdType id, const LabwareType type, const std::vector<std::string>& dataLine)
{
	switch (type)
	{
	case LabwareType::FLASK:
		return getFlask(id, dataLine);
	case LabwareType::ADAPTOR:
		return getAdaptor(id, dataLine);
	default:
		return nullptr;
	}
}


FlaskData* LabwareDataFactory::getFlask(const LabwareIdType id,  const std::vector<std::string>& dataLine)
{
	const auto volume = DataHelpers::toUDouble(dataLine[3]);
	if (volume.status == 0)
		return nullptr;

	const auto jointStrings = DataHelpers::parseList(dataLine[4], ';', true);
	std::vector<LabwareJoint> joints;
	joints.reserve(jointStrings.size());

	for (uint8_t i = 0; i < jointStrings.size(); ++i)
	{
		const auto joint = DataHelpers::parseList(jointStrings[i], ' ', true);

		if (joint.size() != 4)
			return nullptr;

		const auto type = DataHelpers::toUInt(joint[0]);
		const auto x = DataHelpers::toUInt(joint[1]);
		const auto y = DataHelpers::toUInt(joint[2]);
		const auto angle = DataHelpers::toUDouble(joint[3]);

		if (type.status == 0 || x.status == 0 || y.status == 0 || angle.status == 0)
			return nullptr;

		joints.emplace_back(static_cast<PortType>(type.result), x.result, y.result, angle.result);
	}

	return new FlaskData(id, dataLine[2], volume.result, std::move(joints), dataLine[5]);
}

AdaptorData* LabwareDataFactory::getAdaptor(const LabwareIdType id, const std::vector<std::string>& dataLine)
{
	const auto volume = DataHelpers::toUDouble(dataLine[3]);
	if (volume.status == 0)
		return nullptr;

	const auto jointStrings = DataHelpers::parseList(dataLine[4], ';', true);
	std::vector<LabwareJoint> joints;
	joints.reserve(jointStrings.size());

	for (uint8_t i = 0; i < jointStrings.size(); ++i)
	{
		const auto joint = DataHelpers::parseList(jointStrings[i], ' ', true);

		if (joint.size() != 4)
			return nullptr;

		const auto type = DataHelpers::toUInt(joint[0]);
		const auto x = DataHelpers::toUInt(joint[1]);
		const auto y = DataHelpers::toUInt(joint[2]);
		const auto angle = DataHelpers::toUDouble(joint[3]);

		if (type.status == 0 || x.status == 0 || y.status == 0 || angle.status == 0)
			return nullptr;

		joints.emplace_back(static_cast<PortType>(type.result), x.result, y.result, angle.result);
	}

	return new AdaptorData(id, dataLine[2], volume.result, std::move(joints), dataLine[5]);
}