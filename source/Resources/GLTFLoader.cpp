#ifdef USE_MODULES
module;
#endif

#include "Bindless.h"
#include "Macros.h"

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
import Types;
import Entity;
import Materials;
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

struct ResourceMaps {
	std::unordered_map<u32, u32> textureMap;
	std::unordered_map<u32, Component::Material*> materialMap;
	std::unordered_map<u32, Component::Mesh*> meshMap;

	void clear() { textureMap.clear(); materialMap.clear(); meshMap.clear(); }
};
static ResourceMaps maps;

struct LoaderImpl {
	struct TextureInfo {
		uchar* imageData = nullptr;
		int width = 0;
		int height = 0;
		int numChannels = 0;
		size_t imageIndex = 0;
	};
	fastgltf::Asset& asset;
	ResourceMaps& maps;
	Materials& materials;
	vkw::Device& device;
	vkw::Queue& queue;
	SceneGraph& sceneGraph;

	auto LoadTexture(fastgltf::Texture const& assetTexture) -> TextureInfo;
	void LoadTextures();
	void LoadMaterial(fastgltf::Material const& materialData, Component::Material& material);
	void LoadMaterials();
	void LoadMesh(fastgltf::Mesh const& assetMesh, Component::Mesh& mesh);
	void LoadMeshes();
	void LoadNode(const int nodeIndex, size_t offset);
	void LoadNodes();
};

fastgltf::visitor sourcesVisitor {
	[](std::monostate) { return "std::monostate"; },
	[](fastgltf::sources::BufferView) { return "sources::BufferView"; },
	[](fastgltf::sources::URI) { return "sources::URI"; },
	[](fastgltf::sources::Array) { return "sources::Array"; },
	[](fastgltf::sources::Vector) { return "sources::Vector"; },
	[](fastgltf::sources::CustomBuffer) { return "sources::CustomBuffer"; },
	[](fastgltf::sources::ByteView) { return "sources::ByteView"; },
	[](fastgltf::sources::Fallback) { return "sources::Fallback"; },
};

bool Loader::Load(const std::filesystem::path& filepath, SceneGraph& sceneGraph, Materials& materials, vkw::Device& device, vkw::Queue& queue) {

	constexpr auto parserOptions {
		fastgltf::Options::DontRequireValidAssetMember
		| fastgltf::Options::AllowDouble
		| fastgltf::Options::LoadExternalBuffers
		| fastgltf::Options::LoadExternalImages
		| fastgltf::Options::GenerateMeshIndices
	};

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

	maps.clear();
	LoaderImpl loader { asset, maps, materials, device, queue, sceneGraph };
	loader.LoadTextures();
	loader.LoadMaterials();
	loader.LoadMeshes();
	loader.LoadNodes();
	return true;
}

auto LoaderImpl::LoadTexture(fastgltf::Texture const& assetTexture) -> TextureInfo {

	TextureInfo textureInfo;
	auto& data = textureInfo.imageData;
	auto& width = textureInfo.width;
	auto& height = textureInfo.height;
	auto& numChannels = textureInfo.numChannels;
	auto& imageIndex = textureInfo.imageIndex;
	
	if      (assetTexture.imageIndex.has_value())       { imageIndex = assetTexture.imageIndex.value(); }
	else if (assetTexture.basisuImageIndex.has_value()) { imageIndex = assetTexture.basisuImageIndex.value(); }
	else if (assetTexture.ddsImageIndex.has_value())    { imageIndex = assetTexture.ddsImageIndex.value(); }
	else if (assetTexture.webpImageIndex.has_value())   { imageIndex = assetTexture.webpImageIndex.value(); }

	auto &image = asset.images[imageIndex];

	std::visit(
		fastgltf::visitor {
			[&](fastgltf::sources::URI& filePath) {
				ASSERT(filePath.fileByteOffset == 0, "No support for offsets with stbi: {} ", image.name);
				ASSERT(filePath.uri.isLocalPath(), "No support for external files with stbi: {} ", image.name);
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
						LOG_WARN("Unsupported fastgltf::visitor default (BufferView) {}", image.name);
					}
				},
					buffer.data);
			},
			[&](fastgltf::sources::Array& array) {
				data = ImageIO::LoadFromMemory(reinterpret_cast<const uchar*>(array.bytes.data()),
				static_cast<int>(array.bytes.size()), &width, &height, &numChannels, 4);
			},
			[&](auto& arg) {
				LOG_WARN("Unsupported fastgltf::visitor default (Image) {} with variant type: {}", image.name, std::visit(sourcesVisitor, image.data));
			}
		},
		image.data);
	if (data) {
		LOG_INFO ("Loaded texture: {} {} {} {}", image.name, width, height, numChannels);
	}
	return textureInfo;
};

