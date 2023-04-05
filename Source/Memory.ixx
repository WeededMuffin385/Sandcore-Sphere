module;

#include <filesystem>
#include <fstream>

export module Sandcore.Application.Memory;

export namespace Sandcore::Memory {
	std::filesystem::path shaderBlockPath = std::filesystem::current_path() / "Userdata/Shaders/ShaderBlock";
	std::filesystem::path shaderScreenPath = std::filesystem::current_path() / "Userdata/Shaders/ShaderScreen";
	std::filesystem::path shaderInterfacePath = std::filesystem::current_path() / "Userdata/Shaders/ShaderInterface";

	std::filesystem::path texturesPath = std::filesystem::current_path() / "Userdata/TexturePackage";
}