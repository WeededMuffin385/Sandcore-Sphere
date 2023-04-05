module;
#include <stdexcept>
#include <numbers>
#include <cmath>
export module Sandcore.Planet.Display.Precipitation;

import Sandcore.Planet.Display;
import Sandcore.Vector3D;

export namespace Sandcore {
	class DisplayPrecipitation : public Display {
	private:
		int cells = 6;
	public:
		DisplayPrecipitation(int length) : Display(length) {

		}

		virtual void generate() {
			for (int n = 0; n < 6; ++n) {
				for (int j = 0; j < length; ++j) {
					for (int i = 0; i < length; ++i) {
						double U = ((double)i / length) * 2 - 1;
						double V = ((double)j / length) * 2 - 1;
						auto cart = cubeToCart(U, V, (cube_faces)n);

						double r = std::sqrt(cart.x * cart.x + cart.y * cart.y + cart.z * cart.z);
						double theta = std::atan2(cart.y, cart.x);
						double phi = std::acos(cart.z / r);

						cubemap[n](i, j) = std::cos((phi - std::numbers::pi * 0.5) * cells);

						double latitude = phi / std::numbers::pi;
						if (latitude <= 0.25 || latitude >= 0.75) if (cubemap[n](i, j) > 0) cubemap[n](i, j) *= 0.5;
					}
				}
			}
		}
	};
}