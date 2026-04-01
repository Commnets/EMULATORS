#include <COMMODORE/SerialPrinters.hpp>

// ---
COMMODORE::SerialPrinterPeripheralSimulation::SerialPrinterPeripheralSimulation 
		(int id, unsigned char dN, 
		 const COMMODORE::SerialIOPeripheralSimulation::Definition& dt,
		 const MCHEmul::Attributes& attrs, 
		 MCHEmul::MatrixPrinterEmulation* mPE)
	: COMMODORE::SerialIOPeripheralSimulation (id, dN, dt, attrs),
	  _emulation (mPE)
{ 
	// It cannot be nullptr...
	assert (_emulation != nullptr);

	setClassName ("SerialPrinterSimulation");
}

// ---
COMMODORE::SerialPrinterPeripheralSimulation::~SerialPrinterPeripheralSimulation ()
{ 
	finalize ();

	// No longer valid...
	delete (_emulation);
} 

// ---
bool COMMODORE::SerialPrinterPeripheralSimulation::executeCommand (int cId, const MCHEmul::Strings& prms)
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
MCHEmul::InfoStructure COMMODORE::SerialPrinterPeripheralSimulation::getInfoStructure () const
{
	MCHEmul::InfoStructure result = std::move 
		(COMMODORE::SerialIOPeripheralSimulation::getInfoStructure ());

	// Add info about the emulation...
	result.add ("MatrixPrinterEmulation", std::move (_emulation -> getInfoStructure ()));
	result.add ("COMMANDS", MCHEmul::concatenateStrings (commandDescriptions (),"\n"));

	return (result);
}

// ---
unsigned char COMMODORE::SerialPrinterPeripheralSimulation::listen (MCHEmul::CPU* cpu, const MCHEmul::UByte& b)
{
	_emulation -> desactivateAllFunctions ();

	return (definition ()._okResult);
}

// ---
unsigned char COMMODORE::SerialPrinterPeripheralSimulation::sendByte (MCHEmul::CPU* cpu, const MCHEmul::UByte& b)
{
	_emulation -> printChar (b.value ());

	return (definition ()._okResult); // Everything ok...
}

// ---
unsigned char COMMODORE::SerialPrinterPeripheralSimulation::openChannel (MCHEmul::CPU* cpu, const MCHEmul::UByte& ch)
{
	_emulation -> activateFunction (ch.value ());

	return (definition ()._okResult); // Everything ok
}

// ---
unsigned char COMMODORE::SerialPrinterPeripheralSimulation::closeChannel (MCHEmul::CPU* cpu, const MCHEmul::UByte& ch)
{
	_emulation -> desactivateFunction (ch.value ());

	return (definition ()._okResult); // Everything ok
}
