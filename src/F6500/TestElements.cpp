#include <F6500/TestElements.hpp>

//
MCHEmul::Memory::Content F6500::PlainMemoryTest::basicContent ()
{
	// 65536 bytes of RAM...
	MCHEmul::PhysicalStorage* RAM = 
		new MCHEmul::PhysicalStorage (0, MCHEmul::PhysicalStorage::Type::_RAM, 0x10000); 
	MCHEmul::PhysicalStorages storages ({ { 0, RAM } });

	// The logical view...
	// Page 0: 256 bytes...
	MCHEmul::PhysicalStorageSubset* PageZero = new MCHEmul::PhysicalStorageSubset 
		(0, RAM, 0x0000, MCHEmul::Address ({ 0x00, 0x00 }, false), 0x0100);
	// Page 1: Stack = 256 bytes...
	// In F6500 the stack is always at this position and it is always 256 bytes long...
	MCHEmul::Stack* Stack = new MCHEmul::Stack 
		(_STACKSUBSET, RAM, 0x0100, MCHEmul::Address ({ 0x00, 0x01 }, false), 0x0100,
			MCHEmul::Stack::Configuration (true, true, false /** No overflow detection. */, -1)); 
	// The rest of the memory:63,5k...
	MCHEmul::PhysicalStorageSubset* RestRAM = new MCHEmul::PhysicalStorageSubset 
		(2, RAM, 0x0200, MCHEmul::Address ({ 0x00, 0x02 }, false), 0xfe00); 
	MCHEmul::PhysicalStorageSubsets AllSubsets (
		{ { 0, PageZero }, { 1, Stack }, { 2, RestRAM } });

	MCHEmul::MemoryView* CPUView = new MCHEmul::MemoryView (0, AllSubsets);

	// ...and finally the memory that is the result...
	MCHEmul::Memory::Content result;
	result._physicalStorages = storages;
	result._subsets = AllSubsets;
	result._views = MCHEmul::MemoryViews ({ { 0, CPUView } });

	return (result);
}

// ---
MCHEmul::Memory::Content F6500::FragmentatedMemoryTest::basicContent ()
{
	MCHEmul::PhysicalStorage* RAM = 
		new MCHEmul::PhysicalStorage (0, MCHEmul::PhysicalStorage::Type::_RAM, 0x10000); // 64 KBytes
	MCHEmul::PhysicalStorages storages ({ { 0, RAM } });

	MCHEmul::PhysicalStorageSubset* Data1 = new MCHEmul::PhysicalStorageSubset 
		(0, RAM, 0x0100, MCHEmul::Address ({ 0x00, 0x00 }, false), 0x0100); // 0x0000 - 0x00ff = 256 bytes
	// In F6500 the stack is always at this position and it is always 256 bytes long...
	MCHEmul::Stack*  Stack = new MCHEmul::Stack 
		(_STACKSUBSET, RAM, 0x0000, MCHEmul::Address ({ 0x00, 0x01 }, false), 0x0100,
			MCHEmul::Stack::Configuration (true, true, false /** No overflow detection. */, -1)); 
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
	MCHEmul::PhysicalStorageSubset* DataRest = new MCHEmul::PhysicalStorageSubset
		(16, RAM, 0x1000, MCHEmul::Address ({ 0x00, 0x10 }, false), 0xf000); // 0x1000 - 0xffff = 61440 bytes

	MCHEmul::PhysicalStorageSubsets AllSubsets 
		({ 
			{ 0,		Stack }, 
			{ 1,		Data1 },
			{ 2,		Data2 },
			{ 3,		Data3 },
			{ 4,		Data4 },
			{ 5,		Data5 },
			{ 6,		Data6 },
			{ 7,		Data7 },
			{ 8,		Data8 },
			{ 9,		Data9 },
			{ 10,		DataA },
			{ 11,		DataB },
			{ 12,		DataC },
			{ 13,		DataD },
			{ 14,		DataE },
			{ 15,		DataF },
			{ 16,		DataRest }
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
F6500::Test6500::Test6500 (MCHEmul::Memory* m)
	: F6500::C6500 (0),
	  _memory (m)
{
	assert (_memory != nullptr);

	setMemoryRef (m);
}
