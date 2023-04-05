module;
#include <array>
#include <limits>
#include <fstream>
#include <filesystem>
export module Sandcore.Planet.Display;

import Sandcore.Vector3D;
import Sandcore.Array2D;
import Sandcore.Image;

import Sandcore.Print;

export namespace Sandcore {
	class Display {
	protected:
		enum cube_faces { X_POS, X_NEG, Y_POS, Y_NEG, Z_POS, Z_NEG, SIZE };
		Vector3D<double> cubeToCart(double x, double y, cube_faces face) {
			switch (face) {
			case X_POS:
				return Vector3D<double>{ +1.0, -y, -x };
			case X_NEG:
				return Vector3D<double>{ -1.0, -y, x };
			case Y_POS:
				return Vector3D<double>{ x, +1.0, y };
			case Y_NEG:
				return Vector3D<double>{ x, -1.0, -y };
			case Z_POS:
				return Vector3D<double>{ x, -y, +1.0 };
			case Z_NEG:
				return Vector3D<double>{ -x, -y, -1.0 };
			}

			throw std::exception("wrong face!");
		}

		std::size_t length;
		Array2D<float> cubemap[SIZE];
	public:

		Display(std::size_t length) : length(length) {
			for (auto& face : cubemap) {
				face.create(length, length, 0);
			}
		}

		bool generated = false;

		virtual void generate() {
			generated = true;
		}

		void save(std::filesystem::path path) {
			float min = cubemap->operator()(0, 0);
			float max = cubemap->operator()(0, 0);

			for (auto& face : cubemap) {
				for (int j = 0; j < length; ++j) {
					for (int i = 0; i < length; ++i) {
						if (face(i, j) > max) max = face(i, j);
						if (face(i, j) < min) min = face(i, j);
					}
				}
			}

			float delta = max - min;

			saveFace(path / "posx.png", X_POS, min, delta);
			saveFace(path / "negx.png", X_NEG, min, delta);

			saveFace(path / "posy.png", Y_POS, min, delta);
			saveFace(path / "negy.png", Y_NEG, min, delta);

			saveFace(path / "posz.png", Z_POS, min, delta);
			saveFace(path / "negz.png", Z_NEG, min, delta);
		}

		void saveFace(std::filesystem::path path, cube_faces face, float min, float delta) {
			Image image(length, length);

			for (int j = 0; j < length; ++j) {
				for (int i = 0; i < length; ++i) {
					float l = cubemap[face](i, j) - min;

					unsigned char maxColor = (255.f / delta) * l;
					unsigned char minColor = (255.f / delta) * (delta - l);

					image(i, j) = { 0,maxColor,minColor,255 };
				}
			}

			image.save(path);
		}
	
		auto& operator[](std::size_t face) {
			return cubemap[face];
		}
	};
}