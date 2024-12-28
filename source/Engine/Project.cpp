#ifdef USE_MODULES
module Project;
import Lmath;
#else
#include "Lmath.cppm"
#include "Project.cppm"
#endif

/* 
Entity Project::CreateEntity(const std::string_view& name) {
	auto entity = Entity(&registry, registry.create());
	entity.Add<Component::Name>(name);
	return entity;
}
 */