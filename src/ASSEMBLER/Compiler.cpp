#include <ASSEMBLER/Compiler.hpp>
#include <algorithm>

// ---
std::ostream& MCHEmul::Assembler::operator << (std::ostream& o, const MCHEmul::Assembler::ByteCodeLine& c)
{
	o << c._address << ":";

	bool f = true;
	for (const auto& i : c._bytes)
	{
		o << ((!f) ? " " : "") << i; 
		f = false;
	}

	o << ((c._instruction != nullptr) ? (" " + c._instruction -> iTemplate ()) : "");
	o << ((c._label != "") ? " " : "") << c._label;

	return (o);
}

// ---
std::vector <MCHEmul::UByte> MCHEmul::Assembler::ByteCode::asSetOfBytes (MCHEmul::Address& iA) const
{
	if (_lines.empty ())
		return (std::vector <MCHEmul::UByte> ());

	auto sort = [](const MCHEmul::Assembler::ByteCodeLine& l1, 
		const MCHEmul::Assembler::ByteCodeLine& l2) -> bool { return (l1._address < l2._address); };

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
				result.push_back (MCHEmul::UByte::_0);

			nA = i._address;
		}

		result.insert (result.end (), i._bytes.begin (), i._bytes.end ());

		nA += i._bytes.size ();
	}

	return (result);
}

// ---
MCHEmul::DataMemoryBlocks MCHEmul::Assembler::ByteCode::asDataMemoryBlocks () const
{
	if (_lines.empty ())
		return (MCHEmul::DataMemoryBlocks ());

	auto sort = [](const MCHEmul::Assembler::ByteCodeLine& l1, 
		const MCHEmul::Assembler::ByteCodeLine& l2) -> bool { return (l1._address < l2._address); };

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
			result.push_back (MCHEmul::DataMemoryBlock (iA, bytes));

			bytes = { };
			iA = i._address;
			nA = iA;
		}

		bytes.insert (bytes.end (), i._bytes.begin (), i._bytes.end ());

		nA += i._bytes.size ();
	}

	result.push_back (MCHEmul::DataMemoryBlock (iA, bytes));

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
		m -> set (i._address, i._bytes);
}

// ---
bool MCHEmul::Assembler::ByteCode::saveAsBinary (const std::string& fN, const MCHEmul::CPU* cpu) const
{
	if (cpu == nullptr)
		return (false);

	MCHEmul::Address sA;
	std::vector <MCHEmul::UByte> dt = asSetOfBytes (sA);
	MCHEmul::UBytes sAB = MCHEmul::UBytes (sA.bytes (), cpu -> architecture ().bigEndian ());
	sAB.setMinLength (cpu -> architecture ().numberBytes ());
	std::vector <MCHEmul::UByte> sABb = sAB.bytes ();
	// The address in the format understood by the cpu behind (big-endian / little-endian and length)
	// is inserted into the list of bytes to save. 
	// The length of the bytes inserts also depends on the cpu behind
	dt.insert (dt.begin (), sABb.begin (), sABb.end ());

	return (MCHEmul::UBytes::saveBytesTo (fN, dt));
}

// ---
bool MCHEmul::Assembler::ByteCode::saveAsBlocks (const std::string& fN, const MCHEmul::CPU* cpu) const
{
	if (cpu == nullptr)
		return (false);

	MCHEmul::DataMemoryBlocks blks = asDataMemoryBlocks ();
	std::vector <MCHEmul::UByte> dt;
	for (const auto& i : blks)
	{
		std::vector <MCHEmul::UByte> pdt = i._bytes;

		// Insert the length to include
		MCHEmul::UInt l = MCHEmul::UInt ((unsigned int) pdt.size ()); 
		l.setMinLength (4); // ...with a minimum length of 4 bytes (is the maximum size admitted by the emulation)
		pdt.insert (pdt.begin (), l.bytes ().begin (), l.bytes ().end ());

		// Then the address...(@see above), that becomes first...
		// In the format understood by the cpu (little or big endian)
		// ...and with number of bytes that are also managed by the cpu behind!
		MCHEmul::UBytes sAB = MCHEmul::UBytes (i._startAddress.bytes (), cpu -> architecture ().bigEndian ());
		sAB.setMinLength (cpu -> architecture ().numberBytes ());
		std::vector <MCHEmul::UByte> sABb = sAB.bytes (); 
		pdt.insert (pdt.begin (), sABb.begin (), sABb.end ());

		// At this point the address is first and then the length of the data kept.

		// Adds the block to the total list to be saved later...
		dt.insert (dt.end (), pdt.begin (), pdt.end ());
	}

	return (MCHEmul::UBytes::saveBytesTo (fN, dt));
}

