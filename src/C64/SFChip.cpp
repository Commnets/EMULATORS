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

	_lastValue0 = _lastValue1 = 0;

	return (true);
}

// ---
bool C64::SpecialFunctionsChip::simulate (MCHEmul::CPU* cpu)
{
	MCHEmul::UByte val0 = memoryRef () -> value (_POS0);
	MCHEmul::UByte val1 = memoryRef () -> value (_POS1);
	if (val1 == _lastValue1)
		return (true); // Nothing has changed...just to speed everything a little bit more!

	_lastValue1 = val1;

	// Active or desactive the BASIC ROM....
	bool bit10 = val1.bit (0);
	_BasicROM		-> setActiveForReading ( bit10);
	_BasicRAM		-> setActiveForReading (!bit10);

	// Active or desactive the KERNEL ROM...
	bool bit11 = val1.bit (1);
	_KernelROM		-> setActiveForReading ( bit11);
	_KernelRAM		-> setActiveForReading (!bit11);

	// Usually the CHAR ROM is only seen from VICII, 
	// because CPU access to the Chip Registers instead
	// But it could be accessed. Take really care when doing so!
	bool bit12 = val1.bit (2);
	_CharROM		-> setActiveForReading (!bit12);
	_VICIIRegisters -> setActiveForReading ( bit12);
	_SIDRegisters	-> setActiveForReading ( bit12);
	_ColorRAM		-> setActiveForReading ( bit12);
	_CIA1Registers	-> setActiveForReading ( bit12);
	_CIA2registers	-> setActiveForReading ( bit12);
	_IO1Registers	-> setActiveForReading ( bit12);
	_IO2registers	-> setActiveForReading ( bit12);

	// Bit 3 controls what it is written in the datasette...
	// The bit 3 is connected against the output line of the datasette port.
	// The same bit in the memory position 0 hast to be defined as output (value = 1)
	notify (MCHEmul::Event (val1.bit (3) && val0.bit (3)
		? COMMODORE::DatasetteIOPort::_WRITE1 : COMMODORE::DatasetteIOPort::_WRITE0));

	// Bit 5 controls the status of the motor in the datasette...
	// When 1 the motor is stopped (normal situation), when 0 the motor turns.
	// The same bit in the memory position 0 hast to be defined as output (value = 1)
	notify (MCHEmul::Event (val1.bit (5) && val0.bit (5)
		? COMMODORE::DatasetteIOPort::_MOTORSTOPPED : COMMODORE::DatasetteIOPort::_MOTORRUNNING));

	return (true);
}

// ---
void C64::SpecialFunctionsChip::processEvent (const MCHEmul::Event& evnt, MCHEmul::Notifier* ntier)
{
	// Bit 5 reflects whether some key has been pressed on the dataette...
	// When somethind has been pressed this bit is set to 1.
	// The same bit at the position 0 has to be set 0 set to indicate that 
	if (evnt.id () == COMMODORE::DatasetteIOPort::_KEYPRESSED ||
		evnt.id () == COMMODORE::DatasetteIOPort::_NOKEYPRESSED)
		memoryRef () -> set (_POS1, memoryRef () -> value (_POS1) & 0xef | 
			((evnt.id () == COMMODORE::DatasetteIOPort::_KEYPRESSED && !memoryRef () -> value (_POS0).bit (4)) ? 0x10 : 0x00));
}
