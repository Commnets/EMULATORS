#include "Tests.hpp"
#include <assert.h>

Test::Test (const std::string& tIn, const std::string& tOut)
	: _testsIn (readTestsIn (tIn)), 
	  _testsOut (readTestsOut (tOut)), 
	  _errors (), _warnings (), _noimplemented ()
{
	// Verify coherence...
	for (const auto& i : _testsIn)
		if (_testsOut.find (i.first) == _testsOut.end ())
			_errors.emplace_back ("Test " + i.first + " has no output");
	for (const auto& i : _testsOut)
		if (_testsIn.find (i.first) == _testsIn.end ())
			_errors.emplace_back ("Output " + i.first + " has no test");
}

void Test::runTest (FZ80::CZ80* cpu)
{
	assert (cpu != nullptr && 
			cpu -> memoryRef () != nullptr);

	auto instAsString = [](const MCHEmul::ListOfInstructions& inst, 
			const std::string& sp = "\n") -> std::string
		{
			std::string result;

			if (inst.size () == 0)
				return ("");

			if (inst.size () == 1)
				result = inst [0] -> asString ();
			else
			{ 
				bool f = true;
				for (const auto& i : inst)
				{
					result += ((!f) ? sp : "") + i -> asString ();
					f = false;
				}
			}

			return (result);
		};

	// If there is coherence...
	// Then the test starts!...
	for (Test::TestInMap::const_iterator i = _testsIn.begin (); i != _testsIn.end (); i++)
	{
		// First sets the situation of the computer for the test to be executed...
		// Sets the registers...
		cpu -> afRegister ()[0]  -> set ({ (unsigned char) (((*i).second._status._AF  & 0xff00) >> 8) });
		cpu -> afRegister ()[1]  -> set ({ (unsigned char) ((*i).second._status._AF  & 0x00ff) });
		cpu -> bcRegister ()[0]  -> set ({ (unsigned char) (((*i).second._status._BC  & 0xff00) >> 8) });
		cpu -> bcRegister ()[1]  -> set ({ (unsigned char) ((*i).second._status._BC  & 0x00ff) });
		cpu -> deRegister ()[0]  -> set ({ (unsigned char) (((*i).second._status._DE  & 0xff00) >> 8) });
		cpu -> deRegister ()[1]  -> set ({ (unsigned char) ((*i).second._status._DE  & 0x00ff) });
		cpu -> hlRegister ()[0]  -> set ({ (unsigned char) (((*i).second._status._HL  & 0xff00) >> 8) });
		cpu -> hlRegister ()[1]  -> set ({ (unsigned char) ((*i).second._status._HL  & 0x00ff) });
		cpu -> ixRegister ()[0]  -> set ({ (unsigned char) (((*i).second._status._IX  & 0xff00) >> 8) });
		cpu -> ixRegister ()[1]  -> set ({ (unsigned char) ((*i).second._status._IX  & 0x00ff) });
		cpu -> iyRegister ()[0]  -> set ({ (unsigned char) (((*i).second._status._IY  & 0xff00) >> 8) });
		cpu -> iyRegister ()[1]  -> set ({ (unsigned char) ((*i).second._status._IY  & 0x00ff) });
		cpu -> afpRegister ()[0] -> set ({ (unsigned char) (((*i).second._status._AFP & 0xff00) >> 8) });
		cpu -> afpRegister ()[1] -> set ({ (unsigned char) ((*i).second._status._AFP & 0x00ff) });
		cpu -> bcpRegister ()[0] -> set ({ (unsigned char) (((*i).second._status._BCP & 0xff00) >> 8) });
		cpu -> bcpRegister ()[1] -> set ({ (unsigned char) ((*i).second._status._BCP & 0x00ff) });
		cpu -> depRegister ()[0] -> set ({ (unsigned char) (((*i).second._status._DEP & 0xff00) >> 8) });
		cpu -> depRegister ()[1] -> set ({ (unsigned char) ((*i).second._status._DEP & 0x00ff) });
		cpu -> hlpRegister ()[0] -> set ({ (unsigned char) (((*i).second._status._HLP & 0xff00) >> 8) });
		cpu -> hlpRegister ()[1] -> set ({ (unsigned char) ((*i).second._status._HLP & 0x00ff) });
		// ...the I & R registers...
		cpu -> iRegister ().set ({ (*i).second._status._I });
		cpu -> rRegister ().set ({ (*i).second._status._R });
		// ...the stack pointer...
		cpu -> memoryRef () -> stack () -> setPosition ((int) (*i).second._status._SP);
		cpu -> memoryRef () -> stack () -> setNotUsed (false); // it is considered already used!
		// ..the info about the interrupts (INT specially)...
		cpu -> setIFF1 ((*i).second._status._IFF1);
		cpu -> setIFF2 ((*i).second._status._IFF2);
		static_cast <FZ80::INTInterrupt*> (cpu -> interrupt (FZ80::INTInterrupt::_ID)) 
			-> setINTMode ((*i).second._status._IM);
		if ((*i).second._status._halted) cpu -> setHalt ();
		else cpu -> resetHalt ();
		// ...the program counter...
		cpu -> programCounter ().setAddress (MCHEmul::Address (2, (unsigned int) (*i).second._status._PC));
		// The memory content...
		cpu -> setRWInternalRegister ((unsigned char) ((*i).second._status._MEMPTR >> 8));
		// Set the memoty situation...
		for (const auto& j : (*i).second._status._data)
		{
			unsigned int l = 0;
			for (const auto& k : j._data)
				cpu -> memoryRef () -> put (MCHEmul::Address (2, ((unsigned int) j._position) + l++), k);
		}

		// Then get the instruction to execute...
		unsigned int j = 0;
		unsigned short tStates = (*i).second._status._tStates;
		MCHEmul::ListOfInstructions nIs;
		// ...and executes it!
		bool e = false;
		while (j < (unsigned int) tStates && !e)
		{
			MCHEmul::Instruction* nI = const_cast <MCHEmul::Instruction*> (cpu -> nextInstruction ());
			if (nI == nullptr)
			{ 
				e = true;

				continue;
			}

			e = !nI -> execute (cpu, cpu -> memoryRef (), 
				cpu -> memoryRef () -> stack (), &cpu -> programCounter ());
			
			j += nI -> clockCyclesExecuted () + nI -> additionalClockCyclesExecuted ();

			// The block to execute can be a block of instructions...
			nIs.push_back (nI);
		}

		// If there were errors during the execution...
		// print hem out, and try the next test!
		if (e)
		{
			_noimplemented.emplace_back ("Test not implemented:" + (*i).first);

			continue;
		}

		// But if not look whether the expected result is the real result...
		const Test::TestOut& tOut = (*_testsOut.find ((*i).first)).second;
		// First with the registers...
		bool eI1 =  
			cpu -> afRegister ()[0]  -> values ()[0] == (unsigned char) ((tOut._status._AF & 0xff00) >> 8) &&
			cpu -> bcRegister ()[0]  -> values ()[0] == (unsigned char) ((tOut._status._BC  & 0xff00) >> 8) &&
			cpu -> bcRegister ()[1]  -> values ()[0] == (unsigned char) (tOut._status._BC  & 0x00ff) &&
			cpu -> deRegister ()[0]  -> values ()[0] == (unsigned char) ((tOut._status._DE  & 0xff00) >> 8) &&
			cpu -> deRegister ()[1]  -> values ()[0] == (unsigned char) (tOut._status._DE  & 0x00ff) &&
			cpu -> hlRegister ()[0]  -> values ()[0] == (unsigned char) ((tOut._status._HL  & 0xff00) >> 8) &&
			cpu -> hlRegister ()[1]  -> values ()[0] == (unsigned char) (tOut._status._HL  & 0x00ff) &&
			cpu -> ixRegister ()[0]  -> values ()[0] == (unsigned char) ((tOut._status._IX  & 0xff00) >> 8) &&
			cpu -> ixRegister ()[1]  -> values ()[0] == (unsigned char) (tOut._status._IX  & 0x00ff) &&
			cpu -> iyRegister ()[0]  -> values ()[0] == (unsigned char) ((tOut._status._IY  & 0xff00) >> 8) &&
			cpu -> iyRegister ()[1]  -> values ()[0] == (unsigned char) (tOut._status._IY  & 0x00ff) &&
			cpu -> afpRegister ()[0] -> values ()[0] == (unsigned char) ((tOut._status._AFP & 0xff00) >> 8) &&
			cpu -> afpRegister ()[1] -> values ()[0] == (unsigned char) (tOut._status._AFP & 0x00ff) &&
			cpu -> bcpRegister ()[0] -> values ()[0] == (unsigned char) ((tOut._status._BCP & 0xff00) >> 8) &&
			cpu -> bcpRegister ()[1] -> values ()[0] == (unsigned char) (tOut._status._BCP & 0x00ff) &&
			cpu -> depRegister ()[0] -> values ()[0] == (unsigned char) ((tOut._status._DEP & 0xff00) >> 8) &&
			cpu -> depRegister ()[1] -> values ()[0] == (unsigned char) (tOut._status._DEP & 0x00ff) &&
			cpu -> hlpRegister ()[0] -> values ()[0] == (unsigned char) ((tOut._status._HLP & 0xff00) >> 8) &&
			cpu -> hlpRegister ()[1] -> values ()[0] == (unsigned char) (tOut._status._HLP & 0x00ff);
		// ...the status register...
		// ...without the XZ & YZ flags...
		bool eI21 = (cpu -> afRegister ()[1]  -> values ()[0] & 0xd7) == 
			(unsigned char) (tOut._status._AF  & 0x00d7);
		// ...and also those flags!
		bool eI22 = (cpu -> afRegister ()[1]  -> values ()[0] & ~0xd7) == 
			(unsigned char) (tOut._status._AF  & ~0x00d7);
		// ...the I & R registers...
		bool eI3 = 
			cpu -> iRegister ().values ()[0] == tOut._status._I &&
			cpu -> rRegister ().values ()[0] == tOut._status._R;
		// ...the program counter...
		bool eI4 = cpu -> programCounter ().internalRepresentation () == (unsigned int) tOut._status._PC;
		// ...the stack pointer...
		bool eI5 = cpu -> memoryRef () -> stack () -> currentAddress () == MCHEmul::Address (2, (unsigned int) tOut._status._SP);
		// The info about the interrupts (INT specially)...
		bool eI6 =
			cpu -> IFF1 () == tOut._status._IFF1 &&
			cpu -> IFF2 () == tOut._status._IFF2 &&
			static_cast <FZ80::INTInterrupt*> (cpu -> interrupt (FZ80::INTInterrupt::_ID)) 
				-> INTMode () == tOut._status._IM &&
			(tOut._status._halted ? cpu -> haltActive () : ! cpu -> haltActive ());
		// The Tstates executed...
		bool eI7 = j == (tOut._status._tStates);
		// The memory situation...
		bool eI8 = true;
		for (const auto& j : tOut._status._data)
		{ 
			unsigned int l = 0;
			for (const auto& k : j._data)
				eI8 &= (cpu -> memoryRef () -> value (MCHEmul::Address (2, ((unsigned int) j._position) + l++)).value () == k);
		}

		// If after all testings, there is no error...
		if (!(eI1 && eI21 && eI3 && eI4 && eI5 && eI6 && eI7 && eI8))
		{
			std::cout << "Error:" << (*i).first + "(" + instAsString (nIs, "; ") + ")" << std::endl;
			if (!eI1) _errors.emplace_back 
				("Error in test:" + (*i).first + " in Registers [" + instAsString (nIs, "; ") + "]");
			if (!eI21) _errors.emplace_back 
				("Error in test:" + (*i).first + " in Status Register(" + 
					std::to_string ((int) cpu -> statusRegister ().values ()[0].value ()) + " vs " + 
					std::to_string ((int) ((unsigned char) (tOut._status._AF & 0x00ff))) + 
					") [" + instAsString (nIs, "; ") + "]");
			if (!eI3) _errors.emplace_back 
				("Error in test:" + (*i).first + " in IR Registers [" + instAsString (nIs, "; ") + "]");
			if (!eI4) _errors.emplace_back 
				("Error in test:" + (*i).first + " in Program Counter [" + instAsString (nIs, "; ") + "]");
			if (!eI5) _errors.emplace_back 
				("Error in test:" + (*i).first + " in Program Stack Pointer [" + instAsString (nIs, "; ") + "]");
			if (!eI6) _errors.emplace_back 
				("Error in test:" + (*i).first + " in Program Stack Interrupt Management [" + instAsString (nIs, "; ") + "]");
			if (!eI7) _errors.emplace_back 
				("Error in test:" + (*i).first + " in TStates [" + instAsString (nIs, "; ") + "]");
			if (!eI8) _errors.emplace_back 
				("Error in test:" + (*i).first + " in Memory [" + instAsString (nIs, "; ") + "]");
		}

		if (!eI22)
			_warnings.emplace_back 
				("Warning in test:" + (*i).first + " in Registers, flags XZ(" + 
					(((cpu -> statusRegister ().values ()[0].value () & 0x08) != 0x00) ? "1" : "0") + " vs " +
					(((tOut._status._AF & 0x0008) != 0x0000) ? "1" : "0") + "), YZ(" + 
					(((cpu -> statusRegister ().values ()[0].value () & 0x20) != 0x00) ? "1" : "0") + " vs " +
					(((tOut._status._AF & 0x0020) != 0x0000) ? "1" : "0") + ") ([" + instAsString (nIs, "; ") + "]");
	}

	delete (cpu);
}

