#include <C264/StdSerialPrinter.hpp>

const std::vector <unsigned char> C264::StandardSerialPrinterSimulation::_POSSIBLEDEVICENUMBERS = 
	{ 0x04, 0x05 }; // Just the two possiblities...

const COMMODORE::SerialIOPeripheralSimulation::Definition 
	C264::StandardSerialPrinterSimulation::_DEFINITION = {
		MCHEmul::Address ({ 0x09, 0x05 }, false),	// LATTABLE
		MCHEmul::Address ({ 0x13, 0x05 }, false),	// FATTABLE
		MCHEmul::Address ({ 0x1d, 0x05 }, false),	// SATTABLE
		MCHEmul::Address ({ 0x88, 0x00 }, false),	// DFLTN
		MCHEmul::Address ({ 0x89, 0x00 }, false),	// DFLTO
		MCHEmul::Address ({ 0x95, 0x00 }, false),	// The address where the the information to be sent is kept...
		MCHEmul::Address ({ 0x90, 0x00 }, false),	// The address where the status is set...
		0x00, 0x80,
		// Traps...
		{
			{
				COMMODORE::SerialIOPeripheralSimulation::_LISTEN1TRAP, 
				"Serial Listen",
				MCHEmul::Address ({ 0x6b, 0xe1 }, false),
				MCHEmul::Address ({ 0xe7, 0xe1 }, false),
				{ 0x20, 0xc6, 0xe2 }
			}, 
			{ 
				COMMODORE::SerialIOPeripheralSimulation::_LISTEN2TRAP,
				"Serial SA Listen",
				MCHEmul::Address ({ 0x77, 0xe1 }, false),
				MCHEmul::Address ({ 0xe7, 0xe1 }, false),
				{ 0x78, 0x20, 0xbf }
			},
			{ 
				COMMODORE::SerialIOPeripheralSimulation::_SENDBYTETRAP,
				"Serial Send Byte",
				MCHEmul::Address ({ 0x81, 0xe1 }, false),
				MCHEmul::Address ({ 0xe7, 0xe1 }, false),
				{ 0x78, 0x20, 0xc6 } 
			},
			{ 
				COMMODORE::SerialIOPeripheralSimulation::_RECEIVEBYTETRAP,
				"Serial Receive Byte",
				MCHEmul::Address ({ 0x52, 0xe2 }, false),
				MCHEmul::Address ({ 0xe7, 0xe1 }, false),
				{ 0x78, 0xa9, 0x00 } 
			},
			{ 
				COMMODORE::SerialIOPeripheralSimulation::_READYTRAP,
				"Serial Ready",
				MCHEmul::Address ({ 0x16, 0xe2 }, false),
				MCHEmul::Address ({ 0xe7, 0xe1 }, false),
				{ 0x24, 0x01, 0x70 } 
			},
			{ 
				COMMODORE::SerialIOPeripheralSimulation::_READYTRAP2,
				"Serial Ready 2",
				MCHEmul::Address ({ 0xd4, 0xe2 }, false),
				MCHEmul::Address ({ 0xe7, 0xe1 }, false),
				{ 0xa5, 0x01, 0xc5 } 
	}}};

// ---
C264::StandardSerialPrinterSimulation::StandardSerialPrinterSimulation (int id, unsigned char dN,
		MCHEmul::MatrixPrinterEmulation* mPE)
	: COMMODORE::SerialPrinterPeripheralSimulation (id, dN, _DEFINITION,
		{ { "Name", "Standard Serial Printer" },
		  { "Manufacturer", "ICF Printer Software Simulation" } },
		mPE)
{
	assert (isDeviceNumberValid (deviceNumber ()));
}
