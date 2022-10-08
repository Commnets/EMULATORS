#include <CORE/Formatter.hpp>
#include <CORE/FmterBuilder.hpp>
#include <sstream>

const MCHEmul::Formatter MCHEmul::Formatter::_nullFormatter = MCHEmul::Formatter ({ });

// ---
std::ostream& MCHEmul::operator << (std::ostream& o, const MCHEmul::Formatter::Set& s)
{
	MCHEmul::FormatterBuilder::instance () -> setDefaultFormatter (s._format);

	return (o);
}

// ---
MCHEmul::Formatter::Formatter (const MCHEmul::Strings& l)
	: _pieces ()
{
	bool fL = true;
	for (const auto& i : l)
	{
		if (!fL) 
			_pieces.push_back (MCHEmul::Formatter::Piece 
				(MCHEmul::Formatter::Piece::Type::_TEXT, "\n"));

		fL = false;

		std::string l = i;
		while (l != "")
		{
			// Look for a potential data definition within the line..
			size_t oi = l.find ('@');
			// There is no any...
			if (oi == std::string::npos)
			{
				_pieces.push_back (MCHEmul::Formatter::Piece
					(MCHEmul::Formatter::Piece::Type::_TEXT, l));
				l = "";

				continue;
			}

			// There will be a data definition just if there is also other @ simbol near...
			size_t of = l.find ('@', oi + 1); 
			// But not the one after! (would means that the @ char is wanted to be used)
			// So if it the situation, then no data definition at all..
			if (of == std::string::npos || of == (oi + 1)) 
			{
				_pieces.push_back (MCHEmul::Formatter::Piece
					(MCHEmul::Formatter::Piece::Type::_TEXT, l.substr (oi) + '@'));
				l = l.substr (of + 1);

				continue;
			}

			// It seems there is a data definition...
			// It could be either a _DATA or an _ARRAY...
			// Initially it is supposed that it is _DATA!
			MCHEmul::Formatter::Piece::Type tp = MCHEmul::Formatter::Piece::Type::_DATA;
			// The name of the field is everything...
			std::string fName = MCHEmul::trim (l.substr (oi + 1, of - oi - 1));
			// ...and there is nothing to print after ever!
			std::string pt = "";
			// If would be an array just when the character "?" is included in the name
			// What is after that symbol will be inserting after formtting it!
			size_t oK = fName.find ('?');
			if (oK != std::string::npos)
			{
				if (oK != (fName.length () - 1))
					pt = fName.substr (oK + 1);
				fName = fName.substr (0, oK);
				tp = MCHEmul::Formatter::Piece::Type::_ARRAY;
			}

			// Just in case, convert the \\n and \\t into a proper control code!
			if (pt == "\\n") pt = "\n";
			else if (pt == "\\t") pt = "\t";

			// Insert the data definition...
			_pieces.push_back (MCHEmul::Formatter::Piece
				(MCHEmul::Formatter::Piece::Type::_TEXT, l.substr (0, oi)));
			_pieces.push_back (MCHEmul::Formatter::Piece (tp, fName, pt));
			l = l.substr (of + 1);
		}
	}
}

// ---
std::string MCHEmul::Formatter::format (const MCHEmul::Attributes& a) const
{
	std::string result = "";

	// If nothing has been defined...
	if (_pieces.empty ())
	{
		std::stringstream ss;
		ss << a; // A very basic output is choosen...
		result = ss.str ();
	}
	// Otherwise it has to be treaten...
	else
	{
		for (const auto& i : _pieces)
		{
			switch (i._type)
			{
				case MCHEmul::Formatter::Piece::Type::_DATA:
				{
					MCHEmul::Attributes::const_iterator j = a.find (i._data);
					if (j != a.end ())
						result += (*j).second;
				}

				break;

				case MCHEmul::Formatter::Piece::Type::_ARRAY:
				{
					bool f = true; // First element in the array?
					size_t ct = 0; // Element counter!
					bool fd = true; // To indicate whether the array element is or not found...
					while (fd)
					{
						MCHEmul::Attributes::const_iterator j = a.find (i._data + std::to_string (ct++));
						if (fd = (j != a.end ()))
						{
							if (!f) result += i._post;
							f = false;
							result += (*j).second;
						}
					}
				}

				break;

				default:
					result += i._data;
			}
		}
	}

	return (result);
}
