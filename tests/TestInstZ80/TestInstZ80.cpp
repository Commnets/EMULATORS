#include "stdafx.h"

#include "Tests.hpp"
#include <CORE/incs.hpp>
#include <FZ80/incs.hpp>

/** The port. */
class PortTest : public FZ80::Z80Port
{
	public:
	PortTest ()
		: FZ80::Z80Port (0, "test")
						{ }

	virtual MCHEmul::UByte value (unsigned short ab, unsigned char id) const override
							{ return (MCHEmul::UByte ((unsigned char) ((ab & 0xff00) >> 8))); }
	virtual void setValue (unsigned short ab, unsigned char id, const MCHEmul::UByte& v) override
							{ /** does nothing. */ }
};

/** A plain memory for simulation. */
class MemoryTest final : public MCHEmul::Memory
{
	public:
	MemoryTest ()
		: MCHEmul::Memory (0, basicContent ())
							{ }

	protected:
	virtual MCHEmul::Stack* lookForStack () override
						{ return (dynamic_cast <MCHEmul::Stack*> (subset (0))); }
	virtual MCHEmul::MemoryView* lookForCPUView () override
						{ return (view (0)); }

	private:
	MCHEmul::Memory::Content basicContent ();
};

/** Create the basic content of the memory. */
MCHEmul::Memory::Content MemoryTest::basicContent ()
{
	// 65536 bytes
	MCHEmul::PhysicalStorage* RAM = 
		new MCHEmul::PhysicalStorage (0, MCHEmul::PhysicalStorage::Type::_RAM, 0x10000); 
	MCHEmul::PhysicalStorages storages ({ { 0, RAM } });

	// 0x0000 - 0xffff = 65536 bytes
	MCHEmul::Stack*  Stack = new MCHEmul::Stack 
		(0x00, RAM, 0x0000, MCHEmul::Address ({ 0x00, 0x00 }, false), 0x10000, true, false); 

	MCHEmul::PhysicalStorageSubsets AllSubsets ({ { 0x00, Stack } });

	MCHEmul::MemoryView* CPUView = new MCHEmul::MemoryView (0, AllSubsets);

	// ...and finally the memory that is the result...
	MCHEmul::Memory::Content result;
	result._physicalStorages = storages;
	result._subsets = AllSubsets;
	result._views = MCHEmul::MemoryViews ({ { 0, CPUView } });

	return (result);
}

// The main...
int _tmain (int argc, _TCHAR *argv [])
{
	auto printErrors = [&](const std::vector <std::string>& e) -> void
		{
			for (const auto& i : e)
				std::cout << i << std::endl;
		};

	std::cout << "TestInstZ80" << std::endl;
	std::cout << "Copyright (C) 2023 by Ignacio Cea" << std::endl;
	std::cout << "To test (against results) the different instructions of the Z80 family" << std::endl << std::endl;

	Test test ("tests.in", "tests.expected");
	if (!test)
		std::cout << "Error in input files" << std::endl;
	else
	{
		FZ80::CZ80* cpu = new FZ80::CZ80 (0);
		PortTest* pTest = new PortTest;
		FZ80::Z80PortsMap pM;
		for (size_t i = 0; i < 256;
			pM.insert (FZ80::Z80PortsMap::value_type ((unsigned char) (i++), pTest)));
		cpu -> setPorts (pM);

		test.runTest (cpu, new MemoryTest);
	}

	if (!test.errors ().empty () || 
		!test.warnings ().empty () || 
		!test.noimplemented ().empty ())
	{
		std::cout << std::endl << std::endl;

		size_t nT = 0;
		std::cout << "Total Test:" << test.totalTests () << std::endl;

		std::cout << "Not implemented:" << std::endl;
		printErrors (test.noimplemented ());
		std::cout << "Total: " << test.noimplemented ().size () << std::endl;
		std::cout << "----------" << std::endl;
		nT += test.noimplemented ().size ();

		std::cout << "Errors:" << std::endl;
		printErrors (test.errors ());
		std::cout << "Total:" << test.errors ().size () << std::endl;
		std::cout << "----------" << std::endl;
		nT += test.errors ().size ();

		std::cout << "Warnings:" << std::endl;
		printErrors (test.warnings ());
		std::cout << "Total:" << test.warnings ().size () << std::endl;
		std::cout << "----------" << std::endl;
		nT += test.warnings().size ();

		std::cout << "Ok:" << (test.totalTests () - nT) << std::endl;
	}

	return (test.errors ().empty () ? 0 : 1);
}
