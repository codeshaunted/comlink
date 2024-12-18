#ifndef COMLINK_UTIL_HH
#define COMLINK_UTIL_HH

#include <cstdlib>

namespace comlink {

class Util {
	public:
		static void nopFill(uintptr_t dest, size_t len);
};

}; // namespace comlink

#endif // COMLINK_UTIL_HH