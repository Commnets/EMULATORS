#include <CORE/Instruction.hpp>
#include <CORE/CPU.hpp>

// ---
static std::map <unsigned char, MCHEmul::InstructionDefined::Structure::Parameter::Type> _TYPES
	({ 
		{'#', MCHEmul::InstructionDefined::Structure::Parameter::Type::_DATA},
		{'$', MCHEmul::InstructionDefined::Structure::Parameter::Type::_DIR},
		{'&', MCHEmul::InstructionDefined::Structure::Parameter::Type::_RELJUMP}, 
		{'%', MCHEmul::InstructionDefined::Structure::Parameter::Type::_ABSJUMP} 
	});

// ---
MCHEmul::UBytes MCHEmul::Instruction::ExecutionData::parameters (size_t p, size_t nP, bool bE) const
{
	if ((p + nP - 1) >= _parameters.size ())
		return (MCHEmul::UBytes::_E);

	std::vector <MCHEmul::UByte> ub;
	for (size_t i = 0; i < nP; ub.push_back (_parameters [p + i++]));
	return (MCHEmul::UBytes (ub, bE));
}

// ---
std::string MCHEmul::Instruction::ExecutionData::parametersAsString (size_t p, size_t nP, bool bE) const
{ 
	MCHEmul::UBytes ub = parameters (p, nP, bE);

	std::string result = "";
	for (size_t i = 0; i < ub.size (); i++)
		result += ((i == 0) ? "$" : "") + ub [i].asString (MCHEmul::UByte::OutputFormat::_HEXA, 2);
	return (result);
}

// ---
MCHEmul::Instruction::Instruction (unsigned int c, bool bE)
	: _code (c),
	  _codeLength (((size_t) c < ((size_t) 1 << MCHEmul::UByte::sizeBits ()))
		? 1 : ((size_t) c < ((size_t) 1 << (MCHEmul::UByte::sizeBits () * 2)))
		? 2 : ((size_t) c < ((size_t) 1 << (MCHEmul::UByte::sizeBits () * 3))) ? 3 : 4), // No more than 4...
	  _bigEndian (bE),
	  _lastExecutionData ()
{
	// Nothing else to do...
}

// ---
MCHEmul::InstructionDefined::InstructionDefined (unsigned int c, unsigned int mp, unsigned int cc, 
		const MCHEmul::InstructionDefined::CycleStructure& cS,
		const std::string& t, bool bE)
	: MCHEmul::Instruction (c, bE),
	  _memoryPositions (mp), 
	  _clockCycles (cc),
	  _cycleStructure (cS),
	  _iTemplate (MCHEmul::noSpaces (MCHEmul::upper (t))), // The template if stored in uppercase and with no spaces...
	  _iStructure (),
	  _additionalCycles (0)
{ 
	assert (_memoryPositions > 0 && _clockCycles > 0); 
	assert (_iTemplate != ""); 

	if (_cycleStructure.empty ())
		_cycleStructure = 
			MCHEmul::InstructionDefined::CycleStructure (_clockCycles, _CYCLENOTDEFINED);

	assert (_cycleStructure.size () == _clockCycles);

	_iStructure = analyzeInstruction ();
	if (_iStructure._error)
		_iTemplate = _iStructure._templateWithNoParameters;
}

// ---
MCHEmul::InstructionDefined* MCHEmul::InstructionDefined::matchesWith 
	(const std::string& i, MCHEmul::Strings& prms)
{
	if (_iStructure._error)
		return (nullptr); // No match...

	std::string inst = _iStructure._templateWithNoParameters;

	std::string iL = MCHEmul::upperExcept (MCHEmul::noSpaces (i));

	std::string pW = "";

	// The one received can't be shorter than the template never
	if (iL.length () < inst.length ())
		return (nullptr); // if it is, we are speaking about another opcode...

	// If they don't start by the same symbol, thre woldn't be any possibility for them to match
	if (iL [0] != inst [0])
		return (nullptr);

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
				if (result = MCHEmul::validOperation (lP)) // A very generic validation...deeper at compilation time! (if any)
				{ 
					prms.emplace_back (lP);

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
		return (nullptr);

	// If the instruction recevived hasn't been compared until its end,
	// It could mean there is parameters still pending to add...
	// That potential parameter should be a valid label or numbr!
	if (ctIL != iL.length ())
	{
		std::string lP = iL.substr (ctIL);
		if (MCHEmul::validOperation (lP)) // A very generic validation...deeper at compiltion time (if any)
		{
			prms.emplace_back (lP);

			pW += '?';
		}
		else // In other case, it is not something that matches!
			return (nullptr);
	}

	// The number of extracted parameters has to match the number of the ones defined...
	// ...and also the watermark!
	return ((_iStructure._waterMark == MCHEmul::onlyAlphanumeric (MCHEmul::removeAllFrom (iL, prms)) /** Basic structure. */ &&
			 _iStructure._waterMarkPlus == pW && /** But also the extended version. */
			 prms.size () == _iStructure._parameters.size () /** and for sure the paramete4rs. */) ? this : nullptr);
}

// ---
bool MCHEmul::InstructionDefined::defineInstructionFrom
	(MCHEmul::Memory* m, const MCHEmul::Address& addr)
{
	assert (m != nullptr);

	_lastExecutionData._parameters = m -> values (addr, _memoryPositions /** fix number. */);

	return (true);
}

// ---
std::vector <MCHEmul::UByte> MCHEmul::InstructionDefined::shapeCodeWithData
	(const std::vector <std::vector <MCHEmul::UByte>>& b, bool& e) const
{
	// Build the instruction...
	std::vector <MCHEmul::UByte> result = 
		MCHEmul::UInt::fromUnsignedInt (code ()).bytes ();
	for (const auto& i : b)
		result.insert (result.end (), i.begin (), i.end ());

	// Is the result right in length?
	// If not it was because either the number of parameters was wrong
	// or the code of the instruction was longer that expected
	// There might happen however that with both mistakes at the same time the length was ok!
	e = (result.size () != _memoryPositions);

	return (result);
}

// ---
std::string MCHEmul::InstructionDefined::asString () const
{
	if (internalStructure ()._error)
		return (_iTemplate); // Nothing else is possible...

	std::string t = _iTemplate;

	std::string toPrint = "";

	size_t nPrm = codeLength ();
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
					(*iPrm).second == Structure::Parameter::Type::_ABSJUMP) ? _bigEndian : true)
				: true;

			toPrint += t.substr (lP, iPP - lP) + 
				_lastExecutionData.parametersAsString (byteParameter (nPrm), bPrm, bE);

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

