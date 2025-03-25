#ifndef	VirtualHolsters_VERSION_INCLUDED
#define VirtualHolsters_VERSION_INCLUDED

#define MAKE_STR_HELPER(a_str) #a_str
#define MAKE_STR(a_str) MAKE_STR_HELPER(a_str)

#define VirtualHolsters_VERSION_MAJOR	3
#define VirtualHolsters_VERSION_MINOR	0
#define VirtualHolsters_VERSION_PATCH	7
#define VirtualHolsters_VERSION_BETA	0
#define VirtualHolsters_VERSION_VERSTRING	MAKE_STR(VirtualHolsters_VERSION_MAJOR) "." MAKE_STR(VirtualHolsters_VERSION_MINOR) "." MAKE_STR(VirtualHolsters_VERSION_PATCH) "." MAKE_STR(VirtualHolsters_VERSION_BETA)

#endif
