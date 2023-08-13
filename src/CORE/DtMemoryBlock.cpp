#include <CORE/DtMemoryBlock.hpp>
#include <fstream>
#include <sstream>

const std::string MCHEmul::DataMemoryBlock::_NOATTRIBUTE = "";
MCHEmul::DataMemoryBlockTextFormatter::CounterFormatterElement::Elements 
	MCHEmul::DataMemoryBlockTextFormatter::CounterFormatterElement::_counters = { };

// ---
bool MCHEmul::DataMemoryBlock::save (const std::string& fN, size_t nB, bool bE) const
{
	MCHEmul::UBytes sAB = MCHEmul::UBytes (_startAddress.bytes (), bE); 
	sAB.setMinLength (nB);
	std::vector <MCHEmul::UByte> sABb = sAB.bytes ();

	std::vector <MCHEmul::UByte> dt = _bytes;
	dt.insert (dt.begin (), sABb.begin (), sABb.end ());

	return (MCHEmul::UBytes::saveBytesTo (fN, dt));
}

// ---
MCHEmul::DataMemoryBlock MCHEmul::DataMemoryBlock::loadBinaryFile (const std::string& fN, bool& e, size_t nB, bool bE)
{
	e = false;

	std::vector <MCHEmul::UByte> by = UBytes::loadBytesFrom (fN, e);
	if (e)
		return (MCHEmul::DataMemoryBlock ());

	// The length of the data file has to be at least the needed to hold the starting address
	// That is defined by the number of bytes of the cpu behind...
	if (by.size () < nB)
	{
		e = true;

		return (MCHEmul::DataMemoryBlock ());
	}

	// The first bytes (nB) of the data represents the address where to load the rest...
	// That address has to be represented in the format of the computer behind (little or big endian)
	MCHEmul::DataMemoryBlock result 
		(MCHEmul::Address (std::vector <MCHEmul::UByte> (by.begin (), by.begin () + nB), bE),
			std::vector <MCHEmul::UByte> (by.begin () + nB, by.end ()));

	return (result);
}

// ---
MCHEmul::DataMemoryBlocks MCHEmul::DataMemoryBlock::loadBlocksFile (const std::string& fN, bool& e, size_t nB, bool bE)
{
	e = false;

	std::vector <MCHEmul::UByte> by = UBytes::loadBytesFrom (fN, e);
	if (e)
		return (MCHEmul::DataMemoryBlocks ());

	// The length of the data file has to be at least the needed to hold the starting address
	// That is defined by the number of bytes of the cpu behind...
	if (by.size () < (nB + 4 /** 4 bytes for keeping the size of one block. */))
	{
		e = true;

		return (MCHEmul::DataMemoryBlocks ());
	}

	MCHEmul::DataMemoryBlocks result;
	std::vector <MCHEmul::UByte>::const_iterator i = by.begin ();
	while (i != by.end ())
	{
		// Reads a block...
		MCHEmul::Address sA (std::vector <MCHEmul::UByte> (i, i + nB), bE);
		MCHEmul::UInt bS (std::vector <MCHEmul::UByte> (i + nB, i + nB + 4));
		MCHEmul::DataMemoryBlock pR
		(sA, std::vector <MCHEmul::UByte> (i + nB + 4, i + nB + 4 + (size_t) bS.asUnsignedInt ()));

		// Adds to the total...
		result.emplace_back (pR);

		// Next...
		i += nB + 4 + (size_t) bS.asUnsignedInt ();
	}

	return (result);
}

// ---
bool MCHEmul::DataMemoryBlock::saveDataBlocks 
	(const std::string& fN, const MCHEmul::DataMemoryBlocks& blks, size_t nB, bool bE)
{
	std::vector <MCHEmul::UByte> dt;
	for (const auto& i : blks)
	{
		std::vector <MCHEmul::UByte> pdt = i._bytes;

		// Insert the length to include
		MCHEmul::UInt l = MCHEmul::UInt ((unsigned int) pdt.size ()); 
		l.setMinLength (4); // ...with a minimum length of 4 bytes (is the maximum size admitted by the emulation)
		pdt.insert (pdt.begin (), l.bytes ().begin (), l.bytes ().end ());

		// Then the address...(@see above), that becomes first...
		// In the format understood by the cpu (little or big endian)
		// ...and with number of bytes that are also managed by the cpu behind!
		MCHEmul::UBytes sAB = MCHEmul::UBytes (i._startAddress.bytes (), bE);
		sAB.setMinLength (nB);
		std::vector <MCHEmul::UByte> sABb = sAB.bytes (); 
		pdt.insert (pdt.begin (), sABb.begin (), sABb.end ());

		// At this point the address is first and then the length of the data kept.

		// Adds the block to the total list to be saved later...
		dt.insert (dt.end (), pdt.begin (), pdt.end ());
	}

	return (MCHEmul::UBytes::saveBytesTo (fN, dt));
}

