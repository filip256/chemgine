#include "DrawablePort.hpp"
#include "Maths.hpp"

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

void DrawablePort::rotate(const float angle)
{
	const auto tempX = position.x;
	const auto deltaRad = Maths::toRadians(angle - rotationAngle);

	position.x = position.x * std::cosf(deltaRad) - position.y * std::sinf(deltaRad);
	position.y = tempX * std::sinf(deltaRad) + position.y * std::cosf(deltaRad);

	rotationAngle = angle;
}
