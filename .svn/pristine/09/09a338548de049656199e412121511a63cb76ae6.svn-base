#include <ASSEMBLER/Grammar.hpp>
#include <algorithm>

// ---
std::vector <MCHEmul::UByte> MCHEmul::Assembler::Macro::calculateValue 
	(const std::string& e, const MCHEmul::Assembler::Macros& ms) const
{
	std::vector <MCHEmul::UByte> result = { }; 

	_error = MCHEmul::Assembler::ErrorType::_NOERROR;

	// How to read:
	// Check whether it exists operations between other macros to define this one...
	// The checking is done in other or importance!
	// If no operation symbold is found in the macro value definition, 
	// it is supossed that it will directly be either a label or a set of valid data... 

	size_t r = e.find_first_of ('*'); // To multiply values...
	if (r == std::string::npos) r = e.find_first_of ('+'); // To add values...
	if (r == std::string::npos) r = e.find_first_of ('-'); // To subtract values...
	if (r == std::string::npos)
	{
		if (MCHEmul::validLabel (e))
		{
			MCHEmul::Assembler::Macros::const_iterator i = ms.find (e);
			if (i != ms.end ())
			{
				result = (*i).second.value (ms);
				if (!(*i).second)
				{
					_error = (*i).second._error;
					result = { }; // Just to be sure...
				}
			}
			else
				_error = MCHEmul::Assembler::ErrorType::_MACROBADDEFINED; 
		}
		else
		if (MCHEmul::validBytes (e))
			result = MCHEmul::UInt::fromStr (_equivalent).bytes ().values ();
		else
			_error = MCHEmul::Assembler::ErrorType::_MACROBADDEFINED;
	}
	else
	{
		MCHEmul::Assembler::Macro m1 ("" /** No name needed. */, e.substr (0, r)); m1.value (ms);
		MCHEmul::Assembler::Macro m2 ("", e.substr (0, r + 1)); m2.value (ms);
		if (!m1 || !m2)
			_error = MCHEmul::Assembler::ErrorType::_MACRONOEVALUATED;
		else
		{
			MCHEmul::UInt u1 (m1.value (ms));
			MCHEmul::UInt u2 (m2.value (ms));
			if (e [r] == '*') result = (u1 * u2).bytes ().values ();
			else if (e [r] == '+') result = (u1 + u2).bytes ().values ();
			else /** - */ result = (u1 - u2).bytes ().values ();
		}
	}

	return (result);
}

// ---
std::vector <MCHEmul::UByte> MCHEmul::Assembler::GramaticalElement::bytesFromExpression 
	(const std::string& e, const MCHEmul::Assembler::Macros& ms) const
{
	std::vector <MCHEmul::UByte> result;

	if (MCHEmul::validBytes (e))
	{
		MCHEmul::UInt n = MCHEmul::UInt::fromStr (e);
		result.insert (result.end (), 
			n.bytes ().values ().begin (), n.bytes ().values ().end ());
	}
	else
	{
		MCHEmul::Assembler::Macro m ("" /** No name needed. */, e);
		m.value (ms);
		if (!m) // Error?...
			return (result);
		result.insert (result.end (), m.value (ms).begin (), m.value (ms).end ());
	}
	
	return (result);
}

// ---
std::vector <MCHEmul::UByte> MCHEmul::Assembler::BytesInMemoryElement::calculateCodeBytes 
	(const MCHEmul::Assembler::Semantic* s, const MCHEmul::CPUArchitecture& a) const
{
	assert (s != nullptr);

	std::vector <MCHEmul::UByte> result;

	if (!(*this))
		return (result); // Not possible when previous error...

	bool e = false;
	for (std::vector <std::string>::const_iterator i = _elements.begin (); 
			i != _elements.end () && !e; i++)
	{
		std::vector <MCHEmul::UByte> n = bytesFromExpression ((*i), s -> macros ());
		e = n.empty (); // Empty when impossible to determine the value...
		result.insert (result.end (), n.begin (), n.end ());
	}

	if (e)
	{
		_error = MCHEmul::Assembler::ErrorType::_BYTESNOTVALID;
		result = { }; // Nothing when error...
	}

	return (result);
}

// ---
bool MCHEmul::Assembler::InstructionElement::hasAnyLabelAsParameter (const MCHEmul::Assembler::Semantic* s) const
{
	assert (s != nullptr);

	bool result = false;
	for (size_t i = 0; i < _parameters.size () && !result; i++)
		result = MCHEmul::validLabel (_parameters [i]) && 
					(s -> macros ().find (_parameters [i]) == s -> macros ().end ());

	return (result);
}

// ---
std::vector <size_t> MCHEmul::Assembler::InstructionElement::labelParameters (const MCHEmul::Assembler::Semantic* s) const
{
	std::vector <size_t> result;

	for (size_t i = 0; i < _parameters.size (); i++)
		if (MCHEmul::validLabel (_parameters [i]) && 
			(s -> macros ().find (_parameters [i]) == s -> macros ().end ()))
			result.push_back (i);

	return (result);
}

