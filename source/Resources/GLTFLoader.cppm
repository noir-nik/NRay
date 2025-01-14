export module GLTFLoader;
import vulkan_backend;
import Component;
import SceneGraph;
import Materials;
import Types;
import std;

export
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