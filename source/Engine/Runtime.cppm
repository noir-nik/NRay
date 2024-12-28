#ifdef USE_MODULES
export module Runtime;
#define _RUNTIMECONTEXT_EXPORT export

#else
#pragma once
#define _RUNTIMECONTEXT_EXPORT

#endif

_RUNTIMECONTEXT_EXPORT
namespace Runtime {


} // namespace Runtime