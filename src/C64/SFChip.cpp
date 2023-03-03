#include <C64/SFChip.hpp>
#include <C64/Memory.hpp>
#include <C64/CIA1Registers.hpp>
#include <C64/CIA2Registers.hpp>
#include <C64/DatasettePort.hpp>

// ---
C64::SpecialFunctionsChip::SpecialFunctionsChip ()
	: MCHEmul::Chip (_ID, 
		{ { "Name", "SFChip" },
		{ "Manufacturer", "Ignacio Cea" },
		{ "Year", "2022" } }),
	  _SFRegisters (nullptr)
{
	setClassName ("SFChip");
}

// ---
bool C64::SpecialFunctionsChip::initialize ()
{
	_SFRegisters	= dynamic_cast <C64::SpecialFunctionsChipRegisters*>
		(memoryRef () -> subset (C64::SpecialFunctionsChipRegisters::_SPECIALFUNCTIONSCHIP_SUBSET));
	assert (_SFRegisters != nullptr); // It has to exists...
	_SFRegisters -> initialize ();

	return (true);
}

// ---
bool C64::SpecialFunctionsChip::simulate (MCHEmul::CPU* cpu)
{
	if (!_SFRegisters -> changesAtPositions ())
		return (true);

	dynamic_cast <C64::Memory*> (memoryRef ()) -> configureMemoryAccess 
		(_SFRegisters -> LORAM (), _SFRegisters -> HIRAM (), _SFRegisters -> CHAREN ());

	// Send the data to the casette port...
	notify (MCHEmul::Event (_SFRegisters -> casetteData ()
		? COMMODORE::DatasetteIOPort::_WRITE1 : COMMODORE::DatasetteIOPort::_WRITE0));
	// Modify the status of the motor of the casette...
	notify (MCHEmul::Event (_SFRegisters -> casetteMotorStopped ()
		? COMMODORE::DatasetteIOPort::_MOTORSTOPPED : COMMODORE::DatasetteIOPort::_MOTORRUNNING));

	return (true);
}

// ---
void C64::SpecialFunctionsChip::processEvent (const MCHEmul::Event& evnt, MCHEmul::Notifier* ntier)
{
	// Bit 4 reflects whether some key has been pressed on the dataette...
	// When something has been pressed this bit is set to 0, 1 in the other situation.
	// The same bit in the memory position 0 hast to be defined as input (value = 0)
	if (evnt.id () == COMMODORE::DatasetteIOPort::_KEYPRESSED ||
		evnt.id () == COMMODORE::DatasetteIOPort::_NOKEYPRESSED)
		_SFRegisters -> setCasetteNoKeyPressed (evnt.id () == COMMODORE::DatasetteIOPort::_NOKEYPRESSED);
}
