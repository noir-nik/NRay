#ifdef USE_MODULES
module;
#else
#pragma once
#endif

#include <entt/entity/registry.hpp>

// NOLINTBEGIN(misc-unused-using-decls)
#ifdef USE_MODULES
export module entt;

export
namespace entt {

	using entt::basic_entt_traits;

	using entt::entity;
	using entt::registry;
	// export using entt::runtime_view;
	// export using entt::const_runtime_view;
	// export using entt::basic_view;
	// export using entt::basic_common_view;
	// export using entt::observer;
	// export using entt::dispatcher;
	// export using entt::connect_arg_t;
	// export using entt::any_policy;
	// export using entt::entt_traits;
	// export using entt::hashed_string;
	// export using entt::id_type;
	// // export using entt::monostate;
	// // export using entt::process;
	// // export using entt::resource;
	// export using entt::snapshot;
	// export using entt::storage;
	// export using entt::storage_for;

	namespace internal {	
		using ::entt::internal::operator==;
		using ::entt::internal::operator!=;
		using ::entt::internal::operator<;
		using ::entt::internal::operator>;
		using ::entt::internal::operator<=;
		using ::entt::internal::operator>=;
		using ::entt::internal::operator-;
	};

	using entt::operator+;
	using entt::operator<;
	using entt::operator>;
	using entt::operator<=;
	using entt::operator>=;
	using entt::operator==;
	using entt::operator!=;

	using entt::null;
}

#endif
// NOLINTEND(misc-unused-using-decls)

