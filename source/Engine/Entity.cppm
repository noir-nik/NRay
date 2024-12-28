#ifdef USE_MODULES
export module Entity;
#define _ENTITY_EXPORT export
import entt;
import stl;
import Types;
#else
#pragma once
#define _ENTITY_EXPORT
#include <entt/entity/registry.hpp>
#include "Types.cppm"
#endif

_ENTITY_EXPORT
using Registry = entt::registry;

_ENTITY_EXPORT
struct Entity {
	Entity(Registry* registry = nullptr, entt::entity entity = entt::null) : entity(entity), registry(registry) {}
	// Entity(Registry* registry = nullptr, EntityType entity = entt::null) : entity(entity), registry(registry) {}
	Registry* registry;
	entt::entity entity;
	inline static constexpr entt::entity Null = entt::null;
	
	// using Type = entt::registry::entity_type;
	using Type = EntityType;
	// template<typename Type, typename... Args>
	// inline Type& AddComponent(Args&&... args);
	
	// template<typename Type>
	// inline void RemoveComponent();

	// template<typename Type>
	// inline Type& GetComponent();

	// template<typename Type>
	// inline bool HasComponent();
	
    [[nodiscard]] std::size_t hash() const {
        return std::hash<Type>{}(static_cast<Type>(entity));
    }

	template<typename Type, typename... Args>
	inline Type& AddComponent(Args&&... args) {
		return registry->emplace<Type>(entity, std::forward<Args>(args)...);
	}

	template<typename Type>
	inline void RemoveComponent() {
		registry->remove<Type>(entity);
	}

	template<typename Type>
	inline Type& GetComponent() {
		return registry->get<Type>(entity);
	}

	template<typename Type>
	inline const Type& GetComponent() const {
		return registry->get<Type>(entity);
	}

	template<typename Type>
	inline bool HasComponent() {
		return registry->any_of<Type>(entity);
	}

	bool operator==(const Entity& other) const {
		return entity == other.entity
			&& registry == other.registry;
	}

};

