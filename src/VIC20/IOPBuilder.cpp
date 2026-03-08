#include <VIC20/IOPBuilder.hpp>
#include <VIC20/Cartridge.hpp>
#include <VIC20/1530Datasette.hpp>
#include <VIC20/StdSerialPrinter.hpp>
#include <VIC20/SerialIONotPresent.hpp>
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
		result = new VIC20::Datasette1530Injection;
	else if (id >= VIC20::StandardSerialPrinterSimulation::_DEFAULTID && 
			 (id <= VIC20::StandardSerialPrinterSimulation::_DEFAULTID + 1)) // 2 possible printers connected....
		{
			std::string pF = "Printer.txt"; // Default output file name...
			unsigned char dN = VIC20::StandardSerialPrinterSimulation::_DEFAULTDEVICENUMBER;
			MCHEmul::MatrixPrinterEmulation* mPE = nullptr;
			std::tie (pF, dN, mPE) = getDataPrinterFrom (prms, std::make_tuple (pF, dN, mPE));
			if (mPE == nullptr) mPE = new MCHEmul::BasicMatrixPrinterEmulation (80, pF); // Not usual, but just to avoid a crash later!
			if (VIC20::StandardSerialPrinterSimulation::isDeviceNumberValid (dN)) // Only if it is valid...
				result = new VIC20::StandardSerialPrinterSimulation (id, dN, mPE);
			else
				delete mPE; // The emulation temporaly created has not been usedm and must be deleted!
		}
	else
		result = COMMODORE::IOPeripheralBuilder::createPeripheral (id, c, prms);

	// Take care, it could be null...
	return (result);
}
