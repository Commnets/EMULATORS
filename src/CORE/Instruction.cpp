#include <CORE/Instruction.hpp>
#include <CORE/CPU.hpp>

// ---
static std::map <unsigned char, MCHEmul::Instruction::Structure::Parameter::Type> _TYPES
	({ 
		{'#', MCHEmul::Instruction::Structure::Parameter::Type::_DATA},
		{'$', MCHEmul::Instruction::Structure::Parameter::Type::_DIR},
		{'&', MCHEmul::Instruction::Structure::Parameter::Type::_RELJUMP}, 
		{'%', MCHEmul::Instruction::Structure::Parameter::Type::_ABSJUMP} 
	});

// ---
MCHEmul::Instruction::Instruction (unsigned int c, unsigned int mp, unsigned int cc, const std::string& t)
	: _code (c), _memoryPositions (mp), _clockCycles (cc), 
	  _iTemplate (MCHEmul::noSpaces (MCHEmul::upper (t))),
	  _iStructure (), // Assigned later...
	  _lastParameters (), _cpu (nullptr), _memory (nullptr), _stack (nullptr)
{ 
	assert (_memoryPositions > 0 && _clockCycles > 0); 
	assert (_iTemplate != ""); 

	// The template if stored in uppercase and with no spaces...

	_iStructure = analyzeInstruction ();
	if (_iStructure._error)
		_iTemplate = _iStructure._templateWithNoParameters;
}

// ---
bool MCHEmul::Instruction::matchesWith (const std::string& i, MCHEmul::Strings& prms)
{
	if (_iStructure._error)
		return (false);

	std::string inst = _iStructure._templateWithNoParameters;

	std::string iL = MCHEmul::upper (MCHEmul::noSpaces (i));

	std::string pW = "";

	// The one received can't be shorter than the template never
	if (iL.length () < inst.length ())
		return (false); // if it is, we are speaking about another opcode...

	// If they don't start by the same symbol, thre woldn't be any possibility for them to match
	if (iL [0] != inst [0])
		return (false);

	// It is time to review whether 
	// the structure of the received instruction could match with this...
	prms = { };
	bool result = true;
	size_t ctIL = 0;
	for (size_t ctInst = 0; 
			ctInst < inst.length () && ctIL < iL.length () && result; ctInst++, ctIL++)
	{
		// When the character in the received intruction 
		// doesn't match with the equivalent one in the template
		// the system tries to find the next match, assuming that evrything
		// In between should be a parameter...
		// THis is something also checfed later!
		if (iL [ctIL] != inst [ctInst])
		{
			size_t iPP = ctIL;
			while (iL [ctIL] != inst [ctInst] && ctIL < iL.length ()) ctIL++;
			if (result = (ctIL != iL.length ()))
			{
				std::string lP = iL.substr (iPP, ctIL - iPP);
				if (result = MCHEmul::validFunction (lP)) // The parameter to add has to be valid...
				{ 
					prms.push_back (lP);

					pW += '?';
					pW += iL [ctIL];
				}
			}
		}
		else
			pW += iL [ctIL];
	}

	// If the global aspect doesn't match
	// It is not worth to continue...
	if (!result)
		return (false);

	// If the instruction recevived hasn't been compared until its end,
	// It could mean there is parameters still pending to add...
	// That potential parameter should be a valid label or numbr!
	if (ctIL != iL.length ())
	{
		std::string lP = iL.substr (ctIL);
		if (MCHEmul::validFunction (lP))
		{
			prms.push_back (lP);

			pW += '?';
		}
		else // In other case, it is not something that matches!
			return (false);
	}

	// The number of extracted parameters has to match the number of the ones defined...
	// ...and also the watermark!
	return (_iStructure._waterMark == MCHEmul::onlyAlphanumeric (MCHEmul::removeAllFrom (iL, prms)) /** Basic structure. */ &&
			_iStructure._waterMarkPlus == pW && /** But also the extended version. */
			prms.size () == _iStructure._parameters.size () /** and for sure the paramete4rs. */);
}

