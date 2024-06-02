#include <CORE/TestCPUSpeed.hpp>
#include <CORE/Memory.hpp>
#include <CORE/Stack.hpp>
#include <CORE/StdCommands.hpp>
#include <chrono>
#include <random>

// ---
unsigned int MCHEmul::TestCPUSpeed::clocksInASecondExcutingInstruction (const MCHEmul::UBytes& b, bool sM) const
{
	MCHEmul::UInt iCU (b.MSUBytes (_cpu -> architecture ().instructionLength ()));
	unsigned int iC = iCU.asUnsignedInt ();

	if (!_cpu -> existsInstruction (iC))
	{
		std::cout << "Error in definition of instruction:" << std::endl;
		std::cout << "Instruction with code " << iCU << " not in the list" << std::endl;
		std::cout << "The code assigned is not the code used in the definition" << std::endl;
		
		return (0);
	}

	MCHEmul::Instruction* inst = _cpu -> instruction (iC);
	unsigned int clks = 0;
	bool e = false;
	std::chrono::time_point <std::chrono::steady_clock> clk = 
		std::chrono::steady_clock ().now ();
	while (!e)
	{
		// Reset the environment of the execution to avoid overflows...
		_cpu -> memoryRef () -> setActiveView (0);
		// The program counter is set to an address in the middle of the memory...
		_cpu -> programCounter ().setAddress (_cpu -> memoryRef () -> activeView () -> middleMemoryAddress ());
		// ...and the memory is filled up with the bytes received...
		_cpu -> memoryRef () -> set (_cpu -> programCounter ().asAddress (), b);
		// The stack is reset 
		_cpu -> memoryRef () -> stack () -> reset (); 
		// ...and located in the middle.
		_cpu -> memoryRef () -> stack () -> setPosition (_cpu -> memoryRef () -> stack () -> position () / 2);

		// Simulate read in memory (if asked for), like the emulation of the CPU does...
		if (sM)
			_cpu -> memoryRef () -> bytes (MCHEmul::Address (randomVector (_cpu -> architecture ().numberBytes ())), 
				b.size () - _cpu -> architecture ().instructionLength ()); // Simulate and access to the memory...
		// ...and then execute the instruction...
		inst -> execute (_cpu, _cpu -> memoryRef (), _cpu -> memoryRef () -> stack (), &_cpu -> programCounter ());
		clks += (inst -> clockCyclesExecuted () + inst -> additionalClockCyclesExecuted ());

		// Repeat during a second...
		e = (std::chrono::steady_clock ().now () - clk).count () >= 1e9; 
	}

	return (clks);
}

// ---
void MCHEmul::TestCPUSpeed::testAllInstructionSet (std::ostream& o, unsigned int nt, bool sM, bool pS)
{ 
	if (nt == 0)
		return;

	// Formatters for the messages received mainly...
	MCHEmul::FormatterBuilder::instance ({ "./testformatters.fmt" });

	_cpu -> setMemoryRef (_cpu -> memoryRef ());
	_cpu -> memoryRef () -> initialize ();
	_cpu -> memoryRef () -> stack (); // Just to initialize the pointer...
	_cpu -> initialize ();

	// Fill up the memory with random values...
	for (size_t i = 0; i < _cpu -> memoryRef () -> size (); 
		_cpu -> memoryRef () -> set (MCHEmul::Address (_cpu -> architecture ().instructionLength (), (unsigned int) i++), std::rand () % 256));
	_cpu -> memoryRef () -> stack () -> initialize (); // To put it back to 0!

	std::map <unsigned int, unsigned int> _execInstructions;
	std::map <unsigned int, unsigned int> _clockInstructions;
	std::map <unsigned int, std::string> _instructionsTemplates;
	testInstructionSet (o, _cpu -> instructions (), 
		_execInstructions, _clockInstructions, _instructionsTemplates, nt, sM, pS); 

	// Now to present the results...
	// Remember that _speedInstructions will hold the performance per instruction code...
	unsigned int minExecInstId = 0;
	unsigned int minExec = std::numeric_limits <unsigned int>::max ();
	unsigned int maxExecInstId = 0;
	unsigned int maxExec = 0;
	double av = 0.0f;

	for (auto& i : _execInstructions)
	{
		if ((unsigned int) i.second < minExec) 
			{ minExec = (unsigned int) i.second; minExecInstId = i.first; }
		if ((unsigned int) i.second > maxExec) 
			{ maxExec = (unsigned int) i.second; maxExecInstId = i.first; }
	}

	// Print out final results...
	o << std::endl;
	o << "Instructions: " << _instructionsTemplates.size () << std::endl;
	o << "Quickest Instruction: " << _instructionsTemplates [maxExecInstId] 
	  << " => " + std::to_string (maxExec) + " exec/s" 
	  << " [" << (maxExec * _clockInstructions [maxExecInstId]) << " cyles/s]" << std::endl;
	o << "Slowest Instruction:  " << _instructionsTemplates [minExecInstId] 
	  << " => " + std::to_string (minExec) + " exec/s" 
	  << " [" << (minExec * _clockInstructions [minExecInstId]) << " cyles/s]" << std::endl;
}

