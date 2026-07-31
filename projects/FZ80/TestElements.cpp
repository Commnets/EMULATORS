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
		(_STACKSUBSET, RAM, 0x0000, MCHEmul::Address ({ 0x00, 0x00 }, false), 0x10000, 
			MCHEmul::Stack::Configuration (true, false /** pointing always to the last. */, 
				false /** No overflow detection. */, -1));

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
		(_STACKSUBSET, RAM, 0x0000, MCHEmul::Address ({ 0x00, 0x00 }, false), 0x0100,
			MCHEmul::Stack::Configuration (true, false /** pointing always to the last. */, 
				false /** No overflow detection. */, -1)); // 0x0000 - 0x00ff = 256 bytes
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
		(015, RAM, 0x0f00, MCHEmul::Address ({ 0x00, 0x0f }, false), 0x0100); // 0x0f00 - 0x0fff = 256 bytes
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

// ---
bool FZ80::TestZ80::prepareInterruptTest
	(unsigned char mode, unsigned short pc, unsigned short sp, unsigned char i)
{
	if (!_memory -> initialize () || !initialize ())
		return (false);

	setINTMode (mode);
	iRegister ().set ({ i });
	rRegister ().set ({ 0x10 });
	setIFF1 (true);
	setIFF2 (true);
	setHalt ();
	programCounter ().setAddress (MCHEmul::Address (2, pc));
	memoryRef () -> stack () -> reset ();
	memoryRef () -> stack () -> setPosition (sp);
	memoryRef () -> stack () -> setNotUsed (false);
	statusRegister ().set ({ 0x00 });

	return (true);
}

// ---
bool FZ80::TestZ80::reportInterruptTest
	(bool ok, const std::string& name, std::ostream& o) const
{
	o << "Z80 interrupt " << name << ": " << (ok ? "OK" : "ERROR") << std::endl;

	return (ok);
}

// ---
bool FZ80::TestZ80::testInterrupts (std::ostream& o)
{
	bool result = true;

	// IM 2 must use the acknowledged byte and then dereference the table entry.
	bool ok = prepareInterruptTest (2, 0x73a0, 0xbd80, 0xfb);
	memoryRef () -> put (MCHEmul::Address (2, 0xfb7e), 0x78);
	memoryRef () -> put (MCHEmul::Address (2, 0xfb7f), 0x56);
	memoryRef () -> put (MCHEmul::Address (2, 0xfbff), 0x34);
	memoryRef () -> put (MCHEmul::Address (2, 0xfc00), 0x12);
	setLastINOUTData (MCHEmul::UBytes (std::vector <MCHEmul::UByte> ({ 0x7e })));
	requestInterrupt (FZ80::INTInterrupt::_ID, clockCycles (), nullptr, -1,
		MCHEmul::UBytes ({ 0xff }));
	ok &= executeNextCycle ();
	MCHEmul::Memory::configuration ().executeMemorySetCommandsBuffered ();
	ok &= programCounter ().internalRepresentation () == 0x1234;
	ok &= memoryRef () -> stack () -> currentAddress () == MCHEmul::Address (2, 0xbd7e);
	ok &= memoryRef () -> value (MCHEmul::Address (2, 0xbd7f)) == 0x73;
	ok &= memoryRef () -> value (MCHEmul::Address (2, 0xbd7e)) == 0xa1;
	ok &= clockCycles () == 19;
	ok &= rRegister ().values ()[0] == 0x11;
	ok &= !IFF1 () && !IFF2 () && !haltActive ();
	if (!ok)
		o << "  PC=" << programCounter ().internalRepresentation ()
		  << " SP=" << memoryRef () -> stack () -> currentAddress ().value ()
		  << " stack=" << (unsigned int) memoryRef () -> value (MCHEmul::Address (2, 0xbd7f)).value ()
		  << "," << (unsigned int) memoryRef () -> value (MCHEmul::Address (2, 0xbd7e)).value ()
		  << " cycles=" << clockCycles ()
		  << " R=" << (unsigned int) rRegister ().values ()[0].value ()
		  << " IFF=" << IFF1 () << "," << IFF2 ()
		  << " HALT=" << haltActive () << std::endl;
	result &= reportInterruptTest (ok, "IM 2 vector indirection", o);

	// A table entry at $ffff reads its high byte from $0000.
	ok = prepareInterruptTest (2, 0x2000, 0xf000, 0xff);
	memoryRef () -> put (MCHEmul::Address (2, 0xffff), 0x34);
	memoryRef () -> put (MCHEmul::Address (2, 0x0000), 0x12);
	requestInterrupt (FZ80::INTInterrupt::_ID, clockCycles ());
	ok &= executeNextCycle ();
	ok &= programCounter ().internalRepresentation () == 0x1234;
	result &= reportInterruptTest (ok, "IM 2 default bus and wrap", o);

	// IM 1 remains a fixed jump to $0038.
	ok = prepareInterruptTest (1, 0x2000, 0xf000, 0xfb);
	requestInterrupt (FZ80::INTInterrupt::_ID, clockCycles (), nullptr, -1,
		MCHEmul::UBytes ({ 0x7e }));
	ok &= executeNextCycle ();
	ok &= programCounter ().internalRepresentation () == 0x0038;
	ok &= clockCycles () == 13;
	result &= reportInterruptTest (ok, "IM 1 regression", o);

	// IM 0 executes the acknowledged opcode, not stale instruction data.
	ok = prepareInterruptTest (0, 0x2000, 0xf000, 0xfb);
	setLastINOUTData (MCHEmul::UBytes (std::vector <MCHEmul::UByte> ({ 0x00 })));
	requestInterrupt (FZ80::INTInterrupt::_ID, clockCycles (), nullptr, -1,
		MCHEmul::UBytes ({ 0x37 /** SCF. */ }));
	ok &= executeNextCycle ();
	ok &= statusRegister ().bitStatus (FZ80::CZ80::_CARRYFLAG);
	result &= reportInterruptTest (ok, "IM 0 acknowledge data", o);

	return (result);
}
