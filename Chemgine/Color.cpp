#include "Color.hpp"

Color::Color(const uint8_t red, const uint8_t green,
	const uint8_t blue, const uint8_t alpha
) noexcept :
	r(red),
	g(green),
	b(blue),
	a(alpha)
{}
