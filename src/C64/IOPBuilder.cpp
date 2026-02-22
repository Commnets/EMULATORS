#include <C64/IOPBuilder.hpp>
#include <C64/Cartridge.hpp>
#include <C64/1530Datasette.hpp>
#include <C64/1541Disk.hpp>
#include <C64/StdSerialPrinter.hpp>
#include <C64/C64.hpp>

// ---
MCHEmul::IOPeripheral* C64::IOPeripheralBuilder::createPeripheral 
	(int id, MCHEmul::Computer* c, const MCHEmul::Attributes& prms) const
{
	MCHEmul::IOPeripheral* result = nullptr;

	if (id == MCHEmul::Typewriter::_ID)
		result = new MCHEmul::Typewriter (3 /** 1/25 * 3 = 1 every 120 miliseconds. */, c);
	else if (id == C64::Cartridge::_ID)
		result = new C64::Cartridge;
	else if (id == COMMODORE::Datasette1530::_ID)
		{
			/** CN2 datasette keeps three types on short pulses in NTSC:
				long pulses = 1488Hz (672us length),
				medium pulses = 1953Hz (512us length),
				short pulses = 2840Hz (352us length).
				Following Fourier series theory, the sampling period must be half the shortest one,
				or said in the other way around, the sampling frequency must be double that the hightest one = 5680Hz (176us). \n
				Doing a similar calculus for PAL, the sampling frequency should be 5474Hz (182us). */
			unsigned int cR = (dynamic_cast <C64::Commodore64*> (c) -> visualSystem () ==
				C64::Commodore64::VisualSystem::_PAL) ? 182 : 176;
			result = new COMMODORE::Datasette1530
				(cR, new COMMODORE::Datasette1530::TAPFileFormatImplementation (cR));
		}
	else if (id == COMMODORE::Datasette1530Injection::_ID)
		/** When the routines of the kernal are "overpassed". */
		result = new C64::Datasette1530Injection;
	else if (id == C64::Disk1541Simulation::_DEFAULTID)
		{
			// There might be several units connected to the serial port with different device numbers
			// It is also possible to select the name of the output file...
			unsigned char dN = C64::Disk1541Simulation::_DEFAULTDEVICENUMBER;
			if (prms.size () == 1) dN = (unsigned char) std::atoi ((*prms.find ("0")).second.c_str ());
			if (C64::Disk1541Simulation::isDeviceNumberValid (dN)) // Only if it is valid...
				result = new C64::Disk1541Simulation (id, dN); // ...otherwise it will nullptr, and not created...
		}
	else if (id >= C64::StandardSerialPrinterSimulation::_DEFAULTID && 
			 (id <= C64::StandardSerialPrinterSimulation::_DEFAULTID + 1)) // 2 possible printers connected....
		{
			std::string pF = "Printer.txt"; // Default output file name...
			unsigned char dN = C64::StandardSerialPrinterSimulation::_DEFAULTDEVICENUMBER;
			MCHEmul::MatrixPrinterEmulation* mPE = nullptr;
			std::tie (pF, dN, mPE) = getDataPrinterFrom (prms, std::make_tuple (pF, dN, mPE));
			if (mPE == nullptr) mPE = new MCHEmul::BasicMatrixPrinterEmulation (80, pF); // Not usual, but just to avoid a crash later!
			if (C64::StandardSerialPrinterSimulation::isDeviceNumberValid (dN)) // Only if it is valid...
				result = new C64::StandardSerialPrinterSimulation (id, dN, mPE);
			else
				delete mPE; // The emulation temporaly created has not been usedm and must be deleted!
		}
	else
		result = COMMODORE::IOPeripheralBuilder::createPeripheral (id, c, prms);

	// Take care, it could be null...
	return (result);
}
