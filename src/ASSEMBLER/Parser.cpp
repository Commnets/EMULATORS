#include <ASSEMBLER/Parser.hpp>
#include <fstream>
#include <algorithm>
#include <functional>

const std::string MCHEmul::Assembler::BinaryDefinitionParser::_DEFVALUE = "";

// ---
void MCHEmul::Assembler::ParserContext::actualizeGlobalParametersFrom (const MCHEmul::Assembler::ParserContext& pC)
{
	_errors = pC._errors;
	_filesAlreadyParsed = pC._filesAlreadyParsed;
	_lastStartingPointId = pC._lastStartingPointId;
	_lastLabelId = pC._lastLabelId;
	_lastBytesId = pC._lastBytesId;
	_lastInstructionId = pC._lastInstructionId;
}

// ---
void MCHEmul::Assembler::IncludeCommandParser::parse (MCHEmul::Assembler::ParserContext* pC) const
{ 
	if (!canParse (pC))
	{
		pC -> _currentLine = "";

		return; // It doesn't generate error, but "forgets" the line...
	}

	std::string fN = MCHEmul::trim (pC -> _currentLine.substr
		(1, pC -> _currentLine.find (parser () -> commentSymbol () /** Until a potencial comment */) - 1));

	// The file is parsed only if it wasn't before!
	bool fP = false;
	for (size_t i = 0; i < pC -> _filesAlreadyParsed.size () && !fP; fP = (pC -> _filesAlreadyParsed [i++] == fN));
	if (!fP)
	{ 
		pC -> _filesAlreadyParsed.push_back (fN); // To avoid analyze twice...

		// Create a new parser context that inituially will be the same...
		// The semantic is minatined, and the rest of the variables are copied...
		// Many of them has a meaning within the file to be parser
		// and others are general and they will have to be get back after parsing...
		MCHEmul::Assembler::ParserContext* nPC = new MCHEmul::Assembler::ParserContext (*pC);
		// These instructions will keep most of the variables related with the previous context
		parser () -> parse (fN, fN + ".act", nPC /**  no semantic not context is created. */);
		// After parsing the variable nPC is no longer needed
		// But the global parameters actualized during the inner parsing have to be actualized here
		// before destroying it.
		pC -> actualizeGlobalParametersFrom (*nPC);
		delete (nPC);
	}

	// No mere things after this one...
	pC -> _currentLine = "";
}

// ---
void MCHEmul::Assembler::MacroCommandParser::parse (MCHEmul::Assembler::ParserContext* pC) const
{
	if (!canParse (pC))
	{
		pC -> _currentLine = "";

		return; // It doesn't generate error, but "forgets" the line...
	}

	size_t mD = pC -> _currentLine.find (_symbol);
	pC -> _semantic -> addMacro (MCHEmul::Assembler::Macro
		(std::move (MCHEmul::trim (pC -> _currentLine.substr (0, mD))),
		 std::move (MCHEmul::trim (pC -> _currentLine.substr (mD + 1,
			 pC -> _currentLine.find (parser () -> commentSymbol () /** Until a potential comment. */) - (mD + 1)))), 
		 pC -> _file, pC -> _currentLineNumber)); // Including where the macro has been defined...
	// The macro is added just if there is no other with the same value...
	// Otherwise the instruction above will generate an error that will be kept under semantic object...

	pC -> _currentLine = ""; // The line is completed...
}

// ---
bool MCHEmul::Assembler::CodeTemplateDefinitionCommandParser::canParse (MCHEmul::Assembler::ParserContext* pC) const
{
	if (pC -> _currentLine [0] != _initialSymbol) // Starting with the symbol...
		return (false);
	if (pC -> _currentLine.length () < 2) // ..and having at least a name with a single letter...
		return (false);

	bool result = true;
	for (unsigned int i = pC -> _currentLineNumber + 1; i < pC -> _lines.size () && !result; i++)
		result = pC -> _lines [i][0] == _finalSymbol;
	
	return (result);
}

