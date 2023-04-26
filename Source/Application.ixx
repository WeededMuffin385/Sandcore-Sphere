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


#include <iostream>
export module Sandcore.Application;

import Sandcore.Window;
import Sandcore.Event;

import Sandcore.Graphics.TextureCubemap;
import Sandcore.Graphics.Program;
import Sandcore.Graphics.Vertex;
import Sandcore.Graphics.Debug;
import Sandcore.Graphics.Mesh;

import Sandcore.Clock;
import Sandcore.Camera;

import Sandcore.Geometry.Clear;
import Sandcore.Planet;

import Sandcore.Planet.Clouds;
import Sandcore.Print;


import Sandcore.Planet.Display.Elevation;

namespace Sandcore {
	auto rot(glm::mat4 mat, float angle, glm::vec3 vec) {
		return glm::rotate(mat, angle, vec);
	}

	auto len(glm::vec3 vec) {
		return glm::length(vec);
	}
}

export namespace Sandcore {
	class Application {
	public:
		Application() : window(width, height, "Sandcore Sphere"),
			programBorder(std::filesystem::current_path() / "Userdata/Shaders/ShaderObject") ,
			programPlanet(std::filesystem::current_path() / "Userdata/Shaders/ShaderPlanet") ,
			programClouds(std::filesystem::current_path() / "Userdata/Shaders/ShaderClouds") ,
			programPlanetCloudless(std::filesystem::current_path() / "Userdata/Shaders/ShaderPlanetCloudless"){

			debugInit();
			glEnable(GL_CULL_FACE);

			glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glEnable(GL_BLEND);

			generateCloudsMesh();
			clouds.getTexture().bind(1);
			camera.setSpeed(0.2f);
			generatePlanet();

		}

		~Application() {
			if (thread.joinable()) thread.detach();
		}

		void loop() {
			while (window.isOpen()) {
				tick();
			}
		}


	private:
		void tick() {
			input();

			while (window.pollEvent(event)) {
				events();
			}

			update();
			draw();
		}

		void draw() {
			window.clear(50.0 / 256, 16.0 / 256, 81.0 / 256, 1);

			if (len(camera.getPosition()) < 15) glEnable(GL_DEPTH_TEST); else glDisable(GL_DEPTH_TEST);

			if (cloud) {
				glFrontFace(GL_CW);
				window.draw(cloudsMesh, programClouds, clouds.getTexture());
				glFrontFace(GL_CCW);
				window.draw(planetMesh, programPlanet, texture);
				window.draw(cloudsMesh, programClouds, clouds.getTexture());
			} else {
				glFrontFace(GL_CCW);
				window.draw(planetMesh, programPlanetCloudless, texture);
			}

			if (border) {
				glDisable(GL_DEPTH_TEST);
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
				window.draw(cloudsMesh, programBorder, texture);
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
				glEnable(GL_DEPTH_TEST);
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
			if (event.type == Event::Type::Scroll) {
				if (event.scroll.y > 0) {
					speed *= 1.25;
				}

				if (event.scroll.y < 0) {
					speed /= 1.25;
				}
				camera.setSpeed(speed);

			}

			if (event.type == Event::Type::Key) {
				if (event.key.action == GLFW_PRESS) {
					if (event.key.key == GLFW_KEY_I) {
						if (!control) window.toggleCursor(Sandcore::Window::Disabled); else window.toggleCursor(Sandcore::Window::Normal);
						control = !control;
						camera.setFirst();
					}

					if (event.key.key == GLFW_KEY_G) {
						generatePlanet();
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
								display = Planet::DisplayType((int)display - 1);
								if ((int)display < 0) display = Planet::DisplayType::Everything;

								inProcess = true;
								thread = std::thread([this] {generateDisplay(); });
							}
						}

						if (event.key.key == GLFW_KEY_PERIOD) { // right
							if (planet.get() == nullptr) {
								std::print("Planet doesn't exists\n");
							} else {
								display = Planet::DisplayType((int)display + 1);
								if ((int)display > (int)Planet::DisplayType::Everything) display = Planet::DisplayType::Elevation;

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
				generatePlanetMesh();
				generated = false;
				inProcess = false;
			}

			width = window.getSize().x;
			height = window.getSize().y;
			window.setViewport(width, height);

			auto model = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0));
			model = rot(model, angle, glm::vec3(0, 0, 1));

			clock.restart();
			if (rotate) {
				angle += clock.getElapsedTime() * 0.15;
			}

			auto view = camera.getViewMatrix();
			auto proj = camera.getProjMatrix(width, height);

			programBorder.setMat4("model", model);
			programBorder.setMat4("view", view);
			programBorder.setMat4("proj", proj);

			programPlanet.setMat4("model", model);
			programPlanet.setMat4("view", view);
			programPlanet.setMat4("proj", proj);

			programPlanetCloudless.setMat4("model", model);
			programPlanetCloudless.setMat4("view", view);
			programPlanetCloudless.setMat4("proj", proj);

			model = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0));
			model = glm::scale(model, glm::vec3(1.05f, 1.05f, 1.05f));
			model = rot(model, angle * 0.5, glm::vec3(0, 0, 1));
			programPlanet.setMat4("shadowModel", model);