// ---
MCHEmul::DataMemoryBlockTextFormatter::DataMemoryBlockTextFormatter (const std::string& fN)
	: _fmtElements ()
{
	std::ifstream f (fN);
	if (!f)
	{
		_fmtElements =
			{
				// ADDRESS:XXX
				// SIZE:XXX
				// DATA xxx,xxx,xxx,... up to 16 elements per line
				// DATA xxx,xxx,xxx,... and separated each by comma
				// ...

				new MCHEmul::DataMemoryBlockTextFormatter::TextFormatterElement ("ADDRESS:"),
				new MCHEmul::DataMemoryBlockTextFormatter::VariableFormatterElement ("ADDRESS", 0),
				new MCHEmul::DataMemoryBlockTextFormatter::TextFormatterElement ("\n"),
				new MCHEmul::DataMemoryBlockTextFormatter::TextFormatterElement ("SIZE:"),
				new MCHEmul::DataMemoryBlockTextFormatter::VariableFormatterElement ("SIZE", 0),
				new MCHEmul::DataMemoryBlockTextFormatter::TextFormatterElement ("\n"),
				new MCHEmul::DataMemoryBlockTextFormatter::DataFormatterElement (",", 16, 0),
			};

		return;
	}

	MCHEmul::Strings lines;
	while (!f.eof ())
	{
		char str [256];
		f.getline (str, 255);
		lines.emplace_back (std::string (str));
	}

	bool fL = true;
	for (const auto& i : lines)
	{
		std::string cL = i;

		MCHEmul::DataMemoryBlockTextFormatter::FormatterElements fELine;

		auto iLastFormatter = [&]() -> void
			{ fELine.emplace_back 
				(new MCHEmul::DataMemoryBlockTextFormatter::TextFormatterElement (cL));
			  cL = ""; };

		// If the line is not the firt one a carry return is printed out...
		if (!fL)
			_fmtElements.emplace_back 
				(new MCHEmul::DataMemoryBlockTextFormatter::TextFormatterElement ("\n"));
		fL = false;

		while (cL != "")
		{
			// Is there a possible command in the line?
			// if not, the line is just added as pure text...
			size_t iP = cL.find ('[');
			if (iP == std::string::npos)
			{
				iLastFormatter ();

				continue; // ...to finish the line...
			}

			// ...but the command has to be defined between brackets...
			// If not, the line is added as pure text...
			size_t fP = cL.find (']', iP + 1);
			if (fP == std::string::npos)
			{
				iLastFormatter ();

				continue; // ...to finish the line...
			}

			// At this point there a possible command between brackets...

			// All before that possible command should be a text to add...
			if (iP != 0) // whenever there is something to add...
				fELine.emplace_back 
					(new MCHEmul::DataMemoryBlockTextFormatter::TextFormatterElement (cL.substr (0, iP)));

			std::string cmd = MCHEmul::upper (MCHEmul::trim (cL.substr (iP + 1, fP - iP - 1)));
			size_t eP = cmd.find ('='); // To find the parameters that, btw, are not mandatory...
			MCHEmul::DataMemoryBlockTextFormatter::FormatterElement* fmtElmnt = 
				createFormatterElementFrom (cmd.substr (0, eP), // If there hadn't be parameters, the command name is the full command...
					(eP == std::string::npos) // The parameters are not mandatory...
						? MCHEmul::Strings () : MCHEmul::getElementsFrom (cmd.substr (eP + 1), '@'), 
					fELine);
			// If no command were created, nothing would be added to the list...
			if (fmtElmnt != nullptr)
				fELine.emplace_back (fmtElmnt);

			// Time to treat what left from the line...
			cL = cL.substr (fP + 1);
		}

		// Add the remaining elements created for a line into the total...
		if (fELine.size () != 0)
			_fmtElements.insert (_fmtElements.end (), fELine.begin (), fELine.end ());
	}
}

// ---
std::string MCHEmul::DataMemoryBlockTextFormatter::format (const MCHEmul::DataMemoryBlock& dB) const
{ 
	std::string result = ""; 
	
	for (const auto& i : _fmtElements) 
		result += i -> format (dB); 
	
	return (result); 
}