// ---
std::vector <MCHEmul::UByte> MCHEmul::Assembler::InstructionElement::calculateCodeBytes 
	(const MCHEmul::Assembler::Semantic* s, const MCHEmul::CPUArchitecture& a) const
{
	assert (s != nullptr);

	if (!(*this))
		return (std::vector <MCHEmul::UByte> ()); // Not possible with previous error...

	std::vector <MCHEmul::UByte> result = 
		MCHEmul::UInt::fromUnsignedInt (_instruction -> code ()).bytes ().values ();
	for (size_t i = 0; i < _instruction -> internalStructure ()._parameters.size (); i++)
	{
		std::vector <MCHEmul::UByte> bt;
		switch (_instruction -> internalStructure ()._parameters [i]._type)
		{
			case MCHEmul::Instruction::Structure::Parameter::Type::_DATA:
			case MCHEmul::Instruction::Structure::Parameter::Type::_DIR:
				bt = MCHEmul::UBytes (bytesFromExpression (_parameters [i], s -> macros ()), a.bigEndian ()).values ();
				break;

			case MCHEmul::Instruction::Structure::Parameter::Type::_JUMP:
				bt = std::vector <MCHEmul::UByte> 
					(_instruction -> internalStructure ()._parameters [i]._numberBytes, { 0x00 }); // cann't be calculated here...
				break;

			default:
			assert (false); // Just in case...
		}

		result.insert (result.end (), bt.begin (), bt.end ());
	}

	// The number of bytes generated has to match with the size of the instruction...
	// When an instruction has parameters with MACRO it is not possible to know its size
	// before tha MACROS as calculated...
	if (result.size () != _instruction -> memoryPositions ())
	{
		_error = MCHEmul::Assembler::ErrorType::_INSTRUCTIONNOTVALID;
		result = { };
	}

	return (result);
}

// ---
std::vector <MCHEmul::UByte> MCHEmul::Assembler::StartingPointElement::calculateCodeBytes 
	(const MCHEmul::Assembler::Semantic* s, const MCHEmul::CPUArchitecture& a) const
{
	assert (s != nullptr);

	std::vector <MCHEmul::UByte> result;

	std::vector <MCHEmul::UByte> n = bytesFromExpression (_value, s -> macros ());
	if (n.empty ())
		_error = MCHEmul::Assembler::ErrorType::_STARTINGPOINTNOTVALID;
	else
		result.insert (result.end (), n.begin (), n.end ());

	return (result);
}

// ---
void MCHEmul::Assembler::Semantic::addMacro (const Macro& m)
{ 
	if (_macros.find (m.name ()) == _macros.end ()) 
		_macros.insert (MCHEmul::Assembler::Macros::value_type (m.name (), m));
	else
		_error = MCHEmul::Assembler::ErrorType::_DUPLICATEMACRO;
}

// ---
MCHEmul::Assembler::StartingPointElement* MCHEmul::Assembler::Semantic::addNewStartingPoint ()
{ 
	MCHEmul::Assembler::StartingPointElement* nS = new StartingPointElement;
	_startingPoints.push_back (nS);

	_lastGramaticalElementAdded = nS;

	return (nS);
}

// ---
void MCHEmul::Assembler::Semantic::addGramaticalElement (MCHEmul::Assembler::GramaticalElement* g)
{
	_error = MCHEmul::Assembler::ErrorType::_NOERROR;

	// ...only valid grama elements...
	if (g == nullptr)
	{
		_error = MCHEmul::Assembler::ErrorType::_GRAMARELEMENTNOTVALID;
		return; 
	}

	// ...only if there is a starting points defined...
	if (_startingPoints.empty ())
	{
		_error = MCHEmul::Assembler::ErrorType::_STARTINGPOINTNOTDEFINED;
		return; // If there were no entry points the new grama element couldn't be added ever...
	}

	// ...and only then the gramatical element has no errors...
	if (!(*g))
	{
		_error = MCHEmul::Assembler::ErrorType::_SEMANTICERROR;
		return;
	}

	// Other wise is linked to the element linked to the last starting point
	// That will be, BTW, the actual one...
	MCHEmul::Assembler::GramaticalElement* lg = _startingPoints [_startingPoints.size () - 1];
	while (lg -> _nextElement != nullptr) lg = lg -> _nextElement;
	lg -> _nextElement = g;
	// Now if the semantic is deleted this new elements will be also deleted as it is part of the chain

	_lastGramaticalElementAdded = g;
}

// ---
void MCHEmul::Assembler::Semantic::addFrom (const MCHEmul::Assembler::Semantic* s)
{
	assert (s != nullptr);

	// Add the macros 
	// If repeated an error will be generated
	for (auto i : s -> _macros)
		addMacro (i.second);

	// Add the starting points at the end...
	_startingPoints.insert (_startingPoints.end (), s -> _startingPoints.begin (), s -> _startingPoints.end ());

	// The error status will be the same...
	_error = s -> _error;
}
