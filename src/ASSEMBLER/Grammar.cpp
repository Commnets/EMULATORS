#include <ASSEMBLER/Grammar.hpp>
#include <ASSEMBLER/OperationParser.hpp>
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
	if (r == std::string::npos) r = e.find_first_of ('>'); // To get the lowest significant byte of and address
	if (r == std::string::npos) r = e.find_first_of ('<'); // To get the highest significant byte of and address
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
		// Unary operators...
		if (r == 0)
		{
			MCHEmul::Assembler::Macro m1 ("" /** no name needed. */, MCHEmul::trim (e.substr (1))); 
			m1.value (ms);
			if (!m1)
				_error = MCHEmul::Assembler::ErrorType::_MACROBADDEFINED;
			else
			{
				std::vector <MCHEmul::UByte> m1val = m1.value (ms);
				if (e [r] == '>') result = std::vector <MCHEmul::UByte> ({ m1val [m1val.size () - 1] });
				else if (e [r] == '<') result = std::vector <MCHEmul::UByte> ({ m1val [0] });
			}
		}
		// Binary operators...
		else
		{
			// ...but not this for sure...
			// It would mean there is a "symbol" before the unary operator, and can not possible!
			if (e [r] == '<' || e [r] == '>')
				_error = MCHEmul::Assembler::ErrorType::_MACROBADDEFINED;
			else
			{
				MCHEmul::Assembler::Macro m1 ("" /** No name needed. */, MCHEmul::trim (e.substr (0, r))); 
				m1.value (ms);
				MCHEmul::Assembler::Macro m2 ("", MCHEmul::trim (e.substr (r + 1))); 
				m2.value (ms);
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
		}
	}

	return (result);
}

// ---
std::vector <MCHEmul::UByte> MCHEmul::Assembler::Macro::calculateValue 
	(const std::string& e, const MCHEmul::Assembler::Macros& ms, const MCHEmul::Assembler::OperationParser* oP) const
{
	std::vector <MCHEmul::UByte> result = { }; 

	_error = MCHEmul::Assembler::ErrorType::_NOERROR;

	if (oP != nullptr)
	{
		MCHEmul::Assembler::OperationElement* oE = oP -> parser (e);
		if (oE != nullptr)
		{ 
			result = oE -> value (ms, oP).bytes ();
			if (!*oE) // At the same time the result variable might be empty!, but just in case...
			{ 
				result = { };

				_error = MCHEmul::Assembler::ErrorType::_MACRONOTDEFINED;
			}
		}
		else
			_error = MCHEmul::Assembler::ErrorType::_MACROBADDEFINED;

		delete (oE);
	}
	else
		result = calculateValue (e, ms);

	return (result);
}

// ---
MCHEmul::Assembler::CodeTemplate::CodeTemplate (const std::string& id, MCHEmul::Strings& l)
	: _name (""), 
	  _parameters (),
	  _lines (l),
	  _value (),
	  _error (MCHEmul::Assembler::ErrorType::_NOERROR)
{
	size_t pB = MCHEmul::firstSpaceIn (id);
	_name = MCHEmul::upper (MCHEmul::trim (id.substr (0, (pB == std::string::npos) ? id.length () : pB)));
	if (pB != std::string::npos) // Is there parameters?
		_parameters = MCHEmul::getElementsFrom (id.substr (pB + 1), ',');

	// There must be always a name and it has to be a valid label...
	if (!MCHEmul::validLabel (_name))
	{
		_error = MCHEmul::Assembler::ErrorType::_TEMPLATENOTVALID;

		_parameters = { }; // Just in case, but they might be at this point null!
		return;
	}

	// The parameters mut be always in the form of #[number]
	// There can not be more than 10 parameters...
	for (const auto& i : _parameters)
		if (i.length () != 2 ||
			i [0] != '?' ||
			(i [0] == '?' && !std::isdigit (i [1])))
			_error = MCHEmul::Assembler::ErrorType::_TEMPLATENOTVALID;
	if (_error == MCHEmul::Assembler::ErrorType::_TEMPLATENOTVALID)
		_parameters = { };
}