// ---
const MCHEmul::UBytes MCHEmul::Instruction::parameters (size_t p, size_t nP, bool bE) const
{
	if ((p + nP - 1) >= _lastParameters.size ())
		return (MCHEmul::UBytes::_E);

	std::vector <MCHEmul::UByte> ub;
	for (size_t i = 0; i < nP; ub.push_back (_lastParameters [p + i++]));
	return (MCHEmul::UBytes (ub, bE));
}

// ---
std::string MCHEmul::Instruction::parametersAsString (size_t p, size_t nP, bool bE) const
{ 
	MCHEmul::UBytes ub = parameters (p, nP, bE);

	std::string result = "";
	for (size_t i = 0; i < ub.size (); i++)
		result += ((i == 0) ? "$" : "") + ub [i].asString (MCHEmul::UByte::OutputFormat::_HEXA, 2);
	return (result);
}

// ---
std::string MCHEmul::Instruction::asString () const
{
	if (_cpu == nullptr || _memory == nullptr || _stack == nullptr)
		return (""); // If the transaction has not been executed...

	if (internalStructure ()._error)
		return (_iTemplate); // Nothing else is possible...

	std::string t = _iTemplate;

	std::string toPrint = "";

	size_t nPrm = _cpu -> architecture ().instructionLength ();
	size_t lP = 0;
	bool end = false;
	while (!end)
	{
		size_t iPP = t.find ('[', lP);
		if (iPP != std::string::npos)
		{
			// As the template has been already analyzed
			// After "[" there will be always a "]"...
			// No error is now possible!
			size_t fPP = t.find (']', iPP + 1);

			std::string prm = t.substr (iPP + 1, fPP - iPP);
			size_t bPrm = std::atoi (prm.substr (1).c_str ());
			std::map <unsigned char, Structure::Parameter::Type>::const_iterator iPrm = _TYPES.find (prm [0]);
			bool bE = (iPrm != _TYPES.end ()) 
				? (((*iPrm).second == Structure::Parameter::Type::_DIR ||
				    (*iPrm).second == Structure::Parameter::Type::_ABSJUMP) 
						? _cpu -> architecture ().bigEndian () : true)
				: true;

			toPrint += t.substr (lP, iPP - lP) + parametersAsString (nPrm, bPrm, bE);

			nPrm += bPrm;
			lP = fPP + 1;
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
MCHEmul::Instruction::Structure MCHEmul::Instruction::analyzeInstruction () const
{
	MCHEmul::Instruction::Structure result;

	std::string inst = iTemplate ();
	std::string wmak = inst;

	size_t iP = inst.find ('[');
	while (iP != std::string::npos)
	{
		size_t fP = inst.find (']', iP + 1);
		if (fP != std::string::npos)
		{
			std::string pr = inst.substr (iP + 1, fP - iP - 1);
			if (pr != "" && pr.length () >= 2 /** minimum length including type and number of bytes */ &&
				(_TYPES.find (pr [0]) != _TYPES.end ()))
			{
				result._parameters.push_back 
					(Structure::Parameter ((*_TYPES.find (pr [0])).second, std::stoi (pr.substr (1))));
				
				inst = inst.substr (0, iP) + inst.substr (fP + 1);
				wmak = wmak.substr (0, iP) + "?" /** To identify some variable. */ + wmak.substr (fP + 1);
			}
			else
			{
				// There is mistake in the definition of the parameter
				inst = "?";
				result._error = true;
			}
		}
		else
		{
			// There is a mistake in the definition of the instruction...
			// Nothing should coninue...
			inst = "-";
			result._error = true;
		}

		iP = inst.find ('[');
	}

	result._templateWithNoParameters = inst;
	result._waterMark = MCHEmul::onlyAlphanumeric (inst);
	result._waterMarkPlus = wmak;

	return (result);
}
