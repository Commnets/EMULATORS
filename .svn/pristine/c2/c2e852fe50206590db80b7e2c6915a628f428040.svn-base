#include <ASSEMBLER/Compiler.hpp>
#include <algorithm>

// ---
std::ostream& MCHEmul::Assembler::operator << (std::ostream& o, const MCHEmul::Assembler::ByteCodeLine& c)
{
	o << c._address << ":";

	bool f = true;
	for (auto i : c._bytes)
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
	for (auto i : cp._lines)
	{
		if (i._address > nA)
		{
			while (nA != i._address)
			{
				result.push_back (MCHEmul::UByte::_0);
				nA = nA + 1;
			}
		}

		for (auto j : i._bytes)
			result.push_back (j);

		nA += i._bytes.size ();
	}

	return (result);
}

// ---
void MCHEmul::Assembler::ByteCode::loadIntoMemory (MCHEmul::Memory* m)
{
	assert (m != nullptr);

	for (auto i : _lines)
		m -> set (i._address, i._bytes);
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
				(m -> values (a, cpu -> architecture ().instructionLength ()).values ()).asUnsignedInt ());
		if (pi == cpu -> instructions ().end ())
			break; // No sense to continue...the instruction doesn't exist...

		const MCHEmul::Instruction* inst = (*pi).second;
		result._lines.push_back (MCHEmul::Assembler::ByteCodeLine 
			(a, m -> values (a, inst -> memoryPositions ()).values (), "" /** no label ever. */, inst));
		i += inst -> memoryPositions ();
	}

	return (result);
}

// ---
MCHEmul::Assembler::ByteCode MCHEmul::Assembler::Compiler::compile (const std::string& fN) const
{
	MCHEmul::Assembler::ByteCode result;

	_errors = { };

	// First of all, parse...
	MCHEmul::Assembler::Semantic* smt = _parser -> parse (fN);
	if (!(*_parser))
	{
		_errors = _parser -> errors ();
		return (result);
	}

	// Then compile...
	// First determines the value of the macros...
	for (auto i : smt -> macros ())
	{
		i.second.value (smt -> macros ()); // Try to calculate the value (but it wold be used)...
		if (!i.second)
			_errors.push_back (MCHEmul::Assembler::Error (i.second.error (), fN, 0, 0));
	}

	// If there were any error calculating the value of the macros
	// it would not be able to continue
	if (!(*this))
		return (result);

	// Then, with the macros calculated, the lines could be calculated
	// Errors could happen when managing the instructions...
	for (auto i : smt -> startingPoints ())
	{
		assert (dynamic_cast <MCHEmul::Assembler::StartingPointElement*> (i) != nullptr);

		MCHEmul::Address spa (i -> codeBytes (smt, cpu () -> architecture ().bigEndian ()));
		if (!*i)
		{
			_errors.push_back (MCHEmul::Assembler::Error (i -> _error, fN, i -> _line, 0));
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
					lL = "";
					std::vector <MCHEmul::UByte> b = 
						gE -> codeBytes (smt, cpu () -> architecture ().bigEndian ());
					if (!*gE)
						_errors.push_back (MCHEmul::Assembler::Error (gE -> _error, fN, gE -> _line, 0));
					else
						result._lines.push_back (MCHEmul::Assembler::ByteCodeLine (spa, b, lL, 
							(gE -> _type == MCHEmul::Assembler::GrammaticalElement::_INSTRUCTION) 
								? (static_cast <MCHEmul::Assembler::InstructionElement*> (gE)) -> _selectedInstruction : nullptr));

					spa += b.size (); // To the next...
				}

				break;

				default:
					assert (false);
			}
		}
	}
	
	delete (smt);

	return (result);
}
