module;
#include <FastNoise.h>
#include <random>
export module Sandcore.Planet.Display.Desert;

import Sandcore.Planet.Display;
import Sandcore.Planet.Display.Precipitation;
import Sandcore.Planet.Display.Temperature;

export namespace Sandcore {
	class DisplayDesert : public Display {
	public:
		DisplayDesert(std::size_t length, DisplayPrecipitation& precipitation, DisplayTemperature& temperature) : Display(length), precipitation(precipitation), temperature(temperature) {
			std::mt19937 random;
			random.seed(std::random_device()());
			std::uniform_int_distribution<int> dist(0);

			noiseProbability.SetSeed(dist(random));
			noiseProbability.SetNoiseType(FastNoise::NoiseType::SimplexFractal);
			noiseProbability.SetFractalType(FastNoise::FractalType::FBM);
			noiseProbability.SetFractalOctaves(7);
			noiseProbability.SetFrequency(1.75f);
			noiseProbability.SetFractalGain(0.5f);
		}

	protected:
		virtual void create(int x, int y, int z) {
			double u = ((double)x / length) * 2 - 1;
			double v = ((double)y / length) * 2 - 1;
			auto cart = cubeToCart(u, v, z);
			double r = std::sqrt(cart.x * cart.x + cart.y * cart.y + cart.z * cart.z);

			float probability = -precipitation(x, y, z) * 2 * (0.5 + noiseProbability.GetNoise(cart.x / r, cart.y / r, cart.z / r));//  - length  * spread;

			if (temperature(x, y, z) > 0) {
				if (precipitation(x, y, z) < 0) {
					if (probability > 0) {
						if (probability > 1) probability = 1;
						operator()(x, y, z) = probability;
					}
				}
			}
		}

		FastNoise noiseProbability;
		DisplayPrecipitation& precipitation;
		DisplayTemperature& temperature;
	};
}