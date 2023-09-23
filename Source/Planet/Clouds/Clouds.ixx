module;
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
export module Sandcore.Planet.Clouds;

import std;

import Sandcore.Graphics.Texture.D2.Cubemap;

import Sandcore.Planet.Display.Clouds;
import Sandcore.System.Timer;

import Sandcore.Graphics.Image;
import Sandcore.Graphics.Image.Gradient;

export namespace Sandcore {
	class Clouds {
	public:
		Clouds() : clouds(length) {
			texture.create(length);
			texture.setParameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			run = true;
			generated = false;

			thread = std::thread([this] {
				while (run) {
					while (generated); if (!run) [[unlikely]] break;
					clouds.time += timer.getElapsedTime() * 0.07;
					timer.restart();

					generate();
					generated = true;
				}
			});
		}
		
		~Clouds() {
			run = false;
			if (thread.joinable()) thread.join();
		}

		void tick() {
			if (generated) {
				for (int z = 0; z < 6; ++z) {
					texture.load(cubemap[z], z);
				}
				
				generated = false;
			}
		}

		void generate() {
			clouds.generate();
			for (int z = 0; z < 6; ++z) {
				for (int y = 0; y < length; ++y) {
					for (int x = 0; x < length; ++x) {
						if (clouds(x, y, z) > 0) {
							cubemap[z].at(x, y) = gradient(Pixel(255, 255, 255, 0), Pixel(200, 200, 200, 255), 5 * clouds(x, y, z)); //Image::Pixel(195, 195, 195, transparency);
						} else {
							cubemap[z].at(x, y) = Pixel(255, 255, 255, 0);
						}
					}
				}
			}
		}

		auto& getTexture() {
			return texture;
		}

	protected:
		std::size_t length = 196;
		Image cubemap[6] = {
			Image(length, length),
			Image(length, length),
			Image(length, length),
			Image(length, length),
			Image(length, length),
			Image(length, length),
		};
		Timer timer;
		Texture2DCubemap texture;
		DisplayClouds clouds;
		std::thread thread;

		std::atomic<bool> run;
		std::atomic<bool> generated;
	};
}