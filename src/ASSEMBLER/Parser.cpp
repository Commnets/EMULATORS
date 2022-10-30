#include <ASSEMBLER/Parser.hpp>
#include <fstream>
#include <algorithm>

// ---
void MCHEmul::Assembler::IncludeCommandParser::parse 
	(std::string& l, unsigned int lC, MCHEmul::Assembler::Semantic* c) const
{ 
	c -> addFrom (parser () -> parse 
		(MCHEmul::trim (l.substr (1, l.find (parser () -> commentSymbol () /** Until a potencial comment */) - 1)))); 
}

// ---
void MCHEmul::Assembler::MacroCommandParser::parse 
	(std::string& l, unsigned int lC, MCHEmul::Assembler::Semantic* s) const
{
	assert (s != nullptr);

	if (!canParse (l))
	{
		l = ""; 
		return; // It doesn't generate error, but "forgets" the line
	}

	size_t mD = l.find (_symbol);
	s -> addMacro (MCHEmul::Assembler::Macro 
			(MCHEmul::trim (l.substr (0, mD)), 
			 MCHEmul::trim (l.substr (mD + 1, l.find (parser () -> commentSymbol () /** Until a potential comment. */) - (mD + 1)))));
	// The macro is added just if there is no other with the same value...
	// Otherwise the instruction above will generate an error

	l = ""; // The line is completed...
}

// ---
void MCHEmul::Assembler::StartingPointCommandParser::parse 
	(std::string& l, unsigned int lC, MCHEmul::Assembler::Semantic* s) const
{
	assert (s != nullptr);

	if (!canParse (l))
	{
		l = ""; 
		return; // It doesn't generate error, but "forgets" the line
	}

	size_t mD = l.find (_symbol);
	MCHEmul::Assembler::StartingPointElement* nE = s -> addNewStartingPoint (); // Starts a new entry point...
	nE -> _id = _lastStartingPointId++; // Sequential...
	nE -> _line = lC;
	nE -> _value = 
		MCHEmul::trim (l.substr (mD + 1, l.find (parser () -> commentSymbol () /** Until a potential comment. */) - (mD + 1)));

	// Here it is not checked whether _value is or not valid
	// _value could include symbols and it should be treated later like a macro...

	l = ""; // The line is completed...
}

// ---
void MCHEmul::Assembler::LabelCommandParser::parse
	(std::string& l, unsigned int lC, MCHEmul::Assembler::Semantic* s) const
{
	assert (s != nullptr);

	if (!canParse (l))
	{
		l = ""; 
		return; // It doesn't generate error, but "forgets" the line
	}

	size_t eL = l.find (_symbol);
	MCHEmul::Assembler::LabelElement* nE = new MCHEmul::Assembler::LabelElement;
	nE -> _id = _lastLabelId++; // Sequential...
	nE -> _line = lC;
	nE -> _name = MCHEmul::trim (MCHEmul::upper (l.substr (0, eL)));
	// The label has to be valid...
	if (!MCHEmul::validLabel (nE -> _name))
		nE -> _error = MCHEmul::Assembler::ErrorType::_LABELNOTVALID;

	// The line has not been read totally...
	l = MCHEmul::trim (l.substr (eL + 1));

	s -> addGrammaticalElement (nE);
}

// ---
void MCHEmul::Assembler::BytesCommandParser::parse
	(std::string& l, unsigned int lC, MCHEmul::Assembler::Semantic* s) const
{
	assert (s != nullptr);

	if (!canParse (l))
	{
		l = ""; 
		return; // It doesn't generate error, but "forgets" the line
	}

	size_t eL = l.find (' '); // The first space defines where the data starts...
	MCHEmul::Assembler::BytesInMemoryElement* nE = new MCHEmul::Assembler::BytesInMemoryElement;
	nE -> _id = _lastBytesId++; // Sequential...
	nE -> _line = lC;
	nE -> _elements = MCHEmul::getElementsFrom 
		(l.substr (eL + 1, l.find (parser () -> commentSymbol () /** Until a potential comment. */) - (eL + 1)), ' ');
	if (nE -> _elements.empty ())
		nE -> _error = MCHEmul::Assembler::ErrorType::_BYTESNOTVALID;

	// The line is completed...
	l = "";

	s -> addGrammaticalElement (nE);
}

// ---
bool MCHEmul::Assembler::InstructionCommandParser::canParse (const std::string& l) const
{
	std::string cL = MCHEmul::trim 
		(l.substr (0, l.find (parser () -> commentSymbol () /** Until a potential comment. */)));

	MCHEmul::Strings prms;
	for (const auto& i : cpu () -> instructions ())
		if (i.second -> matchesWith (cL, prms /** not used here. */))
			return (true); /** At least 1 matching. */

	return (false);
}

