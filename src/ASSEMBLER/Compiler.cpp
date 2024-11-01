#include <ASSEMBLER/Compiler.hpp>
#include <algorithm>

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
		(*i).second.value (smt -> macros (), operationParser ()); // Try to calculate the value (but it would be used)...
		if (!(*i).second)
			_errors.emplace_back (MCHEmul::Assembler::Error ((*i).second.error (), 
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
			_errors.emplace_back (MCHEmul::Assembler::Error (i -> _error, i -> _file, i -> _line, 0));

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
							_errors.emplace_back (MCHEmul::Assembler::Error (gE -> _error, gE -> _file, gE -> _line, 0));
						else
							result._lines.emplace_back (MCHEmul::ByteCodeLine (spa, b, lL, 
								(gE -> _type == MCHEmul::Assembler::GrammaticalElement::_INSTRUCTION) 
									? (static_cast <MCHEmul::Assembler::InstructionElement*> (gE)) -> _selectedInstruction : nullptr, 
								 gE -> _actionOn /** Action affecting the code bytes. */));

						spa += b.size (); // To the next...
						lL = "";
					}

					break;

				default:
					{
						// It should be here...
						_LOG ("Gramatical element not supported:" + 
							std::to_string ((int) gE -> _type));
						assert (false); // Just for the debug mode...
					}

					break;
			}
		}
	}
	
	delete (smt);

	return (result);
}
