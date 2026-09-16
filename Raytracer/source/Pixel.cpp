#include "Pixel.h"

namespace Raytracer {
Pixel::Pixel(const unsigned int x, const unsigned int y) : x(x), y(y), depth(10000.f) {}
} // namespace Raytracer