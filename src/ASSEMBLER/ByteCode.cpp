#include <ASSEMBLER/ByteCode.hpp>
#include <algorithm>

// ---
std::vector <MCHEmul::UByte> MCHEmul::Assembler::ByteCode::asSetOfBytes (MCHEmul::Address& iA) const
{
	if (_lines.empty ())
		return (std::vector <MCHEmul::UByte> ());

	auto sort = [](const MCHEmul::ByteCodeLine& l1, 
		const MCHEmul::ByteCodeLine& l2) -> bool { return (l1._address < l2._address); };

	MCHEmul::Assembler::ByteCode cp = *this;
	std::sort (cp._lines.begin (), cp._lines.end (), sort);

	std::vector <MCHEmul::UByte> result;
	iA = cp._lines [0]._address;
	MCHEmul::Address nA = iA; 
	for (const auto& i : cp._lines)
	{
		if (i._address > nA)
		{
			for (int j = 0; j < nA.distanceWith (i._address); j++)
				result.emplace_back (MCHEmul::UByte::_0);

			nA = i._address;
		}

		result.insert (result.end (), i._bytes.begin (), i._bytes.end ());

		nA += i._bytes.size ();
	}

	return (result);
}

// ---
MCHEmul::DataMemoryBlock MCHEmul::Assembler::ByteCode::asDataMemoryBlock () const
{
	MCHEmul::Address sA;
	std::vector <MCHEmul::UByte> by = asSetOfBytes (sA);
	return (MCHEmul::DataMemoryBlock (sA, by));
}

// ---
MCHEmul::DataMemoryBlocks MCHEmul::Assembler::ByteCode::asDataMemoryBlocks () const
{
	if (_lines.empty ())
		return (MCHEmul::DataMemoryBlocks ());

	auto sort = [](const MCHEmul::ByteCodeLine& l1, 
		const MCHEmul::ByteCodeLine& l2) -> bool { return (l1._address < l2._address); };

	MCHEmul::Assembler::ByteCode cp = *this;
	std::sort (cp._lines.begin (), cp._lines.end (), sort);

	MCHEmul::DataMemoryBlocks result;
	std::vector <MCHEmul::UByte> bytes;
	MCHEmul::Address iA = cp._lines [0]._address;
	MCHEmul::Address nA = iA; 
	for (const auto& i : cp._lines)
	{
		if (i._address > nA)
		{
			result.emplace_back (MCHEmul::DataMemoryBlock (iA, bytes));

			bytes = { };
			iA = i._address;
			nA = iA;
		}

		bytes.insert (bytes.end (), i._bytes.begin (), i._bytes.end ());

		nA += i._bytes.size ();
	}

	result.emplace_back (MCHEmul::DataMemoryBlock (iA, bytes));

	return (result);
}

// ---
std::map <MCHEmul::Address, unsigned int> MCHEmul::Assembler::ByteCode::listOfActions () const
{
	std::map <MCHEmul::Address, unsigned int> result;

	for (const auto& i : _lines)
		if (i._actionOn != 0)
			result.insert (std::map <MCHEmul::Address, unsigned int>::value_type (i._address, i._actionOn));

	return (result);
}

// ---
void MCHEmul::Assembler::ByteCode::loadIntoMemory (MCHEmul::Memory* m)
{
	assert (m != nullptr);

	for (const auto& i : _lines)
		m -> put (i._address, i._bytes);
}

// ---
bool MCHEmul::Assembler::ByteCode::saveAsBinary (const std::string& fN, const MCHEmul::CPU* cpu) const
{
	if (cpu == nullptr)
		return (false);

	return (asDataMemoryBlock ().save 
		(fN, cpu -> architecture ().numberBytes (), cpu -> architecture ().bigEndian ()));
}

// ---
bool MCHEmul::Assembler::ByteCode::saveAsBlocks (const std::string& fN, const MCHEmul::CPU* cpu) const
{
	if (cpu == nullptr)
		return (false);

	MCHEmul::DataMemoryBlocks blks = asDataMemoryBlocks ();
	return (MCHEmul::DataMemoryBlock::saveDataBlocks (fN, blks,
		cpu -> architecture ().numberBytes (), cpu -> architecture ().bigEndian ()));
}

// ---
bool MCHEmul::Assembler::ByteCode::saveAsBlocksFormatter (const std::string& fN, const std::string& fF) const
{
	MCHEmul::DataMemoryBlocks blks = asDataMemoryBlocks ();

	bool result = false;
	std::ofstream f (fN);
	if (!f)
		return (false);

	MCHEmul::DataMemoryBlockTextFormatter formatter (fF);
	for (const auto& i : blks)
		f << MCHEmul::removeAll0 (formatter.format (i)) << std::endl;

	f.close ();

	return (true);
}

// ---
MCHEmul::Assembler::ByteCode MCHEmul::Assembler::ByteCode::createFromMemory 
	(const MCHEmul::Address& a, unsigned int b, MCHEmul::Memory* m, MCHEmul::Computer* c)
{
	assert (m != nullptr && c != nullptr);

	MCHEmul::Assembler::ByteCode result;

	unsigned int i = 0;
	while (i < b)
	{
		MCHEmul::Instructions::const_iterator pi =
			c -> cpu () -> instructions ().find (MCHEmul::UInt
				(m -> values (a + i, c -> cpu () -> architecture ().instructionLength ()).bytes ()).asUnsignedInt ());
		if (pi == c -> cpu () -> instructions ().end ())
			break; // No sense to continue...the instruction doesn't exist...

		unsigned int mP = 0;
		const MCHEmul::Instruction* inst = (*pi).second;
		// If the instruction couldn't be defined from the info in the memory...
		// better to break the decompilation...
		if (!const_cast <MCHEmul::Instruction*> (inst) -> defineInstructionFrom (m, a)) break;
		result._lines.emplace_back (MCHEmul::ByteCodeLine 
			(a + i, m -> values (a + i, mP = inst -> memoryPositions (m, a)).bytes (), 
				"" /** no label ever. */, inst, c -> action (a + i)));
		i += mP;
	}

	return (result);
}
