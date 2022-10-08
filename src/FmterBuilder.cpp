#include <CORE/FmterBuilder.hpp>
#include <fstream>

// ---
std::unique_ptr <MCHEmul::FormatterBuilder> MCHEmul::FormatterBuilder::_instance = nullptr;

// ---
const MCHEmul::Formatter& MCHEmul::FormatterBuilder::formatter (const std::string& n) const
{
	MCHEmul::Formatters::const_iterator i = _formatters.find (n);
	return ((i == _formatters.end ()) 
		? MCHEmul::Formatter::_nullFormatter : (*i).second);
}

// ---
MCHEmul::FormatterBuilder::FormatterBuilder (const std::string& ff)
	: _formatters (), 
	  _defaultFormatter ({ }) // very basic formatter...
{
	MCHEmul::Strings lns;
	std::ifstream fFile (ff);
	if (fFile.is_open ())
	{
		bool pL = false; // No formatter under definition...
		std::string fmtsN = ""; // The name of the formatter under definition (when any)...
		MCHEmul::Strings fmts; // The lines defining a formatter...
		
		char l [255];
		while (!fFile.eof ())
		{
			fFile.getline (l, 255);
			std::string lS (l);	std::string lSC = MCHEmul::trim (lS); // The line is read with and withput spaces before and after...
			if (lSC != "" && lSC [0] == '#') continue; // Comments are not taken into account...
			else if (lSC != "" && lSC [0] == ';') // If the line starts with ; a new formatter is being defined...
			{
				// If there was a formatter under definition, 
				// the definition of a new one finishes the previous one...
				if (pL)
				{
					_formatters.insert (std::pair <std::string, MCHEmul::Formatter> 
						(fmtsN, MCHEmul::Formatter (fmts)));

					fmts = { }; fmtsN = ""; 

					pL = false;
				}

				pL = true;

				fmtsN = lSC.substr (1);
			}
			else
				fmts.push_back (lS); // The line defining the formatter is kept as it is defined in the file (with spaces and evrything...)
		}

		fFile.close ();
		
		if (pL)
			_formatters.insert (std::pair <std::string, MCHEmul::Formatter>
				(fmtsN, MCHEmul::Formatter (fmts)));
	}
}