// ---
void MCHEmul::Assembler::CodeTemplateDefinitionCommandParser::parse (MCHEmul::Assembler::ParserContext* pC) const
{
	// See the different behaviour details when can or not can be parsed!

	bool cP = canParse (pC);

	std::string id = cP ? MCHEmul::trim (pC -> _currentLine.substr (1)) : "";
	MCHEmul::Strings ctLns;
	for (pC -> _linesPointer++, pC -> _actionLinesPointer++, pC -> _currentLineNumber++;
			pC -> _linesPointer != pC -> _lines.end () && ((*pC -> _linesPointer)[0] != _finalSymbol);
			pC -> _linesPointer++, pC -> _actionLinesPointer++, pC -> _currentLineNumber++)
		if (cP) ctLns.push_back (*pC -> _linesPointer);

	// The definition can finishs if the end of the file comes...
	pC -> _currentLine = "";
	if (cP)
		pC -> _semantic -> addCodeTemplate (MCHEmul::Assembler::CodeTemplate (id, ctLns));
		// Difficult hre to apply the move smemantics as CodeTemplate constructor runs a lot of things...
}

// ---
void MCHEmul::Assembler::CodeTemplateUseCommandParser::parse (MCHEmul::Assembler::ParserContext* pC) const
{
	if (!canParse (pC))
	{
		pC -> _currentLine = "";

		return; // It doesn't generate error, but "forgets" the line...
	}

	MCHEmul::Strings prms;
	std::string cTID = MCHEmul::trim (pC -> _currentLine.substr (1));
	size_t p = MCHEmul::firstSpaceIn (cTID);
	if (p != std::string::npos) // It has parameters...
	{
		prms = std::move (MCHEmul::getElementsFrom (MCHEmul::trim (cTID.substr (p + 1, // Get the parameters...
			cTID.find (parser () -> commentSymbol ()) - (p + 1) /** Until a potential comment at the end. */)), ',')); 
		cTID = cTID.substr (0, p); // ...and the name is just the "thing" before the spaces...
	}

	MCHEmul::Assembler::CodeTemplate cT; // With error initially...
	MCHEmul::Assembler::CodeTemplates::const_iterator i;
	if ((i = pC -> _semantic -> codeTemplates ().find (cTID)) != 
				pC -> _semantic -> codeTemplates ().end ())
	{
		cT = (*i).second;
		MCHEmul::Strings nL = cT.valueFor (prms); // It can generate errors...
		MCHEmul::Strings nLA (nL.size (), ""); // ...but no actions can be defined in them...
		if (!cT)
			pC -> _errors.emplace_back (MCHEmul::Assembler::Error (cT.error (), pC -> _file, pC -> _currentLineNumber, 0));
		else // If no errors, the lines are inserted in the context to be parsed!
		{
			pC -> _templateLinesNumber = (unsigned int) nL.size ();

			// Remove the old lines, and insert the new ones...
			pC -> _linesPointer = pC -> _lines.erase (pC -> _linesPointer); 
			pC -> _actionLinesPointer = pC -> _actionLines.erase (pC -> _actionLinesPointer); 
			pC -> _linesPointer = pC -> _lines.insert (pC -> _linesPointer, nL.begin (), nL.end ()); 
			pC -> _actionLinesPointer = pC -> _actionLines.insert (pC -> _actionLinesPointer, nLA.begin (), nLA.end ()); 
		}
	}

	// If there is nothing strange the line to parser becomes then the new one...
	pC -> _currentLine = (pC -> _linesPointer != pC -> _lines.end ()) ? MCHEmul::trim (*pC -> _linesPointer) : ""; 
}

// ---
void MCHEmul::Assembler::StartingPointCommandParser::parse (MCHEmul::Assembler::ParserContext* pC) const
{
	if (!canParse (pC))
	{
		pC -> _currentLine = "";

		return; // It doesn't generate error, but "forgets" the line
	}

	size_t mD = pC -> _currentLine.find (_symbol);
	MCHEmul::Assembler::StartingPointElement* nE = pC -> _semantic -> addNewStartingPoint (); // Starts a new entry point...
	nE -> _id = pC -> _lastStartingPointId++; // Sequential...
	nE -> _file = pC -> _file;
	nE -> _line = pC -> _currentLineNumber;
	nE -> _value = std::move (MCHEmul::trim (pC -> _currentLine.substr (mD + 1,
		pC -> _currentLine.find (parser () -> commentSymbol () /** Until a potential comment. */) - (mD + 1))));

	// Here it is not checked whether _value is or not valid
	// _value could include symbols and it should be treated later like a macro...

	pC -> _currentLine = ""; // The line is completed...
}

