#include <C64/IOPBuilder.hpp>
#include <C64/Cartridge.hpp>
#include <C64/C64.hpp>

// ---
MCHEmul::IOPeripheral* C64::IOPeripheralBuilder::createPeripheral 
	(int id, MCHEmul::Computer* c, const MCHEmul::Attributes& prms) const
{
	MCHEmul::IOPeripheral* result = nullptr;

	if (id == C64::Cartridge::_ID)
		result = new C64::Cartridge;
	else if (id == COMMODORE::Datasette1530P::_ID)
		/** CN2 datasette keeps three types on short pulses in NTSC:
			long pulses = 1488Hz (672us length),
			medium pulses = 1953Hz (512us length),
			short pulses = 2840Hz (352us length).
			Following Fourier series theory, the sampling period must be half the shortest one,
			or said in the other way around, the sampling frequency must be double that the hightest one = 5680Hz (176us). \n
			Doing a similar calculus for PAL, the sampling frequency should be 5474Hz (182us). */
		result = new COMMODORE::Datasette1530P 
			(dynamic_cast <C64::Commodore64*> (c) -> visualSystem () == 
				C64::Commodore64::VisualSystem::_PAL ? 182 : 176);
	else
		result = COMMODORE::IOPeripheralBuilder::createPeripheral (id, c, prms);

	// Take care, it could be null...
	return (result);
}
