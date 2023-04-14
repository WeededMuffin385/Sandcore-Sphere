module;
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <filesystem>
#include <fstream>
#include <cstdlib>

#include <memory>
#include <thread>
#include <atomic>
export module Sandcore.Application;

import Sandcore.Window;
import Sandcore.Event;

import Sandcore.Graphics.TextureCubemap;
import Sandcore.Graphics.Program;
import Sandcore.Graphics.Mesh;
import Sandcore.Vertex;

import Sandcore.Clock;
import Sandcore.Camera;

import Sandcore.Geometry.Clear;
import Sandcore.Planet;

import Sandcore.Planet.Clouds;

import Sandcore.Print;

import Sandcore.Graphics.Debug;

namespace Sandcore {
	auto foo(glm::mat4 mat, float angle, glm::vec3 vec) {
		return glm::rotate(mat, angle, vec);
	}
}

export namespace Sandcore {
	class Application {
	public:
		Application() : window(width, height, "Sandcore Sphere"),
			programBorder("C:/Users/Mi/Documents/GitHub/Sandcore-Sphere/Userdata/Shaders/ShaderObject") ,
			programPlanet("C:/Users/Mi/Documents/GitHub/Sandcore-Sphere/Userdata/Shaders/ShaderPlanet") ,
			programClouds("C:/Users/Mi/Documents/GitHub/Sandcore-Sphere/Userdata/Shaders/ShaderClouds") {

			debugInit();
			glEnable(GL_CULL_FACE);

			glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glEnable(GL_BLEND);

			generateSphereMesh();
			camera.setSpeed(1);
		}

		~Application() {
			if (thread.joinable()) thread.detach();
		}

		void loop() {
			while (window.isOpen()) {
				input();

				while (window.pollEvent(event)) {
					events();
				}

				update();
				draw();
			}
		}

	private:
		void draw() {
			window.clear(50.0 / 256, 16.0 / 256, 81.0 / 256, 1);
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

			glEnable(GL_DEPTH_TEST);
			window.draw(mesh, programPlanet, texture);
			if (cloud) {
				window.draw(mesh, programClouds, clouds.getTexture());
				glFrontFace(GL_CW);
				window.draw(mesh, programClouds, clouds.getTexture());
				glFrontFace(GL_CCW);
			}
			glDisable(GL_DEPTH_TEST);

			if (border) {
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
				window.draw(mesh, programBorder, texture);
			}
			window.display();
		}

		void input() {
			if (control) {
				camera.mouseInput(window);
				camera.keyboardInput(window);
			}
		}

		void events() {
			if (event.type == Event::Type::Key) {
				if (event.key.action == GLFW_PRESS) {
					if (event.key.key == GLFW_KEY_I) {
						if (!control) window.toggleCursor(Sandcore::Window::Disabled); else window.toggleCursor(Sandcore::Window::Normal);
						control = !control;
						camera.setFirst();
					}

					if (event.key.key == GLFW_KEY_G) {
						if (!inProcess) {
							inProcess = true;
							thread = std::thread([this] {generatePlanet(); });
						}
					}

					if (event.key.key == GLFW_KEY_R) {
						rotate = !rotate;
					}


					if (event.key.key == GLFW_KEY_T) {
						border = !border;
					}

					if (event.key.key == GLFW_KEY_C) {
						cloud = !cloud;
					}

					if (!inProcess) {
						if (event.key.key == GLFW_KEY_COMMA) { // left
							if (planet.get() == nullptr) {
								std::print("Planet doesn't exists\n");
							} else {
								display = Planet::Type((int)display - 1);
								if ((int)display < 0) display = Planet::Type::Everything;

								inProcess = true;
								thread = std::thread([this] {generateDisplay(); });
							}
						}

						if (event.key.key == GLFW_KEY_PERIOD) { // right
							if (planet.get() == nullptr) {
								std::print("Planet doesn't exists\n");
							} else {
								display = Planet::Type((int)display + 1);
								if ((int)display > (int)Planet::Type::Everything) display = Planet::Type::Elevation;

								inProcess = true;
								thread = std::thread([this] {generateDisplay(); });
							}
						}
					}
				}
			}
		}

		void update() {
			clouds.tick();

			if (generated) {
				if (thread.joinable()) thread.join();
				texture.loadFromFile(std::filesystem::current_path() / "Userdata/Planet");
				elevation.loadFromFile(std::filesystem::current_path() / "Userdata/Elevation");
				elevation.bind(1);
				generated = false;
				inProcess = false;
			}

			width = window.getSize().x;
			height = window.getSize().y;
			window.setViewport(width, height);

			auto model = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0));
			model = glm::scale(model, glm::vec3(5.f, 5.f, 5.f));
			model = foo(model, angle, glm::vec3(0, 0, 1));

			clock.restart();
			if (rotate) {
				angle += clock.getElapsedTime() * 0.3;
			}

			auto view = camera.getViewMatrix();
			auto proj = camera.getProjMatrix(width, height);

			programBorder.setMat4("model", model);
			programBorder.setMat4("view", view);
			programBorder.setMat4("proj", proj);

			programPlanet.setMat4("model", model);
			programPlanet.setMat4("view", view);
			programPlanet.setMat4("proj", proj);

			model = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0));
			model = glm::scale(model, glm::vec3(1.05f, 1.05f, 1.05f));
			model = glm::scale(model, glm::vec3(5.f, 5.f, 5.f));
			model = foo(model, angle * 0.5, glm::vec3(0, 0, 1));

			programClouds.setMat4("model", model);
			programClouds.setMat4("view", view);
			programClouds.setMat4("proj", proj);
		}

		void generateSphereMesh() {
			std::vector<glm::vec3> vertices;
			std::vector<Triangle> triangles;
			icosphere(vertices, triangles, 6);

			for (auto& vertex : vertices) mesh.vertices.push_back(vertex);
			for (auto& triangle : triangles) {
				mesh.indices.push_back(triangle.a);
				mesh.indices.push_back(triangle.b);
				mesh.indices.push_back(triangle.c);
			}
			mesh.update();
		}

		void generatePlanet() {
			planet = std::unique_ptr<Planet>(new Planet(size));
			planet->save(display);
			generated = true;
		}

		void generateDisplay() {
			planet->save(display);
			texture.loadFromFile(std::filesystem::current_path() / "Userdata/Planet");
			std::print("{}\n", (int)display);
			generated = true;
		}

		int width = 700;
		int height = 700;
		std::size_t size = 512;

		Window window;
		Event event;

		Mesh<Vertex<glm::vec3>> mesh;
		Program programBorder;
		Program programPlanet;
		Program programClouds;

		TextureCubemap texture;
		TextureCubemap elevation;

		std::unique_ptr<Planet> planet;
		std::atomic<bool> generated = false;
		std::atomic<bool> inProcess = false;
		std::thread thread;
		Planet::Type display = Planet::Type::Elevation;
		
		Clouds clouds;


		Clock clock;
		Camera camera;

		bool control = false;
		bool rotate = false;
		bool border = true;
		bool cloud = false;
		float angle = 0;
	};
}