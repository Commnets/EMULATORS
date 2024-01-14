#include <VIC20/VIA2Registers.hpp>

// ---
VIC20::VIA2Registers::VIA2Registers (MCHEmul::PhysicalStorage* ps, size_t pp, const MCHEmul::Address& a, size_t s)
	: COMMODORE::VIARegisters (_VIA2_SUBSET, ps, pp, a, s)
	  // At this point all internal variables will have random values...
{ 
	setClassName ("VIA2Registers");
}
