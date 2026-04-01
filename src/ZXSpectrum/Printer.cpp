#include <ZXSpectrum/Printer.hpp>

// ---
ZXSPECTRUM::ThermalPrinterSimulation::ThermalPrinterSimulation (MCHEmul::MatrixPrinterEmulation* mPE)
	: ZXSPECTRUM::EdgeConnectorPeripheral (_ID, 
		{ { "Name", "ZXSpectrum Printer" },
		  { "Manufacturer", "ICF Standard Printer Simulation" } }),
	  _printTrap { 
		0, 
		"PRINTER", 
		MCHEmul::Address ({ 0x51, 0x08 }, false),
		MCHEmul::Address ({ 0x68, 0x08 }, false),
		{  } // No finger tip defined...
	  },
	 _emulation (mPE)
{
	assert (_emulation != nullptr);

	setClassName ("ZXSpectrumPrinterSimulation");
}

// ---
bool ZXSPECTRUM::ThermalPrinterSimulation::executeCommand (int cId, const MCHEmul::Strings& prms)
{
	bool result = false;

	switch (cId)
	{
		case 1: // Flush the printer...
			{
				result = true;
			}
			break;

		case 2: // Cut the page...
			{
				_emulation -> closeCurrentPage ();

				result = true;
			}
			break;

		default:
			// It shouldn't be here...
			break;
	}

	return (result);
}

// ---
bool ZXSPECTRUM::ThermalPrinterSimulation::simulate (MCHEmul::CPU* cpu)
{
	if (cpu -> programCounter ().internalRepresentation () == _printTrap._addressIn.value ())
	{
		_IFDEBUG debugSimulation (cpu);

		// Empty any pending set action...
		// Because a trap is breaking the normal flow of the code...
		// so no set commands pendings can be left!
		MCHEmul::Memory::configuration ().executeMemorySetCommandsBuffered ();

		// TODO

		cpu -> programCounter ().setAddress (_printTrap._addressOut);
	}

	return (true);
}

// ---
MCHEmul::InfoStructure ZXSPECTRUM::ThermalPrinterSimulation::getInfoStructure () const
{
	MCHEmul::InfoStructure result = 
		std::move (MCHEmul::IOPeripheral::getInfoStructure ());

	result.add ("TRAP",	_printTrap.asString ());
	result.add ("MatrixPrinterEmulation", std::move (_emulation -> getInfoStructure ()));
	result.add ("COMMANDS", MCHEmul::concatenateStrings (commandDescriptions (),"\n"));

	return (result);
}

// ---
void ZXSPECTRUM::ThermalPrinterSimulation::debugSimulation (MCHEmul::CPU* cpu)
{
	assert (_deepDebugFile != nullptr);

	_deepDebugFile -> writeCompleteLine ("ZX81PRT", cpu -> clockCycles (), "Printer trap executed");
}