// ---
void MCHEmul::Assembler::LabelCommandParser::parse (MCHEmul::Assembler::ParserContext* pC) const
{
	if (!canParse (pC))
	{
		pC -> _currentLine = "";

		return; // It doesn't generate error, but "forgets" the line
	}

	size_t eL = pC -> _currentLine.find (_symbol);
	MCHEmul::Assembler::LabelElement* nE = new MCHEmul::Assembler::LabelElement;
	nE -> _id = pC -> _lastLabelId++; // Sequential...
	nE -> _file = pC -> _file;
	nE -> _line = pC -> _currentLineNumber;
	nE -> _name = std::move (MCHEmul::trim (pC -> _currentLine.substr (0, eL)));
	// The label has to be valid...
	if (!MCHEmul::validLabel (nE -> _name))
		nE -> _error = MCHEmul::Assembler::ErrorType::_LABELNOTVALID;

	// The line has not been read totally...
	pC -> _currentLine = std::move (MCHEmul::trim (pC -> _currentLine.substr (eL + 1)));
	// The element created is added to the semantic...
	pC -> _semantic -> addGrammaticalElement (nE);
}

// ---
void MCHEmul::Assembler::BytesCommandParser::parse (MCHEmul::Assembler::ParserContext* pC) const
{
	if (!canParse (pC))
	{
		pC -> _currentLine = "";

		return; // It doesn't generate error, but "forgets" the line
	}

	size_t eL = MCHEmul::firstSpaceIn (pC -> _currentLine); // The first space like defines where the data starts...
	MCHEmul::Assembler::BytesInMemoryElement* nE = new MCHEmul::Assembler::BytesInMemoryElement;
	nE -> _id = pC -> _lastBytesId++; // Sequential...
	nE -> _file = pC -> _file;
	nE -> _line = pC -> _currentLineNumber;
	MCHEmul::Strings dt = MCHEmul::getElementsFrom (pC -> _currentLine.substr (eL + 1,
			pC -> _currentLine.find (parser () -> commentSymbol () /** Until a potential comment. */) - (eL + 1)), ' ');
	// Erase all "empty" elements!
	MCHEmul::Strings::iterator i = dt.begin (); 
	while (i != dt.end ()) if (MCHEmul::trim ((*i)) == "") i = dt.erase (i); else i++;
	nE -> _elements = std::move (dt); // No longer needed...
	if (nE -> _elements.empty ())
		nE -> _error = MCHEmul::Assembler::ErrorType::_BYTESNOTVALID;

	// The element created is added to the gramatic...
	pC -> _semantic -> addGrammaticalElement (nE);

	// The line is completed...
	pC -> _currentLine = "";
}

// ---
void MCHEmul::Assembler::BinaryDefinitionParser::parse (MCHEmul::Assembler::ParserContext* pC) const
{
	if (!canParse (pC))
	{
		pC -> _currentLine = "";

		return; // It doesn't generate error, but "forgets" the line
	}

	size_t eL = MCHEmul::firstSpaceIn (pC -> _currentLine); // The first space like defines where the data really starts...
	pC -> _currentLineNumber++;
	MCHEmul::Strings defs = MCHEmul::getElementsFrom (pC -> _currentLine.substr (eL + 1,
		pC -> _currentLine.find (parser () -> commentSymbol () /** Until a potential comment. */) - (eL + 1)), ' ');
	for (const auto& i : defs)
	{	
		std::string def = MCHEmul::trim (i);
		size_t ePos = def.find ('=');
		if (ePos != std::string::npos)
		{
			std::string chr = def.substr (0, ePos);
			std::string value = def.substr (ePos + 1);
			if (chr.length () == 1 && std::find_if (value.begin (), value.end (), 
				[](char c) -> bool { return (c != '1' && c != '0'); }) == value.end ())
				_definitionMap.insert (MCHEmul::Assembler::BinaryDefinitionParser::DefMap::value_type (chr [0], value));
			else
				pC -> _errors.emplace_back (MCHEmul::Assembler::Error 
					(MCHEmul::Assembler::ErrorType::_MACROBADDEFINED, pC -> _file, pC -> _currentLineNumber, 0, def));
		}
	}

	// The line is completed...
	pC -> _currentLine = "";
}

