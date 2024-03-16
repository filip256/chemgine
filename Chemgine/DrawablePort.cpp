#include "DrawablePort.hpp"
#include "Maths.hpp"

#include <cmath>

DrawablePort::DrawablePort(const LabwarePort& port, const sf::Vector2f& origin) noexcept :
	type(port.type),
	position(port.x - origin.x, port.y - origin.y),
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
