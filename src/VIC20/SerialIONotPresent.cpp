#include <VIC20/SerialIONotPresent.hpp>

const COMMODORE::SerialIOPeripheralSimulation::Definition 
	VIC20::SerialNotPresentIOPeripheralSimulation::_DEFINITION = {
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
	}}};
