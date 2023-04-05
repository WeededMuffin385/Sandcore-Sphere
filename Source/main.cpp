#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <filesystem>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <numbers>
#include <atomic>
import Sandcore.Graphics.Program;
import Sandcore.Window;
import Sandcore.Graphics.Mesh;
import Sandcore.Event;
import Sandcore.Graphics.TextureCubemap;
import Sandcore.Vertex;
import Sandcore.Camera;
import Sandcore.Geometry;
import Sandcore.Graphics.Debug;
import Sandcore.Planet.Display.Elevation;
import Sandcore.Planet.Display.Temperature;
import Sandcore.Planet.Display.Precipitation;

import Sandcore.Clock;

using namespace Sandcore;


int main() {
	int width = 700;
	int height = 700;
	std::size_t size = 256;
	Window window(width, height, "Test Window");
	Event event;
	debugInit();
	glEnable(GL_CULL_FACE);

	TextureCubemap texture;
	texture.resize(size);
	texture.loadFromFile(R"(C:\Workbench\Spherification\Userdata\test3)");
	Program screenShader(R"(C:\Users\Mi\Documents\GitHub\Sandcore-Multimedia\Userdata\Shaders\ShaderObject)");
	Program planetShader(R"(C:\Users\Mi\Documents\GitHub\Sandcore-Multimedia\Userdata\Shaders\ShaderPlanet)");

	Mesh<Vertex<glm::vec3>> mesh;
	mesh = icosahedron();
	for (int i = 0; i < 6; ++i) subdivide(mesh);
	mesh.update();

	bool showOutline = true;

	std::atomic<bool> planetGenerated = false;
	bool planetGenerationInProcess = false;
	std::thread thread;
	auto generatePlanet = [&planetGenerated, &size]() {
		DisplayElevation elevation(size);
		DisplayPrecipitation precipitation(size);
		DisplayTemperature temperature(size, elevation, precipitation);
		elevation.generate();
		precipitation.generate();
		temperature.generate();
		temperature.save(std::filesystem::current_path() / "Userdata/Planet");

		planetGenerated = true;
	};

	Clock clock;
	Camera camera;
	camera.setSpeed(5);

	bool control = false;
	bool textureB = false;
	float angle = 0;

	while (window.isOpen()) {
		while (window.pollEvent(event)) {
			if (event.type == Event::Type::Key) {
				if (event.key.action == GLFW_PRESS) {
					if (event.key.key == GLFW_KEY_I) {
						if (!control) window.toggleCursor(Sandcore::Window::Disabled); else window.toggleCursor(Sandcore::Window::Normal);
						control = !control;
						camera.setFirst();
					}

					if (event.key.key == GLFW_KEY_G) {
						if (!planetGenerationInProcess) {
							planetGenerationInProcess = true;
							thread = std::thread(generatePlanet);
						}
					}


					if (event.key.key == GLFW_KEY_T) {
						showOutline = !showOutline;
					}
				}
			}
		}

		if (planetGenerated) {
			if (thread.joinable()) thread.join();
			texture.loadFromFile(std::filesystem::current_path() / "Userdata/Planet");
			planetGenerated = false;
			planetGenerationInProcess = false;
		}

		if (control) {
			camera.mouseInput(window);
			camera.keyboardInput(window);
		}


		width = window.getSize().x;
		height = window.getSize().y;
		window.setViewport(width, height);


		auto model = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0));
		model = glm::rotate(model, angle, glm::vec3(0, 0, 1));

		clock.restart();
		angle += clock.getElapsedTime() * 0.3;

		screenShader.setMat4("model", model);
		screenShader.setMat4("view", camera.getViewMatrix());
		screenShader.setMat4("proj", camera.getProjMatrix(width, height));

		planetShader.setMat4("model", model);
		planetShader.setMat4("view", camera.getViewMatrix());
		planetShader.setMat4("proj", camera.getProjMatrix(width, height));

		window.clear(50.0 / 256, 16.0 / 256, 81.0 / 256, 1);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		window.draw(mesh, planetShader, texture);
		if (showOutline) {
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			window.draw(mesh, screenShader, texture);
		}
		window.display();
	}
}