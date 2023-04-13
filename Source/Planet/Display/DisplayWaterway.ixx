module;
#include <set>
#include <list>
#include <cstdlib>
#include <unordered_set>
export module Sandcore.Planet.Display.Waterway;

import Sandcore.Planet.Display;
import Sandcore.Planet.Display.Elevation;

import Sandcore.Vector2D;
import Sandcore.Vector2D.Hash;

import Sandcore.Image.Drawing;

export namespace Sandcore {
	class DisplayWaterway : public Display {
	public:
		DisplayWaterway(std::size_t length, DisplayElevation& elevation) : Display(length), elevation(elevation) {
		}

		virtual void create(int x, int y, int z) {
		}

	protected:
		DisplayElevation& elevation;

		std::list<Vector2D<int>> sources[SIZE];
		std::list<Vector2D<int>> basins[SIZE];

		int searchBasinsInRadius = 3;
		int searchSourcesInRadius = 50;
	};
}