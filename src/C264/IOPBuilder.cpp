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
		{
			/** CN2 datasette keeps three types on short pulses in NTSC:
				long pulses = 1488Hz (672us length),
				medium pulses = 1953Hz (512us length),
				short pulses = 2840Hz (352us length).
				Following Fourier series theory, the sampling period must be half the shortest one,
				or said in the other way around, the sampling frequency must be double that the hightest one = 5680Hz (176us). \n
				Doing a similar calculus for PAL, the sampling frequency should be 5474Hz (182us). */
			unsigned int cR = (dynamic_cast <C264::Commodore264*> (c) -> visualSystem () ==
				C264::Commodore264::VisualSystem::_PAL) ? 182 : 176;
			result = new C264::Datasette1531
				(cR, new C264::Datasette1531::TAPFileFormatImplementation (cR));
		}
	else if (id == COMMODORE::Datasette1530Injection::_ID)
		/** When the routines of the kernal are "overpassed". */
		result = new C264::Datasette1531Injection;
	else
		result = COMMODORE::IOPeripheralBuilder::createPeripheral (id, c, prms);

	// Take care, it could be null...
	return (result);
}
