#include <ZX81/Printer.hpp>
#include <FZ80/CZ80.hpp>

MCHEmul::Address ZX81::ThermalPrinterSimulation::_PR_CC	= MCHEmul::Address ({ 0x38, 0x40 }, false);
MCHEmul::Address ZX81::ThermalPrinterSimulation::_PRBUFF = MCHEmul::Address ({ 0x3c, 0x40 }, false);
const MCHEmul::UByte ZX81::ThermalPrinterSimulation::_EOL = MCHEmul::UByte (0x76);
const MCHEmul::UByte ZX81::ThermalPrinterSimulation::_IBP = MCHEmul::UByte (0x3c);
const MCHEmul::UByte ZX81::ThermalPrinterSimulation::_LBP = MCHEmul::UByte (0x5c); 
const size_t ZX81::ThermalPrinterSimulation::_BLEN = (size_t) (_LBP.value () - _IBP.value ());

// ---
ZX81::ThermalPrinterSimulation::ThermalPrinterSimulation (MCHEmul::MatrixPrinterEmulation* mPE)
	: ZX81::EdgeConnectorPeripheral (_ID, 
		{ { "Name", "ZX81 Printer" },
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

	setClassName ("ZX81PrinterSimulation");
}

// ---
bool ZX81::ThermalPrinterSimulation::executeCommand (int cId, const MCHEmul::Strings& prms)
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
bool ZX81::ThermalPrinterSimulation::simulate (MCHEmul::CPU* cpu)
{
	if (cpu -> programCounter ().internalRepresentation () == _printTrap._addressIn.value ())
	{
		_IFDEBUG debugSimulation (cpu);

		// Empty any pending set action in the memory...
		// Because a trap is breaking the normal flow of the code...
		// so no set commands pendings can be left...if any!
		MCHEmul::Memory::configuration ().executeMemorySetCommandsBuffered ();

		// Gets the character to be be printed out...
		// ..that comes into the register A
		MCHEmul::UByte aR = static_cast <FZ80::CZ80*> (cpu) -> aRegister ().values ()[0];
		// Gets also the LSB of the memory address where to insert that new character...
		// That is pointed out in the memory address 0x4038 (masking the bit 7)
		// The bt 7 indicates whether the buffer is or not empty...
		MCHEmul::UByte pI = cpu -> memoryRef () -> value (_PR_CC) & 0x7f;

		// If either the byte received is 0x76 (= enter)...
		// ...or the position where to write the next character is
		// already the max allowed (= 0x5c) the buffer is emptied to the printer...
		// the buffer cleared, and the pointer to the next element moved to the first position!
		if (aR == _EOL || pI == _LBP)
		{
			// Empty the buffer 
			// ...and forcing always a new line at the end...
			for (size_t i = 0; i < (size_t) ((pI.value () - _IBP.value ()));
				_emulation -> printChar // Using the standard method of the emulation...
					(cpu -> memoryRef () -> value (_PRBUFF + i++).value ())); 
			_emulation -> printChar ('\n'); 

			// Empty the buffer...
			// the last element is always the end on line...
			for (size_t i = 0; i < _BLEN; 
				cpu -> memoryRef () -> set (_PRBUFF + i++, MCHEmul::UByte::_0));
			cpu -> memoryRef () -> set (_PRBUFF + _BLEN, _EOL);
			// And updates the pointer to the new element...
			cpu -> memoryRef () -> set (_PR_CC, _IBP | 0x80 /** Always with the bit 7 on = meaning empty */);
		}
		// In any other circunstance, the buffer is feeded with the new element...
		// ...and the position in the buffer updated...
		else
		{
			cpu -> memoryRef () -> set (_PRBUFF + 
				((size_t) pI.value () - (size_t) _IBP.value ()), aR);
			cpu -> memoryRef () -> set (_PR_CC, pI.value () + 1);
		}

		// Finally the return position is set up...
		cpu -> programCounter ().setAddress (_printTrap._addressOut);
	}

	return (true);
}

// ---
MCHEmul::InfoStructure ZX81::ThermalPrinterSimulation::getInfoStructure () const
{
	MCHEmul::InfoStructure result = 
		std::move (MCHEmul::IOPeripheral::getInfoStructure ());

	result.add ("TRAP",	_printTrap.asString ());
	result.add ("MatrixPrinterEmulation", std::move (_emulation -> getInfoStructure ()));
	result.add ("COMMANDS", MCHEmul::concatenateStrings (commandDescriptions (),"\n"));

	return (result);
}

// ---
void ZX81::ThermalPrinterSimulation::debugSimulation (MCHEmul::CPU* cpu)
{
	assert (_deepDebugFile != nullptr);

	_deepDebugFile -> writeCompleteLine ("ZX81PRT", cpu -> clockCycles (), "Printer trap executed");
}
