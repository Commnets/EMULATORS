#include <C64/1541Disk.hpp>

const std::vector <unsigned char> C64::Disk1541Simulation::_POSSIBLEDEVICENUMBERS = 
	{ 0x08, 0x09 }; // Just the two possiblities...
const COMMODORE::SerialIOPeripheralSimulation::Definition 
	C64::Disk1541Simulation::_DEFINITION = {
		MCHEmul::Address ({ 0x59, 0x02 }, false),	// LATTABLE
		MCHEmul::Address ({ 0x63, 0x02 }, false),	// FATTABLE
		MCHEmul::Address ({ 0x6d, 0x02 }, false),	// SATTABLE
		MCHEmul::Address ({ 0x99, 0x00 }, false),	// DFLTN
		MCHEmul::Address ({ 0x9a, 0x00 }, false),	// DFLTO
		MCHEmul::Address ({ 0x95, 0x00 }, false),	// The address where the the information to be sent is kept...
		MCHEmul::Address ({ 0x90, 0x00 }, false),	// The address where the status is set...
		0x00, 0x80,
		// Traps...
		{
			{
				COMMODORE::SerialIOPeripheralSimulation::_LISTEN1TRAP, 
				"Serial Listen",
				MCHEmul::Address ({ 0x24, 0xed }, false),
				MCHEmul::Address ({ 0xab, 0xed }, false),
				{ 0x20, 0x97, 0xee }
			}, 
			{ 
				COMMODORE::SerialIOPeripheralSimulation::_LISTEN2TRAP,
				"Serial SA Listen",
				MCHEmul::Address ({ 0x37, 0xed }, false),
				MCHEmul::Address ({ 0xab, 0xed }, false),
				{ 0x20, 0x8e, 0xee }
			},
			{ 
				COMMODORE::SerialIOPeripheralSimulation::_SENDBYTETRAP,
				"Serial Send Byte",
				MCHEmul::Address ({ 0x41, 0xed }, false),
				MCHEmul::Address ({ 0xab, 0xed }, false),
				{ 0x20, 0x97, 0xee } 
			},
			{ 
				COMMODORE::SerialIOPeripheralSimulation::_RECEIVEBYTETRAP,
				"Serial Receive Byte",
				MCHEmul::Address ({ 0x14, 0xee }, false),
				MCHEmul::Address ({ 0xab, 0xed }, false),
				{ 0xa9, 0x00, 0x85 } 
			},
			{ 
				COMMODORE::SerialIOPeripheralSimulation::_READYTRAP,
				"Serial Ready",
				MCHEmul::Address ({ 0xa9, 0xee }, false),
				MCHEmul::Address ({ 0xab, 0xed }, false),
				{ 0xad, 0x00, 0xdd } 
	}}};

// ---
C64::Disk1541Simulation::Disk1541Simulation (int id, unsigned char dN)
	: COMMODORE::Disk1540SeriesSimulation (id, dN, C64::Disk1541Simulation::_DEFINITION)
{ 
	// The device number can be either 0x04 or 0x05...
	// Notice that this only verified in the debug mode...
	assert (isDeviceNumberValid (deviceNumber ()));
}
