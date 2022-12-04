#include <CORE/FmterBuilder.hpp>
#include <CORE/StdFmterBuilder.hpp>
#include <CORE/StdFormatter.hpp>
#include <fstream>

// ---
const std::shared_ptr <MCHEmul::Formatter> MCHEmul::FormatterBuilder::_noFormatter (new MCHEmul::StdFormatter ({ }));
std::shared_ptr <MCHEmul::FormatterBuilder> MCHEmul::FormatterBuilder::_instance = nullptr;

// ---
MCHEmul::FormatterBuilder::FormatterBuilder (const MCHEmul::Strings& nF)
	: _defaultFormatFile (""), _formatters ({ }), _defaultFormatter (nullptr), 
	  _error (MCHEmul::_NOERROR),
	  _linesPerFile ({ })
{
	if (_instance != nullptr)
		exit (1); // Only one instance allowed...

	for (const auto& i : nF)
		_linesPerFile.insert (std::map <std::string, MCHEmul::Strings>::value_type (i, readLinesFrom (i)));
}

// ---
bool MCHEmul::FormatterBuilder::initialize ()
{
	if (_error != MCHEmul::_NOERROR)
		return (false);

	if (!_formatters.empty ())
		return (true); // Already initialized...

	for (const auto& i : _linesPerFile)
	{
		MCHEmul::Formatters fmts;

		bool pL = false; // No formatter under definition...
		std::string fmtsN = ""; // The name of the formatter under definition (when any)...
		MCHEmul::Strings lfmts; // The lines defining a formatter...

		auto crtFmt = [&](const std::string& fN /** file name. */, 
			const std::string& n /** formatter name. */, const MCHEmul::Strings& ls /** formatter lines. */) -> void
		{
			MCHEmul::Formatter* fmt = createFormatter (fN, n, ls);
			if (fmt != nullptr)
			{
				fmt -> initialize ();

				fmts.insert (std::pair <std::string, std::shared_ptr <MCHEmul::Formatter>> (fmtsN, fmt));
			}
			else
			_error = MCHEmul::_FORMATTERNOTVALID_ERROR;
		};

		for (const auto& j : i.second)
		{
			std::string tJ = MCHEmul::trim (j);
			if (tJ[0] == ';') // If the line starts with ; a new formatter is being defined...
			{
				// If there was a formatter under definition, 
				// the definition of a new one finishes the previous one...
				if (pL)
				{
					crtFmt (i.first, fmtsN, lfmts);

					lfmts = { }; fmtsN = "";

					pL = false;
				}

				pL = true;

				fmtsN = tJ.substr (1);
			}
			else
				lfmts.push_back (j);
		}

		if (pL)
			crtFmt (i.first, fmtsN, lfmts);

		_formatters.insert (std::map <std::string, MCHEmul::Formatters>::value_type (i.first, fmts));
	}

	_linesPerFile = { };

	return (_error == MCHEmul::_NOERROR);
}

// ---
std::shared_ptr <MCHEmul::FormatterBuilder> MCHEmul::FormatterBuilder::instance (const Strings& nF)
{
	if (_instance != nullptr)
		return (_instance);

	_instance = std::shared_ptr <MCHEmul::FormatterBuilder> (new MCHEmul::StdFormatterBuilder (nF));
	_instance -> initialize ();
	if (nF.size () > 0) _instance -> _defaultFormatFile = nF [0];
	_instance -> setDefaultFormatter (_noFormatter);

	return (_instance);
}

// ---
bool MCHEmul::FormatterBuilder::existsFormatter (const std::string& n) const
{
	bool result = false;

	std::map <std::string, MCHEmul::Formatters>::const_iterator i = _formatters.find (_defaultFormatFile);
	if (i != _formatters.end ())
		result = ((*i).second.find (n) != (*i).second.end ());

	return (result);
}

// ---
const std::shared_ptr <MCHEmul::Formatter>& MCHEmul::FormatterBuilder::formatter (const std::string& n) const
{
	std::map <std::string, MCHEmul::Formatters>::const_iterator i = _formatters.find (_defaultFormatFile);
	if (i != _formatters.end ())
	{
		const MCHEmul::Formatters& fmts = i -> second;
		MCHEmul::Formatters::const_iterator j = fmts.find (n);
		if (j != fmts.end ())
			return (j -> second);
	}

	return (defaultFormatter ());
}

// ---
MCHEmul::Strings MCHEmul::FormatterBuilder::readLinesFrom (const std::string& nF)
{
	MCHEmul::Strings result;

	std::ifstream fFile (nF);
	if (fFile.is_open ())
	{
		char l[255];
		while (!fFile.eof ())
		{
			fFile.getline (l, 255);
			std::string lS (l);	std::string lSC = MCHEmul::trim (lS); // The line is read with and withput spaces before and after...
			if (lSC != "" && lSC[0] == '#') continue; // Comments are not taken into account...
			else if (lSC != "" && lSC[0] == '?') // It is an include...
			{
				MCHEmul::Strings ils = readLinesFrom (MCHEmul::trim (lSC.substr (1)));

				result.insert (result.end (), ils.begin (), ils.end ());
			}
			else 
				result.push_back (lS); 
		}

		fFile.close ();
	}

	return (result);
}
