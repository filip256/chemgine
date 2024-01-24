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
	const auto volume = DataHelpers::parseUnsigned<double>(dataLine[3]);
	if (volume.has_value() == false)
		return nullptr;

	const auto portStrings = DataHelpers::parseList(dataLine[4], ';', true);
	std::vector<LabwarePort> ports;
	ports.reserve(portStrings.size());

	for (uint8_t i = 0; i < portStrings.size(); ++i)
	{
		const auto port = DataHelpers::parseList(portStrings[i], ' ', true);

		if (port.size() != 4)
			return nullptr;

		const auto type = DataHelpers::parse<unsigned int>(port[0]);
		const auto x = DataHelpers::parse<unsigned int>(port[1]);
		const auto y = DataHelpers::parse<unsigned int>(port[2]);
		const auto angle = DataHelpers::parseUnsigned<double>(port[3]);

		if (type.has_value() == false || x.has_value() == false || y.has_value() == false || angle.has_value() == false)
			return nullptr;

		ports.emplace_back(static_cast<PortType>(type.value()), x.value(), y.value(), angle.value());
	}

	return new FlaskData(id, dataLine[2], std::move(ports), volume.value(), dataLine[5]);
}

AdaptorData* LabwareDataFactory::getAdaptor(const LabwareIdType id, const std::vector<std::string>& dataLine)
{
	const auto volume = DataHelpers::parseUnsigned<double>(dataLine[3]);
	if (volume.has_value() == false)
		return nullptr;

	const auto portStrings = DataHelpers::parseList(dataLine[4], ';', true);
	std::vector<LabwarePort> ports;
	ports.reserve(portStrings.size());

	for (uint8_t i = 0; i < portStrings.size(); ++i)
	{
		const auto port = DataHelpers::parseList(portStrings[i], ' ', true);

		if (port.size() != 4)
			return nullptr;

		const auto type = DataHelpers::parse<unsigned int>(port[0]);
		const auto x = DataHelpers::parse<unsigned int>(port[1]);
		const auto y = DataHelpers::parse<unsigned int>(port[2]);
		const auto angle = DataHelpers::parseUnsigned<double>(port[3]);

		if (type.has_value() == false || x.has_value() == false || y.has_value() == false || angle.has_value() == false)
			return nullptr;

		ports.emplace_back(static_cast<PortType>(type.value()), x.value(), y.value(), angle.value());
	}

	return new AdaptorData(id, dataLine[2], std::move(ports), volume.value(), dataLine[5]);
}