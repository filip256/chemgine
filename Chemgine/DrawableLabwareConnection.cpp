#include "DrawableLabwareConnection.hpp"

#include <cmath>

DrawableLabwareConnection::DrawableLabwareConnection(
	const LabwareConnectionData& data,
	const sf::Vector2f& origin,
	const float scale
) noexcept :
	data(data),
	position(data.x, data.y)
{}

void DrawableLabwareConnection::rotate(const Amount<Unit::DEGREE> angle)
{
	const auto tempX = position.x;
	const  Amount<Unit::RADIAN> deltaRad = angle - rotationAngle;

	position.x = position.x * std::cosf(deltaRad.asStd()) - position.y * std::sinf(deltaRad.asStd());
	position.y = tempX * std::sinf(deltaRad.asStd()) + position.y * std::cosf(deltaRad.asStd());

	rotationAngle = angle;
}

PortType DrawableLabwareConnection::getType() const
{
	return data.type;
}

Amount<Unit::DEGREE> DrawableLabwareConnection::getAngle() const
{
	return data.angle;
}

const sf::Vector2f& DrawableLabwareConnection::getPosition() const
{
	return position;
}
