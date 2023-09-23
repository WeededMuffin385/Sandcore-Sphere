export module Sandcore.Array2D;

import std;

export namespace Sphere {
	template<typename T>
	class Array2D {
	public:
		Array2D() {

		}

		Array2D(int width, int height, T value) : width(width), height(height), data(width* height, value) {

		}

		Array2D(int width, int height) : width(width), height(height), data(width * height) {
		}

		T& operator()(int x, int y) {
			return data[x + y * width];
		}

	private:
		int width = 0;
		int height = 0;

		std::vector<float> data;
	};
}