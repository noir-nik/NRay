module;

import Types;

#define ENTT_ID_TYPE EntityType
#include <entt/entity/registry.hpp>

// NOLINTBEGIN(misc-unused-using-decls)
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
		using internal::operator==;
		using internal::operator!=;
		using internal::operator<;
		using internal::operator>;
		using internal::operator<=;
		using internal::operator>=;
		using internal::operator-;
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
// NOLINTEND(misc-unused-using-decls)
