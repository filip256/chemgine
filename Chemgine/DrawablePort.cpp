#include "DrawablePort.hpp"

#include <cmath>

DrawablePort::DrawablePort(
	const LabwarePort& port,
	const sf::Vector2f& origin,
	const float scale
) noexcept :
	type(port.type),
	position((port.x - origin.x) * scale, (port.y - origin.y) * scale),
	angle(port.angle)
{}

void DrawablePort::rotate(const Amount<Unit::DEGREE> angle)
{
	const auto tempX = position.x;
	const  Amount<Unit::RADIAN> deltaRad = angle - rotationAngle;

	position.x = position.x * std::cosf(deltaRad.asStd()) - position.y * std::sinf(deltaRad.asStd());
	position.y = tempX * std::sinf(deltaRad.asStd()) + position.y * std::cosf(deltaRad.asStd());

	rotationAngle = angle;
}
