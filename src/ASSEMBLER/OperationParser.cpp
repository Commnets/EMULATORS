#include <ASSEMBLER/OperationParser.hpp>

// ---
bool MCHEmul::Assembler::OperationParser::valid (const std::string& o) const
{
	MCHEmul::Assembler::OperationElement* oE = parser (o);
	bool result = (oE != nullptr);
	delete (oE);
	return (result);
}

// ---
MCHEmul::Assembler::OperationElement* MCHEmul::Assembler::OperationParser::parser (const std::string& o) const
{
	std::string oC = MCHEmul::noSpaces (o);

	bool e = false;

	// First of all the parenthesis are parsed, stored in a tmp structure and replaced by labels
	MCHEmul::Assembler::OperationParser::tOpElements tmpOpElmnts;
	unsigned int ct = 0;
	while (oC.find ("(") != std::string::npos && !e) // While there is a parenthesis...
		removeAndParserParenthesis (oC, tmpOpElmnts, ct, e);

	// While removing the parenthesis, error can happen
	// Then nothing is valid, and the temporal structures created must be removed...
	// The previous process would also be invalid if there had still parenthesis remaining!
	if (e || oC.find ("(") != std::string::npos || oC.find (")") != std::string::npos)
	{
		deleteTOpElements (); 

		return (nullptr);
	}

	// After this "oC" shouldn't have any parenthesis...
	// and the content of those parenthesis will be kept into the tmpOpElemnts variable (that is a set)
	// so there is nothing else to do that parsing the remaining string...
	MCHEmul::Assembler::OperationElements result = parserPiece (oC, tmpOpElmnts, ct, e);
	// Here there should be only one element in the list...
	return (result [0]);
}

// ---
MCHEmul::Assembler::OperationElement* MCHEmul::Assembler::OperationParser::createBinaryOperationElement
	(unsigned char s, MCHEmul::Assembler::OperationElement* l, MCHEmul::Assembler::OperationElement* r) const
{
	MCHEmul::Assembler::OperationElement* result = nullptr;
	switch (s)
	{
		case '+':
			result = new MCHEmul::Assembler::AddFunctionOperationElement (l, r);
			break;

		case '-':
			result = new MCHEmul::Assembler::SubtractFunctionOperationElement (l, r);
			break;

		case '*':
			result = new MCHEmul::Assembler::MultiplyFunctionOperationElement (l, r);
			break;

		case '/':
			result = new MCHEmul::Assembler::DivideFunctionOperationElement (l, r);
			break;

		default:
			break;
	}

	return (result);
}

// ---
MCHEmul::Assembler::OperationElement* MCHEmul::Assembler::OperationParser::createUnaryOperationElement
	(unsigned char s, MCHEmul::Assembler::OperationElement* o) const
{
	MCHEmul::Assembler::OperationElement* result = nullptr;
	switch (s)
	{
		case '>':
			result = new MCHEmul::Assembler::LSBFunctionOperationElement (o);
			break;

		case '<':
			result = new MCHEmul::Assembler::MSBFunctionOperationElement (o);
			break;

		default:
			break;
	}

	return (result);
}

// ---
void MCHEmul::Assembler::OperationParser::removeAndParserParenthesis
	(std::string& f, MCHEmul::Assembler::OperationParser::tOpElements& tmp, unsigned int& ct, bool& e) const
{
	auto cToString = [&](unsigned int n, unsigned int s) -> std::string
		{ std::string r = std::to_string (n); 
		  return (((r.length () < s) ? MCHEmul::_SPACES.substr (0, s - r.length ()) : "") + r); };

	e = false;

	size_t pO = f.find_last_of ("(");
	if (pO == std::string::npos)
		return; // No errors but no inner parenthesis has been found...

	size_t pC = f.find_first_of (")", pO);
	if (pC == std::string::npos)
	{
		e = true; // Error, as no closing symbol has been found for the parenthesis...

		return;
	}

	// Gets the content of the parenthesis to be parsed...
	std::string pCnt = f.substr (pO + 1, pC - pO - 1);
	// ...replace the inner parentheis by a symbol...
	std::string pId = "##" + cToString (ct++, 4) + "##";
	f = f.substr (0, pO) + pId + f.substr (pC + 1);
	// Then parser the parenthesis and storing the outcome into the temporal structure of data...
	tmp [pId] = MCHEmul::Assembler::OperationParser::tOpElement (pId, parserPiece (pCnt, tmp, ct, e));
}

