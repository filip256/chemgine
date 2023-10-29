#include "LabwareDataFactory.hpp"
#include "DataHelpers.hpp"

BaseLabwareData* LabwareDataFactory::get(const LabwareIdType id, const LabwareType type, const std::vector<std::string>& dataLine)
{
	switch (type)
	{
	case LabwareType::FLASK:
		return LabwareDataFactory::getFlask(id, dataLine);
	default:
		return nullptr;
	}
}


FlaskData* LabwareDataFactory::getFlask(const LabwareIdType id,  const std::vector<std::string>& dataLine)
{
	return new FlaskData(id, dataLine[2], DataHelpers::toUDouble(dataLine[3]).result);
}