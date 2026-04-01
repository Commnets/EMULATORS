#include <ZXSpectrum/Printer.hpp>
#include <FZ80/CZ80.hpp>

const MCHEmul::MatrixPrinterEmulation::Configuration 
	ZXSPECTRUM::ThermalPrinterSimulation::_CONFIGURATION = 
	MCHEmul::MatrixPrinterEmulation::Configuration (
		(unsigned char) 6, (unsigned char) 7,
		MCHEmul::MatrixPrinterEmulation::Configuration::CharSetDefinition 
			({
			}),
		// max width in chars, this printer can print up!
		(unsigned short) 80, 
		// usual height in chars (when the paper is 11 inches height), 
		// although it has to be adjusted to the the height in inches of the paper...
		(unsigned short) 66, 
		(unsigned char) 0x0d, (unsigned char) 0x20, (unsigned char) 0x09,
		(unsigned short) 4);

size_t ZXSPECTRUM::ThermalPrinterSimulation::_PR_CC_POS = 0x46;
size_t ZXSPECTRUM::ThermalPrinterSimulation::_PR_FLAG2_POS = 0x30;
MCHEmul::Address ZXSPECTRUM::ThermalPrinterSimulation::_PRBUFF = MCHEmul::Address ({ 0x00, 0x5b }, false);
const size_t ZXSPECTRUM::ThermalPrinterSimulation::_BLEN = 0x100;

// ---
ZXSPECTRUM::ThermalPrinterSimulation::ThermalPrinterSimulation (MCHEmul::MatrixPrinterEmulation* mPE)
	: ZXSPECTRUM::EdgeConnectorPeripheral (_ID, 
		{ { "Name", "ZXSpectrum Printer" },
		  { "Manufacturer", "ICF Standard Printer Simulation" } }),
	  _printTrap { 
		0, 
		"PRINTER", // Really it does the COPY-BUFF + CLEAR-PRB 
		MCHEmul::Address ({ 0xcd, 0x0e }, false),
		MCHEmul::Address ({ 0xd9, 0x0d }, false),
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

		// Get the value of the register IY as an address...
		MCHEmul::Address iYA = static_cast <FZ80::CZ80*> (cpu) -> addressFromRegisters
			(static_cast <FZ80::CZ80*> (cpu) -> iyRegister ());
		// Gets the number of characters to print out...
		size_t nc = (size_t) cpu -> memoryRef () -> value (iYA + _PR_CC_POS).value ();

		// Empty the buffer
		MCHEmul::UByte vb = MCHEmul::UByte::_0;
		for (size_t i = 0; i < nc;
			_emulation -> printChar (cpu -> memoryRef () -> value (_PRBUFF + i++).value ()));
		_emulation -> printChar (_emulation -> configuration ()._charNewLine); 

		// Clear the buffer...
		for (size_t i = 0; i < _BLEN; 
			cpu -> memoryRef () -> set (_PRBUFF + i++, MCHEmul::UByte::_0));
		// Set the counter of positions to 0
		cpu -> memoryRef () -> set (iYA + _PR_CC_POS, MCHEmul::UByte::_0);
		// FLAG 2 bit 1 to indicate that the buffer is empty...
		cpu -> memoryRef () -> set (iYA + _PR_FLAG2_POS, 
			cpu -> memoryRef () -> value (iYA + _PR_FLAG2_POS) & ~0x02);
		// The register C must be set to 0x21...
		static_cast <FZ80::CZ80*> (cpu) -> cRegister ().set ({ 0x21 });

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
