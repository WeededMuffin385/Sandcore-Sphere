module;
export module Sandcore.Planet.Display.Temperature;

import std;

import Sandcore.Planet.Display;
import Sandcore.Planet.Display.Elevation;
import Sandcore.Planet.Display.Precipitation;

export namespace Sphere {
	class DisplayTemperature : public Display {
	public:
		DisplayTemperature(std::size_t length, DisplayElevation& elevation, DisplayPrecipitation& precipitation) : Display(length), elevation(elevation), precipitation(precipitation) {
		}

	protected:
		virtual void create(int x, int y, int z) {
			double u = ((double)x / length) * 2 - 1;
			double v = ((double)y / length) * 2 - 1;
			auto cart = cubeToCart(u, v, z);

			double r = std::sqrt(cart.x * cart.x + cart.y * cart.y + cart.z * cart.z);
			double theta = std::atan2(cart.y, cart.x);
			double phi = std::acos(cart.z / r);

			float t = std::sin(phi) * 2 - 1;
			float h = elevation(x, y, z);
			if (h > 0) t -= h * 0.5;
			if (h < 0) t /= 3;

			operator()(x, y, z) = t;
		}

		DisplayElevation& elevation;
		DisplayPrecipitation& precipitation;
	};
}