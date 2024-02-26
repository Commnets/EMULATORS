#include <COMMODORE/C6529B/C6529BRegisters.hpp>

// ---
MCHEmul::InfoStructure COMMODORE::C6529BRegisters::getInfoStructure () const
{
	MCHEmul::InfoStructure result = 
		std::move (MCHEmul::ChipRegisters::getInfoStructure ());

	result.add ("LATCH", _latchKeyboard);

	return (result);
}
