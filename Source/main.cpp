#include <filesystem>
#include <GLFW/glfw3.h>
import Sandcore.Application;
import Sandcore.Window;
using namespace Sandcore;


int main() {
	Window::setWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, GLFW_TRUE);

	Application app;
	app.loop();
}