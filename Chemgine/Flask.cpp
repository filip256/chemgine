#include "Flask.hpp"
#include "DataStore.hpp"

Flask::Flask(const LabwareId id) noexcept :
	DrawableComponent(id)
{}

const FlaskData& Flask::getData() const
{
	return static_cast<const FlaskData&>(data);
}

Flask* Flask::clone() const
{
	return new Flask(this->data.id);
}