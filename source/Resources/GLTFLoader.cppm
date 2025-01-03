#ifdef USE_MODULES
export module GLTFLoader;
#define _GLTFLOADER_EXPORT export
import vulkan_backend;
import Component;
import SceneGraph;
import Materials;
import Types;
import std;
#else
#pragma once
#define _GLTFLOADER_EXPORT
// #include <vulkan_backend/core.hpp>
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
	unsigned char const* data;
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

// inline constexpr u32 NotSpecified = ~0u;

struct LoadInfo {
	std::filesystem::path const& filepath;
	SceneGraph& sceneGraph;
	Materials& materials;
	vb::Device& device;
	vb::Queue& queue;
	vb::Command& cmd;
	u32 const& errorImageID;
};

class Loader {
public:
	bool Load(LoadInfo const& info);
};

} // namespace glTF