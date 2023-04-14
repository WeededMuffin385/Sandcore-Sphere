module;
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <filesystem>
export module Sandcore.Planet.Clouds;

import Sandcore.Graphics.TextureCubemap;

import Sandcore.Planet.Display.Clouds;
import Sandcore.Image;
import Sandcore.Timer;

export namespace Sandcore {
	class Clouds {
	public:
		Clouds() : clouds(length) {
			texture.resize(length);
			texture.setParameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		}
		
		void tick() {
			if (timer.getElapsedTime() > 1.0/24.0) {
				clouds.time += timer.getElapsedTime() * 0.07;
				timer.restart();
				generate();
			}
		}

		void generate() {
			clouds.generate();
			for (int z = 0; z < 6; ++z) {
				for (int y = 0; y < length; ++y) {
					for (int x = 0; x < length; ++x) {
						if (clouds(x, y, z) > 0) {
							float transparency = 256  * 5 * clouds(x, y, z);
							if (transparency > 255) transparency = 255;
							cubemap[z](x, y) = Image::Pixel(255, 255, 255, transparency);
						} else {
							cubemap[z](x, y) = Image::Pixel(255, 255, 255, 0);
						}
					}
				}
			}

			for (int z = 0; z < 6; ++z) {
				texture.loadFromImage(cubemap[z], z);
			}
		}

		auto& getTexture() {
			return texture;
		}

	protected:
		std::size_t length = 128;
		Image cubemap[6] = {
			Image(length, length),
			Image(length, length),
			Image(length, length),
			Image(length, length),
			Image(length, length),
			Image(length, length),
		};
		Timer timer;
		TextureCubemap texture;
		DisplayClouds clouds;
	};
}