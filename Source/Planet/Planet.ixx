export module Sandcore.Planet;

import std;

import Sandcore.Planet.Display.Precipitation;
import Sandcore.Planet.Display.Elevation;
import Sandcore.Planet.Display.Temperature;
import Sandcore.Planet.Display.Desert;
import Sandcore.Planet.Display.Ice;
import Sandcore.Planet.Display;

import Sandcore.Graphics.Image;
import Sandcore.Graphics.Image.Gradient;

export namespace Sphere {
	class Planet {
	public:
		Planet(std::size_t length) : length(length), elevation(length), precipitation(length, elevation), temperature(length, elevation, precipitation),
			desert(length, precipitation, temperature), ice(length, temperature) {
			generate();
		}

		void generate() {
			elevation.generate();
			precipitation.generate();
			temperature.generate();

			desert.generate();
			ice.generate();

			elevation.save(std::filesystem::current_path() / "Userdata/Planet");
		}

		enum class DisplayType {
			Elevation,
			Ice,
			Precipitation,
			Temperature,
			Desert,
			Everything,
		};

		void save(DisplayType type) {
			std::filesystem::path path = std::filesystem::current_path() / "Userdata/Planet";

			switch (type) {
			using enum DisplayType;
			case Elevation:
				elevation.save(path);
				break;
			case Ice:
				ice.save(path);
				break;
			case Precipitation:
				precipitation.save(path);
				break;
			case Temperature:
				temperature.save(path);
				break;
			case Desert:
				desert.save(path);
				break;
			case Everything:
				std::cout << "name: Everything | Enjoy The View\n";
				save();
				break;
			}
		}

		void save() {
			Sandcore::Image cubemap[6] = {
				Sandcore::Image(length, length),
				Sandcore::Image(length, length),
				Sandcore::Image(length, length),
				Sandcore::Image(length, length),
				Sandcore::Image(length, length),
				Sandcore::Image(length, length),
			};

			for (int z = 0; z < 6; ++z) {
				for (int y = 0; y < length; ++y) {
					for (int x = 0; x < length; ++x) {
						if (elevation(x, y, z) > 0) cubemap[z].at(x, y) = Sandcore::gradient(Sandcore::Pixel(0, 255, 100, 255), Sandcore::Pixel(0, 150, 150, 255), elevation(x, y, z));
						if (elevation(x, y, z) <= 0) cubemap[z].at(x, y) = Sandcore::Pixel(22, 187, 255, 255);

						if (elevation(x, y, z) > 0) {
							if (desert(x, y, z) > 0) {
								cubemap[z].at(x, y) = gradient(cubemap[z].at(x, y), Sandcore::Pixel(255, 255, 0, 255), desert(x, y, z));
							}
						}

						if (ice(x, y, z) > 0) {
							cubemap[z].at(x, y) = gradient(cubemap[z].at(x, y), Sandcore::Pixel(255, 255, 255, 255), ice(x, y, z));
						}
					}
				}
			}

			std::filesystem::path path = std::filesystem::current_path() / "Userdata/Planet";

			auto saveFace = [&cubemap](std::filesystem::path path, int z) {
				cubemap[z].save(path);
			};

			saveFace(path / "posx.png", Display::CubeFace::X_POS);
			saveFace(path / "negx.png", Display::CubeFace::X_NEG);

			saveFace(path / "posy.png", Display::CubeFace::Y_POS);
			saveFace(path / "negy.png", Display::CubeFace::Y_NEG);

			saveFace(path / "posz.png", Display::CubeFace::Z_POS);
			saveFace(path / "negz.png", Display::CubeFace::Z_NEG);
		}

	private:
		DisplayElevation elevation;
		DisplayPrecipitation precipitation;
		DisplayTemperature temperature;
		DisplayDesert desert;
		DisplayIce ice;

		std::size_t length;

		friend class Application;
	};
}