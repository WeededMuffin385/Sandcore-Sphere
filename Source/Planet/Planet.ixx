module;
#include <cstdlib>
#include <fstream>
#include <numbers>
#include <filesystem>
export module Sandcore.Planet;

import Sandcore.Planet.Display.Precipitation;
import Sandcore.Planet.Display.Elevation;
import Sandcore.Planet.Display.Temperature;
import Sandcore.Planet.Display.Waterway;
import Sandcore.Planet.Display.Desert;
import Sandcore.Planet.Display.Ice;
import Sandcore.Planet.Display;

import Sandcore.Image;
import Sandcore.Image.Gradient;

import Sandcore.Print;

export namespace Sandcore {
	class Planet {
	public:
		Planet(std::size_t length) : length(length), elevation(length), precipitation(length, elevation), temperature(length, elevation, precipitation),
			waterway(length, elevation), desert(length, precipitation, temperature), ice(length, temperature) {
			generate();
		}

		void generate() {
			elevation.generate();
			precipitation.generate();
			temperature.generate();

			waterway.generate();
			desert.generate();
			ice.generate();


			elevation.save(std::filesystem::current_path() / "Userdata/Elevation");
		}

		enum class Type {
			Elevation,
			Ice,
			Precipitation,
			Temperature,
			Desert,
			Everything,
		};

		void save(Type type) {
			std::filesystem::path path = std::filesystem::current_path() / "Userdata/Planet";

			switch (type) {
			case Type::Elevation:
				elevation.save(path);
				elevation.showExtremum();
				break;
			case Type::Ice:
				ice.save(path);
				ice.showExtremum();
				break;
			case Type::Precipitation:
				precipitation.save(path);
				precipitation.showExtremum();
				break;
			case Type::Temperature:
				temperature.save(path);
				temperature.showExtremum();
				break;
			case Type::Desert:
				desert.save(path);
				break;
			case Type::Everything:
				save();
				break;
			}
		}

		Image::Pixel gradient(Image::Pixel a, Image::Pixel b, float c) {
			float sr = a.r * (1 - c) + b.r * c;
			float sg = a.g * (1 - c) + b.g * c;
			float sb = a.b * (1 - c) + b.b * c;
			float sa = a.a * (1 - c) + b.a * c;

			return Image::Pixel(sr, sg, sb, sa);
		}

		void save() {
			Image cubemap[6] = {
				Image(length, length),
				Image(length, length),
				Image(length, length),
				Image(length, length),
				Image(length, length),
				Image(length, length),
			};

			for (int z = 0; z < 6; ++z) {
				for (int y = 0; y < length; ++y) {
					for (int x = 0; x < length; ++x) {
						if (elevation(x, y, z) > 0) cubemap[z](x, y) = gradient(Image::Pixel(0, 255, 100, 255), Image::Pixel(0, 150, 150, 255), elevation(x, y, z));
						if (elevation(x, y, z) <= 0) cubemap[z](x, y) = Image::Pixel(22, 187, 255, 255);

						if (elevation(x, y, z) > 0) {
							if (desert(x, y, z) > 0) {
								cubemap[z](x, y) = gradient(cubemap[z](x, y), Image::Pixel(255, 255, 0, 255), desert(x, y, z));
							}
						}

						if (ice(x, y, z) > 0) {
							if (elevation(x, y, z) > 0) 
								cubemap[z](x, y) = gradient(cubemap[z](x, y), Image::Pixel(255, 255, 255, 255), ice(x, y, z));//Image::Pixel(255 * ice(x, y, z), 255, 255 * ice(x, y, z), 255);
							else 
								cubemap[z](x, y) = gradient(Image::Pixel(22, 187, 255, 255), Image::Pixel(255, 255, 255, 255), ice(x, y, z));
						}
					}
				}
			}

			std::filesystem::path path = std::filesystem::current_path() / "Userdata/Planet";

			auto saveFace = [&cubemap](std::filesystem::path path, int z) {
				cubemap[z].save(path);
			};

			saveFace(path / "posx.png", Display::cube_faces::X_POS);
			saveFace(path / "negx.png", Display::cube_faces::X_NEG);

			saveFace(path / "posy.png", Display::cube_faces::Y_POS);
			saveFace(path / "negy.png", Display::cube_faces::Y_NEG);

			saveFace(path / "posz.png", Display::cube_faces::Z_POS);
			saveFace(path / "negz.png", Display::cube_faces::Z_NEG);
		}

	private:
		DisplayElevation elevation;
		DisplayPrecipitation precipitation;
		DisplayTemperature temperature;
		DisplayWaterway waterway;
		DisplayDesert desert;
		DisplayIce ice;

		std::size_t length;

		friend class Application;
		friend class RenderPlanet;
	};
}