// ---
MCHEmul::Assembler::OperationElements MCHEmul::Assembler::OperationParser::parserPiece 
	(const std::string& o, MCHEmul::Assembler::OperationParser::tOpElements& r, unsigned int& ct, bool& e) const
{
	MCHEmul::Assembler::OperationElements result;

	size_t pos = std::string::npos;
	size_t rubbish = 0; // Info not used...

	if ((pos = o.find (',')) != std::string::npos)
		result = parserListPiece (o, r, ct, e);
	else
	if ((pos = MCHEmul::firstOf (o, validBinarySymbols ())) != std::string::npos)
		result = parserBinaryPiece (o, r, ct, e);
	else
	if ((pos = MCHEmul::firstOf (o, validUnarySymbols ())) != std::string::npos)
		result = parserBinaryPiece (o, r, ct, e);
	else
	if ((pos = MCHEmul::firstOf (o, validFunctionNames (), rubbish)) != std::string::npos)
		result = parserFunctionPiece (o, r, ct, e);
	else
	if ((pos = o.find_first_of ("##")) != std::string::npos)
	{
		MCHEmul::Assembler::OperationParser::tOpElements::const_iterator i = r.find (o.substr (pos, 8 /** ##XXX## */));
		if (i == r.end ()) e = true;
		else result = (*i).second._elements; // Just copy it!
	}

	return (result);
}

// ---
MCHEmul::Assembler::OperationElements MCHEmul::Assembler::OperationParser::parserListPiece 
	(const std::string& o, MCHEmul::Assembler::OperationParser::tOpElements& r, unsigned int& ct, bool& e) const
{
	static MCHEmul::Assembler::OperationElements _NORESULT;

	MCHEmul::Assembler::OperationElements result;

	MCHEmul::Strings elmts = MCHEmul::getElementsFrom (o, ',');

	for (size_t i = 0; i < elmts.size () && !e; i++)
	{
		MCHEmul::Assembler::OperationElements rst = parserPiece (elmts [i], r, ct, e);
		if (!e) 
		{ 
			deleteOperationElements (rst); 
			deleteOperationElements (result);  
			result = _NORESULT;
				
			continue; 
		}
		else 
			result.insert (result.end (), rst.begin (), rst.end ());
	}

	return (result);
}

// ---
MCHEmul::Assembler::OperationElements MCHEmul::Assembler::OperationParser::parserBinaryPiece 
	(const std::string& o, MCHEmul::Assembler::OperationParser::tOpElements& r, unsigned int& ct, bool& e) const
{
	static MCHEmul::Assembler::OperationElements _NORESULT;

	size_t pos = MCHEmul::firstOf (o, validBinarySymbols ());
	
	MCHEmul::Assembler::OperationElements oLeft = parserPiece (o.substr (0, pos), r, ct, e);
	if (!e || oLeft.size () != 1) 
	{ 
		deleteOperationElements (oLeft); 
		
		return (_NORESULT); 
	}
	
	MCHEmul::Assembler::OperationElements oRight = parserPiece (o.substr (pos + 1), r, ct, e);
	if (!e || oRight.size () != 1) 
	{ 
		deleteOperationElements (oLeft); deleteOperationElements (oRight); 
		
		return (_NORESULT); 
	}

	MCHEmul::Assembler::OperationElement* rst = createBinaryOperationElement (o [pos], oLeft [0], oRight [0]);
	if (rst == nullptr) 
	{ 
		deleteOperationElements (oLeft); deleteOperationElements (oRight); 
		
		return (_NORESULT); 
	}
	
	return (MCHEmul::Assembler::OperationElements ({ rst }));
}

// ---
MCHEmul::Assembler::OperationElements MCHEmul::Assembler::OperationParser::parserUnaryPiece 
	(const std::string& o, MCHEmul::Assembler::OperationParser::tOpElements& r, unsigned int& ct, bool& e) const
{
	static MCHEmul::Assembler::OperationElements _NORESULT;

	size_t pos = MCHEmul::firstOf (o, validUnarySymbols ());
	MCHEmul::Assembler::OperationElements oRight = parserPiece (o.substr (pos + 1), r, ct, e);
	if (!e || oRight.size () != 1) 
	{ 
		deleteOperationElements (oRight); 
		
		return (_NORESULT); 
	}

	MCHEmul::Assembler::OperationElement* rst = createUnaryOperationElement (o [pos], oRight [0]);
	if (rst == nullptr) 
	{ 
		deleteOperationElements (oRight); 
		
		return (_NORESULT); 
	}

	return (MCHEmul::Assembler::OperationElements ({ rst }));
}

// ---
MCHEmul::Assembler::OperationElements MCHEmul::Assembler::OperationParser::parserFunctionPiece 
	(const std::string& o, MCHEmul::Assembler::OperationParser::tOpElements& r, unsigned int& ct, bool& e) const
{
	MCHEmul::Strings vF = validFunctionNames ();
	size_t fPos = 0; size_t pos = MCHEmul::firstOf (o, vF, fPos);
	return (MCHEmul::Assembler::OperationElements 
		({ createFunctionOperationElement (parserPiece (o.substr (pos + vF [fPos].length ()), r, ct, e)) }));
}

// ---
void MCHEmul::Assembler::OperationParser::deleteTOpElements () const
{
	for (const auto& i : _tOpElements)
		for (const auto& j : i.second._elements)
			delete (j);
}