unsigned short Test::fromHexa (const std::string& str)
{
	unsigned short result = 0;
	std::istringstream ss (str);
	ss >> std::hex >> result;
	return (result);
}

Test::Tokens Test::getLineTokens (std::ifstream& f, bool& fF)
{
	Test::Tokens result;

	fF = false;
	std::string data;
	
	if (!std::getline (f, data)) 
	{ 
		fF = true;

		return (result);
	}

	std::stringstream ss (data);
	std::string tk;
	while (std::getline (ss, tk, ' '))
		result.emplace_back (tk);
	result.erase (std::remove (result.begin (), result.end (), ""), result.end ());

	return (result);
}

Test::TestStatus Test::getTestStatus (const Test::TestTokens& tT, size_t i)
{
	Test::TestStatus result;
			
	assert (tT [i].size () == 13);
	result._AF		= fromHexa (tT [i][0]);
	result._BC		= fromHexa (tT [i][1]);
	result._DE		= fromHexa (tT [i][2]);
	result._HL		= fromHexa (tT [i][3]);
	result._AFP		= fromHexa (tT [i][4]);
	result._BCP		= fromHexa (tT [i][5]);
	result._DEP		= fromHexa (tT [i][6]);
	result._HLP		= fromHexa (tT [i][7]);
	result._IX		= fromHexa (tT [i][8]);
	result._IY		= fromHexa (tT [i][9]);
	result._SP		= fromHexa (tT [i][10]);
	result._PC		= fromHexa (tT [i][11]);
	result._MEMPTR	= fromHexa (tT [i][12]);
	
	i++;
	assert (tT [i].size () == 7);
	result._I		= (unsigned char) fromHexa (tT [i][0]);
	result._R		= (unsigned char) fromHexa (tT [i][1]);
	result._IFF1	= (tT [i][2] == "1") ? true : false;
	result._IFF2	= (tT [i][3] == "1") ? true : false;
	result._IM		= std::atoi (tT [i][4].c_str ()); // Interrupt state...
	result._halted	= (tT [i][5] == "1" ? true : false);
	result._tStates	= (unsigned short) std::atoi (tT [i][6].c_str ());
			
	for (i++ ;i < tT.size (); i++)
	{
		Test::TestStatus::Memory mS;
		mS._position	= fromHexa (tT [i][0]);
		assert (tT [i].size () >= 2);
		for (size_t j = 1; j < tT [i].size (); j++)
			if (tT [i][j] != "-1")
				mS._data.emplace_back ((unsigned char) fromHexa (tT [i][j]));
		result._data.emplace_back (std::move (mS));
	}

	return (result);
}

