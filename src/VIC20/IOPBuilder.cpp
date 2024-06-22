#include <VIC20/IOPBuilder.hpp>
#include <VIC20/Cartridge.hpp>
#include <VIC20/VIC20.hpp>

// ---
MCHEmul::IOPeripheral* VIC20::IOPeripheralBuilder::createPeripheral 
	(int id, MCHEmul::Computer* c, const MCHEmul::Attributes& prms) const
{
	MCHEmul::IOPeripheral* result = nullptr;

	if (id == VIC20::Cartridge::_ID)
		result = new VIC20::Cartridge;
	else if (id == COMMODORE::Datasette1530P::_ID)
		/** CN2 datasette keeps three types on short pulses in NTSC:
			long pulses = 1488Hz (672us length = 336 up & 336 down),
			medium pulses = 1953Hz (512us length),
			short pulses = 2840Hz (352us length).
			Following Fourier's signal analysis theory, the sampling ideal period must be half the shortest one,
			or said in the other way around, the sampling frequency must be double that the hightest one = 5681Hz (176us). \n
			Doing a similar calculus for PAL, the sampling frequency should be 5473,4Hz (182,7us). \n
			With TurboLoaders only short and long pulses are used, but still this definition will be useful. */
		result = new COMMODORE::Datasette1530P
			(dynamic_cast <VIC20::CommodoreVIC20*> (c) -> visualSystem () == 
				VIC20::CommodoreVIC20::VisualSystem::_PAL ? 183 : 176);
	else
		result = COMMODORE::IOPeripheralBuilder::createPeripheral (id, c, prms);

	// Take care, it could be null...
	return (result);
}