// ---
MCHEmul::Strings MCHEmul::Assembler::CodeTemplate::valueFor (const MCHEmul::Strings& prmsVal) const
{
	// Any time this method is onvoked, the errors are set back to null...
	_error = MCHEmul::Assembler::ErrorType::_NOERROR;

	// If there is no match between the parameters defined and the ones received, 
	// no calculation will be possible at all...
	if (prmsVal.size () != _parameters.size ())
	{
		_error = MCHEmul::Assembler::ErrorType::_TEMPLATENOTCALCULATED;

		return (_value = { });
	}

	// Assign the parameters received to every equivalent
	// and in the same order...
	// After the replacement symbols could be not replaced, so the normal parser will fail...
	size_t nP = 0;
	MCHEmul::Strings cLines = _lines;
	for (const auto& i : prmsVal)
	{
		for (auto& j : cLines)
			j = MCHEmul::replaceAll (j, _parameters [nP], i);
		nP++;
	}

	return (_value = cLines);
}

// ---
MCHEmul::Address MCHEmul::Assembler::GrammaticalElement::address 
	(const MCHEmul::Assembler::Semantic* s, const OperationParser* oP) const
{
	assert (s != nullptr);

	size_t b = 0;
	const MCHEmul::Assembler::GrammaticalElement* gE = this -> _previousElement;
	const MCHEmul::Assembler::StartingPointElement* sP = nullptr;
	while (gE != nullptr &&
		   (sP = dynamic_cast <const MCHEmul::Assembler::StartingPointElement*> (gE)) == nullptr)
	{
		b += gE -> size (s, oP);
		gE = gE -> _previousElement;
	}

	return ((sP == nullptr) 
		? MCHEmul::Address () /** Bad address. */ : sP ->  address (s, oP) + b);
}

// ---
std::vector <MCHEmul::UByte> MCHEmul::Assembler::GrammaticalElement::bytesFromExpression 
	(const std::string& e, const MCHEmul::Assembler::Macros& ms, bool& er, const MCHEmul::Assembler::OperationParser* oP) const
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
		m.value (ms, oP);
		if (!m) // Error?...
		{
			er = true;

			return (result);
		}

		result.insert (result.end (), m.value (ms, oP).begin (), m.value (ms, oP).end ());
	}
	
	return (result);
}

// ---
size_t MCHEmul::Assembler::BytesInMemoryElement::size (const MCHEmul::Assembler::Semantic* s, 
	const MCHEmul::Assembler::OperationParser* oP) const
{
	size_t result = 0;

	if (!(*this))
		return (0); // With error, no size...

	/** The architecture is not important at this point, so big endian is taken as default. */
	return ((_codeBytes.size () != 0) 
		? _codeBytes.size () : calculateCodeBytes (s, true, oP).size ());
}

// ---
std::vector <MCHEmul::UByte> MCHEmul::Assembler::BytesInMemoryElement::calculateCodeBytes 
	(const MCHEmul::Assembler::Semantic* s, bool bE, const MCHEmul::Assembler::OperationParser* oP) const
{
	assert (s != nullptr);

	std::vector <MCHEmul::UByte> result;

	if (!*this)
		return (result); // Not possible when previous error...

	bool e = false;
	for (MCHEmul::Strings::const_iterator i = _elements.begin (); 
			i != _elements.end () && !e; i++)
	{
		std::vector <MCHEmul::UByte> n = bytesFromExpression ((*i), s -> macros (), e, oP);
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
size_t MCHEmul::Assembler::BytesFileElement::size (const MCHEmul::Assembler::Semantic* s, 
	const MCHEmul::Assembler::OperationParser* oP) const
{
	size_t result = 0;

	if (!*this)
		return (0); // With error, no size...

	/** The architecture is not important at this point, so big endian is taken as default. */
	return ((_codeBytes.size () != 0) 
		? _codeBytes.size () : calculateCodeBytes (s, true, oP).size ());
}

// ---
std::vector <MCHEmul::UByte> MCHEmul::Assembler::BytesFileElement::calculateCodeBytes
	(const MCHEmul::Assembler::Semantic* s, bool bE, const MCHEmul::Assembler::OperationParser* oP) const
{
	assert (s != nullptr);

	std::vector <MCHEmul::UByte> result;

	if (!*this)
		return (result);

	bool e = false;
	result = MCHEmul::UBytes::loadBytesFrom (_binaryFile, e);
	if (e)
		_error = MCHEmul::Assembler::ErrorType::_BINARYFILENOTVALID;

	return (result);
}

// ---
size_t MCHEmul::Assembler::TextBytesElement::size 
	(const MCHEmul::Assembler::Semantic* s, const MCHEmul::Assembler::OperationParser* oP) const
{
	size_t result = 0;

	if (!*this)
		return (0); // With error, no size...

	/** The architecture is not important at this point, so big endian is taken as default. */
	return ((_codeBytes.size () != 0) 
		? _codeBytes.size () : calculateCodeBytes (s, true, oP).size ());
}

// ---
size_t MCHEmul::Assembler::InstructionElement::size (const Semantic* s, const MCHEmul::Assembler::OperationParser* oP) const 
{ 
	size_t result = 0;

	if (!*this)
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
				for (std::vector <MCHEmul::InstructionDefined::Structure::Parameter>::const_iterator j = 
						i -> internalStructure ()._parameters.begin (); 
						j != i -> internalStructure ()._parameters.end () && !jS; j++)
					jS = ((*j)._type == MCHEmul::InstructionDefined::Structure::Parameter::Type::_RELJUMP ||
						  (*j)._type == MCHEmul::InstructionDefined::Structure::Parameter::Type::_ABSJUMP);

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
				for (size_t i = 0; i < _possibleInstructions.size (); i++)
				{
					// The instruction "calculateCodeBytesForInstruction" sets the variable _error when it is wrong!
					// So it has to be deleted before calling it...
					_error = MCHEmul::Assembler::ErrorType::_NOERROR;
					if ((result = calculateCodeBytesForInstruction 
						(_possibleInstructions [i], _possibleParameters [i], 
						 s /** true or false is the same at this point. */, true, oP).size ()) == 
							_possibleInstructions [i] -> memoryPositions ())
						break; // Stops when the first right size is found...
				}
			}
		}
	}

	return (result);
}

