#include <C64/SFChip.hpp>
#include <C64/Memory.hpp>

// ---
bool C64::SpecialFunctionsChip::initialize ()
{
	_BasicROM		= memoryRef () -> subset (C64::Memory::_BASICROM_SUBSET);
	_BasicRAM		= memoryRef () -> subset (C64::Memory::_BASICRAM_SUBSET);
	_KernelROM		= memoryRef () -> subset (C64::Memory::_KERNELROM_SUBSET);
	_KernelRAM		= memoryRef () -> subset (C64::Memory::_KERNELRAM_SUBSET);
	_CharROM		= memoryRef () -> subset (C64::Memory::_CHARROM_SUBSET);
	_VICIIRegisters = memoryRef () -> subset (C64::Memory::_VICREGS_SUBSET);
	_SIDRegisters	= memoryRef () -> subset (C64::Memory::_SIDREGS_SUBSET);
	_ColorRAM		= memoryRef () -> subset (C64::Memory::_COLOR_SUBSET);
	_CIA1Registers	= memoryRef () -> subset (C64::Memory::_CIA1_SUBSET);
	_CIA2registers	= memoryRef () -> subset (C64::Memory::_CIA2_SUBSET);
	_IO1Registers	= memoryRef () -> subset (C64::Memory::_IO1_SUBSET);
	_IO2registers	= memoryRef () -> subset (C64::Memory::_IO2_SUBSET);

	return (true);
}

// ---
bool C64::SpecialFunctionsChip::simulate (MCHEmul::CPU* cpu)
{
	MCHEmul::UByte val0 = memoryRef () -> value (MCHEmul::Address ({ 0x01, 0x00 }, false));

	// Active or desactive the BASIC ROM....
	_BasicROM		-> setActiveForReading (val0.bit (0));
	_BasicRAM		-> setActiveForReading (!val0.bit (0));

	// Active or desactive the KERNEL ROM...
	_KernelROM		-> setActiveForReading (val0.bit (1));
	_KernelRAM		-> setActiveForReading (!val0.bit (1));

	// Usually the CHAR ROM is only seen from VICII, 
	// because CPU access to the Chip Registers instead
	// But it could be accessed. Take really care when doing so!
	_CharROM		-> setActiveForReading (!val0.bit (2));
	_VICIIRegisters -> setActiveForReading (val0.bit (2));
	_SIDRegisters	-> setActiveForReading (val0.bit (2));
	_ColorRAM		-> setActiveForReading (val0.bit (2));
	_CIA1Registers	-> setActiveForReading (val0.bit (2));
	_CIA2registers	-> setActiveForReading (val0.bit (2));
	_IO1Registers	-> setActiveForReading (val0.bit (2));
	_IO2registers	-> setActiveForReading (val0.bit (2));

	return (true);
}
