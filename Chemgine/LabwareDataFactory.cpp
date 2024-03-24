#include "LabwareDataFactory.hpp"
#include "DataHelpers.hpp"

BaseLabwareData* LabwareDataFactory::get(const LabwareId id, const LabwareType type, const std::vector<std::string>& dataLine)
{
	switch (type)
	{
	case LabwareType::FLASK:
		return getFlaskData(id, dataLine);
	case LabwareType::ADAPTOR:
		return getAdaptorData(id, dataLine);
	case LabwareType::HEATSOURCE:
		return getHeatsourceData(id, dataLine);
	default:
		return nullptr;
	}
}


FlaskData* LabwareDataFactory::getFlaskData(const LabwareId id,  const std::vector<std::string>& dataLine)
{
	if (dataLine.size() != 7)
		return nullptr;

	const auto volume = DataHelpers::parseUnsigned<double>(dataLine[3]);
	if (volume.has_value() == false)
		return nullptr;

	auto ports = DataHelpers::parseList<LabwarePort>(dataLine[4], ';', true);
	if (ports.has_value() == false)
		return nullptr;

	auto txScale = DataHelpers::parseUnsigned<float>(dataLine[6]);
	if (txScale.has_value() == false)
		return nullptr;

	return new FlaskData(id, dataLine[2], std::move(*ports), *volume, dataLine[5], *txScale);
}

AdaptorData* LabwareDataFactory::getAdaptorData(const LabwareId id, const std::vector<std::string>& dataLine)
{
	if (dataLine.size() != 7)
		return nullptr;

	const auto volume = DataHelpers::parseUnsigned<double>(dataLine[3]);
	if (volume.has_value() == false)
		return nullptr;

	auto ports = DataHelpers::parseList<LabwarePort>(dataLine[4], ';', true);
	if (ports.has_value() == false)
		return nullptr;

	auto txScale = DataHelpers::parseUnsigned<float>(dataLine[6]);
	if (txScale.has_value() == false)
		return nullptr;

	return new AdaptorData(id, dataLine[2], std::move(*ports), *volume, dataLine[5], *txScale);
}

HeatsourceData* LabwareDataFactory::getHeatsourceData(const LabwareId id, const std::vector<std::string>& dataLine)
{
	if (dataLine.size() != 7)
		return nullptr;

	auto ports = DataHelpers::parseList<LabwarePort>(dataLine[4], ';', true);
	if (ports.has_value() == false)
		return nullptr;

	auto txScale = DataHelpers::parseUnsigned<float>(dataLine[6]);
	if (txScale.has_value() == false)
		return nullptr;

	const auto power = DataHelpers::parseUnsigned<Unit::WATT>(dataLine[3]);
	if (power.has_value() == false)
		return nullptr;

	return new HeatsourceData(id, dataLine[2], std::move(*ports), dataLine[5], *txScale, *power);
}