// ---
void MCHEmul::Assembler::BinaryCommandParser::parse (MCHEmul::Assembler::ParserContext* pC) const
{
	if (!canParse (pC))
	{
		pC -> _currentLine = "";

		return; // It doesn't generate error, but "forgets" the line
	}

	size_t eL = MCHEmul::firstSpaceIn (pC -> _currentLine); // The first space like defines where the data starts...
	MCHEmul::Assembler::BytesInMemoryElement* nE = new MCHEmul::Assembler::BytesInMemoryElement;
	nE -> _id = pC -> _lastBytesId++; // Sequential...
	nE -> _file = pC -> _file;
	nE -> _line = pC -> _currentLineNumber;
	std::string dt = MCHEmul::noSpaces (pC -> _currentLine.substr (eL));
	for (size_t i = 0; i < dt.length (); i++)
		if (dt [i] != '0' && dt [i] != '1') // Other thing than an 0 or an 1, should be managed by a definition...
			dt = dt.substr (0, i) + 
				 ((_definitionParser != nullptr) ? _definitionParser -> definitionFor (dt [i]) : "0") +
				 dt.substr (i + 1);
	nE -> _elements = std::move ((dt == "") ? MCHEmul::Strings () : MCHEmul::Strings ({ "z" /** for binary. */ + dt }));
	if (nE -> _elements.empty ())
		nE -> _error = MCHEmul::Assembler::ErrorType::_BYTESNOTVALID;

	// The element created is added to the gramatic...
	pC -> _semantic -> addGrammaticalElement (nE);

	// The line is completed...
	pC -> _currentLine = "";
}

// ---
void MCHEmul::Assembler::TextCommandParser::parse (MCHEmul::Assembler::ParserContext* pC) const
{
	if (!canParse (pC))
	{
		pC -> _currentLine = "";

		return; // It doesn't generate error, but "forgets" the line
	}

	size_t eL = MCHEmul::firstSpaceIn (pC -> _currentLine); // The first space like defines where the data starts...
	MCHEmul::Assembler::TextBytesElement* nE = new MCHEmul::Assembler::TextBytesElement;
	nE -> _id = pC -> _lastBytesId++; // Sequential...
	nE -> _file = pC -> _file;
	nE -> _line = pC -> _currentLineNumber;
	std::string dt = MCHEmul::trim (pC -> _currentLine.substr (eL));
	if (dt [0] != '"' || dt [dt.length () - 1] != '"') // The text has to be separed with "...
		nE -> _error = MCHEmul::Assembler::ErrorType::_BYTESNOTVALID;
	nE -> _text = std::move (dt.substr (1, dt.length () - 2));
	nE -> _ASCIIConverter = _ASCIIConverter; // Transfer the ascii converter...

	// The element created is added to the gramatic...
	pC -> _semantic -> addGrammaticalElement (nE);

	// The line is completed...
	pC -> _currentLine = "";
}

