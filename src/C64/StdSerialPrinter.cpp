#include <C64/StdSerialPrinter.hpp>

const std::vector <unsigned char> C64::StandardSerialPrinterSimulation::_POSSIBLEDEVICENUMBERS = 
	{ 0x04, 0x05 }; // Just the two possiblities...

const COMMODORE::SerialIOPeripheralSimulation::Definition 
	C64::StandardSerialPrinterSimulation::_DEFINITION = {
		MCHEmul::Address ({ 0x59, 0x02 }, false),	// LATTABLE
		MCHEmul::Address ({ 0x63, 0x02 }, false),	// FATTABLE
		MCHEmul::Address ({ 0x6d, 0x02 }, false),	// SATTABLE
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
C64::StandardSerialPrinterSimulation::StandardSerialPrinterSimulation (int id, unsigned char dN,
		MCHEmul::MatrixPrinterEmulation* mPE)
	: COMMODORE::SerialPrinterPeripheralSimulation (id, dN, _DEFINITION,
		{ { "Name", "Standard Serial Printer" },
		  { "Manufacturer", "ICF Printer Software Simulation" } },
		mPE)
{
	assert (isDeviceNumberValid (deviceNumber ()));
}