// ---
MCHEmul::Assembler::ByteCode MCHEmul::Assembler::ByteCode::createFromMemory 
	(const MCHEmul::Address& a, unsigned int b, MCHEmul::Memory* m, MCHEmul::CPU* cpu)
{
	assert (m != nullptr && cpu != nullptr);

	MCHEmul::Assembler::ByteCode result;

	unsigned int i = 0;
	while (i < b)
	{
		MCHEmul::Instructions::const_iterator pi =
			cpu -> instructions ().find (MCHEmul::UInt 
				(m -> values (a, cpu -> architecture ().instructionLength ()).bytes ()).asUnsignedInt ());
		if (pi == cpu -> instructions ().end ())
			break; // No sense to continue...the instruction doesn't exist...

		const MCHEmul::Instruction* inst = (*pi).second;
		result._lines.push_back (MCHEmul::Assembler::ByteCodeLine 
			(a, m -> values (a, inst -> memoryPositions ()).bytes (), "" /** no label ever. */, inst, 0 /** no action. */));
		i += inst -> memoryPositions ();
	}

	return (result);
}

// ---
MCHEmul::Assembler::ByteCode MCHEmul::Assembler::Compiler::compile (const std::string& fN) const
{
	MCHEmul::Assembler::ByteCode result;

	if (_parser == nullptr)
		return (result);

	_errors = { };

	// First of all, parse...
	MCHEmul::Assembler::Semantic* smt = _parser -> parse (fN, fN + ".act");
	if (!*_parser)
	{
		_errors = _parser -> errors ();

		delete (smt);

		return (result);
	}

	// Then compile...
	// First determines the value of the macros...
	for (MCHEmul::Assembler::Macros::const_iterator i = smt -> macros ().begin (); 
			i != smt -> macros ().end (); i++) // Instead for (auto i : smt -> macros ()) because it makes a copy of the content
											   // and the values are not then actualized over the original list!
	{
		(*i).second.value (smt -> macros (), operationParser ()); // Try to calculate the value (but it wold be used)...
		if (!(*i).second)
			_errors.push_back (MCHEmul::Assembler::Error ((*i).second.error (), 
				(*i).second.definitionFile (), (*i).second.definitionLine (), 0, (*i).second.name ()));
	}

	// If there were any error calculating the value of the macros
	// it would not be able to continue
	if (!(*this))
	{
		delete (smt);

		return (result);
	}

	// Then, with the macros calculated, the lines could be calculated
	// Errors could happen when managing the instructions...
	for (auto i : smt -> startingPoints ())
	{
		assert (dynamic_cast <MCHEmul::Assembler::StartingPointElement*> (i) != nullptr);

		MCHEmul::Address spa (i -> codeBytes (smt, cpu () -> architecture ().bigEndian (), operationParser ()));
		if (!*i)
		{
			_errors.push_back (MCHEmul::Assembler::Error (i -> _error, i -> _file, i -> _line, 0));

			continue;
		}

		std::string lL = "";
		for (MCHEmul::Assembler::GrammaticalElement* gE = i -> _nextElement; 
			gE != nullptr; gE = gE -> _nextElement)
		{
			switch (gE -> _type)
			{
				case MCHEmul::Assembler::GrammaticalElement::_LABEL:
				{
					lL = static_cast <MCHEmul::Assembler::LabelElement*> (gE) -> _name;
				}

				break;

				case MCHEmul::Assembler::GrammaticalElement::_BYTESINMEMORY:
				case MCHEmul::Assembler::GrammaticalElement::_INSTRUCTION:
				{
					std::vector <MCHEmul::UByte> b = 
						gE -> codeBytes (smt, cpu () -> architecture ().bigEndian (), operationParser ());
					if (!*gE)
						_errors.push_back (MCHEmul::Assembler::Error (gE -> _error, gE -> _file, gE -> _line, 0));
					else
						result._lines.push_back (MCHEmul::Assembler::ByteCodeLine (spa, b, lL, 
							(gE -> _type == MCHEmul::Assembler::GrammaticalElement::_INSTRUCTION) 
								? (static_cast <MCHEmul::Assembler::InstructionElement*> (gE)) -> _selectedInstruction : nullptr, 
							 gE -> _actionOn /** Action affecting the code bytes. */));

					spa += b.size (); // To the next...
					lL = "";
				}

				break;

				default:
					// It should be here...
					assert (false);
			}
		}
	}
	
	delete (smt);

	return (result);
}
