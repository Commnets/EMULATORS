#include <language/Instruction.hpp>

// ---
MCHEmul::Instruction::Instruction (unsigned int c, unsigned int mp, unsigned int cc, const std::string& t)
	: _code (c), _memoryPositions (mp), _clockCycles (cc), _iTemplate (t),
	  _lastParameters ()
{ 
	assert (_memoryPositions > 0 && _clockCycles > 0); 
	assert (_iTemplate != ""); 
}


// ---
std::string MCHEmul::Instruction::asString () const
{
	std::string t = iTemplate ();

	std::string toPrint = "";

	size_t lP = 0;
	bool end = false;
	while (!end)
	{
		size_t iPP = t.find ('[', lP);
		if (iPP != std::string::npos)
		{
			size_t fPP = t.find (']', iPP + 1);
			if (fPP != std::string::npos)
			{
				toPrint += t.substr (lP, iPP) + 
					lastParameterAsString ((size_t) std::atoi (t.substr (iPP + 1, fPP - iPP).c_str ()));

				lP = fPP + 1;
			}
			else
			{
				toPrint += t.substr (lP);

				end = true;
			}
		}
		else
		{
			toPrint += t.substr (lP);

			end = true;
		}
	}

	return (toPrint);
}

bool MCHEmul::Instruction::execute (const MCHEmul::UBytes& p, MCHEmul::CPU* c, MCHEmul::Memory* m, MCHEmul::Stack* stk)
{
	assert (p.size () == _memoryPositions);
	assert (c != nullptr && m != nullptr);

	_lastParameters = p;
	_cpu = c;
	_memory = m;
	_stack = stk;

	_additionalCycles = 0; // executeImpl could add additional cycles...

	return (executeImpl ());
}

// ---
std::ostream& MCHEmul::operator << (std::ostream& o, const MCHEmul::Instruction& i)
{
	return (o << i.asString ());
}
