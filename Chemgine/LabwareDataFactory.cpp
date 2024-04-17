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
	case LabwareType::CONDENSER:
		return getCondenserData(id, dataLine);
	default:
		return nullptr;
	}
}


FlaskData* LabwareDataFactory::getFlaskData(const LabwareId id,  const std::vector<std::string>& dataLine)
{
	if (dataLine.size() != 7)
		return nullptr;

	const auto volume = DataHelpers::parseUnsigned<Unit::LITER>(dataLine[3]);
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

	const auto volume = DataHelpers::parseUnsigned<Unit::LITER>(dataLine[3]);
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

CondenserData* LabwareDataFactory::getCondenserData(const LabwareId id, const std::vector<std::string>& dataLine)
{
	if (dataLine.size() != 11)
		return nullptr;

	auto ports = DataHelpers::parseList<LabwarePort>(dataLine[3], ';', true);
	if (ports.has_value() == false)
		return nullptr;

	const auto length = DataHelpers::parseUnsigned<Unit::METER>(dataLine[4]);
	if (length.has_value() == false)
		return nullptr;

	const auto efficiency = DataHelpers::parseUnsigned<Unit::PER_METER>(dataLine[5]);
	if (efficiency.has_value() == false)
		return nullptr;

	const auto volume = DataHelpers::parseUnsigned<Unit::LITER>(dataLine[6]);
	if (volume.has_value() == false)
		return nullptr;

	auto txScale = DataHelpers::parseUnsigned<float>(dataLine[8]);
	if (txScale.has_value() == false)
		return nullptr;

	return new CondenserData(id, dataLine[2], std::move(*ports), *length, *efficiency, *volume, dataLine[7], *txScale, dataLine[9], dataLine[10]);
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