void LoaderImpl::LoadTextures() {
	auto numTextures = asset.textures.size();
	VLA(TextureInfo, textureInfos, numTextures);


	auto& cmd = queue.GetCommandBuffer();
	device.ResetStaging();
	cmd.Begin();
	
	for (size_t textureIndex = 0; auto const& assetTexture : asset.textures) {
		auto& textureInfo = textureInfos[textureIndex];
		auto& imageIndex = textureInfo.imageIndex;
		textureInfo = LoadTexture(assetTexture);

		if (textureInfo.imageData != nullptr) {

			// Determine format
			auto format = vkw::Format::RGBA8_UNORM; // Default
			for (fastgltf::Material& material : asset.materials) {
				if (material.pbrData.baseColorTexture.has_value()) { // BaseColor
					uint diffuseTextureIndex = material.pbrData.baseColorTexture.value().textureIndex;
					auto& diffuseTexture = asset.textures[diffuseTextureIndex];
					if (imageIndex == diffuseTexture.imageIndex.value()) {
						format =  vkw::Format::RGBA8_SRGB;
					}
				}
				if (material.emissiveTexture.has_value()) { // Emissive
					uint emissiveTextureIndex = material.emissiveTexture.value().textureIndex;
					auto& emissiveTexture = asset.textures[emissiveTextureIndex];
					if (imageIndex == emissiveTexture.imageIndex.value()) {
						format =  vkw::Format::RGBA8_SRGB;
					}
				}
			}

			// Get sampler
			auto glTFsampler = asset.samplers[assetTexture.samplerIndex.value()];
			auto magFilterGltf = glTFsampler.magFilter.value_or(fastgltf::Filter::Linear);
			auto minFilterGltf = glTFsampler.minFilter.value_or(fastgltf::Filter::Linear);

			auto magFilter {
					magFilterGltf == fastgltf::Filter::Nearest
				? vkw::Filter::Nearest
				: vkw::Filter::Linear
			};
			auto minFilter {
					minFilterGltf == fastgltf::Filter::Nearest ||
					minFilterGltf == fastgltf::Filter::NearestMipMapNearest ||
					minFilterGltf == fastgltf::Filter::NearestMipMapLinear
				? vkw::Filter::Nearest
				: vkw::Filter::Linear
			};
			auto mipmapMode {
					minFilterGltf == fastgltf::Filter::NearestMipMapNearest ||
					minFilterGltf == fastgltf::Filter::LinearMipMapNearest
				? vkw::MipmapMode::Nearest
				: vkw::MipmapMode::Linear
			};
			
			// Create image
			auto newImageEntity = sceneGraph.CreateEntity(asset.images[imageIndex].name);
			auto& newImage = newImageEntity.Add<vkw::Image>(device.CreateImage({
				.extent = { (uint32_t)textureInfo.width, (uint32_t)textureInfo.height, 1 },
				.format = format,
				.usage = vkw::ImageUsage::Sampled | vkw::ImageUsage::TransferDst,
				.sampler = {magFilter, minFilter, mipmapMode},
				.name = assetTexture.name.data(),
			}));

			// Copy image
			bool result;
			uint imageSize = textureInfo.width * textureInfo.height * textureInfo.numChannels; // todo: assure numChannels == 4
			cmd.Barrier(newImage, {vkw::ImageLayout::TransferDst});
			result = cmd.Copy(newImage, textureInfo.imageData, imageSize);
			if (!result) {
				cmd.End();
				cmd.QueueSubmit();
				device.ResetStaging();
				cmd.Begin();
				result = cmd.Copy(newImage, textureInfo.imageData, imageSize);
				ASSERT(result, "Failed to copy image: not enough staging capacity");
			}

			// Add to map
			maps.textureMap[textureIndex++] = newImage.RID();
		}
	}

	cmd.End();
	cmd.QueueSubmit();

	for (size_t textureIndex = 0; textureIndex < numTextures; ++textureIndex) {
		ImageIO::Free(textureInfos[textureIndex].imageData);
	}
}

