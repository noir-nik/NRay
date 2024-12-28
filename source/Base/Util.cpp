
#ifdef USE_MODULES
module Util;
import stl;
#else
#include "Util.cppm"
#include <atomic>
#endif


static std::atomic<UID> next_uid = 0;

UID UIDGenerator::Next() {
	return next_uid.fetch_add(1);
}