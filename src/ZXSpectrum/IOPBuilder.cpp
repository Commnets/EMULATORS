#include <ZXSpectrum/IOPBuilder.hpp>
#include <ZXSpectrum/Cartridge.hpp>
#include <ZXSpectrum/Datasette.hpp>
#include <ZXSpectrum/StdPrinter.hpp>
#include <ZXSpectrum/ZXSpectrum.hpp>

// ---
MCHEmul::IOPeripheral* ZXSPECTRUM::IOPeripheralBuilder::createPeripheral 
	(int id, MCHEmul::Computer* c, const MCHEmul::Attributes& prms) const
{
	assert (dynamic_cast <ZXSPECTRUM::SinclairZXSpectrum*> (c) != nullptr);

	MCHEmul::IOPeripheral* result = nullptr;

	if (id == MCHEmul::Typewriter::_ID)
		result = new MCHEmul::Typewriter (4 /** 1/25 * 4 = 1 every 160 miliseconds. */, c);
	else if (id == ZXSPECTRUM::Cartridge::_ID)
		result = new ZXSPECTRUM::Cartridge;
	// https://en.wikipedia.org/wiki/ZX81
	else if (id == SINCLAIR::Datasette::_ID)
		/** In the Datasette for the ZXSpectrum, every logical bit (1 or 0) is codified with a train of pulses. \n
			For both just two pulses will be needed. One with the tone "up", and the other one with the tone "down". \n
			The different between a logical 1 and a logical 0 is the duration of the that pulses. \n
			For a logical 1 the pulse "up" is 1710 T-States long and same for the pulse "down" = 0,977 ms = 1,024 kHz. \n
			For a logical 0 the pulse "up" is 855 T-States long and same for the pulse "down" = 0,488 ms = 2,048 kHz.
			Following Nyquist-Shannon's theorem, the minimum sampling frequency to reproduce 
			the signal must be double the maximum frequency minimum. so, in this case, 
			the minimum sampling frequency is 4,096 kHz = 855/2 T.States = 427 T-States \n
			However that sampling frequency will be modified accorfind with the parameters of the TZX file defining it. */
		result = new SINCLAIR::Datasette (427);
	else if (id == ZXSPECTRUM::DatasetteInjection::_ID)
		result = new ZXSPECTRUM::DatasetteInjection (static_cast <ZXSPECTRUM::SinclairZXSpectrum*> (c) -> type ());
	else if (id == ZXSPECTRUM::ThermalPrinterSimulation::_ID)
	{
		// IMPORTANT: 
		// This basic one is destroyed in getDataPrinterFrom if is not finally used...
		std::string pF = "Printer.txt"; // Default output file name...
		MCHEmul::MatrixPrinterEmulation* mPE = nullptr;
		std::tie (pF, mPE) = getDataPrinterFrom 
			(prms, std::make_tuple 
				([](unsigned char chr) -> unsigned char 
					{ 
						unsigned char result = ' ';

						// Control codes, with no translation...
						if (chr < 0x20)
							return (result);

						// 0x20..0x7f: Visible ASCII
						// The base matches with ASCII for these codes except
						// specific SPECTRUM chars like £ (0x60) y © (0x7f).
						if (chr <= 0x7f)
						{
							if (chr == 0x60) result = '£';
							else if (chr == 0x7f ) result = '©';
							else result = chr;
						}

						// The rest of the codes will be printed out like an space...
						return (result);
					},
				 ZXSPECTRUM::ThermalPrinterSimulation::_CONFIGURATION, 
				 pF, // these two last will also return...
				 mPE));
		if (mPE == nullptr) mPE = new MCHEmul::BasicMatrixPrinterEmulation (32, pF); // Not usual, but just to avoid a crash later!
		result = new ZXSPECTRUM::ThermalPrinterSimulation (mPE);
	}
	else
		result = SINCLAIR::IOPeripheralBuilder::createPeripheral (id, c, prms);

	// Take care, it could be null...
	return (result);
}
