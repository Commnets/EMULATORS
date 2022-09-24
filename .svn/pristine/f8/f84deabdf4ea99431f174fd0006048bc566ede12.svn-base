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
				_error = MCHEmul::Assembler::ErrorType::_MACRONOTDEFINED; 
		}
		else
		if (MCHEmul::validBytes (e))
			result = MCHEmul::UInt::fromStr (_equivalent).bytes ();
		else
			_error = MCHEmul::Assembler::ErrorType::_MACROBADDEFINED;
	}
	else
	{
		MCHEmul::Assembler::Macro m1 ("" /** No name needed. */, MCHEmul::trim (e.substr (0, r))); m1.value (ms);
		MCHEmul::Assembler::Macro m2 ("", MCHEmul::trim (e.substr (r + 1))); m2.value (ms);
		if (!m1 || !m2)
			_error = MCHEmul::Assembler::ErrorType::_MACROBADDEFINED;
		else
		{
			MCHEmul::UInt u1 (m1.value (ms));
			MCHEmul::UInt u2 (m2.value (ms));
			if (e [r] == '*') result = (u1 * u2).bytes ();
			else if (e [r] == '+') result = (u1 + u2).bytes ();
			else /** - */ result = (u1 - u2).bytes ();
		}
	}

	return (result);
}

// ---
MCHEmul::Address MCHEmul::Assembler::GrammaticalElement::address (const MCHEmul::Assembler::Semantic* s) const
{
	assert (s != nullptr);

	size_t b = 0;
	const MCHEmul::Assembler::GrammaticalElement* gE = this -> _previousElement;
	while (dynamic_cast <const MCHEmul::Assembler::StartingPointElement*> (gE) == nullptr)
	{
		b += gE -> size (s);
		gE = gE -> _previousElement;
	}

	const MCHEmul::Assembler::StartingPointElement* sP = 
		dynamic_cast <const MCHEmul::Assembler::StartingPointElement*> (gE);
	assert (sP != nullptr); // Just in case...

	return (sP -> address (s) + b);
}

// ---
std::vector <MCHEmul::UByte> MCHEmul::Assembler::GrammaticalElement::bytesFromExpression 
	(const std::string& e, const MCHEmul::Assembler::Macros& ms, bool& er) const
{
	std::vector <MCHEmul::UByte> result;

	er = false;

	if (MCHEmul::validBytes (e))
	{
		MCHEmul::UInt n = MCHEmul::UInt::fromStr (e);
		result.insert (result.end (), n.bytes ().begin (), n.bytes ().end ());
	}
	else
	{
		MCHEmul::Assembler::Macro m ("" /** No name needed. */, e);
		m.value (ms);
		if (!m) // Error?...
		{
			er = true;
			return (result);
		}

		result.insert (result.end (), m.value (ms).begin (), m.value (ms).end ());
	}
	
	return (result);
}

