#include <C264/IOPBuilder.hpp>
#include <C264/Cartridge.hpp>
#include <C264/1531Datasette.hpp>
#include <C264/StdSerialPrinter.hpp>
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
	else if (id >= C264::StandardSerialPrinterSimulation::_DEFAULTID && 
			 (id <= C264::StandardSerialPrinterSimulation::_DEFAULTID + 1)) // 2 possible printers connected....
		{
			std::string pF = "Printer.txt"; // Default output file name...
			unsigned char dN = C264::StandardSerialPrinterSimulation::_DEFAULTDEVICENUMBER;
			MCHEmul::MatrixPrinterEmulation* mPE = nullptr;
			std::tie (pF, dN, mPE) = getDataPrinterFrom (prms, std::make_tuple (pF, dN, mPE));
			if (mPE == nullptr) mPE = new MCHEmul::BasicMatrixPrinterEmulation (80, pF); // Not usual, but just to avoid a crash later!
			if (C264::StandardSerialPrinterSimulation::isDeviceNumberValid (dN)) // Only if it is valid...
				result = new C264::StandardSerialPrinterSimulation (id, dN, mPE);
			else
				delete mPE; // The emulation temporaly created has not been usedm and must be deleted!
		}
	else
		result = COMMODORE::IOPeripheralBuilder::createPeripheral (id, c, prms);

	// Take care, it could be null...
	return (result);
}
