#include "Util.hpp"
#include <atomic>

static std::atomic<UID> next_uid = 0;

UID UIDGenerator::Next() {
	return next_uid.fetch_add(1);
}