// ---
std::vector <MCHEmul::UByte> MCHEmul::Assembler::BytesInMemoryElement::calculateCodeBytes 
	(const MCHEmul::Assembler::Semantic* s, bool bE) const
{
	assert (s != nullptr);

	std::vector <MCHEmul::UByte> result;

	if (!(*this))
		return (result); // Not possible when previous error...

	bool e = false;
	for (std::vector <std::string>::const_iterator i = _elements.begin (); 
			i != _elements.end () && !e; i++)
	{
		std::vector <MCHEmul::UByte> n = bytesFromExpression ((*i), s -> macros (), e);
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
size_t MCHEmul::Assembler::InstructionElement::size (const Semantic* s) const 
{ 
	size_t result = 0;

	if (!(*this))
		return (0); // With error, no size...

	if (_possibleInstructions.size () == 1)
		result = (*_possibleInstructions.begin ()) -> memoryPositions ();
	else
	{
		if (_codeBytes.size () != 0) // It has already been calculated, so the size can be determine looking at this variable...
			result = _codeBytes.size ();
		else
		{
			// The best way to calculate the size of an instruction is to determine its codebytes and then look at its size...
			// However trying to calculate the codeBytes of a jump here could generate a dead lock.
			
			// The first step to take is to determine whether there are jump instructions among the potential ones
			bool jS = false;
			unsigned short nJS = 0;
			for (auto i : _possibleInstructions)
			{
				for (std::vector <MCHEmul::Instruction::Structure::Parameter>::const_iterator j = 
						i -> internalStructure ()._parameters.begin (); 
						j != i -> internalStructure ()._parameters.end () && !jS; j++)
					jS = ((*j)._type == MCHEmul::Instruction::Structure::Parameter::Type::_RELJUMP ||
						  (*j)._type == MCHEmul::Instruction::Structure::Parameter::Type::_ABSJUMP);

				if (jS) nJS++;
			}

			// If there are jump instructions among the possible ones...
			if (nJS != 0)
			{
				// ...and not all of them are jump instructions, 
				// It would be impossible to determine the size of the instructions...
				if (nJS != _possibleInstructions.size ())
					_error = MCHEmul::Assembler::ErrorType::_GRAMARELEMENTNOTVALID;
	
				// Otherwise, all of them should have the same potential size...
				// or...the calculation would be impossible again
				// However in this case the size 0 will be returned!
				else
				{
					bool sC = false;				
					for (auto i : _possibleInstructions)
					{
						if (sC && result != i -> memoryPositions ())
							_error = MCHEmul::Assembler::ErrorType::_GRAMARELEMENTNOTVALID;
						
						sC = true;
						result = i -> memoryPositions ();
					}
				}
			}
			else
			{
				for (auto i : _possibleInstructions)
				{
					// The instruction "calculateCodeBytesForInstruction" sets the variable _error when it is wrong!
					// So it has to be deleted before calling it...
					_error = MCHEmul::Assembler::ErrorType::_NOERROR;
					if ((result = calculateCodeBytesForInstruction 
						(i, s /** true or false is the same at this point. */).size ()) == i -> memoryPositions ())
						break; // Stops when the first right size is found...
				}
			}
		}
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
	(const MCHEmul::Assembler::Semantic* s, bool bE) const
{
	assert (s != nullptr);

	if (!(*this))
		return (std::vector <MCHEmul::UByte> ()); // Not possible with previous error...

	std::vector <MCHEmul::UByte> result;
	for (size_t i = 0; i < _possibleInstructions.size (); i++)
	{
		_error = MCHEmul::Assembler::ErrorType::_NOERROR;
		result = calculateCodeBytesForInstruction (_possibleInstructions [i], s, bE);
		if (!(*this))
			continue;
		else
		{
			_selectedInstruction = _possibleInstructions [i];
			break;
		}
	}

	return (result);
}

// ---
std::vector <MCHEmul::UByte> MCHEmul::Assembler::InstructionElement::calculateCodeBytesForInstruction 
	(const MCHEmul::Instruction* inst, const MCHEmul::Assembler::Semantic* s, bool bE) const
{
	if (!(*this))
		return (std::vector <MCHEmul::UByte> ()); // Nothing possible when prervious error...

	std::vector <MCHEmul::UByte> result = 
		MCHEmul::UInt::fromUnsignedInt (inst -> code ()).bytes ();
	for (size_t i = 0; i < inst -> internalStructure ()._parameters.size (); i++)
	{
		bool e = false;
		std::vector <MCHEmul::UByte> bt;
		switch (inst -> internalStructure ()._parameters [i]._type)
		{
			case MCHEmul::Instruction::Structure::Parameter::Type::_DATA:
			case MCHEmul::Instruction::Structure::Parameter::Type::_DIR:
			{
				bt = MCHEmul::UBytes (bytesFromExpression (_parameters [i], s -> macros (), e), bE).bytes ();
			}

			break;

			case MCHEmul::Instruction::Structure::Parameter::Type::_RELJUMP:
			case MCHEmul::Instruction::Structure::Parameter::Type::_ABSJUMP:
			{
				if (MCHEmul::validLabel (_parameters [i]))
				{
					std::vector <const MCHEmul::Assembler::LabelElement*> lbs = s -> labels ();
					std::vector <const MCHEmul::Assembler::LabelElement*>::const_iterator lbsP = 
						std::find_if (lbs.begin (), lbs.end (), 
							[=](const MCHEmul::Assembler::LabelElement* lb) -> bool { return (lb -> _name == _parameters [i]); });
					if (lbsP != lbs.end ())
					{
						if (inst -> internalStructure ()._parameters [i]._type == 
								MCHEmul::Instruction::Structure::Parameter::Type::_RELJUMP)
						{
							MCHEmul::Address iA = address (s) + inst -> memoryPositions ();
							bt = MCHEmul::UInt::fromInt (iA.distanceWith ((*lbsP) -> address (s))).bytes ();
						}
						else
							bt = MCHEmul::UBytes ((*lbsP) -> address (s).bytes (), bE).bytes ();
					}
				}
				else
					bt = MCHEmul::UBytes (bytesFromExpression (_parameters [i], s -> macros (), e), bE).bytes ();
			}

			break;

			default:
				assert (false); // Just in case...
		}

		if (!e)
			result.insert (result.end (), bt.begin (), bt.end ());
	}

	// The number of bytes generated has to match with the size of the instruction...
	// When an instruction has parameters with MACRO it is not possible to know its size
	// before tha MACROS as calculated...
	if (result.size () != inst -> memoryPositions ())
	{
		_error = MCHEmul::Assembler::ErrorType::_INSTRUCTIONNOTVALID;
		result = { };
	}

	return (result);
}

// ---
std::vector <MCHEmul::UByte> MCHEmul::Assembler::StartingPointElement::calculateCodeBytes 
	(const MCHEmul::Assembler::Semantic* s, bool bE) const
{
	assert (s != nullptr);

	std::vector <MCHEmul::UByte> result;

	bool e = false;
	std::vector <MCHEmul::UByte> n = bytesFromExpression (_value, s -> macros (), e);
	if (e)
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

	_lastGrammaticalElementAdded = nS;

	return (nS);
}

// ---
void MCHEmul::Assembler::Semantic::addGrammaticalElement (MCHEmul::Assembler::GrammaticalElement* g)
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
	MCHEmul::Assembler::GrammaticalElement* lg = _startingPoints [_startingPoints.size () - 1];
	while (lg -> _nextElement != nullptr) lg = lg -> _nextElement;
	lg -> _nextElement = g;
	g -> _previousElement = lg;
	// Now if the semantic is deleted this new elements will be also deleted as it is part of the chain

	_lastGrammaticalElementAdded = g;
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

// ---
bool MCHEmul::Assembler::Semantic::existsLabel (const std::string& l) const
{
	bool result = false;

	for (size_t i = 0; i < _startingPoints.size () && !result; i++)
	{
		MCHEmul::Assembler::GrammaticalElement* gE = _startingPoints [i] -> _nextElement;
		while (gE != nullptr && !result)
		{
			result = dynamic_cast <MCHEmul::Assembler::LabelElement*> (gE) != nullptr && 
				static_cast <MCHEmul::Assembler::LabelElement*> (gE) -> _name == l;
			gE = gE -> _nextElement;
		}
	}

	return (result);
}

// ---
MCHEmul::Address MCHEmul::Assembler::Semantic::addressForLabel (const std::string& l) const
{
	MCHEmul::Assembler::LabelElement* lE = nullptr;
	for (size_t i = 0; i < _startingPoints.size () && lE == nullptr; i++)
	{
		for (MCHEmul::Assembler::GrammaticalElement* gE = _startingPoints [i] -> _nextElement;
			gE != nullptr && lE == nullptr; gE = gE -> _nextElement)
		{
			MCHEmul::Assembler::LabelElement* t = dynamic_cast <MCHEmul::Assembler::LabelElement*> (gE);
			if (t != nullptr && t -> _name == l)
				lE = t;
		}
	}

	return (lE == nullptr ? MCHEmul::Address () : lE -> address (this));
}

// ---
std::vector <const MCHEmul::Assembler::LabelElement*> MCHEmul::Assembler::Semantic::labels () const
{
	std::vector <const MCHEmul::Assembler::LabelElement*> result;

	for (auto i : _startingPoints)
		for (MCHEmul::Assembler::GrammaticalElement* e = i -> _nextElement; e != nullptr; e = e -> _nextElement)
			if (dynamic_cast <MCHEmul::Assembler::LabelElement*> (e) != nullptr)
				result.push_back (static_cast <MCHEmul::Assembler::LabelElement*> (e));

	return (result);
}

// ---
std::map <std::string, MCHEmul::Address> MCHEmul::Assembler::Semantic::labelAddresses () const
{
	std::map <std::string, MCHEmul::Address> result;
	std::vector <const MCHEmul::Assembler::LabelElement*> l = labels ();

	for (auto i : l)
		result [i -> _name] = i -> address (this);

	return (result);
}