// ---
std::vector <MCHEmul::UByte> MCHEmul::Assembler::InstructionElement::calculateCodeBytes 
	(const MCHEmul::Assembler::Semantic* s, bool bE, const MCHEmul::Assembler::OperationParser* oP) const
{
	assert (s != nullptr);

	std::vector <MCHEmul::UByte> result;

	if (!*this)
		return (result); // Not possible with previous error...

	for (size_t i = 0; i < _possibleInstructions.size (); i++)
	{
		_error = MCHEmul::Assembler::ErrorType::_NOERROR;
		result = calculateCodeBytesForInstruction (_possibleInstructions [i], _possibleParameters [i], s, bE, oP);
		if (!*this)
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
	(const MCHEmul::InstructionDefined* inst, const MCHEmul::Strings& prms, 
	 const MCHEmul::Assembler::Semantic* s, bool bE, const MCHEmul::Assembler::OperationParser* oP) const
{
	std::vector <MCHEmul::UByte> result;

	if (!*this)
		return (result); // Nothing possible when prervious error...

	bool e = false;
	std::vector <std::vector <MCHEmul::UByte>> bts;
	for (size_t i = 0; i < inst -> internalStructure ()._parameters.size (); i++)
	{
		std::vector <MCHEmul::UByte> bt;
		switch (inst -> internalStructure ()._parameters [i]._type)
		{
			case MCHEmul::InstructionDefined::Structure::Parameter::Type::_DATA:
			case MCHEmul::InstructionDefined::Structure::Parameter::Type::_DIR:
			{
				bt = MCHEmul::UBytes (bytesFromExpression (prms [i], s -> macros (), e, oP), bE).bytes ();
			}

			break;

			case MCHEmul::InstructionDefined::Structure::Parameter::Type::_RELJUMP:
			case MCHEmul::InstructionDefined::Structure::Parameter::Type::_ABSJUMP:
			{
				if (MCHEmul::validLabel (prms [i]))
				{
					std::vector <const MCHEmul::Assembler::LabelElement*> lbs = s -> labels ();
					std::vector <const MCHEmul::Assembler::LabelElement*>::const_iterator lbsP = 
						std::find_if (lbs.begin (), lbs.end (), 
							[=](const MCHEmul::Assembler::LabelElement* lb) -> bool { return (lb -> _name == prms [i]); });
					if (lbsP != lbs.end ())
					{
						if (inst -> internalStructure ()._parameters [i]._type == 
								MCHEmul::InstructionDefined::Structure::Parameter::Type::_RELJUMP)
						{
							MCHEmul::Address iA = address (s, oP) + inst -> memoryPositions ();
							bt = MCHEmul::UInt::fromInt (iA.distanceWith ((*lbsP) -> address (s, oP))).bytes ();
						}
						else
							bt = MCHEmul::UBytes ((*lbsP) -> address (s, oP).bytes (), bE).bytes ();
					}
					else // It is a macro and translate direcly into bytes...
						bt = MCHEmul::UBytes (bytesFromExpression (prms [i], s -> macros (), e, oP), bE).bytes ();
				}
				else
					bt = MCHEmul::UBytes (bytesFromExpression (prms [i], s -> macros (), e, oP), bE).bytes ();
			}

			break;

			default:
				assert (false); // Just in case...
		}

		// If there haven't been errors building the data...
		// They are added to the set that will be used to build up the full intruction later on...
		if (!e)
			bts.emplace_back (std::move (bt));
		// Otherwise makes no sense to continue...
		else
			break;
	}

	// If no errors have happened building the data...
	// The system tries to build up the instruction!
	if (!e)
		result = inst -> shapeCodeWithData (bts, e);

	// But if something was wrong...
	// the method finalizes with an error telling that the instruction is not valid!
	if (e)
	{
		_error = MCHEmul::Assembler::ErrorType::_INSTRUCTIONNOTVALID;

		result = { };
	}

	// ...which is finally returned...
	return (result);
}

// ---
std::vector <MCHEmul::UByte> MCHEmul::Assembler::StartingPointElement::calculateCodeBytes 
	(const MCHEmul::Assembler::Semantic* s, bool bE, const MCHEmul::Assembler::OperationParser* oP) const
{
	assert (s != nullptr);

	std::vector <MCHEmul::UByte> result;

	bool e = false;
	std::vector <MCHEmul::UByte> n = bytesFromExpression (_value, s -> macros (), e, oP);
	if (e)
		_error = MCHEmul::Assembler::ErrorType::_STARTINGPOINTNOTVALID;
	else
		result.insert (result.end (), n.begin (), n.end ());

	return (result);
}

// ---
void MCHEmul::Assembler::Semantic::addMacro (const MCHEmul::Assembler::Macro& m)
{ 
	if (_macros.find (m.name ()) == _macros.end ()) 
		_macros.insert (MCHEmul::Assembler::Macros::value_type (m.name (), m));
	else
		_error = MCHEmul::Assembler::ErrorType::_DUPLICATEMACRO;
}

// ---
void MCHEmul::Assembler::Semantic::addMacro (MCHEmul::Assembler::Macro&& m)
{
	if (_macros.find (m.name ()) == _macros.end ()) 
		_macros.insert (MCHEmul::Assembler::Macros::value_type 
			(m.name () /** A copy still needed here. */, std::move (m)));
	else
		_error = MCHEmul::Assembler::ErrorType::_DUPLICATEMACRO;
}

// ---
void MCHEmul::Assembler::Semantic::addCodeTemplate (const MCHEmul::Assembler::CodeTemplate& cT)
{
	if (_codeTemplates.find (cT.name ()) == _codeTemplates.end ()) 
	{ 
		_codeTemplates.insert (MCHEmul::Assembler::CodeTemplates::value_type (cT.name (), cT));
		if (!cT) 
			_error = cT.error (); // It could be created with error...
	}
	else
		_error = MCHEmul::Assembler::ErrorType::_DUPLICATECODETEMPLATE;
}

// ---
void MCHEmul::Assembler::Semantic::addCodeTemplate (MCHEmul::Assembler::CodeTemplate&& cT)
{
	if (_codeTemplates.find (cT.name ()) == _codeTemplates.end ()) 
	{ 
		if (!cT) 
			_error = cT.error (); // It could be created with error...

		_codeTemplates.insert (MCHEmul::Assembler::CodeTemplates::value_type 
			(cT.name () /** a copy still needed. */, std::move (cT)));
	}
	else
		_error = MCHEmul::Assembler::ErrorType::_DUPLICATECODETEMPLATE;
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

	_gramaticalElementAdded = true;
	_lastGrammaticalElementAdded = g;
}

// ---
void MCHEmul::Assembler::Semantic::addFrom (const MCHEmul::Assembler::Semantic* s)
{
	if (s == nullptr)
		return;

	// Add the macros 
	// If repeated an error will be generated
	for (const auto& i : s -> _macros)
		addMacro (i.second);

	// The same with the code termplates
	for (const auto& i : s -> codeTemplates ())
		addCodeTemplate (i.second);

	// Add the starting points at the end...
	_startingPoints.insert (_startingPoints.end (), s -> _startingPoints.begin (), s -> _startingPoints.end ());

	// drag the error...
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
		MCHEmul::Assembler::LabelElement* t = nullptr;
		for (MCHEmul::Assembler::GrammaticalElement* gE = _startingPoints [i] -> _nextElement;
			 gE != nullptr && lE == nullptr; gE = gE -> _nextElement)
		{
			t = dynamic_cast <MCHEmul::Assembler::LabelElement*> (gE);
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
