#ifdef USE_MODULES
export module GLTFLoader;
#define _GLTFLOADER_EXPORT export
import VulkanBackend;
import Component;
import SceneGraph;
import Materials;
import stl;
#else
#pragma once
#define _GLTFLOADER_EXPORT
// #include "VulkanBackend.cppm"
#include "Component.cppm"
#include "SceneGraph.cppm"
#include "Materials.cppm"
#include <string>

#include <filesystem>
#include <memory>
#include <vector>
#endif


_GLTFLOADER_EXPORT
namespace glTF {
/* 
enum class Format {
	sRGB,
	Linear,
};

enum class Filter : std::uint16_t {
	Nearest,
	Linear,
	NearestMipMapNearest,
	LinearMipMapNearest,
	NearestMipMapLinear,
	LinearMipMapLinear,
};

enum class Wrap : std::uint16_t {
	ClampToEdge,
	MirroredRepeat,
	Repeat,
};

struct Sampler {
	Filter magFilter = Filter::Linear;
	Filter minFilter = Filter::Linear;
	Wrap wrapS = Wrap::Repeat;
	Wrap wrapT = Wrap::Repeat;

	std::string name = "";
};

struct Texture {
	std::string name = "";
	const unsigned char* data;
	int width, height, numChannels;
	Sampler sampler;
};
 */
/*  
struct glTFMetalicRoughness {
	struct {
		vec4 baseColorFactor = vec4(1.0f);
		Texture baseColorTexture;
		float metallicFactor = float(1.0f);
		float roughnessFactor = float(1.0f);
		Texture metallicRoughnessTexture;
		// extensions
		// extras
	} pbrMetallicRoughness;
	
	Texture normalTexture;
	Texture occlusionTexture;

	Texture emissiveTexture;
	vec3 emissiveFactor = vec3(0.0f);

	std::string alphaMode = "OPAQUE";
	float alphaCutoff = 0.5f;

	bool doubleSided = false;
};

 */

class Loader {
public:
	bool Load(const std::filesystem::path& filepath, SceneGraph& sceneGraph, Materials& materials, vkw::Device& device, vkw::Queue& queue);
};

} // namespace glTF