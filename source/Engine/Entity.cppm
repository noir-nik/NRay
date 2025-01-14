export module Entity;
import entt;
import std;
import Types;
import Component;

export using Registry = entt::registry;

export struct Entity {
	Entity(Registry* registry = nullptr, entt::entity entity = entt::null) :
		registry(registry),
		entity(entity) {}
		
	Entity(Registry* registry, std::string_view const& name) :
		registry(registry)
	{
		entity = registry->create();
		Add<Component::Name>(name);
	}

	static inline constexpr entt::entity Null = entt::null;
	Registry* registry;
	entt::entity entity;
	
	// using Type = entt::registry::entity_type;
	using Type = EntityType;

    [[nodiscard]] std::size_t hash() const {
        return std::hash<Type>{}(static_cast<Type>(entity));
    }

	template<typename Type, typename... Args>
	inline Type& Add(Args&&... args) {
		return registry->emplace<Type>(entity, std::forward<Args>(args)...);
	}

	template<typename Type>
	inline void Remove() {
		registry->remove<Type>(entity);
	}

	template<typename Type>
	inline Type& Get() {
		return registry->get<Type>(entity);
	}

	template<typename Type>
	inline Type const& Get() const {
		return registry->get<Type>(entity);
	}

	template<typename Type>
	inline bool Has() {
		return registry->any_of<Type>(entity);
	}

	bool operator==(Entity const& other) const {
		return entity == other.entity
			&& registry == other.registry;
	}

};


