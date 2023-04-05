module;
#include <cstdlib>
#include <cmath>
#include <numbers>
export module Sandcore.Planet.Display.Temperature;
import Sandcore.Planet.Display;

import Sandcore.Planet.Display.Elevation;
import Sandcore.Planet.Display.Precipitation;

export namespace Sandcore {
	class DisplayTemperature : public Display {
	public:
		DisplayTemperature(std::size_t length, DisplayElevation& elevation, DisplayPrecipitation& precipitation) : Display(length), elevation(elevation), precipitation(precipitation) {
		}

		virtual void generate() {
			if (!elevation.generated) elevation.generate();
			if (!precipitation.generated) precipitation.generate();

			for (int n = 0; n < SIZE; ++n) {
				for (int j = 0; j < length; ++j) {
					for (int i = 0; i < length; ++i) {
						double U = ((double)i / length) * 2 - 1;
						double V = ((double)j / length) * 2 - 1;
						auto cart = cubeToCart(U, V, (cube_faces)n);

						double r = std::sqrt(cart.x * cart.x + cart.y * cart.y + cart.z * cart.z);
						double theta = std::atan2(cart.y, cart.x);
						double phi = std::acos(cart.z / r);

						float delta = max - min;

						float t = std::sin(phi) * delta + min;
						float h = elevation[n](i, j);

						if (h <= 0) t /= 3;
						if (h > 0) t -= h / 100 * 5.f;

						t *= (1.5 - precipitation[n](i, j));

						cubemap[n](i, j) = t;
					}
				}
			}
		}

	private:
		float min = -20;
		float max = +30;

		DisplayElevation& elevation;
		DisplayPrecipitation& precipitation;
	};
}