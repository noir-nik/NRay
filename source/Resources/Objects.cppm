export module Objects;
import lmath;
import vulkan_backend;
import std;

export
namespace Objects {
using namespace lmath;

struct Vertex {
	vec3  position;
	vec3  normal;
	vec2  uv;
	vec4  color;
	vec4  tangent;
};





} // namespace Objects