			programClouds.setMat4("model", model);
			programClouds.setMat4("view", view);
			programClouds.setMat4("proj", proj);
		}

		void generateCloudsMesh() {
			std::vector<glm::vec3> vertices;
			std::vector<Triangle> triangles;
			icosphere(vertices, triangles, 6);

			cloudsMesh.clear();

			for (auto& vertex : vertices) cloudsMesh.vertices.push_back(vertex);
			for (auto& triangle : triangles) {
				cloudsMesh.indices.push_back(triangle.a);
				cloudsMesh.indices.push_back(triangle.b);
				cloudsMesh.indices.push_back(triangle.c);
			}
			cloudsMesh.update();
		}

		void generatePlanetMesh() {
			std::vector<glm::vec3> vertices;
			std::vector<Triangle> triangles;
			icosphere(vertices, triangles, 6);

			planetMesh.clear();

			if (planet) {
				for (auto& vertex : vertices) {
					float continent = planet->elevation.noiseContinental.GetNoise(vertex.x, vertex.y, vertex.z);
					//float mountain = planet->elevation.noiseMountain.GetNoise(vertex.x, vertex.y, vertex.z);
					//if ((mountain < 0) || (continent < 0)) mountain = 0; else mountain *= 0.4;

					float h = continent;// +mountain;
					if (h > 0) h = 1 + h * 0.065; else h = 1;
					vertex *= h;
				}
			}

			for (auto& vertex : vertices) planetMesh.vertices.push_back(vertex);
			for (auto& triangle : triangles) {
				planetMesh.indices.push_back(triangle.a);
				planetMesh.indices.push_back(triangle.b);
				planetMesh.indices.push_back(triangle.c);
			}
			planetMesh.update();
		}

		void generatePlanet() {
			if (inProcess) return;

			inProcess = true;
			thread = std::thread([this] {
				planet = std::unique_ptr<Planet>(new Planet(size));
				planet->save(display);
				generated = true;
			});
		}

		void generateDisplay() {
			planet->save(display);
			texture.loadFromFile(std::filesystem::current_path() / "Userdata/Planet");
			generated = true;
		}

		int width = 800;
		int height = 800;
		std::size_t size = 512;

		Window window;
		Event event;

		Mesh<Vertex<glm::vec3>> planetMesh;
		Mesh<Vertex<glm::vec3>> cloudsMesh;
		Program programBorder;
		Program programClouds;
		Program programPlanet;
		Program programPlanetCloudless;

		TextureCubemap texture;

		Clouds clouds;
		std::unique_ptr<Planet> planet;
		std::atomic<bool> generated = false;
		std::atomic<bool> inProcess = false;
		std::thread thread;
		Planet::DisplayType display = Planet::DisplayType::Everything;
		

		double speed = 1;
		Clock clock;
		Camera camera;

		bool control = false;
		bool rotate = false;
		bool border = false;
		bool cloud = true;
		float angle = 0;
	};
}