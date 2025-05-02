#include <MSX/PPI8255.hpp>
#include <MSX/OSIO.hpp>

// ---
MSX::PPI8255::PPI8255 (MSX::PPI8255Registers* reg)
	: MCHEmul::Chip (MSX::PPI8255::_ID,
		 { { "Name", "PPI 8255" },
		   { "Manufacturer", "Intel" },
		   { "Year", "1970" } }),
	  _PPI8255Registers (reg),
	  _lastCPUCycles (0),
	  _internalRegisters (nullptr)
{
	// If nullptr a temporal one is created that it will be deleted when the object is destroyed...
	if (_PPI8255Registers == nullptr)
		_internalRegisters = _PPI8255Registers = new MSX::PPI8255Registers;
}

// ---
MSX::PPI8255::~PPI8255 ()
{
	// That could be nullptr, it they would have been created externally!
	delete (_internalRegisters); // ...and in that case, nothing will happen...
}

// ---
bool MSX::PPI8255::simulate (MCHEmul::CPU* cpu)
{
	// First time?
	if (_lastCPUCycles == 0)
	{ 
		_lastCPUCycles = cpu -> clockCycles (); // Nothing to do...

		return (true);
	}

	// Simulate the the chip...
	// IN this case it is not needed to iterate over all cycles executed at CPU level...
	_IFDEBUG debugPPI8255Cycle (cpu);

	if (_PPI8255Registers -> slotChanged ())
		notify (MCHEmul::Event (MSX::PPI8255::_SLOTCHANGED, 
			(int) _PPI8255Registers -> peekRegister (0).value ()));

	_lastCPUCycles = cpu -> clockCycles ();

	return (true);
}

// ---
MCHEmul::InfoStructure MSX::PPI8255::getInfoStructure () const
{
	MCHEmul::InfoStructure result = std::move (MCHEmul::Chip::getInfoStructure ());

	// TODO

	return (result);
}

// ---
void MSX::PPI8255::processEvent (const MCHEmul::Event& evnt, MCHEmul::Notifier* n)
{
	switch (evnt.id ())
	{
		case MCHEmul::InputOSSystem::_KEYBOARDKEYPRESSED:
			{
				const MSX::InputOSSystem::Keystrokes& ks = ((MSX::InputOSSystem*) n) -> keystrokesFor
					(std::static_pointer_cast <MCHEmul::InputOSSystem::KeyboardEvent> (evnt.data ()) -> _key);
				if (!ks.empty ()) // The key has to be defined...
					for (const auto& j : ks)
						_PPI8255Registers -> setKeyboardStatusMatrix (j.first, j.second, false);
			}

			break;

		case MCHEmul::InputOSSystem::_KEYBOARDKEYRELEASED:
			{
				const MSX::InputOSSystem::Keystrokes& ks = ((MSX::InputOSSystem*) n) -> keystrokesFor
					(std::static_pointer_cast <MCHEmul::InputOSSystem::KeyboardEvent> (evnt.data ()) -> _key);
				if (!ks.empty ()) // The key has to be defined...
					for (const auto& j : ks)
						_PPI8255Registers -> setKeyboardStatusMatrix (j.first, j.second, true);
			}

			break;

		// Event not managed...
		default:
			break;
	}
}

// ---
void MSX::PPI8255::debugPPI8255Cycle (MCHEmul::CPU* cpu)
{
	assert (_deepDebugFile != nullptr);

	// TODO
}