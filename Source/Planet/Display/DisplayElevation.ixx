module;
#include <FastNoiseLite.h>
#include <numbers>
#include <random>
export module Sandcore.Planet.Display.Elevation;

import Sandcore.Planet.Display;
import Sandcore.Print;

export namespace Sandcore {
	class DisplayElevation : public Display {
	public:
		DisplayElevation(std::size_t length) : Display(length) {
			std::mt19937 random;
			random.seed(std::random_device()());
			std::uniform_int_distribution<int> dist(0);

			noiseContinental.SetSeed(dist(random));
			noiseMountain.SetSeed(dist(random));

			noiseContinental.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
			noiseMountain.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
			
			noiseContinental.SetFractalType(FastNoiseLite::FractalType_FBm);
			noiseMountain.SetFractalType(FastNoiseLite::FractalType_FBm);

			noiseContinental.SetFractalOctaves(13);
			noiseMountain.SetFractalOctaves(7);

			noiseContinental.SetFrequency(0.7f);
			noiseMountain.SetFrequency(0.0005f);
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

						float continent = noiseContinental.GetNoise(cart.x / r, cart.y / r, cart.z / r) * 150;
						float mountain = 0;

						if (continent > 0) mountain = noiseMountain.GetNoise(cart.x / r, cart.y / r, cart.z / r) - 0.5f;
						if (mountain < 0) mountain = 0; else mountain *= 1;

						float elevation = continent +mountain;

						cubemap[n](i, j) = elevation;
					}
				}
			}
		}
	protected:
		FastNoiseLite noiseContinental;
		FastNoiseLite noiseMountain;
	};
}