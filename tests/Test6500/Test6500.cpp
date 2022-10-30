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
		new MCHEmul::PhysicalStorage (0, MCHEmul::PhysicalStorage::Type::_RAM, 0x1000); // 4 KBytes
	MCHEmul::PhysicalStorages storages ({ { 0, RAM } });

	MCHEmul::Stack*  Stack = new MCHEmul::Stack 
		(0, RAM, 0x0000, MCHEmul::Address ({ 0x00, 0x00 }, false), 0x0100); // 0x0000 - 0x00ff = 256 bytes
	MCHEmul::PhysicalStorageSubset* Data1 = new MCHEmul::PhysicalStorageSubset 
		(1, RAM, 0x0100, MCHEmul::Address ({ 0x00, 0x01 }, false), 0x0100); // 0x0100 - 0x01ff = 256 bytes
	MCHEmul::PhysicalStorageSubset* Data2 = new MCHEmul::PhysicalStorageSubset 
		(2, RAM, 0x0200, MCHEmul::Address ({ 0x00, 0x02 }, false), 0x0100); // 0x0200 - 0x02ff = 256 bytes
	MCHEmul::PhysicalStorageSubset* Data3 = new MCHEmul::PhysicalStorageSubset 
		(3, RAM, 0x0300, MCHEmul::Address ({ 0x00, 0x03 }, false), 0x0100); // 0x0300 - 0x03ff = 256 bytes
	MCHEmul::PhysicalStorageSubset* Data4 = new MCHEmul::PhysicalStorageSubset 
		(4, RAM, 0x0400, MCHEmul::Address ({ 0x00, 0x04 }, false), 0x0100); // 0x0400 - 0x04ff = 256 bytes
	MCHEmul::PhysicalStorageSubset* Data5 = new MCHEmul::PhysicalStorageSubset 
		(5, RAM, 0x0500, MCHEmul::Address ({ 0x00, 0x05 }, false), 0x0100); // 0x0500 - 0x05ff = 256 bytes
	MCHEmul::PhysicalStorageSubset* Data6 = new MCHEmul::PhysicalStorageSubset 
		(6, RAM, 0x0600, MCHEmul::Address ({ 0x00, 0x06 }, false), 0x0100); // 0x0600 - 0x06ff = 256 bytes
	MCHEmul::PhysicalStorageSubset* Data7 = new MCHEmul::PhysicalStorageSubset 
		(7, RAM, 0x0700, MCHEmul::Address ({ 0x00, 0x07 }, false), 0x0100); // 0x0700 - 0x07ff = 256 bytes
	MCHEmul::PhysicalStorageSubset* Data8 = new MCHEmul::PhysicalStorageSubset 
		(8, RAM, 0x0800, MCHEmul::Address ({ 0x00, 0x08 }, false), 0x0100); // 0x0800 - 0x08ff = 256 bytes
	MCHEmul::PhysicalStorageSubset* Data9 = new MCHEmul::PhysicalStorageSubset 
		(9, RAM, 0x0900, MCHEmul::Address ({ 0x00, 0x09 }, false), 0x0100); // 0x0900 - 0x09ff = 256 bytes
	MCHEmul::PhysicalStorageSubset* DataA = new MCHEmul::PhysicalStorageSubset
		(10, RAM, 0x0a00, MCHEmul::Address ({ 0x00, 0x0a }, false), 0x0100); // 0x0a00 - 0x0aff = 256 bytes
	MCHEmul::PhysicalStorageSubset* DataB = new MCHEmul::PhysicalStorageSubset
		(11, RAM, 0x0b00, MCHEmul::Address ({ 0x00, 0x0b }, false), 0x0100); // 0x0b00 - 0x0bff = 256 bytes
	MCHEmul::PhysicalStorageSubset* DataC = new MCHEmul::PhysicalStorageSubset
		(12, RAM, 0x0c00, MCHEmul::Address ({ 0x00, 0x0c }, false), 0x0100); // 0x0c00 - 0x0cff = 256 bytes
	MCHEmul::PhysicalStorageSubset* DataD = new MCHEmul::PhysicalStorageSubset
		(13, RAM, 0x0d00, MCHEmul::Address ({ 0x00, 0x0d }, false), 0x0100); // 0x0d00 - 0x0dff = 256 bytes
	MCHEmul::PhysicalStorageSubset* DataE = new MCHEmul::PhysicalStorageSubset
		(14, RAM, 0x0e00, MCHEmul::Address ({ 0x00, 0x0e }, false), 0x0100); // 0x0e00 - 0x0eff = 256 bytes
	MCHEmul::PhysicalStorageSubset* DataF = new MCHEmul::PhysicalStorageSubset
		(15, RAM, 0x0f00, MCHEmul::Address ({ 0x00, 0x0f }, false), 0x0100); // 0x0f00 - 0x0fff = 256 bytes

	MCHEmul::PhysicalStorageSubsets AllSubsets 
		({ { 0x00,		Stack }, 
		   { 0x01,		Data1 },
		   { 0x02,		Data2 },
		   { 0x03,		Data3 },
		   { 0x04,		Data4 },
		   { 0x05,		Data5 },
		   { 0x06,		Data6 },
		   { 0x07,		Data7 },
		   { 0x08,		Data8 },
		   { 0x09,		Data9 },
		   { 0x0a,		DataA },
		   { 0x0b,		DataB },
		   { 0x0c,		DataC },
		   { 0x0d,		DataD },
		   { 0x0e,		DataE },
		   { 0x0f,		DataF },
		});

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
		// Just to simulate the access...
		memory -> setActiveView (0);
		MCHEmul::UBytes dt1 = memory -> values (MCHEmul::Address ({ 0x00, 0x0f }, false), u.size ()); 
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
	{
		memory -> setActiveView (0);
		MCHEmul::UBytes dt1 =
			memory -> values (MCHEmul::Address ({ 0x00, 0x0f }, false), u.size ()); // Just to simulate the access...
		inst -> execute (u, cpu, memory, memory -> stack ());
	}

	std::cout << inst -> asString () << ":\t\t\t" << clks << " clocks (" << clks / inst -> clockCycles () << " times) = "
			  << ((std::chrono::steady_clock ().now () - clk).count () / 1e9) << " seconds" << std::endl;
}

