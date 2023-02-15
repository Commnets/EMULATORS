#include <C64/SFChip.hpp>
#include <C64/Memory.hpp>
#include <C64/CIA1Registers.hpp>
#include <C64/CIA2Registers.hpp>
#include <C64/DatasettePort.hpp>

const MCHEmul::Address C64::SpecialFunctionsChip::_POS0 = MCHEmul::Address ({ 0x00, 0x00 }, false);
const MCHEmul::Address C64::SpecialFunctionsChip::_POS1 = MCHEmul::Address ({ 0x01, 0x00 }, false);

// ---
C64::SpecialFunctionsChip::SpecialFunctionsChip ()
	: MCHEmul::Chip (_ID, 
		{ { "Name", "SFChip" },
		{ "Manufacturer", "Ignacio Cea" },
		{ "Year", "2022" } }),
	  _SFRegisters (nullptr),
	  _BasicRAM (nullptr), 
	  _BasicROM (nullptr), 
	  _KernelROM (nullptr), 
	  _KernelRAM (nullptr),
	  _CharROM (nullptr), 
	  _VICIIRegisters (nullptr), 
	  _SIDRegisters (nullptr), 
	  _ColorRAM (nullptr),
	  _CIA1Registers (nullptr), 
	  _CIA2registers (nullptr), 
	  _IO1Registers (nullptr), 
	  _IO2registers (nullptr)
{
	setClassName ("SFChip");
}

// ---
bool C64::SpecialFunctionsChip::initialize ()
{
	_SFRegisters	= dynamic_cast <C64::SpecialFunctionsChipRegisters*>
		(memoryRef () -> subset (C64::SpecialFunctionsChipRegisters::_SPECIALFUNCTIONSCHIP_SUBSET));
	assert (_SFRegisters != nullptr); // It has to exists...

	_BasicROM		= memoryRef () -> subset (C64::Memory::_BASICROM_SUBSET);
	_BasicRAM		= memoryRef () -> subset (C64::Memory::_BASICRAM_SUBSET);
	_KernelROM		= memoryRef () -> subset (C64::Memory::_KERNELROM_SUBSET);
	_KernelRAM		= memoryRef () -> subset (C64::Memory::_KERNELRAM_SUBSET);
	_CharROM		= memoryRef () -> subset (C64::Memory::_CHARROM_SUBSET);
	_VICIIRegisters = memoryRef () -> subset (COMMODORE::VICIIRegisters::_VICREGS_SUBSET);
	_SIDRegisters	= memoryRef () -> subset (COMMODORE::SIDRegisters::_SIDREGS_SUBSET);
	_ColorRAM		= memoryRef () -> subset (C64::Memory::_COLOR_SUBSET);
	_CIA1Registers	= memoryRef () -> subset (C64::CIA1Registers::_CIA1_SUBSET);
	_CIA2registers	= memoryRef () -> subset (C64::CIA2Registers::_CIA2_SUBSET);
	_IO1Registers	= memoryRef () -> subset (C64::Memory::_IO1_SUBSET);
	_IO2registers	= memoryRef () -> subset (C64::Memory::_IO2_SUBSET);

	_lastValue1 = MCHEmul::UByte::_0;

	_SFRegisters -> initialize ();

	return (true);
}

// ---
bool C64::SpecialFunctionsChip::simulate (MCHEmul::CPU* cpu)
{
	MCHEmul::UByte val1 = memoryRef () -> value (_POS1);
	if (val1 == _lastValue1)
		return (true); // Nothing has changed...just to speed everything a little bit more!

	_lastValue1 = val1;

	// Active or desactive the BASIC ROM....
	_BasicROM		-> setActiveForReading ( _SFRegisters -> LORAM ());
	_BasicRAM		-> setActiveForReading (!_SFRegisters -> LORAM ());

	// Active or desactive the KERNEL ROM...
	_KernelROM		-> setActiveForReading ( _SFRegisters -> HIRAM ());
	_KernelRAM		-> setActiveForReading (!_SFRegisters -> HIRAM ());

	// Usually the CHAR ROM is only seen from VICII,
	// because CPU access to the Chip Registers instead
	// But it could be accessed. Take really care when doing so!
	_VICIIRegisters -> setActiveForReading ( _SFRegisters -> CHAREN ());
	_SIDRegisters	-> setActiveForReading ( _SFRegisters -> CHAREN ());
	_ColorRAM		-> setActiveForReading ( _SFRegisters -> CHAREN ());
	_CIA1Registers	-> setActiveForReading ( _SFRegisters -> CHAREN ());
	_CIA2registers	-> setActiveForReading ( _SFRegisters -> CHAREN ());
	_IO1Registers	-> setActiveForReading ( _SFRegisters -> CHAREN ());
	_IO2registers	-> setActiveForReading ( _SFRegisters -> CHAREN ());
	_CharROM		-> setActiveForReading (!_SFRegisters -> CHAREN ());

	// Send the data to the casette port...
	notify (MCHEmul::Event (_SFRegisters -> casetteData ()
		? COMMODORE::DatasetteIOPort::_WRITE1 : COMMODORE::DatasetteIOPort::_WRITE0));
	// Modify the status of the motor of the casette...
	notify (MCHEmul::Event (_SFRegisters -> casetteMotorRunning ()
		? COMMODORE::DatasetteIOPort::_MOTORRUNNING : COMMODORE::DatasetteIOPort::_MOTORSTOPPED));

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
		_SFRegisters -> setValue (0x01, (evnt.id () == COMMODORE::DatasetteIOPort::_NOKEYPRESSED));
}