// ---
void MCHEmul::Assembler::LoadBytesFileCommandParser::parse (MCHEmul::Assembler::ParserContext* pC) const
{
	if (!canParse (pC))
	{
		pC -> _currentLine = "";

		return; // It doesn't generate error, but "forgets" the line
	}

	size_t eL = MCHEmul::firstSpaceIn (pC -> _currentLine); // The first space like defines where the data starts...
	MCHEmul::Assembler::BytesFileElement* nE = new MCHEmul::Assembler::BytesFileElement;
	nE -> _id = pC -> _lastBytesId++; // Sequential...
	nE -> _file = pC -> _file;
	nE -> _line = pC -> _currentLineNumber;
	nE -> _binaryFile = std::move (MCHEmul::noSpaces (pC -> _currentLine.substr (eL)));
	if (nE -> _binaryFile == "")
		nE -> _error = MCHEmul::Assembler::ErrorType::_BYTESNOTVALID;

	// The element created is added to the gramatic...
	pC -> _semantic -> addGrammaticalElement (nE);

	// The line is completed...
	pC -> _currentLine = "";
}

// ---
bool MCHEmul::Assembler::InstructionCommandParser::canParse (MCHEmul::Assembler::ParserContext* pC) const
{
	std::string cL = MCHEmul::trim 
		(pC -> _currentLine.substr (0, pC -> _currentLine.find (parser () -> commentSymbol () /** Until a potential comment. */)));

	MCHEmul::Strings prms;
	for (const auto& i : cpu () -> instructions ())
		if (i.second -> matchesWith (cL, prms /** not used here. */) != nullptr)
			return (true); /** At least 1 matching. */

	return (false);
}

// ---
void MCHEmul::Assembler::InstructionCommandParser::parse (MCHEmul::Assembler::ParserContext* pC) const
{
	if (!canParse (pC))
	{
		pC -> _currentLine = "";

		return; // It doesn't generate error, but "forgets" the line
	}

	std::string cL = MCHEmul::trim 
		(pC -> _currentLine.substr (0, pC -> _currentLine.find (parser () -> commentSymbol () /** Until a potential comment. */)));

	MCHEmul::Assembler::InstructionElement* nE = new MCHEmul::Assembler::InstructionElement;
	nE -> _id = pC -> _lastInstructionId++;
	nE -> _file = pC -> _file;
	nE -> _line = pC -> _currentLineNumber;
	MCHEmul::Strings prms;
	for (MCHEmul::Instructions::const_iterator i = cpu () -> instructions ().begin ();
			i != cpu () -> instructions ().end (); i++) // Can be thousands!
	{
		MCHEmul::InstructionDefined* pI = nullptr;
		if ((pI = (*i).second -> matchesWith (cL, prms)) != nullptr) 
		// One option minimum will match this (as it can be parsed)
		{
			nE -> _possibleInstructions.push_back (pI); 
			// In complex instructions the one returned can not be the one that passed...
			nE -> _possibleParameters.emplace_back (prms);
		}
	}

	// Just in case...
	if (nE -> _possibleInstructions.empty ())
		nE -> _error = MCHEmul::Assembler::ErrorType::_INSTRUCTIONNOTDEFINED;

	// More errors could happen later if e.g. the instruction is using labels...
	// Or the macros used don't match the size of the instruction!

	// The element created is added to the gramatic...
	pC -> _semantic -> addGrammaticalElement (nE);

	// The line is completed in any case...
	pC -> _currentLine = "";
}

// ---
MCHEmul::Assembler::Parser::Parser (const MCHEmul::CPU* c, 
		const MCHEmul::ASCIIConverter* aC, const MCHEmul::Assembler::CommandParsers& lP)
	: _cpu (c),
	  _ASCIIConverter (aC),
	  _commandParsers (lP),
	  _printOutProcess (false), 
	  _errors (), 
	  _commentSymbol (' '),
	  _lastLinePrintedOut ("")
{ 
	assert (_cpu != nullptr);

	bool cP = false;
	MCHEmul::Assembler::BinaryDefinitionParser* bD = nullptr;
	MCHEmul::Assembler::BinaryCommandParser* bP = nullptr;
	MCHEmul::Assembler::TextCommandParser* bT = nullptr;
	for (auto i : _commandParsers) 
	{ 
		cP |= (dynamic_cast <MCHEmul::Assembler::CommentCommandParser*> (i) != nullptr);
		if (bD == nullptr) bD = dynamic_cast <MCHEmul::Assembler::BinaryDefinitionParser*> (i); // Get the first if any...
		if (bP == nullptr) bP = dynamic_cast <MCHEmul::Assembler::BinaryCommandParser*> (i); // Get the first if any...
		if (bT == nullptr) bT = dynamic_cast <MCHEmul::Assembler::TextCommandParser*> (i); // Get the first oif any...

		i -> setCPU (c);
		i -> setParser (this); 
	}

	// The comment command parser is always mandatory...
	if (!cP)
		_commandParsers.push_back (new MCHEmul::Assembler::CommentCommandParser ());
	// The binary definition parser could be or not, but if it is it has to be linked to the binary command parser...
	if (bD != nullptr && bP != nullptr)
		bP -> setDefinitionParser (bD);
	// The text command parser might or not be, but if it is, then the ConvertASCIIIntoBytes is assigned to it...
	if (bT != nullptr)
		bT -> setASCIIConverter (aC);
}

