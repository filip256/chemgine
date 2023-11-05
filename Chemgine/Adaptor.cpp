#include "Adaptor.hpp"

Adaptor::Adaptor(const LabwareIdType id) noexcept :
	DrawableComponent(id)
{}

const AdaptorData& Adaptor::getData() const
{
	return static_cast<const AdaptorData&>(data);
}

Adaptor* Adaptor::clone() const
{
	return new Adaptor(*this);
}