#include "labware/DrawablePort.hpp"

#include <cmath>

DrawablePort::DrawablePort(
	const LabwarePort& port,
	const sf::Vector2f& origin,
	const float_s scale
) noexcept :
	type(port.type),
	position((port.x - origin.x) * scale, (port.y - origin.y) * scale),
	angle(port.angle)
{}

void DrawablePort::rotate(const Amount<Unit::DEGREE> angle)
{
	const auto tempX = position.x;
	const  Amount<Unit::RADIAN> deltaRad = angle - rotationAngle;

	position.x = position.x * std::cos(deltaRad.asStd()) - position.y * std::sin(deltaRad.asStd());
	position.y = tempX * std::sin(deltaRad.asStd()) + position.y * std::cos(deltaRad.asStd());

	rotationAngle = angle;
}
