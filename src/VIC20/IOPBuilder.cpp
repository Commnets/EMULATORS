#include <VIC20/IOPBuilder.hpp>
#include <VIC20/Cartridge.hpp>
#include <VIC20/VIC20.hpp>

// ---
MCHEmul::IOPeripheral* VIC20::IOPeripheralBuilder::createPeripheral 
	(int id, MCHEmul::Computer* c, const MCHEmul::Attributes& prms) const
{
	MCHEmul::IOPeripheral* result = nullptr;

	if (id == MCHEmul::Typewriter::_ID)
		result = new MCHEmul::Typewriter (3 /** 1/25 * 3 = 1 every 120 miliseconds. */, c);
	else if (id == VIC20::Cartridge::_ID)
		result = new VIC20::Cartridge;
	else if (id == COMMODORE::Datasette1530::_ID)
		{
			/** CN2 datasette keeps three types on short pulses in NTSC:
				long pulses = 1488Hz (672us length = 336 up & 336 down),
				medium pulses = 1953Hz (512us length),
				short pulses = 2840Hz (352us length).
				Following Fourier's signal analysis theory, the sampling ideal period must be half the shortest one,
				or said in the other way around, the sampling frequency must be double that the hightest one = 5681Hz (176us). \n
				Doing a similar calculus for PAL, the sampling frequency should be 5473,4Hz (182,7us). \n
				With TurboLoaders only short and long pulses are used, but still this definition will be useful. */
			unsigned int cR = (dynamic_cast <VIC20::CommodoreVIC20*> (c) -> visualSystem () == 
				VIC20::CommodoreVIC20::VisualSystem::_PAL) ? 183 : 176;
			result = new COMMODORE::Datasette1530
				(cR, new COMMODORE::Datasette1530::TAPFileFormatImplementation (cR));
		}
	else if (id == COMMODORE::Datasette1530Injection::_ID)
		/** When the routines of the kernal are "overpassed". */
		result = new COMMODORE::Datasette1530Injection
			(
				{	
					// General definitions for the injection (@see Datasette1530Injection class too)...
					MCHEmul::Address ({ 0xb2 ,0x00 }, false), // Start of the tape buffer...
					MCHEmul::Address ({ 0x90, 0x00 }, false), // The casette routines use this space to track the operations and status...
					MCHEmul::Address ({ 0x93, 0x00 }, false), // 0 = Load, 1 = Verify as the same kernel rountine could do both functions...
					MCHEmul::Address ({ 0x9f, 0x02 }, false), // Place when temporal IRQ value are kept...
					0x0000,									  // Must the IRQ be taken into account?
					MCHEmul::Address ({ 0xc1, 0x00 }, false), // Pointer (0xc1, 0xc2) to the beginning of the RAM being loaded...
					MCHEmul::Address ({ 0xae, 0x00 }, false), // Pointer (0xae, 0xaf) to the end of the load operation...
					MCHEmul::Address ({ 0x77, 0x02 }, false), // Keyboard buffer start address...
					MCHEmul::Address ({ 0xc6, 0x00 }, false), // Number of characters in the keyboard buffer... (to simulate "run")
					// Traps...
					{	// Trap to the routine finding the right header (when "load" is done by name)...
						{ COMMODORE::Datasette1530Injection::_FINDHEADERTRAP, 
						  "Find Header", 
						  MCHEmul::Address ({ 0xb2, 0xf7 }, false), 
						  MCHEmul::Address ({ 0xb5, 0xf7 }, false),
						  { 0x20, 0xc0, 0xf8 } },
						// Traps to the routine to get the content of the file once it has been found...
						{ COMMODORE::Datasette1530Injection::_RECEIVEDATATRAP, 
						  "Receive",
						  MCHEmul::Address ({ 0x0b, 0xf9 }, false), 
						  MCHEmul::Address ({ 0xcf, 0xfc }, false),
						  { 0x20, 0xfb, 0xfc } }
					}
				}
			);
	else
		result = COMMODORE::IOPeripheralBuilder::createPeripheral (id, c, prms);

	// Take care, it could be null...
	return (result);
}
