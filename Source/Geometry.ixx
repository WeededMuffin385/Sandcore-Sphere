module;
#include <glm/glm.hpp>
#include <numbers>
#include <vector>
export module Sandcore.Geometry.Clear;

export namespace Sandcore {
	using Index = int;

	struct Triangle {
		Index a, b, c;
	};

	void project_to_unit_sphere(std::vector<glm::vec3>& vertices) {
		for (auto& vertex : vertices) {
			vertex = vertex / glm::length(vertex);
		}
	}

	void icosahedron(std::vector<glm::vec3>& vertices, std::vector<Triangle>& triangles) {
		vertices.clear();
		triangles.clear();

		float a = 1.0f;
		float b = 1.0f / std::numbers::phi;

		auto addVertex = [&vertices](glm::vec3 vertex) {
			vertices.push_back(vertex);
			return vertices.size() - 1;
		};

		auto addTriangle = [&triangles](Index a, Index b, Index c) {
			triangles.push_back(Triangle(a, b, c));
		};

		auto v1 = addVertex(glm::vec3( 0, b, -a ));
		auto v2 = addVertex(glm::vec3(b, a, 0));
		auto v3 = addVertex(glm::vec3(-b, a, 0));
		auto v4 = addVertex(glm::vec3(0, b, a));
		auto v5 = addVertex(glm::vec3(0, -b, a));
		auto v6 = addVertex(glm::vec3(-a, 0, b));
		auto v7 = addVertex(glm::vec3(0, -b, -a));
		auto v8 = addVertex(glm::vec3(a, 0, -b));
		auto v9 = addVertex(glm::vec3(a, 0, b));
		auto v10 = addVertex(glm::vec3(-a, 0, -b));
		auto v11 = addVertex(glm::vec3(b, -a, 0));
		auto v12 = addVertex(glm::vec3(-b, -a, 0));

		project_to_unit_sphere(vertices);

		addTriangle(v3, v2, v1);
		addTriangle(v2, v3, v4);
		addTriangle(v6, v5, v4);
		addTriangle(v5, v9, v4);
		addTriangle(v8, v7, v1);
		addTriangle(v7, v10, v1);
		addTriangle(v12, v11, v5);
		addTriangle(v11, v12, v7);
		addTriangle(v10, v6, v3);
		addTriangle(v6, v10, v12);
		addTriangle(v9, v8, v2);
		addTriangle(v8, v9, v11);
		addTriangle(v3, v6, v4);
		addTriangle(v9, v2, v4);
		addTriangle(v10, v3, v1);
		addTriangle(v2, v8, v1);
		addTriangle(v12, v10, v7);
		addTriangle(v8, v11, v7);
		addTriangle(v6, v12, v5);
		addTriangle(v11, v9, v5);
	}

	void subdivide(std::vector<glm::vec3>& vertices, std::vector<Triangle>& triangles) {
		std::vector<Triangle> tmp;

		auto addVertex = [&vertices](glm::vec3 vertex) {
			vertices.push_back(vertex);
			return vertices.size() - 1;
		};

		auto addTriangle = [&tmp](Index a, Index b, Index c) {
			tmp.push_back(Triangle(a, b, c));
		};

		auto getMiddlePoint = [&vertices, &addVertex](Index a, Index b) {
			glm::vec3 c = vertices[a] + vertices[b];
			glm::vec3 n = glm::normalize(c);
			return addVertex(n);
		};

		for (auto& triangle : triangles) {
			auto a = getMiddlePoint(triangle.a, triangle.b);
			auto b = getMiddlePoint(triangle.b, triangle.c);
			auto c = getMiddlePoint(triangle.a, triangle.c);

			addTriangle(triangle.a, a, c);
			addTriangle(triangle.b, b, a);
			addTriangle(triangle.c, c, b);
			addTriangle(a, b, c);
		}
		
		triangles = std::move(tmp);
	}

	void icosphere(std::vector<glm::vec3>& vertices, std::vector<Triangle>& triangles, int iterations) {
		icosahedron(vertices, triangles);
		for (int i = 0; i < iterations; ++i) subdivide(vertices, triangles);
	}
}