#pragma once
#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))

#ifdef USE_VLA
#define VLA(type, name, count) \
	type name##_vla[count]; \
	std::span<type> name(name##_vla, count)
#else
#define VLA(type, name, count) \
	std::vector<type> name##_vla(count); \
	std::span<type> name(name##_vla.data(), count)
#endif