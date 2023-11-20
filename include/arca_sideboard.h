
#include "sys/mman.h"


class SysAlloc {
public:
	using VoidP = void *;
	using SizeT = size_t;

	static VoidP req(size_t len) {
		void * addr = (void *) mmap(NULL, len, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_ANON | MAP_PRIVATE, -1, 0);
		return addr;
	}

	static void rel(VoidP addr, SizeT len) {
		munmap(addr, len);
	}
};