// ---
void MCHEmul::TestCPUSpeed::testInstructionSet (std::ostream& o, const MCHEmul::Instructions& inst, 
	std::map <unsigned int, unsigned int>& sPI, std::map <unsigned int, unsigned int>& cPI, std::map <unsigned int, std::string>& iT,
	unsigned int nt, bool sM, bool pS)
{
	static char _PROGRESS[4] { '\\', '|', '/', '-' };
	unsigned int* clks = new unsigned int [nt];

	// Now test all instructions...
	for (const auto& i : inst)
	{
		if (i.second == nullptr)
			continue; // Just in case...

		// If the instruction found is a group, everything inside is tested...
		if (dynamic_cast <MCHEmul::InstructionUndefined*> (i.second) != nullptr)
		{
			testInstructionSet (o, static_cast <MCHEmul::InstructionUndefined*> (i.second) -> instructions (), 
				sPI, cPI, iT,nt, sM, pS); 

			continue;
		}

		// From this point onwards the instructions is defined...
		MCHEmul::InstructionDefined* iD = 
			static_cast <MCHEmul::InstructionDefined*> (i.second);

		// if it is not a group, but a single instruction...
		// ...the test is done...
		// Before testing the instruction the register is set...
		setRandomRegisters ();
		// ...and the status is printed out if requested...
		if (pS)
			printOutCPUStatus (o);

		// the number of times that the received parameter says.
		for (size_t ct = 0; ct < nt; clks [ct++] = 0);
		for (unsigned int ct = 0; ct < nt; ct++)
		{
			// To show the progress...
			std::cout << _PROGRESS [ct % 4] << MCHEmul::_BACKS.substr (0,1);

			// Shape the instruction with data!
			bool e = false;
			std::vector <MCHEmul::UByte> byInst =
				MCHEmul::UInt::fromUnsignedInt (iD -> code (), MCHEmul::UInt::_BINARY).bytes ();
			std::vector <MCHEmul::UByte> byDt = iD -> shapeCodeWithData 
					({ randomVector (iD -> memoryPositions () - byInst.size ()) }, e);
			if (e)
			{
				std::cout << "Instruction " << iD -> iTemplate () << " bad defined" << std::endl;

				continue;
			}

			// Execute the instruction and get the number of cycles spent in a second...
			clks [ct] = clocksInASecondExcutingInstruction (MCHEmul::UBytes (byDt), sM);
		}

		// Store the outcome because it will managed later...
		std::string instTxt = iD -> asString () + 
			"(code:" + std::to_string (iD -> code ()) + ", " + std::to_string (i.first) + ")";
		iT [i.first] = instTxt;
		double a = 0.0f; for (size_t ct = 0; ct < nt; a += (double) clks [ct++]); a /= (double) nt;
		sPI [i.first] = (unsigned int) (a / (cPI [i.first] = iD -> clockCycles ())); // Execs in a second...
		// Print out the just tested instruction...
		o << instTxt << MCHEmul::_TABS.substr (0, 6 - (instTxt.length () >> 3)) 
		  << sPI [i.first] << " exec/s" 
		  << " [" << (sPI [i.first] * cPI [i.first]) << " cycles/s]" << std::endl;

		// ...and also the status of the CPU if requested...
		if (pS)
			printOutCPUStatus (o);
	}
}

// ---
std::vector <MCHEmul::UByte> MCHEmul::TestCPUSpeed::randomVector (size_t nB) const
{
	std::vector <MCHEmul::UByte> result (nB, MCHEmul::UByte::_0);
	for (size_t i = 0; i < nB; result [i++] = MCHEmul::UByte (rand () % 255));
	return (result);
}

// ---
void MCHEmul::TestCPUSpeed::setRandomRegisters ()
{
	// First of all, the internal registers of the CPU...
	for (auto& i : _cpu -> internalRegisters ())
		i.set (randomVector (i.size ()));
	// Then the status register...
	_cpu -> statusRegister ().set (randomVector (_cpu -> statusRegister ().size ()));
	// ...and finally the stack pointer, to any random position in memory...
	_cpu -> memoryRef () -> stack () -> setPosition (std::rand () % ((1 << _cpu -> architecture ().numberBytes ()) - 1));
}

// ---
void MCHEmul::TestCPUSpeed::printOutCPUStatus (std::ostream& o) const
{
	MCHEmul::InfoStructure rst;
	rst.add ("CPU", _cpu -> getInfoStructure ());
	rst.add ("Stack", _cpu -> memoryRef () -> stack () -> getInfoStructure ());
	o << MCHEmul::FormatterBuilder::instance () ->
		formatter ("TESTCPUSTATUS") -> format (rst) << std::endl;
	o << "----------" << std::endl;
}
