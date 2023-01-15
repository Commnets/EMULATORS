#include <C64/SFChip.hpp>
#include <C64/Memory.hpp>
#include <C64/CIA1Registers.hpp>
#include <C64/CIA2Registers.hpp>

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

	return (true);
}

// ---
bool C64::SpecialFunctionsChip::simulate (MCHEmul::CPU* cpu)
{
	// To avoid repeat calculus...
	static const MCHEmul::Address _POS1 ({ 0x01, 0x00 }, false);

	MCHEmul::UByte val0 = memoryRef () -> value (_POS1);

	// Active or desactive the BASIC ROM....
	bool bit0 = val0.bit (0);
	_BasicROM		-> setActiveForReading ( bit0);
	_BasicRAM		-> setActiveForReading (!bit0);

	// Active or desactive the KERNEL ROM...
	bool bit1 = val0.bit (1);
	_KernelROM		-> setActiveForReading ( bit1);
	_KernelRAM		-> setActiveForReading (!bit1);

	// Usually the CHAR ROM is only seen from VICII, 
	// because CPU access to the Chip Registers instead
	// But it could be accessed. Take really care when doing so!
	bool bit2 = val0.bit (2);
	_CharROM		-> setActiveForReading (!bit2);
	_VICIIRegisters -> setActiveForReading ( bit2);
	_SIDRegisters	-> setActiveForReading ( bit2);
	_ColorRAM		-> setActiveForReading ( bit2);
	_CIA1Registers	-> setActiveForReading ( bit2);
	_CIA2registers	-> setActiveForReading ( bit2);
	_IO1Registers	-> setActiveForReading ( bit2);
	_IO2registers	-> setActiveForReading ( bit2);

	return (true);
}
