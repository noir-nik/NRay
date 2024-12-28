#pragma once
#include <string>
#include <filesystem>
#include <memory>
#include <vector>

#include "Component.hpp"
#include "SceneGraph.hpp"
#include "VulkanBase.hpp"

#ifdef USE_MODULES
import Lmath;
#else
#include "Lmath.hpp"
#endif

namespace glTF {
using namespace Component;
using namespace Lmath;
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
	bool Load(const std::filesystem::path& filepath, SceneGraph& sceneGraph, vkw::Device& device);
	
	std::unordered_map<int, Component::Mesh> meshMap;
};

} // namespace glTF