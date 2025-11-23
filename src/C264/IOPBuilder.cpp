#include <C264/IOPBuilder.hpp>
#include <C264/Cartridge.hpp>
#include <C264/1531Datasette.hpp>
#include <C264/C264.hpp>


// ---
MCHEmul::IOPeripheral* C264::IOPeripheralBuilder::createPeripheral 
	(int id, MCHEmul::Computer* c, const MCHEmul::Attributes& prms) const
{
	MCHEmul::IOPeripheral* result = nullptr;

	if (id == MCHEmul::Typewriter::_ID)
		result = new MCHEmul::Typewriter (3 /** 1/25 * 3 = 1 every 120 miliseconds. */, c);
	else if (id == C264::Cartridge::_ID)
		result = new C264::Cartridge;
	else if (id == COMMODORE::Datasette1530::_ID)
		/** CN2 datasette keeps three types on short pulses in NTSC:
			long pulses = 1488Hz (672us length),
			medium pulses = 1953Hz (512us length),
			short pulses = 2840Hz (352us length).
			Following Fourier series theory, the sampling period must be half the shortest one,
			or said in the other way around, the sampling frequency must be double that the hightest one = 5680Hz (176us). \n
			Doing a similar calculus for PAL, the sampling frequency should be 5474Hz (182us). */
		result = new COMMODORE::Datasette1530 
			(dynamic_cast <C264::Commodore264*> (c) -> visualSystem () == 
				C264::Commodore264::VisualSystem::_PAL ? 182 : 176);
	else if (id == COMMODORE::Datasette1530Injection::_ID)
		/** When the routines of the kernal are "overpassed". */
		result = new C264::Datasette1531Injection
			(
				{	
					// General definitions for the injection (@see Datasette1531Injection class too)...
					MCHEmul::Address ({ 0x33 ,0x03 }, false), // Start of the tape buffer...
					MCHEmul::Address ({ 0x90, 0x00 }, false), // The casette routines use this space to track the operations and status...
					MCHEmul::Address ({ 0x93, 0x00 }, false), // 0 = Load, 1 = Verify as the same kernel rountine could do both functions...
					MCHEmul::Address ({ 0x00, 0x00 }, false), // Place when temporal IRQ value are kept...
					0x0000,									  // Must the IRQ be taken into account?
					MCHEmul::Address ({ 0xb4, 0x00 }, false), // Pointer (0xc1, 0xc2) to the beginning of the RAM being loaded...
					MCHEmul::Address ({ 0x9d, 0x00 }, false), // Pointer (0xae, 0xaf) to the end of the load operation...
					MCHEmul::Address ({ 0x27, 0x05 }, false), // Keyboard buffer start address...
					MCHEmul::Address ({ 0xef, 0x00 }, false), // Number of characters in the keyboard buffer... (to simulate "run")
					// Traps...
					{	// Trap to the routine finding the right header (when "load" is done by name)...
						{ COMMODORE::Datasette1530Injection::_FINDHEADERTRAP,
						  "Find Header", 
						  MCHEmul::Address ({ 0xcc, 0xe9 }, false),
						  MCHEmul::Address ({ 0xcf, 0xe9 }, false), 
						  { 0x20, 0xd3, 0xe8 } },
						// Traps to the routine to get the content of the file once it has been found...
						{ COMMODORE::Datasette1530Injection::_RECEIVEDATATRAP,
						  "Receive",
						  MCHEmul::Address ({ 0x4b, 0xe7 }, false),
						  MCHEmul::Address ({ 0xc7, 0xe8 }, false),
						  { 0xba, 0x8e, 0xbe } }
					}
				}
			);
	else
		result = COMMODORE::IOPeripheralBuilder::createPeripheral (id, c, prms);

	// Take care, it could be null...
	return (result);
}
