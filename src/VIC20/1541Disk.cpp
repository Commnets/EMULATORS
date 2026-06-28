#include <VIC20/1541Disk.hpp>

const std::vector <unsigned char> VIC20::Disk1541Simulation::_POSSIBLEDEVICENUMBERS = 
	{ 0x08, 0x09, 0x0a, 0x0b }; // Just the 4 possiblities...
const COMMODORE::SerialIOPeripheralSimulation::Definition 
	VIC20::Disk1541Simulation::_DEFINITION = {
		MCHEmul::Address ({ 0x59, 0x02 }, false),	// LATTABLE
		MCHEmul::Address ({ 0x63, 0x02 }, false),	// FATTABLE
		MCHEmul::Address ({ 0x6d, 0x02 }, false),	// SATTABLE
		MCHEmul::Address ({ 0x99, 0x00 }, false),	// DFLTN
		MCHEmul::Address ({ 0x9a, 0x00 }, false),	// DFLTO
		MCHEmul::Address ({ 0x95, 0x00 }, false),	// The address where the the information to be sent is kept...
		MCHEmul::Address ({ 0x90, 0x00 }, false),	// The address where the status is set...
		MCHEmul::Address ({ 0x01, 0x10 }, false),	// The address where the result of the "$" command is kept,
													// but it might be adjusted at construction time...
		0x00, 0x80,
		// Traps...
		{
			{
				COMMODORE::SerialIOPeripheralSimulation::_LISTEN1TRAP, 
				"Serial Listen",
				MCHEmul::Address ({ 0x2e, 0xee }, false),
				MCHEmul::Address ({ 0xb2, 0xee }, false),
				{ 0x20, 0xa0, 0xe4 }
			}, 
			{ 
				COMMODORE::SerialIOPeripheralSimulation::_LISTEN2TRAP,
				"Serial SA Listen",
				MCHEmul::Address ({ 0x40, 0xee }, false),
				MCHEmul::Address ({ 0xb2, 0xee }, false),
				{ 0x20, 0x8d, 0xef }
			},
			{ 
				COMMODORE::SerialIOPeripheralSimulation::_SENDBYTETRAP,
				"Serial Send Byte",
				MCHEmul::Address ({ 0x49, 0xee }, false),
				MCHEmul::Address ({ 0xb2, 0xee }, false),
				{ 0x78, 0x20, 0xa0 } 
			},
			{ 
				COMMODORE::SerialIOPeripheralSimulation::_RECEIVEBYTETRAP,
				"Serial Receive Byte",
				MCHEmul::Address ({ 0x19, 0xef }, false),
				MCHEmul::Address ({ 0xb2, 0xee }, false),
				{ 0x78, 0xa9, 0x00 } 
			},
			{ 
				COMMODORE::SerialIOPeripheralSimulation::_READYTRAP,
				"Serial Ready",
				MCHEmul::Address ({ 0xb2, 0xe4 }, false),
				MCHEmul::Address ({ 0xb2, 0xee }, false),
				{ 0xad, 0x1f, 0x91 }
			}
	}};

// ---
VIC20::Disk1541Simulation::Disk1541Simulation (
		Memory::Configuration cfg,
		const MCHEmul::ASCIIToCodeConverter* cvs,
		int id, unsigned char dN)
	: COMMODORE::Disk1540SeriesSimulation 
		(id, dN, cvs, VIC20::Disk1541Simulation::_DEFINITION)
{
	// The position where the dir command works...
	// might depend on the configuration of the machine...
	MCHEmul::Address dA = MCHEmul::Address ({ 0x01, 0x12 }, false);
	if (cfg == VIC20::Memory::Configuration::_NOEXPANDED) dA = MCHEmul::Address ({ 0x01, 0x10 }, false);
	else if (cfg == VIC20::Memory::Configuration::_3KEXPANSION) dA = MCHEmul::Address ({ 0x01, 0x04 }, false);
	definition ()._dirAddress = dA;

	// The device number can be either 0x08 - 0x0b...
	// Notice that this only verified in the debug mode...
	assert (isDeviceNumberValid (deviceNumber ()));
}
