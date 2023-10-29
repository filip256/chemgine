#include "FlaskData.hpp"

FlaskData::FlaskData(const LabwareIdType id, const std::string& name, const double volume) noexcept :
	BaseLabwareData(id, LabwareType::FLASK, name),
	volume(volume)
{}