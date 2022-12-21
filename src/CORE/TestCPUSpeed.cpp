#include <CORE/TestCPUSpeed.hpp>
#include <CORE/CPU.hpp>
#include <CORE/Memory.hpp>
#include <CORE/Stack.hpp>
#include <CORE/StdCommands.hpp>
#include <chrono>
#include <random>

// ---
MCHEmul::TestCPUSpeed::~TestCPUSpeed ()
{ 
	delete (_cpu); 
	
	delete (_memory); 
}

// ---
unsigned int MCHEmul::TestCPUSpeed::clocksInASecondExcutingInstruction (const MCHEmul::UBytes& b) const
{
	MCHEmul::Instruction* inst = 
		_cpu -> instruction (MCHEmul::UInt 
			(b.MSUBytes (_cpu -> architecture ().instructionLength ())).asUnsignedInt ());

	unsigned int clks = 0;

	bool e = false;
	std::chrono::time_point <std::chrono::steady_clock> clk = 
		std::chrono::steady_clock ().now ();
	while (!e)
	{
		// Reset the environment of the execution to avoid overflows...
		_memory -> setActiveView (0);
		// The program counter is set to an address in the middle of the memory...
		_cpu -> programCounter ().setAddress (_memory -> activeView () -> middleMemoryAddress ()); 
		// The stack is reset 
		_memory -> stack () -> reset (); 
		// ...and located in the middle.
		_memory -> stack () -> setPosition (_memory -> stack () -> position () / 2);

		// Simulate read in memory, like the emulation of the CPU does...
		_memory -> bytes (MCHEmul::Address (randomVector (_cpu -> architecture ().numberBytes ())), 
			b.size () - _cpu -> architecture ().instructionLength ()); // Simulate and access to the memory...
		// ...and then execute the instruction...
		inst -> execute (b, _cpu, _memory, _memory -> stack ());
		clks += inst -> clockCycles ();

		// Repeat during a second...
		e = (std::chrono::steady_clock ().now () - clk).count () >= 1e9; 
	}

	return (clks);
}

// ---
void MCHEmul::TestCPUSpeed::testInstructionSet (std::ostream& o, unsigned int nt, bool pS) const
{
	static char _PROGRESS[4] { '\\', '|', '/', '-' };

	if (nt == 0)
		return;

	// Formatters for the messages received mainly...
	MCHEmul::FormatterBuilder::instance ({ "./testformatters.fmt" });

	_cpu -> setMemoryRef (_memory);
	_memory -> initialize ();
	_memory -> stack (); // Just to initialize the pointer...
	_cpu -> initialize ();

	unsigned int minSpeed = std::numeric_limits <unsigned int>::max ();
	std::string minInst = "";
	unsigned int maxSpeed = 0;
	std::string maxInst = "";
	unsigned int* clks = new unsigned int [nt];

	// Now test all instructions...
	std::map <unsigned int, double> _speedPerInstruction;
	for (const auto& i : _cpu -> instructions ())
	{
		for (size_t ct = 0; ct < nt; clks [ct++] = 0);
		for (unsigned int ct = 0; ct < nt; ct++)
		{
			// To show the progress...
			std::cout << _PROGRESS [ct % 4] << MCHEmul::_BACKS.substr (0,1);

			// The parameters of the instruction will be random...
			std::vector <MCHEmul::UByte> byDt = 
				randomVector (i.second -> memoryPositions () - _cpu -> architecture ().instructionLength ());
			std::vector <MCHEmul::UByte> byInst =
				MCHEmul::UInt::fromUnsignedInt (i.second -> code (), MCHEmul::UInt::_BINARY).bytes ();
			if (!_cpu -> architecture ().bigEndian ()) 
				byInst = MCHEmul::UBytes (byInst).reverse ().bytes ();
			std::vector <MCHEmul::UByte> b = byDt;
			b.insert (b.begin (), byInst.begin (), byInst.end ());

			// Execute the text...
			clks [ct] = clocksInASecondExcutingInstruction (MCHEmul::UBytes (b));
		}

		// ..and manage the result...
		std::string instTxt = i.second -> asString () + "(code:" + std::to_string (i.second -> code ()) + ")";
		double a = 0.0f; for (size_t ct = 0; ct < nt; a += (double) clks [ct++]); a /= (double) nt;
		if ((unsigned int) a < minSpeed) { minSpeed = (unsigned int) a; minInst = instTxt; }
		if ((unsigned int) a > maxSpeed) { maxSpeed = (unsigned int) a; maxInst = instTxt; }
		_speedPerInstruction [i.second -> code ()] = a;
		o << instTxt << MCHEmul::_TABS.substr (0, 4 - (instTxt.length () >> 3)) 
		  << (unsigned int) a << " clks/s" << std::endl;

		// Print out the status of the cpu just in case of being required...
		if (pS)
			printOutCPUStatus (o);
	}

	// Print out final results...
	o << std::endl;
	o << "Quickest Instruction: " << maxInst << " => " + std::to_string (maxSpeed) + " clks/s" << std::endl;
	o << "Slowest Instruction:  " << minInst << " => " + std::to_string (minSpeed) + " clks/s" << std::endl;
	double av = 0.0f; for (const auto& i : _speedPerInstruction) av += i.second; av /= (double) _speedPerInstruction.size ();
	o << "Average speed: " << std::to_string (av) << " clks/s" << std::endl;
}

// ---
std::vector <MCHEmul::UByte> MCHEmul::TestCPUSpeed::randomVector (size_t nB) const
{
	std::vector <MCHEmul::UByte> result (nB, MCHEmul::UByte::_0);
	for (size_t i = 0; i < nB; result [i++] = MCHEmul::UByte (rand () % 255));
	return (result);
}

// ---
void MCHEmul::TestCPUSpeed::printOutCPUStatus (std::ostream& o) const
{
	MCHEmul::InfoStructure rst;
	rst.add ("CPU", _cpu -> getInfoStructure ());
	rst.add ("Stack", _memory -> stack () -> getInfoStructure ());
	o << MCHEmul::FormatterBuilder::instance () ->
		formatter ("TESTCPUSTATUS") -> format (rst) << std::endl;
	o << "----------" << std::endl;
}
