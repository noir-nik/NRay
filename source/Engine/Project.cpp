#ifdef USE_MODULES
import Lmath;
#else
#include "Lmath.cxx"
#endif
#include "Project.hpp"

/* 
Entity Project::CreateEntity(const std::string_view& name) {
	auto entity = Entity(&registry, registry.create());
	entity.AddComponent<Component::Name>(name);
	return entity;
}
 */