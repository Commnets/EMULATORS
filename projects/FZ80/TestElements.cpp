#include <FZ80/TestElements.hpp>

//
MCHEmul::Memory::Content FZ80::PlainMemoryTest::basicContent ()
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

// ---
MCHEmul::Memory::Content FZ80::FragmentatedMemoryTest::basicContent ()
{
	MCHEmul::PhysicalStorage* RAM = 
		new MCHEmul::PhysicalStorage (0, MCHEmul::PhysicalStorage::Type::_RAM, 0x10000); // 64 KBytes
	MCHEmul::PhysicalStorages storages ({ { 0, RAM } });

	MCHEmul::Stack*  Stack = new MCHEmul::Stack 
		(0x00, RAM, 0x0000, MCHEmul::Address ({ 0x00, 0x00 }, false), 0x0100); // 0x0000 - 0x00ff = 256 bytes
	MCHEmul::PhysicalStorageSubset* Data1 = new MCHEmul::PhysicalStorageSubset 
		(0x01, RAM, 0x0100, MCHEmul::Address ({ 0x00, 0x01 }, false), 0x0100); // 0x0100 - 0x01ff = 256 bytes
	MCHEmul::PhysicalStorageSubset* Data2 = new MCHEmul::PhysicalStorageSubset 
		(0x02, RAM, 0x0200, MCHEmul::Address ({ 0x00, 0x02 }, false), 0x0100); // 0x0200 - 0x02ff = 256 bytes
	MCHEmul::PhysicalStorageSubset* Data3 = new MCHEmul::PhysicalStorageSubset 
		(0x03, RAM, 0x0300, MCHEmul::Address ({ 0x00, 0x03 }, false), 0x0100); // 0x0300 - 0x03ff = 256 bytes
	MCHEmul::PhysicalStorageSubset* Data4 = new MCHEmul::PhysicalStorageSubset 
		(0x04, RAM, 0x0400, MCHEmul::Address ({ 0x00, 0x04 }, false), 0x0100); // 0x0400 - 0x04ff = 256 bytes
	MCHEmul::PhysicalStorageSubset* Data5 = new MCHEmul::PhysicalStorageSubset 
		(0x05, RAM, 0x0500, MCHEmul::Address ({ 0x00, 0x05 }, false), 0x0100); // 0x0500 - 0x05ff = 256 bytes
	MCHEmul::PhysicalStorageSubset* Data6 = new MCHEmul::PhysicalStorageSubset 
		(0x06, RAM, 0x0600, MCHEmul::Address ({ 0x00, 0x06 }, false), 0x0100); // 0x0600 - 0x06ff = 256 bytes
	MCHEmul::PhysicalStorageSubset* Data7 = new MCHEmul::PhysicalStorageSubset 
		(0x07, RAM, 0x0700, MCHEmul::Address ({ 0x00, 0x07 }, false), 0x0100); // 0x0700 - 0x07ff = 256 bytes
	MCHEmul::PhysicalStorageSubset* Data8 = new MCHEmul::PhysicalStorageSubset 
		(0x08, RAM, 0x0800, MCHEmul::Address ({ 0x00, 0x08 }, false), 0x0100); // 0x0800 - 0x08ff = 256 bytes
	MCHEmul::PhysicalStorageSubset* Data9 = new MCHEmul::PhysicalStorageSubset 
		(0x09, RAM, 0x0900, MCHEmul::Address ({ 0x00, 0x09 }, false), 0x0100); // 0x0900 - 0x09ff = 256 bytes
	MCHEmul::PhysicalStorageSubset* DataA = new MCHEmul::PhysicalStorageSubset
		(0x0a, RAM, 0x0a00, MCHEmul::Address ({ 0x00, 0x0a }, false), 0x0100); // 0x0a00 - 0x0aff = 256 bytes
	MCHEmul::PhysicalStorageSubset* DataB = new MCHEmul::PhysicalStorageSubset
		(0x0b, RAM, 0x0b00, MCHEmul::Address ({ 0x00, 0x0b }, false), 0x0100); // 0x0b00 - 0x0bff = 256 bytes
	MCHEmul::PhysicalStorageSubset* DataC = new MCHEmul::PhysicalStorageSubset
		(0x0c, RAM, 0x0c00, MCHEmul::Address ({ 0x00, 0x0c }, false), 0x0100); // 0x0c00 - 0x0cff = 256 bytes
	MCHEmul::PhysicalStorageSubset* DataD = new MCHEmul::PhysicalStorageSubset
		(0x0d, RAM, 0x0d00, MCHEmul::Address ({ 0x00, 0x0d }, false), 0x0100); // 0x0d00 - 0x0dff = 256 bytes
	MCHEmul::PhysicalStorageSubset* DataE = new MCHEmul::PhysicalStorageSubset
		(0x0e, RAM, 0x0e00, MCHEmul::Address ({ 0x00, 0x0e }, false), 0x0100); // 0x0e00 - 0x0eff = 256 bytes
	MCHEmul::PhysicalStorageSubset* DataF = new MCHEmul::PhysicalStorageSubset
		(0x0f, RAM, 0x0f00, MCHEmul::Address ({ 0x00, 0x0f }, false), 0x0100); // 0x0f00 - 0x0fff = 256 bytes
	MCHEmul::PhysicalStorageSubset* DataRest = new MCHEmul::PhysicalStorageSubset
		(0x10, RAM, 0x1000, MCHEmul::Address ({ 0x00, 0x10 }, false), 0xf000); // 0x1000 - 0xffff = 61440 bytes

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
			{ 0x10,		DataRest }
		});

	MCHEmul::MemoryView* CPUView = new MCHEmul::MemoryView (0, AllSubsets);

	// ...and finally the memory that is the result...
	MCHEmul::Memory::Content result;
	result._physicalStorages = storages;
	result._subsets = AllSubsets;
	result._views = MCHEmul::MemoryViews ({ { 0, CPUView } });

	return (result);
}

// ---
FZ80::TestZ80::TestZ80 (MCHEmul::Memory* m)
	: FZ80::CZ80 (0),
	  _memory (m)
{
	assert (_memory != nullptr);

	FZ80::PortTest* pTest = new FZ80::PortTest;
	FZ80::Z80PortsMap pM;
	for (size_t i = 0; i < 256;
		pM.insert (FZ80::Z80PortsMap::value_type ((unsigned char) (i++), pTest)));
	setPorts (pM);
	setMemoryRef (m);
}
