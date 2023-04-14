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

namespace Sandcore {
	Image::Pixel gradient(Image::Pixel a, Image::Pixel b, float c) {
		float sr = a.r + (b.r - a.r) * c;
		float sg = a.r + (b.g - a.g) * c;
		float sb = a.r + (b.b - a.b) * c;
		float sa = a.r + (b.a - a.a) * c;

		if (sr > 255) sr = 255;
		if (sg > 255) sg = 255;
		if (sb > 255) sb = 255;
		if (sa > 255) sa = 255;

		return Image::Pixel(sr, sg, sb, sa);
	}
}

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
						if (clouds(x, y, z) > -0.25) {
							//cubemap[z](x, y) = Image::Pixel(255, 0, 0, 255);
							cubemap[z](x, y) = gradient(Image::Pixel(0, 0, 0, 0), Image::Pixel(255, 255, 255, 255), clouds(x, y, z) + 0.5);
						} else {
							cubemap[z](x, y) = Image::Pixel(0, 0, 0, 0);
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