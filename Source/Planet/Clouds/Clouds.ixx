module;
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <filesystem>
#include <thread>
#include <atomic>
export module Sandcore.Planet.Clouds;

import Sandcore.Graphics.TextureCubemap;

import Sandcore.Planet.Display.Clouds;
import Sandcore.Timer;

import Sandcore.Image;
import Sandcore.Image.Gradient;

export namespace Sandcore {
	class Clouds {
	public:
		Clouds() : clouds(length) {
			texture.resize(length);
			texture.setParameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		}
		
		~Clouds() {
			if (thread.joinable()) thread.join();
		}

		void tick() {
			if (inProcess) return;
			if (generated) {
				for (int z = 0; z < 6; ++z) {
					texture.loadFromImage(cubemap[z], z);
				}
				if (thread.joinable()) thread.join();
			}

			inProcess = true;
			thread = std::thread([this] {
				clouds.time += timer.getElapsedTime() * 0.07;
				timer.restart();

				generate();
				generated = true;
				inProcess = false;
			});
		}

		void generate() {
			clouds.generate();
			for (int z = 0; z < 6; ++z) {
				for (int y = 0; y < length; ++y) {
					for (int x = 0; x < length; ++x) {
						if (clouds(x, y, z) > 0) {
							cubemap[z](x, y) = gradient(Image::Pixel(255, 255, 255, 0), Image::Pixel(200, 200, 200, 255), 5 * clouds(x, y, z)); //Image::Pixel(195, 195, 195, transparency);
						} else {
							cubemap[z](x, y) = Image::Pixel(255, 255, 255, 0);
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
		TextureCubemap texture;
		DisplayClouds clouds;
		std::thread thread;

		std::atomic<bool> inProcess;
		std::atomic<bool> generated;
	};
}