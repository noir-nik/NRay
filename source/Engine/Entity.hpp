#pragma once
#include <entt/entity/registry.hpp>

using Registry = entt::registry;

struct Entity {
	Entity(Registry* registry, entt::entity entity = entt::null) : entity(entity), registry(registry) {}
	Registry* registry;
	entt::entity entity;
	inline static constexpr entt::entity Null = entt::null;
	// template<typename Type, typename... Args>
	// inline Type& AddComponent(Args&&... args);
	
	// template<typename Type>
	// inline void RemoveComponent();

	// template<typename Type>
	// inline Type& GetComponent();

	// template<typename Type>
	// inline bool HasComponent();

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
	inline bool HasComponent() {
		return registry->any_of<Type>(entity);
	}


};