// ---
std::string MCHEmul::DataMemoryBlockTextFormatter::FormatterElement::convertNumberToString (unsigned int n, unsigned int f)
{
	std::string result = "";

	switch (f)
	{
		case 0:
			{
				result = std::to_string (n);
			}

			break;

		case 1:
			{
				while (n != 0) 
				{ 
					result = ((n % 2) == 0 ? "0" : "1" ) + result; 

					n /= 2;
				}
			}
	
			break;

		case 2:
		case 3:
		default:
			{
				std::stringstream ss;
				ss.clear ();
				ss << ((f == 2) ? std::oct : std::hex) << n;
				result = ss.str ();
			}

			break;
	}

	return (result);
}

// ---
std::string MCHEmul::DataMemoryBlockTextFormatter::VariableFormatterElement::format 
	(const MCHEmul::DataMemoryBlock& bc) const
{
	std::string result = "";

	unsigned int val = 0;
	if (_variable == "ADDRESS") val = bc.startAddress ().value ();
	else if (_variable == "FADDRESS") val = bc.endAddress ().value ();
	else if (_variable == "SIZE") val = (unsigned int) bc.bytes ().size ();

	return (convertNumberToString (val, _outputFormat));
}

// ---
std::string MCHEmul::DataMemoryBlockTextFormatter::GroupFormatterElement::format
	(const MCHEmul::DataMemoryBlock& dB) const
{
	MCHEmul::Strings lns = generateLinesFrom (dB);

	std::string result = "";
	for (const auto& i : lns)
	{
		for (const auto& j : _formatters)
			result += j -> format (dB); // Add the formatters of the line to every line...
		result += i + "\n";
	}

	return (result);
}

// ---
MCHEmul::Strings MCHEmul::DataMemoryBlockTextFormatter::DataFormatterElement::generateLinesFrom
	(const MCHEmul::DataMemoryBlock& dB) const
{
	MCHEmul::Strings result;

	const auto& dt = dB.bytes ();
	size_t ct = 0;
	while (ct < dt.size ())
	{
		std::string l = "";
		bool fe = true;
		for (size_t i = 0; i < _numberElements && (ct + i) < dt.size (); i++)
		{ 
			l += (!fe ? _separator : "") + convertNumberToString (dt [ct + i].value (), _outputFormat);

			fe = false;
		}

		result.emplace_back (std::move (l));

		ct += _numberElements;
	}

	return (result);
}

// ---
MCHEmul::DataMemoryBlockTextFormatter::FormatterElement* 
	MCHEmul::DataMemoryBlockTextFormatter::createFormatterElementFrom 
		(const std::string& nC, const MCHEmul::Strings& vC, MCHEmul::DataMemoryBlockTextFormatter::FormatterElements& fE)
{
	MCHEmul::DataMemoryBlockTextFormatter::FormatterElement* result = nullptr;

	// Counter
	if (nC == "CT") 
	{
		if (vC.size () != 3) return (result); // Command not well defined (3 parameters are needed)
		else result = new MCHEmul::DataMemoryBlockTextFormatter::CounterFormatterElement 
			(vC [0], (unsigned int) std::atoi (vC [1].c_str ()), unsigned int (std::atoi (vC [2].c_str ())));
	}
	else
	// Use of the counter
	if (nC == "US")
	{
		MCHEmul::DataMemoryBlockTextFormatter::CounterFormatterElement* ct = nullptr;
		if (vC.size () != 1 ||
			(vC.size () == 1 && (ct = MCHEmul::DataMemoryBlockTextFormatter::CounterFormatterElement::findCounter (vC [0])) == nullptr)) 
			return (result); // Command not well defined (1 parameter is needed) and the counter has to exist...
		else result = new MCHEmul::DataMemoryBlockTextFormatter::UseCounterFormatterElement (ct);
	}
	else
	// A variable
	if (nC == "VR")
	{
		if (vC.size () != 2) return (result); // Command not well defined (1 parameter is needed)
		else result = new MCHEmul::DataMemoryBlockTextFormatter::VariableFormatterElement 
			(vC [0], (unsigned int) std::atoi (vC [1].c_str ()));
	}
	else
	// The data itself...
	if (nC == "DT")
	{
		if (vC.size () != 3) return (result); // Command not well defined (1 parameter is meeded)
		else result = new MCHEmul::DataMemoryBlockTextFormatter::DataFormatterElement 
			(vC [0], std::atoi (vC [1].c_str ()), (unsigned int) std::atoi (vC [2].c_str ()), fE);
		fE = { }; // The previous commands are no longer valid...
	}

	return (result);
}
