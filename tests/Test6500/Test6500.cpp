#include "stdafx.h"

#include <iostream>

#include <F6500/incs.hpp>

class MemoryTest final : public MCHEmul::Memory
{
	public:
	MemoryTest ()
		: MCHEmul::Memory (basicContent ())
							{ }

	protected:
	virtual MCHEmul::Stack* lookForStack () override
						{ return (dynamic_cast <MCHEmul::Stack*> (subset (0))); }
	virtual MCHEmul::MemoryView* lookForCPUView () override
						{ return (view (0)); }

	private:
	MCHEmul::Memory::Content basicContent ();
};

MCHEmul::Memory::Content MemoryTest::basicContent ()
{
	MCHEmul::PhysicalStorage* RAM = 
		new MCHEmul::PhysicalStorage (0, MCHEmul::PhysicalStorage::Type::_RAM, 0x400); // 1 KByte
	MCHEmul::PhysicalStorages storages ({ { 0, RAM } });

	MCHEmul::Stack*  Stack = new MCHEmul::Stack 
		(0, RAM, 0x0000, MCHEmul::Address ({ 0x00, 0x00 }, false), 0x0100);
	MCHEmul::PhysicalStorageSubset* Data = new MCHEmul::PhysicalStorageSubset 
		(1, RAM, 0x0100, MCHEmul::Address ({ 0x00, 0x01 }, false), 0x0300);

	MCHEmul::PhysicalStorageSubsets AllSubsets 
		({ { 0, Stack }, { 1, Data } });

	MCHEmul::MemoryView* CPUView = new MCHEmul::MemoryView (0, AllSubsets);

	// ...and finally the memory that is the result...
	MCHEmul::Memory::Content result;
	result._physicalStorages = storages;
	result._subsets = AllSubsets;
	result._views = MCHEmul::MemoryViews ({ { 0, CPUView } });

	return (result);
}

unsigned int testInstruction1Second (const MCHEmul::UBytes& u, MCHEmul::CPU* cpu, MCHEmul::Memory* memory)
{
	MCHEmul::Instruction* inst = cpu -> instruction ((unsigned int) u [0].value ());


	unsigned int clks = 0;
	bool e = false;
	std::chrono::time_point <std::chrono::steady_clock> clk = 
		std::chrono::steady_clock ().now ();
	while (!e)
	{
		inst -> execute (u, cpu, memory, memory -> stack ());
		clks += inst -> clockCycles ();
		e = (std::chrono::steady_clock ().now () - clk).count () >= 1e9; // 1 second...
	}

	std::cout << inst -> asString () << ":\t\t\t" << clks << " clocks per second" << std::endl;

	return (clks);
}

void testInstructionTimes (unsigned int clks, const MCHEmul::UBytes& u, MCHEmul::CPU* cpu, MCHEmul::Memory* memory)
{
	MCHEmul::Instruction* inst = cpu -> instruction ((unsigned int) u [0].value ());

	std::chrono::time_point <std::chrono::steady_clock> clk = 
		std::chrono::steady_clock ().now ();
	for (unsigned int i = clks / inst -> clockCycles (); i > 0; i--)
		inst -> execute (u, cpu, memory, memory -> stack ());

	std::cout << inst -> asString () << ":\t\t\t" << clks << " clocks (" << clks / inst -> clockCycles () << " times) = "
			  << ((std::chrono::steady_clock ().now () - clk).count () / 1e9) << " seconds" << std::endl;
}

int _tmain (int argc, _TCHAR *argv [])
{
	F6500::C6510 cpu;
	MemoryTest memory;

	// NOP
	testInstructionTimes (testInstruction1Second (MCHEmul::UBytes ({ 0xea }), &cpu, &memory), 
		MCHEmul::UBytes ({ 0xea }), &cpu, &memory);
	
	// LDA $Absolut
	testInstructionTimes (testInstruction1Second (MCHEmul::UBytes ({ 0xad, 0x00, 0x02 }), &cpu, &memory),
		MCHEmul::UBytes ({ 0xad, 0x00, 0x02 }), &cpu, &memory);
	
	// ADC $Absolut, X Index
	testInstructionTimes (testInstruction1Second (MCHEmul::UBytes ({ 0x7d, 0x01, 0x02 }), &cpu, &memory),
		MCHEmul::UBytes ({ 0x7d, 0x01, 0x02 }), &cpu, &memory);
	
	// SBC $Absolut, X Index
	testInstructionTimes (testInstruction1Second (MCHEmul::UBytes ({ 0xfd, 0x01, 0x02 }), &cpu, &memory),
		MCHEmul::UBytes ({ 0xfd, 0x01, 0x02 }), &cpu, &memory);
	
	// CMP $Absolut, X Index
	testInstructionTimes (testInstruction1Second (MCHEmul::UBytes ({ 0xdd, 0x03, 0x02 }), &cpu, &memory),
		MCHEmul::UBytes ({ 0xdd, 0x03, 0x02 }), &cpu, &memory);

	std::string ex;
	std::cin >> ex;

	return (0);
}