// ---
unsigned char MCHEmul::Assembler::Parser::commentSymbol () const
{
	if (_commentSymbol != ' ')
		return (_commentSymbol);

	MCHEmul::Assembler::CommentCommandParser* cP = nullptr;
	for (MCHEmul::Assembler::CommandParsers::const_iterator i = _commandParsers.begin (); 
			i != _commandParsers.end () && cP == nullptr; i++)
		cP = dynamic_cast <MCHEmul::Assembler::CommentCommandParser*> ((*i));

	return (_commentSymbol = ((cP == nullptr) ? ';' /** the default one. */ : cP -> symbol ()));
}

// ---
MCHEmul::Assembler::Semantic* MCHEmul::Assembler::Parser::parse 
	(const std::string& fN, const std::string& fA, std::ostream& oC) const
{ 
	MCHEmul::Assembler::ParserContext* pC = createParserContext ();

	// When evrything is comming from internal,
	// the manage of the printout can be managed...
	_lastLinePrintedOut = "";
	MCHEmul::Assembler::Semantic* result = parse (fN, fA, pC, oC);
	if (_printOutProcess)
		oC << std::endl;

	delete (pC);
	
	return (result);
}

// ---
MCHEmul::Assembler::Semantic* MCHEmul::Assembler::Parser::parse 
	(const std::string& fN, const std::string& fA, MCHEmul::Assembler::ParserContext* pC, std::ostream& oC) const
{
	if (pC == nullptr)
		return (nullptr);

	_errors = { };

	// If there were any error reeading the file, 
	// "lines" would be equal to "", and nothing else will happen!
	MCHEmul::Strings lines = readLines (fN);
	// The file of possible actions is also read, but it might be empty!
	MCHEmul::Strings actions = readLines (fA);
	actions.resize (lines.size ()); // Both must have the same size...

	// If there is nothign to parse...
	if (lines.empty ())
	{ 
		_errors.emplace_back (MCHEmul::Assembler::Error 
			(MCHEmul::Assembler::ErrorType::_FILEEMPTY, fN, 0, 0));

		return (pC -> _semantic); // finishes...
	}

	// When some to parser...
	pC -> _lines = lines;
	pC -> _linesPointer = pC -> _lines.begin ();
	pC -> _file = fN;
	pC -> _currentLine = "";
	pC -> _currentLineNumber = 0;
	pC -> _actionLines = actions;
	pC -> _actionLinesPointer = pC -> _actionLines.begin ();
	parseLines (pC, oC);

	// ...and return the result, 
	// that is a mixture of errors and a semantic...
	_errors.insert (_errors.end (), pC -> _errors.begin (), pC -> _errors.end ());
	return (pC -> _semantic);
}

// ---
MCHEmul::Strings MCHEmul::Assembler::Parser::readLines (const std::string& fN) const
{
	MCHEmul::Strings result;

	std::ifstream f;
	f.open (fN.c_str (), std::ios::in);
	if (!f)
		return (result);

	char l [512 /** Maximum. */];
	while (!f.eof ())
		{ f.getline (l, 512); result.emplace_back (std::string (l)); }

	f.close ();

	return (result);
}