void LoaderImpl::LoadMaterial(fastgltf::Material const& assetMaterial, Component::Material& material) {

	auto& gpuMaterial = material.gpuMaterial;
	
	if (const auto& tex = assetMaterial.pbrData.baseColorTexture) {
		gpuMaterial.baseColorTexture = maps.textureMap[tex->textureIndex];
	}
	if (const auto& tex = assetMaterial.pbrData.metallicRoughnessTexture) {
		gpuMaterial.metallicRoughnessTexture = maps.textureMap[tex->textureIndex];
	}
	if (const auto& tex = assetMaterial.emissiveTexture) {
		gpuMaterial.emissiveTexture = maps.textureMap[tex->textureIndex];
	}
	if (const auto& tex = assetMaterial.normalTexture) {
		gpuMaterial.normalTexture = maps.textureMap[tex->textureIndex];
	}
	if (const auto& tex = assetMaterial.occlusionTexture) {
		gpuMaterial.occlusionTexture = maps.textureMap[tex->textureIndex];
	}

	gpuMaterial.baseColorFactor = make_float4(assetMaterial.pbrData.baseColorFactor.data());
	gpuMaterial.metallicFactor  = assetMaterial.pbrData.metallicFactor;
	gpuMaterial.roughnessFactor = assetMaterial.pbrData.roughnessFactor;
	gpuMaterial.emissiveFactor  = make_float3(assetMaterial.emissiveFactor.data());
	material.emissiveStrength = assetMaterial.emissiveStrength;
	material.doubleSided = assetMaterial.doubleSided;
	material.alphaCutoff = assetMaterial.alphaCutoff;
	
	switch(assetMaterial.alphaMode) {
		case fastgltf::AlphaMode::Opaque: material.alphaMode = Component::Material::AlphaMode::Opaque; break;
		case fastgltf::AlphaMode::Mask:   material.alphaMode = Component::Material::AlphaMode::Mask;   break;
		case fastgltf::AlphaMode::Blend:  material.alphaMode = Component::Material::AlphaMode::Blend;  break;
	}
}

void LoaderImpl::LoadMaterials() {
	auto numMaterials = asset.materials.size();
	
	auto& cmd = queue.GetCommandBuffer();
	device.ResetStaging();
	cmd.Begin();
	
	for (size_t materialIndex = 0; auto const& assetMaterial : asset.materials) {
		auto mat = sceneGraph.CreateEntity();
		auto& material = mat.Add<Component::Material>(assetMaterial.name.c_str());
		auto materialID = materials.CreateSlot();
		material.deviceMaterialID = materialID;
		LoadMaterial(assetMaterial, material);
		maps.materialMap[materialIndex++] = &material;

		cmd.Copy(materials.GetBuffer(materialID), &material.gpuMaterial, sizeof(GpuTypes::Material), materials.GetOffset(materialID));
	}
	
	cmd.End();
	cmd.QueueSubmit();
}

// std::vector<uint32_t> indices;
// std::vector<Vertex> vertices;

void LoaderImpl::LoadMesh(fastgltf::Mesh const& assetMesh, Component::Mesh& mesh) {
	auto numPrimitives = assetMesh.primitives.size();
	mesh.primitives.resize(numPrimitives);

	for (size_t primitiveIndex = 0; primitiveIndex < numPrimitives; ++primitiveIndex) {
		auto& p = assetMesh.primitives[primitiveIndex];
		auto& primitive = mesh.primitives[primitiveIndex];
		size_t initialVertex = mesh.vertices.size();

		if (p.materialIndex.has_value()) {
			DEBUG_ASSERT(maps.materialMap.find(p.materialIndex.value()) != maps.materialMap.end(), "Material index {} not found", p.materialIndex.value());
			primitive.material = maps.materialMap[p.materialIndex.value()];
		} else {
			primitive.material = nullptr;
		}
		
		// Indexes
		if (p.indicesAccessor.has_value()) {
			fastgltf::Accessor& indexaccessor = asset.accessors[p.indicesAccessor.value()];
			mesh.indices.reserve(mesh.indices.size() + indexaccessor.count);
			primitive.indexCount = indexaccessor.count;
			primitive.firstIndex = mesh.indices.size();
			primitive.vertexOffset = initialVertex;
			primitive.firstInstance = 0;

			fastgltf::iterateAccessor<std::uint32_t>(asset, indexaccessor,
				[&](std::uint32_t idx) {
					mesh.indices.push_back(idx + initialVertex);
				});
		}

		// Vertices
		auto position = p.findAttribute("POSITION");
		if (position != p.attributes.end()) {
			auto& posAccessor = asset.accessors[position->accessorIndex];
			mesh.vertices.resize(mesh.vertices.size() + posAccessor.count);

			fastgltf::iterateAccessorWithIndex<vec3>(asset, posAccessor,
				[&](vec3 v, size_t index) {
					Objects::Vertex newvtx;
					newvtx.position = v;
					newvtx.normal = { 1, 0, 0 };
					newvtx.color = vec4 { 1.f };
					newvtx.uv.x = 0;
					newvtx.uv.y = 0;
					mesh.vertices[initialVertex + index] = newvtx;
				});
		}
		
		// Normals
		auto normals = p.findAttribute("NORMAL");
		if (normals != p.attributes.end()) {
			fastgltf::iterateAccessorWithIndex<vec3>(asset, asset.accessors[normals->accessorIndex],
				[&](vec3 v, size_t index) {
					mesh.vertices[initialVertex + index].normal = v;
				});
		}

		// UV
		auto uv = p.findAttribute("TEXCOORD_0");
		if (uv != p.attributes.end()) {
			fastgltf::iterateAccessorWithIndex<vec2>(asset, asset.accessors[uv->accessorIndex],
				[&](vec2 v, size_t index) {
					mesh.vertices[initialVertex + index].uv.x = v.x;
					mesh.vertices[initialVertex + index].uv.y = v.y;
				});
		}

		// Colors
		auto colors = p.findAttribute("COLOR_0");
		if (colors != p.attributes.end()) {
			fastgltf::iterateAccessorWithIndex<vec4>(asset, asset.accessors[colors->accessorIndex],
				[&](vec4 v, size_t index) {
					mesh.vertices[initialVertex + index].color = v;
				});
		}

		// Tangent
		auto tangent = p.findAttribute("TANGENT");
		if (tangent != p.attributes.end()) {
			fastgltf::iterateAccessorWithIndex<vec3>(asset, asset.accessors[tangent->accessorIndex],
				[&](vec3 v, size_t index) {
					mesh.vertices[initialVertex + index].tangent = v;
				});
		}
	}
}