Test::TestInMap Test::readTestsIn (const std::string& n)
{
	Test::TestInMap result;

	std::ifstream f (n);
	if (!f)
		return (result);

	bool end = false;
	Test::TestTokens tT;
	while (!end)
	{
		Test::Tokens tk = getLineTokens (f, end);
		if (end)
			continue;
		
		if (tk.empty ())
		{
			Test::TestIn tIn;

			assert (tT.size () >= 4);
			assert (tT [0].size () == 1);

			tIn._name = tT [0][0];

			tIn._status = getTestStatus (tT, 1);

			result [tIn._name] = tIn;

			tT =  { };
		}
		else
		if (tk.size () != 1 || tk [0] != "-1")
			tT.emplace_back (std::move (tk));
	}

	return (result);
}

Test::TestOutMap Test::readTestsOut (const std::string& n)
{
	Test::TestOutMap result;

	std::ifstream f (n);
	if (!f)
		return (result);

	bool end = false;
	Test::TestTokens tT;
	while (!end)
	{
		Test::Tokens tk = getLineTokens (f, end);
		if (end)
			continue;
		
		if (tk.empty ())
		{
			Test::TestOut tOut;

			assert (tT.size () >= 4);
			assert (tT [0].size () == 1);

			tOut._name = tT [0][0];
			
			size_t i = 1;
			do
			{
				assert (tT [i].size () >= 3);
				Test::TestOut::Step stp;
				stp._time = std::atoi (tT [i][0].c_str ());
				if (tT [i][1] == "MR") stp._type = TestOut::Step::Type::_MR;
				else if ((tT [i][1] == "MW")) stp._type = TestOut::Step::Type::_MW;
				else if ((tT [i][1] == "MC")) stp._type = TestOut::Step::Type::_MC;
				else if ((tT [i][1] == "PR")) stp._type = TestOut::Step::Type::_PR;
				else if ((tT [i][1] == "PW")) stp._type = TestOut::Step::Type::_PW;
				else if ((tT [i][1] == "PC")) stp._type = TestOut::Step::Type::_PC;
				stp._position = fromHexa (tT [i][2]);
				stp._data = (tT [i].size () == 4) ? (unsigned char) fromHexa (tT [i][3]) : 0x00;
				tOut._steps.emplace_back (std::move (stp));
			} while (tT [++i].size () != 13);

			tOut._status = getTestStatus (tT, i);

			result [tOut._name] = tOut;

			tT =  { };
		}
		else
			tT.emplace_back (std::move (tk));
	}

	return (result);
}