int _tmain (int argc, _TCHAR *argv [])
{
	F6500::C6510 cpu;
	MemoryTest memory;

	// NOP
	for (unsigned int i = 0; i < 10; i++)
		testInstructionTimes (testInstruction1Second (MCHEmul::UBytes ({ 0xea }), &cpu, &memory), 
			MCHEmul::UBytes ({ 0xea }), &cpu, &memory);
	
	// LDA $Absolut
	for (unsigned int i = 0; i < 10; i++)
		testInstructionTimes (testInstruction1Second (MCHEmul::UBytes ({ 0xad, 0x00, 0x02 }), &cpu, &memory),
			MCHEmul::UBytes ({ 0xad, 0x00, 0x02 }), &cpu, &memory);
	
	// ADC $Absolut, X Index
	for (unsigned int i = 0; i < 10; i++)
		testInstructionTimes (testInstruction1Second (MCHEmul::UBytes ({ 0x7d, 0x01, 0x02 }), &cpu, &memory),
			MCHEmul::UBytes ({ 0x7d, 0x01, 0x02 }), &cpu, &memory);
	
	// SBC $Absolut, X Index
	for (unsigned int i = 0; i < 10; i++)
		testInstructionTimes (testInstruction1Second (MCHEmul::UBytes ({ 0xfd, 0x01, 0x02 }), &cpu, &memory),
			MCHEmul::UBytes ({ 0xfd, 0x01, 0x02 }), &cpu, &memory);
	
	// CMP $Absolut, X Index
	for (unsigned int i = 0; i < 10; i++)
		testInstructionTimes (testInstruction1Second (MCHEmul::UBytes ({ 0xdd, 0x03, 0x02 }), &cpu, &memory),
			MCHEmul::UBytes ({ 0xdd, 0x03, 0x02 }), &cpu, &memory);

	std::string ex;
	std::cin >> ex;

	return (0);
}
