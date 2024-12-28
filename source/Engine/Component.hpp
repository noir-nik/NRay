#ifdef USE_MODULES
import Lmath;
#else
#include "Lmath.cxx"
#endif
#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include "Objects.hpp"
#include "Phong.h"


namespace Component {
using namespace Lmath;

struct Name {
    std::string name;
    
    Name(const std::string_view& name) : name(name) {}
};

struct Transform {
	mat4 local = {};
	mat4 global = {};

	bool dirty;

	Transform(bool dirty = true) : dirty(dirty) {}
};


// struct Mesh {
// 	Mesh(std::shared_ptr<Objects::Mesh> mesh) : mesh(mesh) {}
// 	std::shared_ptr<Objects::Mesh> mesh;
// };
using Mesh = std::shared_ptr<Objects::Mesh>;

} // namespace Component
