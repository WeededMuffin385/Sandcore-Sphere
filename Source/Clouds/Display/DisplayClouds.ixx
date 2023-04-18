module;
#include <FastNoise.h>
#include <cmath>
#include <random>
export module Sandcore.Planet.Display.Clouds;

import Sandcore.Planet.Display;

export namespace Sandcore {
	class DisplayClouds : public Display {
	public:
		DisplayClouds(std::size_t length) : Display(length) {
			std::mt19937 random;
			random.seed(std::random_device()());
			std::uniform_int_distribution<int> dist(0);

			noise.SetSeed(dist(random));


			noise.SetNoiseType(FastNoise::NoiseType::SimplexFractal);
			noise.SetFractalType(FastNoise::FractalType::FBM);
			noise.SetFractalOctaves(3);
			noise.SetFrequency(1.0f);
			noise.SetFractalGain(0.5f);
		}

		float time = 0;
	protected:
		virtual void create(int x, int y, int z) {
			double u = ((double)x / length) * 2 - 1;
			double v = ((double)y / length) * 2 - 1;
			auto cart = cubeToCart(u, v, z);

			double r = std::sqrt(cart.x * cart.x + cart.y * cart.y + cart.z * cart.z);

			float continent = noise.GetNoise(cart.x / r, cart.y / r, cart.z / r, time);
			operator()(x, y, z) = continent;
		}

		FastNoise noise;
	};
}