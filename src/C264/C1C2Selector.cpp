#include <C264/C1C2Selector.hpp>
#include <C264/C1C2SelectorRegisters.hpp>
#include <C264/Memory.hpp>
#include <C264/TED.hpp>

// ---
C264::C1C2Selector::C1C2Selector ()
	: MCHEmul::Chip (_ID,
		{ { "Name", "FDD0 simulation" } }),
	  _C1C2SelectorRegisters (nullptr)
{ 
	// Nothing else to do...
}

// ---
bool C264::C1C2Selector::initialize ()
{
	assert (memoryRef () != nullptr);

	// The parent is not initialized because the registers are not the parent ones...

	// Gets the memory block dedicated to this register...
	if (!(_C1C2SelectorRegisters =
		dynamic_cast <C264::C1C2SelectorRegisters *> 
			(memoryRef () -> subset (C264::C1C2SelectorRegisters::_C1C2SELECTORREGS_SUBSET))))
	{
		_error = MCHEmul::_INIT_ERROR;

		return (false);
	}

	_C1C2SelectorRegisters -> initialize ();

	return (true);
}

// ---
bool C264::C1C2Selector::simulate (MCHEmul::CPU* cpu)
{
	assert (_C1C2SelectorRegisters != nullptr);

	if (_C1C2SelectorRegisters -> configurationChanged ())
	{ 
		_IFDEBUG debugC1C2SelectorCycle (cpu);

		// The memory is always C264::Memory!
		static_cast <C264::Memory*> (memoryRef ()) -> 
			setMemoryConfiguration (_C1C2SelectorRegisters -> memoryConfiguration ());
	}

	return (true);
}

// ---
void C264::C1C2Selector::debugC1C2SelectorCycle (MCHEmul::CPU* cpu)
{
	assert (_deepDebugFile != nullptr);

	*_deepDebugFile
		// Where
		<< "FDD0/FDDF,X\t" 
		// When
		<< std::to_string (cpu -> clockCycles ()) << "\t" // clock cycles at that point
		// What
		<< "Info cycle\t\t"
		// Data
		<< "Memory Cfg:"
		<< std::to_string (_C1C2SelectorRegisters -> memoryConfiguration ()) << "\n";
}
