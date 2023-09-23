module;
export module Sandcore.Planet.Display.Elevation;

import std;
import Sandcore.Planet.Display;

import FastNoise;

export namespace Sphere {
	class DisplayElevation : public Display {
	public:
		DisplayElevation(std::size_t length) : Display(length) {
			std::mt19937 random;
			random.seed(std::random_device()());
			std::uniform_int_distribution<int> dist(0);

			noiseContinental.SetSeed(dist(random));
			noiseMountain.SetSeed(dist(random));

			noiseContinental.SetNoiseType(FastNoise::NoiseType::SimplexFractal);
			noiseMountain.SetNoiseType(FastNoise::NoiseType::SimplexFractal);
			
			noiseContinental.SetFractalType(FastNoise::FractalType::FBM);
			noiseMountain.SetFractalType(FastNoise::FractalType::FBM);

			noiseContinental.SetFractalOctaves(13);
			noiseMountain.SetFractalOctaves(7);

			noiseContinental.SetFrequency(0.7f);
			noiseMountain.SetFrequency(1.35f);
		}


	protected:
		virtual void create(int x, int y, int z) {
			double u = ((double)x / length) * 2 - 1;
			double v = ((double)y / length) * 2 - 1;
			auto cart = cubeToCart(u, v, z);

			double r = std::sqrt(cart.x * cart.x + cart.y * cart.y + cart.z * cart.z);
			double theta = std::atan2(cart.y, cart.x);
			double phi = std::acos(cart.z / r);

			float continent = noiseContinental.GetNoise(cart.x / r, cart.y / r, cart.z / r);
			//float mountain = noiseMountain.GetNoise(cart.x / r, cart.y / r, cart.z / r);


			//if ((mountain < 0) || (continent < 0)) mountain = 0; else mountain *= 1000;


			operator()(x, y, z) = continent;// +mountain;
		}
	public:
		FastNoise noiseContinental;
		FastNoise noiseMountain;
	};
}