void LoaderImpl::LoadMeshes() {
	auto numMeshes = asset.meshes.size();
	LOG_INFO("Loading {} meshes", numMeshes);
	for (u32 meshIndex = 0; auto const& assetMesh : asset.meshes) {
		auto meshEntity = sceneGraph.CreateEntity();
		auto& mesh = meshEntity.Add<Component::Mesh>();
		maps.meshMap[meshIndex++] = &mesh;
		mesh.name = assetMesh.name;
		LoadMesh(assetMesh, mesh);
		mesh.vertexBuffer = device.CreateBuffer({
			.size = mesh.vertices.size() * sizeof(Objects::Vertex),
			.usage = vkw::BufferUsage::Vertex,
			.name = "Vertex buffer " + mesh.name,
		});
		mesh.indexBuffer = device.CreateBuffer({
			.size = mesh.indices.size() * sizeof(uint32_t),
			.usage = vkw::BufferUsage::Index,
			.name = "Index buffer " + mesh.name,
		});
	}

	// auto transferQueue = device.GetQueue(vkw::QueueFlagBits::Transfer);
	// auto& cmd = transferQueue.GetCommandBuffer();
	auto& cmd = queue.GetCommandBuffer();
	device.ResetStaging();
	cmd.Begin();
	bool result;
	auto copyToBuffer = [&](vkw::Buffer& buffer, const void* data, uint32_t size) {
		result = cmd.Copy(buffer, data, size);
		if (!result) [[unlikely]] { 
			cmd.End();
			cmd.QueueSubmit();
			device.ResetStaging();
			cmd.Begin();
			result = cmd.Copy(buffer, data, size);
			ASSERT(result, "Failed to copy buffer: not enough staging capacity");
		}
	};
	for (auto& [_, mesh] : maps.meshMap) {
		copyToBuffer(mesh->vertexBuffer, mesh->vertices.data(), mesh->vertices.size() * sizeof(Objects::Vertex));
		copyToBuffer(mesh->indexBuffer, mesh->indices.data(), mesh->indices.size() * sizeof(uint32_t));
	}
	cmd.End();
	cmd.QueueSubmit();
}

void LoaderImpl::LoadNode(const int nodeIndex, size_t offset) {
	auto& glTFNode = asset.nodes[nodeIndex];
	auto& newNode = sceneGraph.nodes[offset + nodeIndex];
	newNode.entity = sceneGraph.CreateEntity(glTFNode.name);
	DEBUG_TRACE("Loading node {}", glTFNode.name.c_str());
	newNode.entity.Add<Component::Transform>();
	if (glTFNode.meshIndex.has_value()) {
		auto mesh = maps.meshMap[glTFNode.meshIndex.value()];
		newNode.entity.Add<Component::Mesh*>(mesh);
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
		LoadNode(glfwNodeIndex, offset);
	}
}

void LoaderImpl::LoadNodes() {
	
	auto numNodes = asset.nodes.size();

	auto nodeOffset = sceneGraph.nodes.size();
	// auto sceneOffset = sceneGraph.root.children.size();

	sceneGraph.nodes.resize(nodeOffset + numNodes); // All Nodes
	
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
			LoadNode(glfwNodeIndex, nodeOffset);
			currentScene.children[numChildren + childOffsetsForScenes[sceneIndex] + childNodeIndex] = nodeOffset + glfwNodeIndex;
		}
	}
}

} // namespace glTF