// ---
void MCHEmul::Assembler::InstructionCommandParser::parse
	(std::string& l, unsigned int lC, MCHEmul::Assembler::Semantic* s) const
{
	assert (s != nullptr);

	if (!canParse (l))
	{
		l = ""; 
		return; // It doesn't generate error, but "forgets" the line
	}

	std::string cL = MCHEmul::trim 
		(l.substr (0, l.find (parser () -> commentSymbol () /** Until a potential comment. */)));

	MCHEmul::Assembler::InstructionElement* nE = new MCHEmul::Assembler::InstructionElement;
	nE -> _id = _lastInstructionId; 
	nE -> _line = lC;
	MCHEmul::Strings prms;
	for (MCHEmul::Instructions::const_iterator i = cpu () -> instructions ().begin ();
			i != cpu () -> instructions ().end (); i++)
	{
		if ((*i).second -> matchesWith (cL, prms)) // One option minimum will match this (as it can be parsed)
		{
			nE -> _possibleInstructions.push_back ((*i).second);
			nE -> _parameters = prms;
		}
	}

	// Just in case...
	if (nE -> _possibleInstructions.empty ())
		nE -> _error = MCHEmul::Assembler::ErrorType::_INSTRUCTIONNOTDEFINED;

	_lastInstructionId++;

	// More errors could happen later if e.g. the instruction is using labels...
	// Or the macros used don't match the size of the instruction!

	// The line is completed in any case...
	l = "";

	s -> addGrammaticalElement (nE);
}

// ---
MCHEmul::Assembler::Parser::Parser (const MCHEmul::CPU* c, const MCHEmul::Assembler::CommandParsers& lP)
	: _cpu (c), _commandParsers (lP),
	  _errors (), _commentSymbol (' ')
{ 
	assert (_cpu != nullptr);

	bool cP = false;
	for (auto i : _commandParsers) 
	{ 
		cP |= (dynamic_cast <MCHEmul::Assembler::CommentCommandParser*> (i) != nullptr);

		i -> setCPU (c);
		i -> setParser (this); 
	}

	// The comment command parser is always mandatory...
	if (!cP)
		_commandParsers.push_back (new MCHEmul::Assembler::CommentCommandParser ());
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

	assert (cP != nullptr); // It shouldn't...but just in case!
	return (_commentSymbol = ((cP == nullptr) ? '#' /** the default one. */ : cP -> symbol ()));
}

// ---
MCHEmul::Assembler::Semantic* MCHEmul::Assembler::Parser::parse (const std::string& fN, const std::string& fA) const
{
	MCHEmul::Assembler::Semantic* result = new MCHEmul::Assembler::Semantic;

	_errors = { };

	// If there were any error reeading the file, 
	// "lines" would be equal to "", and nothing else will happen!
	MCHEmul::Strings lines = readLines (fN);
	// The file of possible actions is also read, but it might be empty!
	MCHEmul::Strings actions = readLines (fA);
	if (lines.empty ())
		_errors.push_back (MCHEmul::Assembler::Error 
			(MCHEmul::Assembler::ErrorType::_FILEEMPTY, fN, 0, 0));
	actions.resize (lines.size ()); // Both must have the same size...

	int lC = 0;
	for (const auto& i : lines)
	{
		std::string l = MCHEmul::trim (i);
		std::string aLS = MCHEmul::trim (actions [lC]);
		unsigned int aL = (aLS == "") ? 0 /** no action defined. */ : (unsigned int) std::stoi (aLS);
		while (l != "")
		{
			unsigned int col = (unsigned int) i.find (l);

			MCHEmul::Assembler::CommandParsers::const_iterator j =
				std::find_if (_commandParsers.begin (), _commandParsers.end (), 
					[=](MCHEmul::Assembler::CommandParser* p) -> bool { return (p -> canParse (l)); });
			if (j != _commandParsers.end ())
			{
				(*j) -> parse (l, lC, result);
				if (!result)
					_errors.push_back (MCHEmul::Assembler::Error 
						(result -> lastGrammaticalElementAdded () -> _error, fN, lC, col));
				else
				// adds the action defined (if a grammatical element was created)
				if (result -> lastGrammaticalElementAdded () != nullptr)
						result -> lastGrammaticalElementAdded () -> _actionOn = aL; 
			}
			else
			{
				l = ""; // The line is considered as treated...
				_errors.push_back (MCHEmul::Assembler::Error 
					(MCHEmul::Assembler::ErrorType::_PARSERNOTFOUND, fN, lC, col));
			}
		}

		lC++;
	}

	return (result);
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
		{ f.getline (l, 512); result.push_back (std::string (l)); }

	f.close ();

	return (result);
}
