#include <COMMODORE/IOPBuilder.hpp>
#include <COMMODORE/UserIOPeripherals.hpp>
#include <COMMODORE/ExpansionPeripherals.hpp>
#include <COMMODORE/1530Datasette.hpp>
#include <C64/Cartridge.hpp>

// ---
MCHEmul::IOPeripheral* COMMODORE::IOPeripheralBuilder::createPeripheral 
	(int id, MCHEmul::Computer* c, const MCHEmul::Attributes& prms) const
{
	MCHEmul::IOPeripheral* result = nullptr;

	if (id == COMMODORE::NoUserIOPeripheral::_ID)
		result = new COMMODORE::NoUserIOPeripheral;
	else if (id == MCHEmul::NoDatasettePeripheral::_ID)
		result = new MCHEmul::NoDatasettePeripheral;
	else if (id == COMMODORE::NoExpansionPeripheral::_ID)
		result = new COMMODORE::NoExpansionPeripheral;
	else if (id == COMMODORE::Datasette1530::_ID)
		// https://archive.org/details/COMPUTEs_VIC-20_and_Commodore_64_Tool_Kit_Kernal_1985_COMPUTE_Publications_a (page 275).
		/** To write a bit using the STANDARD KERNEL ROUTINE of the CMB64/VIC20/... that is the same,
			uses 3284 clock cycles (read in internet). \n
			As CPU-PAL speed is in C64 = 985.248 c/s, the bit/s = baudios of the standard encoding is = 300 b/s.
			Doing a similar calculus for NTSC system = 311 b/s. \n
			It means that when changing the ROM to use another loading system, the construction parameters must be changed. */
		result = new COMMODORE::Datasette1530 (3284);
	else if (id == COMMODORE::Datasette1530P::_ID)
		/** CN2 datasette keeps thre types on short pulses:
			long pulses = 1488Hz (672us length),
			medium pulses = 1953Hz (512us length),
			short pulses = 2840Hz (352us length).
			Following Fourier series theory, the sampling period must be half the shortest one,
			or said in the other way around, 
			the sampling frequency must be double that the hightest one = 5680Hz (176us) .*/
		result = new COMMODORE::Datasette1530P (176);

	// Take care, it could be null...
	return (result);
}
