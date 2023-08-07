module;
#include <stdexcept>
#include <numbers>
#include <cmath>
#include <print>
export module Sandcore.Planet.Display.Precipitation;

import Sandcore.Planet.Display;

import Sandcore.Planet.Display.Elevation;

export namespace Sandcore {
	class DisplayPrecipitation : public Display {
	private:
		int cells = 6;
		int radius = 4;
	public:
		DisplayPrecipitation(int length, DisplayElevation& elevation) : Display(length), elevation(elevation) {

		}
	protected:

		virtual void create(int x, int y, int z) {
			double u = ((double)x / length) * 2 - 1;
			double v = ((double)y / length) * 2 - 1;
			auto cart = cubeToCart(u, v, z);

			double r = std::sqrt(cart.x * cart.x + cart.y * cart.y + cart.z * cart.z);
			double theta = std::atan2(cart.y, cart.x);
			double phi = std::acos(cart.z / r);

			operator()(x, y, z) = std::cos((phi - std::numbers::pi * 0.5) * cells);

			double latitude = phi / std::numbers::pi;
			if (latitude <= 0.25 || latitude >= 0.75) if (operator()(x, y, z) > 0) operator()(x, y, z) *= 0.75;
		}

		DisplayElevation& elevation;
	};
}