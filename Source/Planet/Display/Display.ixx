export module Sandcore.Planet.Display;

import std;
import glm;

import Sandcore.Graphics.Image;
import Sandcore.Array2D;

export namespace Sphere {
	class Display {
	public:
		enum CubeFace { X_POS, X_NEG, Y_POS, Y_NEG, Z_POS, Z_NEG, SIZE };

		Display(std::size_t length) : length(length) {
			for (auto& face : cubemap) {
				face = Array2D(length, length, 0.f);
			}
		}

		virtual void generate() final {
			for (int z = 0; z < SIZE; ++z) {
				// std::print("{}%\n", (z) * 100 / SIZE);
				for (int y = 0; y < length; ++y) {
					for (int x = 0; x < length; ++x) {

						create(x, y, z);
					}
				}
			}
		}

		void showExtremum() {
			float max = operator()(0, 0, 0);
			float min = operator()(0, 0, 0);

			for (int z = 0; z < 6; ++z) {
				for (int y = 0; y < length; ++y) {
					for (int x = 0; x < length; ++x) {
						if (max < operator()(x, y, z)) {
							max = operator()(x, y, z);
						}

						if (min > operator()(x, y, z)) {
							min = operator()(x, y, z);
						}
					}
				}
			}

			std::print("name: {} | max t: {} | min t: {}\n", typeid(*this).name(), max, min);
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


			showExtremum();
		}

	
		float& operator()(std::size_t x, std::size_t y, std::size_t z) {
			return cubemap[z](x, y);
		}

	protected:
		void saveFace(std::filesystem::path path, CubeFace face, float min, float delta) {
			Sandcore::Graphics::Image image(length, length);

			for (int j = 0; j < length; ++j) {
				for (int i = 0; i < length; ++i) {
					float l = cubemap[face](i, j) - min;

					unsigned char maxColor = (255.f / delta) * l;
					unsigned char minColor = (255.f / delta) * (delta - l);

					image.at(i, j) = { 0,maxColor,minColor,255 };
				}
			}

			image.save(path);
		}

		glm::f64vec3 cubeToCart(double x, double y, int face) {
			switch (face) {
			case X_POS:
				return { +1.0, -y, -x };
			case X_NEG:
				return { -1.0, -y, x };
			case Y_POS:
				return { x, +1.0, y };
			case Y_NEG:
				return { x, -1.0, -y };
			case Z_POS:
				return { x, -y, +1.0 };
			case Z_NEG:
				return { -x, -y, -1.0 };
			}

			throw std::exception("wrong face!");
		}

		virtual void create(int x, int y, int z) = 0;

		std::size_t length;
	private:
		Array2D<float> cubemap[SIZE];
	};
}