// ---
bool MCHEmul::InstructionDefined::execute (MCHEmul::CPU* c, MCHEmul::Memory* m, MCHEmul::Stack* stk, 
	MCHEmul::ProgramCounter* pc)
{
	// It can not be executed unless the references are not nullptr...
	assert (c != nullptr && 
			m != nullptr && 
			stk != nullptr &&
			pc != nullptr);

	_lastExecutionData._cpu	= c;
	_lastExecutionData._memory = m;
	_lastExecutionData._stack = stk;

	// Gets the data that the instruction occupies
	// before updating the Program Counter...
	_lastExecutionData._parameters = 
		_lastExecutionData._memory -> values (pc -> asAddress (), _memoryPositions);
	_lastExecutionData._programCounter = *pc;

	// Then, the Program Counter is moved to the next instruction...
	// This is done in this way because the intruction itself could
	// modify the value of the Program Counter (Jumps, returns,...)
	// Some CPU modify the program counter as they execute the instruction...
	// take care codifying the right behaviour per instruction...
	*pc += (size_t) _memoryPositions;

	// executeImpl could add additional cycles...
	_additionalCycles = _lastExecutionData._additionalClockCycles = 0; 

	bool f = true;
	bool r = executeImpl (f);
	if (r)
	{
		if (!f)
			*pc -= (size_t) _memoryPositions; // Back to the beginning...

		_lastExecutionData._clockCycles = _clockCycles;

		_lastExecutionData._additionalClockCycles = _additionalCycles;

		_lastExecutionData._memoryPositions = _memoryPositions;

		c -> setLastINOUTAddress (_lastExecutionData._INOUTAddress);

		c -> setLastINOUTData (_lastExecutionData._INOUTData);
	}

	return (r);
}

// ---
MCHEmul::InstructionDefined::Structure MCHEmul::InstructionDefined::analyzeInstruction () const
{
	MCHEmul::InstructionDefined::Structure result;

	std::string inst = _iTemplate;
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

// ---
MCHEmul::InstructionUndefined::InstructionUndefined (unsigned int c, const MCHEmul::Instructions& inst)
	: MCHEmul::Instruction (c, true),
	  _instructions (inst),
	  _lastInstruction (nullptr)
{
	assert (_instructions.size () != 0);
}

// ---
MCHEmul::InstructionDefined* MCHEmul::InstructionUndefined::matchesWith (const std::string& i, MCHEmul::Strings& prms)
{
	MCHEmul::InstructionDefined* result = nullptr;

	for (MCHEmul::Instructions::const_iterator j = _instructions.begin (); 
		j != _instructions.end () && result == nullptr; j++)
			result = (*j).second -> matchesWith (i, prms);

	return (result);
}

// ---
bool MCHEmul::InstructionUndefined::defineInstructionFrom (MCHEmul::Memory* m, const MCHEmul::Address& addr)
{
	assert (m != nullptr);

	// First select the instructions...
	if ((_lastInstruction = 
		const_cast <MCHEmul::Instruction*> (selectInstruction (m, addr))) == nullptr)
		return (false);

	// Then define the details...and drag the parameters defined!
	bool result = false;
	if ((result = _lastInstruction -> defineInstructionFrom (m, addr)))
		_lastExecutionData = _lastInstruction -> lastExecutionData (); // Not std::move because info can be used in other levels...

	return (result);
}

// ---
bool MCHEmul::InstructionUndefined::execute (MCHEmul::CPU* c, MCHEmul::Memory* m, MCHEmul::Stack* stk, 
	MCHEmul::ProgramCounter* pc)
{
	bool result = false; // By default...
	MCHEmul::Instruction* sI = 
		const_cast <MCHEmul::Instruction*> (selectInstruction (m, pc -> asAddress ())); // To select the instruction...
	if (sI != nullptr && (result = sI -> execute (c, m, stk, pc)))
		_lastExecutionData = (_lastInstruction = sI) -> lastExecutionData (); // Not std::move because info can be used in other levels...

	return (result);
}
