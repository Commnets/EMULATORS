#include <ASSEMBLER/Compiler.hpp>

// ---
std::ostream& MCHEmul::Assembler::operator << (std::ostream& o, const MCHEmul::Assembler::CodeLine& c)
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
MCHEmul::Assembler::CodeLines MCHEmul::Assembler::Compiler::compile (const std::string& fN) const
{
	MCHEmul::Assembler::CodeLines result;

	_errors = { };

	// First of all, parse...
	MCHEmul::Assembler::Semantic* smt = _parser -> parse (fN);
	if (!(*smt))
	{
		_errors = _parser -> errors ();
		return (result);
	}

	// Then compile...
	// First determines the value of the macros...
	bool mE = false;
	for (auto i : smt -> macros ())
	{
		i.second.value (smt -> macros ()); // Try to calculate the value (but it wold be used)...
		if (!i.second)
		{
			mE = true;
			_errors.push_back (MCHEmul::Assembler::Error (i.second.error (), fN, 0, 0));
		}
	}

	// If there were any error calculating the value of the macros
	// it would not be able to continue
	if (mE)
		return (result);

	// Then, with the macros calculated, the lines could be calculated
	// Errors could happen when managing the instructions...
	for (auto i : smt -> startingPoints ())
	{
		std::vector <MCHEmul::UByte> spaB = i -> codeBytes (smt, cpu () -> architecture ());
		if (!*i)
			_errors.push_back (MCHEmul::Assembler::Error (i -> _error, fN, 0, 0));
		else
		{
			std::string lL = "";
			MCHEmul::Address spa (spaB);
			for (MCHEmul::Assembler::GramaticalElement* sp = i -> _nextElement; 
				sp != nullptr; sp = sp -> _nextElement)
			{
				if (dynamic_cast <MCHEmul::Assembler::LabelElement*> (sp) != nullptr)
					lL = static_cast <MCHEmul::Assembler::LabelElement*> (sp) -> _name;
				else
				{
					MCHEmul::Assembler::InstructionElement* iE = 
						dynamic_cast <MCHEmul::Assembler::InstructionElement*> (sp);
					std::vector <MCHEmul::UByte> b = sp -> codeBytes (smt, cpu () -> architecture ());
					result.push_back (MCHEmul::Assembler::CodeLine (spa, b, lL, iE ? iE -> _instruction : nullptr));
					spa += b.size (); // To the next...

					lL = "";
				}
			}
		}
	}
	
	delete (smt);

	return (result);
}
