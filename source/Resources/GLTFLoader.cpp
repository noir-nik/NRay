#ifdef USE_MODULES
module GLTFLoader;
import fastgltf;
import Lmath;
import stl;
import Objects;
import ImageIO;
import VulkanBackend;
import SceneGraph;
import Log;
import Component;
#else
#include "GLTFLoader.cppm"
#include "VulkanBackend.cppm"
#include "Lmath.cppm"
#include "ImageIO.cppm"
#include "Log.cppm"

#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <unordered_map>
#include <vector>

#include <fastgltf/core.hpp>
#include <fastgltf/tools.hpp>

#endif

// #include "Entity.hpp"
// #include "Scene.hpp"

namespace fastgltf {
template<> struct ElementTraits<Lmath::vec4> : ElementTraitsBase<Lmath::vec4, AccessorType::Vec4, float> {};
template<> struct ElementTraits<Lmath::vec3> : ElementTraitsBase<Lmath::vec3, AccessorType::Vec3, float> {};
template<> struct ElementTraits<Lmath::vec2> : ElementTraitsBase<Lmath::vec2, AccessorType::Vec2, float> {};
}

namespace glTF {
using namespace Lmath;
// auto LoadTexture(fastgltf::Asset& asset, size_t imageIndex) -> Texture;
void LoadTextures(fastgltf::Asset& asset);
bool LoadMaterial(fastgltf::Asset& asset, size_t materialIndex);
void LoadMaterials(fastgltf::Asset& asset);
void LoadMeshes(fastgltf::Asset& asset, std::unordered_map<size_t, Component::Mesh>& meshMap, vkw::Device& device);
void LoadScene(fastgltf::Asset& asset, SceneGraph& sceneGraph);
void LoadScenes(fastgltf::Asset& asset, std::unordered_map<size_t, Component::Mesh>& meshMap, SceneGraph& sceneGraph);


bool Loader::Load(const std::filesystem::path& filepath, SceneGraph& sceneGraph, vkw::Device& device) {

	constexpr auto parserOptions =
	  fastgltf::Options::DontRequireValidAssetMember
	| fastgltf::Options::AllowDouble
	| fastgltf::Options::LoadExternalBuffers
	| fastgltf::Options::LoadExternalImages
	| fastgltf::Options::GenerateMeshIndices;

	fastgltf::Parser parser;
	// auto gltfFile = fastgltf::MappedGltfFile::FromPath(filepath);

	fastgltf::GltfDataBuffer dataBuffer;

	auto data = fastgltf::GltfDataBuffer::FromPath(filepath);
    if (data.error() != fastgltf::Error::None) {
		LOG_WARN("Failed to load asset file: {}", fastgltf::getErrorName(data.error()));
		return false;
	}

	auto expectedAsset = parser.loadGltf(data.get(), filepath.parent_path(), parserOptions);
    if (expectedAsset.error() != fastgltf::Error::None) {
        LOG_WARN("Failed to load asset file: {}", fastgltf::getErrorName(expectedAsset.error()));
		return false;
    }

	auto& asset = expectedAsset.get();

	// LoadTextures(asset, loadedData);
	// LoadMaterials(asset, loadedData);

	std::unordered_map<size_t, Component::Mesh> meshMap;
	LoadMeshes(asset, meshMap, device);
	LoadScenes(asset, meshMap, sceneGraph);
	return true;
}


/* 
auto LoadTexture(fastgltf::Asset& asset, size_t textureIndex) -> Texture {
	fastgltf::Texture& texture = asset.textures[textureIndex];

	std::size_t imageIndex;

	if      (texture.imageIndex.has_value())       { imageIndex = texture.imageIndex.value(); }
	else if (texture.basisuImageIndex.has_value()) { imageIndex = texture.basisuImageIndex.value(); }
	else if (texture.ddsImageIndex.has_value())    { imageIndex = texture.ddsImageIndex.value(); }
	else if (texture.webpImageIndex.has_value())   { imageIndex = texture.webpImageIndex.value(); }

	auto &image = asset.images[imageIndex];

	int width = 0, height = 0, numChannels = 0;
	unsigned char* data = nullptr;

	std::visit(
		fastgltf::visitor {
			[&](fastgltf::sources::URI& filePath) {
				ASSERT(filePath.fileByteOffset == 0, "No support for offsets with stbi: {} ", texture.name);
				ASSERT(filePath.uri.isLocalPath(), "No support for external files with stbi: {} ", texture.name);
				data = ImageIO::Load(std::string(filePath.uri.path()).c_str(), &width, &height, &numChannels, 4);
			},
			[&](fastgltf::sources::Vector& vector) {
				data = ImageIO::LoadFromMemory(reinterpret_cast<const uchar*>(vector.bytes.data()),
				static_cast<int>(vector.bytes.size()), &width, &height, &numChannels, 4);
			},
			[&](fastgltf::sources::BufferView& view) {
				auto& bufferView = asset.bufferViews[view.bufferViewIndex];
				auto& buffer = asset.buffers[bufferView.bufferIndex];

				std::visit(fastgltf::visitor {
					[&](fastgltf::sources::Vector& vector) {
						data = ImageIO::LoadFromMemory(reinterpret_cast<const uchar*>(vector.bytes.data() + bufferView.byteOffset),
						static_cast<int>(bufferView.byteLength), &width, &height, &numChannels, 4);
					},
					[&](auto& arg) {
						LOG_WARN("Unsupported fastgltf::visitor default (BUfferView) {}", texture.name);
					}
				},
					buffer.data);
			},
			[&](auto& arg) {
				LOG_WARN("Unsupported fastgltf::visitor default {}", texture.name);
			}
		},
		image.data);
	
	// TODO: do this in external asset -> vulkan parser
 	
	if (data) {
		auto [magFilterGltf, minFilterGltf] = GetFilters(asset, imageIndex);
		auto magFilter = magFilterGltf == fastgltf::Filter::Nearest ? vkw::Filter::Nearest : vkw::Filter::Linear;
		auto minFilter = minFilterGltf == fastgltf::Filter::Nearest
			|| minFilterGltf == fastgltf::Filter::NearestMipMapNearest
			|| minFilterGltf == fastgltf::Filter::NearestMipMapLinear
			? vkw::Filter::Nearest
			: vkw::Filter::Linear;
		auto mipmapMode = minFilterGltf == fastgltf::Filter::NearestMipMapNearest
			|| minFilterGltf == fastgltf::Filter::LinearMipMapNearest
			? vkw::MipmapMode::Nearest
			: vkw::MipmapMode::Linear;
		newImage = device.CreateImage({
			.width = uint32_t(width),
			.height = uint32_t(height),
			.format = GetImageFormat(asset, imageIndex),
			.usage = vkw::ImageUsage::Sampled,
			.magFilter = magFilter,
			.minFilter = minFilter,
			.mipmapMode = mipmapMode,
			.name = "Texture " + std::to_string(imageIndex),
		});

		ImageIO::Free(data);
		return true;
	}
	

	// Determine format
	auto format = Format::Linear; // Default

	for (fastgltf::Material& material : asset.materials) {
		if (material.pbrData.baseColorTexture.has_value()) { // BaseColor
			uint diffuseTextureIndex = material.pbrData.baseColorTexture.value().textureIndex;
			auto& diffuseTexture = asset.textures[diffuseTextureIndex];
			if (imageIndex == diffuseTexture.imageIndex.value()) {
				format =  Format::sRGB;
			}
		}
		if (material.emissiveTexture.has_value()) { // Emissive
			uint emissiveTextureIndex = material.emissiveTexture.value().textureIndex;
			auto& emissiveTexture = asset.textures[emissiveTextureIndex];
			if (imageIndex == emissiveTexture.imageIndex.value()) {
				format =  Format::sRGB;
			}
		}
	}
	
	// Get sampler data
	Sampler sampler {
		.magFilter = Filter::Linear,
		.minFilter = Filter::Linear,
		.wrapS = Wrap::Repeat,
		.wrapT = Wrap::Repeat,
		.name = "",
	};

	auto CastFilter = [](fastgltf::Filter filter) -> Filter {
		switch (filter) {
		case fastgltf::Filter::Nearest: return Filter::Nearest;
		case fastgltf::Filter::Linear: return Filter::Linear;
		case fastgltf::Filter::NearestMipMapNearest: return Filter::NearestMipMapNearest;
		case fastgltf::Filter::NearestMipMapLinear: return Filter::NearestMipMapLinear;
		case fastgltf::Filter::LinearMipMapNearest: return Filter::LinearMipMapNearest;
		case fastgltf::Filter::LinearMipMapLinear: return Filter::LinearMipMapLinear;
		default: return Filter::Linear;
		}
	};

	auto CastWrap = [](fastgltf::Wrap wrap) -> Wrap {
		switch (wrap) {
		case fastgltf::Wrap::ClampToEdge: return Wrap::ClampToEdge;
		case fastgltf::Wrap::MirroredRepeat: return Wrap::MirroredRepeat;
		case fastgltf::Wrap::Repeat: return Wrap::Repeat;
		default: return Wrap::Repeat;
		}
	};

	if (texture.samplerIndex.has_value()) {
		auto glTFsampler = asset.samplers[texture.samplerIndex.value()];
		sampler = {
			.magFilter = CastFilter(glTFsampler.magFilter.value_or(fastgltf::Filter::Linear)),
			.minFilter = CastFilter(glTFsampler.minFilter.value_or(fastgltf::Filter::Linear)),
			.wrapS = CastWrap(glTFsampler.wrapS),
			.wrapT = CastWrap(glTFsampler.wrapT),
			.name = glTFsampler.name.c_str(),
		};
	}

	return {
		.name = texture.name.c_str(),
		.data = data,
		.width = width,
		.height = height,
		.numChannels = numChannels,
		.sampler = sampler,
	};
};

void LoadTextures(fastgltf::Asset& asset, Data& loadedData) {
	auto numImages = asset.images.size();
	for (size_t imageIndex = 0; imageIndex < numImages; ++imageIndex) {
		loadedData.textures.push_back(LoadTexture(asset, imageIndex));
	}
}
 

/* 
bool LoadMaterial(fastgltf::Asset& asset, size_t materialIndex) {
	auto& materialData = asset.materials[materialIndex];
	glTFMetalicRoughness material;
	
	load(materialData.pbrData.baseColorFactor.data(), material.pbrMetallicRoughness.baseColorFactor);

	if (materialData.pbrData.baseColorTexture.has_value()) {
		uint diffuseMapIndex = materialData.pbrData.baseColorTexture.value().textureIndex;
		uint imageIndex = asset.textures[diffuseMapIndex].imageIndex.value();
		// material.pbrMetallicRoughness.baseColorTexture = textures[imageIndex];
	}
	// todo: other fields
	return true;
}

void LoadMaterials(fastgltf::Asset& asset) {
	auto numMaterials = asset.materials.size();
	for (size_t materialIndex = 0; materialIndex < numMaterials; ++materialIndex) {
		LoadMaterial(asset, materialIndex);
	}
}
 */

 

// std::vector<uint32_t> indices;
// std::vector<Vertex> vertices;

void LoadMesh(fastgltf::Asset& asset, size_t meshIndex, Component::Mesh& mesh, vkw::Device& device) {
	auto& glTFmesh = asset.meshes[meshIndex];
	auto numPrimitives = glTFmesh.primitives.size();

	for (size_t primitiveIndex = 0; primitiveIndex < numPrimitives; ++primitiveIndex) {
		auto& p = glTFmesh.primitives[primitiveIndex];
		// Primitive primitive;

		size_t initialVertex = mesh->vertices.size();

		// Indexes
		if (p.indicesAccessor.has_value()) {
			fastgltf::Accessor& indexaccessor = asset.accessors[p.indicesAccessor.value()];
			mesh->indices.reserve(mesh->indices.size() + indexaccessor.count);

			fastgltf::iterateAccessor<std::uint32_t>(asset, indexaccessor,
				[&](std::uint32_t idx) {
					mesh->indices.push_back(idx + initialVertex);
				});
		}

		// Vertices
		auto position = p.findAttribute("POSITION");
		if (position != p.attributes.end()) {
			auto& posAccessor = asset.accessors[position->accessorIndex];
			mesh->vertices.resize(mesh->vertices.size() + posAccessor.count);

			fastgltf::iterateAccessorWithIndex<vec3>(asset, posAccessor,
				[&](vec3 v, size_t index) {
					Objects::Vertex newvtx;
					newvtx.position = v;
					newvtx.normal = { 1, 0, 0 };
					newvtx.color = vec4 { 1.f };
					newvtx.uv.x = 0;
					newvtx.uv.y = 0;
					mesh->vertices[initialVertex + index] = newvtx;
				});
		}
		
		// Normals
		auto normals = p.findAttribute("NORMAL");
		if (normals != p.attributes.end()) {

			fastgltf::iterateAccessorWithIndex<vec3>(asset, asset.accessors[normals->accessorIndex],
				[&](vec3 v, size_t index) {
					mesh->vertices[initialVertex + index].normal = v;
				});
		}

		// UV
		auto uv = p.findAttribute("TEXCOORD_0");
		if (uv != p.attributes.end()) {

			fastgltf::iterateAccessorWithIndex<vec2>(asset, asset.accessors[uv->accessorIndex],
				[&](vec2 v, size_t index) {
					mesh->vertices[initialVertex + index].uv.x = v.x;
					mesh->vertices[initialVertex + index].uv.y = v.y;
				});
		}

		// Colors
		auto colors = p.findAttribute("COLOR_0");
		if (colors != p.attributes.end()) {
			fastgltf::iterateAccessorWithIndex<vec4>(asset, asset.accessors[colors->accessorIndex],
				[&](vec4 v, size_t index) {
					mesh->vertices[initialVertex + index].color = v;
				});
		}
	}
}

void LoadMeshes(fastgltf::Asset& asset, std::unordered_map<size_t, Component::Mesh>& meshMap, vkw::Device& device) {
	auto numMeshes = asset.meshes.size();
	LOG_INFO("Loading {} meshes", numMeshes);
	for (size_t meshIndex = 0; meshIndex < numMeshes; ++meshIndex) {
		auto mesh = std::make_shared<Objects::Mesh>(asset.meshes[meshIndex].name);
		auto it = meshMap.emplace(meshIndex, mesh);
		if (!it.second) {
			LOG_WARN("Component::Mesh {} already exists", meshIndex);
			continue;
		}
		mesh->name = asset.meshes[meshIndex].name;
		LoadMesh(asset, meshIndex, mesh, device);
		mesh->vertexBuffer = device.CreateBuffer({
			.size = mesh->vertices.size() * sizeof(Objects::Vertex),
			.usage = vkw::BufferUsage::Vertex,
			.name = "Vertex buffer " + mesh->name,
		});
		mesh->indexBuffer = device.CreateBuffer({
			.size = mesh->indices.size() * sizeof(uint32_t),
			.usage = vkw::BufferUsage::Index,
			.name = "Index buffer " + mesh->name,
		});
	}

	auto transferQueue = device.GetQueue(vkw::QueueFlagBits::Transfer);
	auto& cmd = transferQueue.GetCommandBuffer();
	device.ResetStaging();
	cmd.BeginCommandBuffer();
	bool result;
	for (auto& [_, mesh] : meshMap) {
		result = cmd.Copy(mesh->vertexBuffer, mesh->vertices.data(), mesh->vertices.size() * sizeof(Objects::Vertex));
		if (!result) {
			cmd.EndCommandBuffer();
			cmd.QueueSubmit({});
			cmd.BeginCommandBuffer();
			result = cmd.Copy(mesh->vertexBuffer, mesh->vertices.data(), mesh->vertices.size() * sizeof(Objects::Vertex));
			ASSERT(result, "Failed to copy vertex buffer: not enough staging capacity");
		}
		result = cmd.Copy(mesh->indexBuffer, mesh->indices.data(), mesh->indices.size() * sizeof(uint32_t));
		if (!result) {
			cmd.EndCommandBuffer();
			cmd.QueueSubmit({});
			cmd.BeginCommandBuffer();
			result = cmd.Copy(mesh->indexBuffer, mesh->indices.data(), mesh->indices.size() * sizeof(uint32_t));
			ASSERT(result, "Failed to copy index buffer: not enough staging capacity");
		}
	}
	cmd.EndCommandBuffer();
	cmd.QueueSubmit({});
}

void LoadNode(fastgltf::Asset& asset, const int gltfNodeIndex, std::unordered_map<size_t, Component::Mesh>& meshMap, SceneGraph& sceneGraph, size_t offset) {
	auto& glTFNode = asset.nodes[gltfNodeIndex];
	auto& newNode = sceneGraph.nodes[offset + gltfNodeIndex];
	newNode.entity = sceneGraph.CreateEntity(glTFNode.name);
	DEBUG_TRACE("Loading node {}", glTFNode.name.c_str());
	newNode.entity.Add<Component::Transform>();
	if (glTFNode.meshIndex.has_value()) {
		Component::Mesh mesh = meshMap[glTFNode.meshIndex.value()];
		newNode.entity.Add<Component::Mesh>(mesh); //todo: fix
	} else {
		// todo : empty, volume flags
	}

	auto& local = newNode.entity.Get<Component::Transform>().local;
	std::visit(
		fastgltf::visitor {
		[&](fastgltf::math::fmat4x4& matrix) {
			memcpy(&local, matrix.data(), sizeof(matrix));
		},
		[&](fastgltf::TRS transform) {
			memcpy(&local, scale(rotate(translate(fastgltf::math::fmat4x4(), transform.translation), transform.rotation), (transform.scale)).data(), sizeof(fastgltf::math::fmat4x4));
		},
		[&](auto&&) {
			LOG_WARN("Unsupported fastgltf::visitor default (transform) {}", glTFNode.name);
		}
		},
		glTFNode.transform);

	size_t childNodeCount = glTFNode.children.size();
	newNode.children.resize(childNodeCount);
	for (size_t i = 0; i < childNodeCount; ++i) {
		auto glfwNodeIndex = glTFNode.children[i];
		newNode.children[i] = glfwNodeIndex;
		LoadNode(asset, glfwNodeIndex, meshMap, sceneGraph, offset);
	}
}

void LoadScenes(fastgltf::Asset& asset, std::unordered_map<size_t, Component::Mesh>& meshMap, SceneGraph& sceneGraph) {
	
	auto numNodes = asset.nodes.size();

	auto nodeOffset = sceneGraph.nodes.size();
	// auto sceneOffset = sceneGraph.root.children.size();

	// sceneGraph.root.children.resize(sceneOffset + numScenes); // All Scenes
	sceneGraph.nodes.resize(nodeOffset + numNodes); // All Nodes
	
	// auto currentOffset = nodeOffset + numScenes;
	// std::vector<size_t> nodeOffsetsForScenes(numScenes);
	// for (size_t sceneIndex = 0; sceneIndex < numScenes; ++sceneIndex) {
	// 	nodeOffsetsForScenes[sceneIndex] = currentOffset;
	// 	currentOffset += asset.scenes[sceneIndex].nodeIndices.size();
	// }

	auto& currentScene = sceneGraph.GetCurrentScene();
	auto numScenes = asset.scenes.size(); //glTF scenes, not ours

	auto currentOffset = 0;
	std::vector<size_t> childOffsetsForScenes(numScenes);
	for (size_t sceneIndex = 0; sceneIndex < numScenes; ++sceneIndex) {
		childOffsetsForScenes[sceneIndex] = currentOffset;
		currentOffset += asset.scenes[sceneIndex].nodeIndices.size();
	}

	auto numChildren = currentScene.children.size();
	currentScene.children.resize(numChildren + currentOffset);

	for (size_t sceneIndex = 0; sceneIndex < numScenes; ++sceneIndex) {
		auto numChildNodes = asset.scenes[sceneIndex].nodeIndices.size();
		for (auto childNodeIndex = 0; childNodeIndex < numChildNodes; ++childNodeIndex) {
			auto glfwNodeIndex = asset.scenes[sceneIndex].nodeIndices[childNodeIndex];
			LoadNode(asset, glfwNodeIndex, meshMap, sceneGraph, nodeOffset);
			currentScene.children[numChildren + childOffsetsForScenes[sceneIndex] + childNodeIndex] = nodeOffset + glfwNodeIndex;
		}
	}
}

} // namespace glTF