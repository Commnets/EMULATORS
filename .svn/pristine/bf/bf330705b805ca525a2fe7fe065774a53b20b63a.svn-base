#include <C64/global.hpp>

// ---
void C64::actualizeGlobalTime ()
{
	C64::Time n = std::chrono::time_point_cast <C64::Duration> (std::chrono::steady_clock::now ());
	C64::_TENTHSSECONDPAST = C64::_NOW - n;
	C64::_NOW = n;
}