// ---
void MCHEmul::Assembler::Parser::parseLines (MCHEmul::Assembler::ParserContext* pC, std::ostream& oC) const
{
	while (pC -> _linesPointer != pC -> _lines.end ())
	{
		pC -> _currentLine = MCHEmul::upperExcept (MCHEmul::trim (*pC -> _linesPointer));

		// Print out the line being parser...
		// If it defined in that way...
		if (_printOutProcess)
		{
			oC << MCHEmul::_SPACES.substr (0, _lastLinePrintedOut.length ()) 
			   << MCHEmul::_BACKS.substr (0, _lastLinePrintedOut.length ()); // Delete the previous...
			_lastLinePrintedOut = "(" + std::to_string ((pC -> _currentLineNumber + 1) * 100 / pC -> _lines.size ()) + 
				"%) Parsing file:" + pC -> _file + " line:" + std::to_string (pC -> _currentLineNumber + 1 /** Staring from 1. */); // With % in the current file...
			if (_lastLinePrintedOut.length () > MCHEmul::_BACKS.length ()) // No more than the limit...
				_lastLinePrintedOut = _lastLinePrintedOut.substr (0, MCHEmul::_BACKS.length ());
			oC << _lastLinePrintedOut << MCHEmul::_BACKS.substr (0, _lastLinePrintedOut.length ()); // Print the new one...
		}

		std::string aLS = MCHEmul::trim (*pC -> _actionLinesPointer);
		unsigned int aL = (aLS == "") ? 0 /** no action defined. */ : (unsigned int) std::stoi (aLS);
		while (pC -> _currentLine != "") // Until the line has been treated...
		{
			unsigned int col = (unsigned int) (*pC -> _linesPointer).find (pC -> _currentLine);

			MCHEmul::Assembler::CommandParsers::const_iterator j =
				std::find_if (_commandParsers.begin (), _commandParsers.end (), 
					[&](MCHEmul::Assembler::CommandParser* p) -> bool { return (p -> canParse (pC)); });
			if (j != _commandParsers.end ())
			{
				(*j) -> parse (pC);
				if (!*pC -> _semantic)
				{ 
					MCHEmul::Assembler::ErrorType et = MCHEmul::Assembler::ErrorType::_NOERROR;
					// If the error happens with in a code template, the line of the error is recorded...
					_errors.emplace_back (MCHEmul::Assembler::Error 
						(et = pC -> _semantic -> error (), pC -> _file, pC -> _currentLineNumber + 1 /** Starting from 1. */, col,
							(pC -> _templateLinesNumber != 0 ? std::to_string (pC -> _templateLinesNumber): "")));
					// An error in the last element added...more info including the error in the semantic!
					if (et == MCHEmul::Assembler::ErrorType::_SEMANTICERROR)
						_errors.emplace_back (MCHEmul::Assembler::Error 
							(pC -> _semantic -> lastGrammaticalElementAdded () -> error (), 
								pC -> _file, pC -> _currentLineNumber + 1 /** Starting from 1. */, col));
				}
				else
				// adds the action defined (if a grammatical element was added)
				if (pC -> _semantic -> gramaticalElementAdded () /** latch. */ &&
					pC -> _semantic -> lastGrammaticalElementAdded () != nullptr)
						pC -> _semantic -> lastGrammaticalElementAdded () -> _actionOn = aL;
			}
			else
			{
				pC -> _currentLine = ""; // The line is considered as treated...

				_errors.emplace_back (MCHEmul::Assembler::Error 
					(MCHEmul::Assembler::ErrorType::_PARSERNOTFOUND, pC -> _file, pC -> _currentLineNumber + 1 /** Starting from 1. */, col));
			}
		}

		// The pointer to treat with the lines is one...
		pC -> _linesPointer++; pC -> _actionLinesPointer++;
		// ..but in terms of which number of line is being parsed, is other!
		if (pC -> _templateLinesNumber != 0) --pC -> _templateLinesNumber;
		// Templates are not taking into account in the reporting of error...
		if (pC -> _templateLinesNumber == 0) ++pC -> _currentLineNumber;
	}
}
