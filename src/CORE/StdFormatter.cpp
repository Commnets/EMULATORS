#include <CORE/StdFormatter.hpp>
#include <CORE/FmterBuilder.hpp>
#include <sstream>

// ---
void MCHEmul::StdFormatter::initialize ()
{
	bool fL = true;
	for (const auto& i : _lines)
	{
		if (!fL)
			_pieces.emplace_back (new MCHEmul::StdFormatter::FixTextPiece ("\n"));

		fL = false;

		std::string l = i;
		while (l != "")
		{
			// Look for a potential data definition within the line..
			size_t oi = l.find ('@');
			// There is no any...
			if (oi == std::string::npos)
			{
				_pieces.emplace_back (new MCHEmul::StdFormatter::FixTextPiece (l));

				l = "";

				continue;
			}

			// There will be a data definition just if there is also other @ simbol near...
			size_t of = l.find ('@', oi + 1); 
			// But not the one after! (would means that the @ char is wanted to be used)
			// So if it the situation, then no data definition at all..
			if (of == std::string::npos || of == (oi + 1)) 
			{
				_pieces.emplace_back (new MCHEmul::StdFormatter::FixTextPiece (l.substr (oi) + '@'));
				
				l = (of == std::string::npos) ? "" : l.substr (of + 1);

				continue;
			}

			// It seems that a data definition has been found
			// The type of the field
			MCHEmul::StdFormatter::Piece::Type tp = MCHEmul::StdFormatter::Piece::Type::_DATA;
			// The name of the field is, initially, everything...
			std::string fName = MCHEmul::trim (l.substr (oi + 1, of - oi - 1));
			// ...the piece can have parameters
			MCHEmul::Strings prms;
			// ...and can be also and array, defined by what is needed to separate the elements...
			std::string pt = "";

			// Looks for the parameters first...
			size_t pPI = fName.find ('{');
			if (pPI != std::string::npos)
			{
				size_t pPF = fName.find ('}');
				if (pPF != std::string::npos && pPF > pPI)
				{
					std::string prmsS = fName.substr (pPI + 1, pPF - pPI - 1);
					// A comma in the parameter can be got wrong by "getElementsFrom"...
					prmsS = MCHEmul::replaceAll (prmsS, "\\,", "\\&2C");
					// The spaces are eliminated in getElementsFrom, so...
					prmsS = MCHEmul::replaceAll (prmsS, "\\ ", "\\&20");
					// The returns are replaced by the right form...
					prmsS = MCHEmul::replaceAll (prmsS, "\\n", "\\&13");
					// ...and also the tabs...
					prmsS = MCHEmul::replaceAll (prmsS, "\\t", "\\&11");

					prms = MCHEmul::getElementsFrom (prmsS, ',');

					fName = fName.substr (0, pPI) + fName.substr (pPF + 1);
				}
			}

			// Then analyze whether is a structure...
			size_t oK = fName.find ('?');
			if (oK != std::string::npos)
				tp = MCHEmul::StdFormatter::Piece::Type::_ARRAY;
			else
			{
				// ...or an invocation to another formatter...
				oK = fName.find ('>');
				if (oK != std::string::npos)
					tp = MCHEmul::StdFormatter::Piece::Type::_INVOKE;
			}

			// If found, rebuilt up the name and the post...
			if (oK != std::string::npos)
			{
				if (oK != (fName.length () - 1))
					pt = fName.substr (oK + 1);
				fName = fName.substr (0, oK);
			}

			// Just in case, convert the \\n and \\t into a proper control code!
			pt = MCHEmul::replaceAll (pt, "\\n", "\n");
			pt = MCHEmul::replaceAll (pt, "\\t", "\t");

			// There will be a fix text before the definition of the data...
			if (oi != 0)
				_pieces.emplace_back (new MCHEmul::StdFormatter::FixTextPiece (l.substr (0, oi)));
			// ...and a maybe a piece if there is a name...
			if (fName != "")
				_pieces.emplace_back (createPiece (tp, fName, prms, pt));

			l = l.substr (of + 1);
		}
	}
}

// ---
std::string MCHEmul::StdFormatter::format (const MCHEmul::InfoStructure& iS) const
{
	std::string result = "";

	// If no piece has been defined...
	if (_pieces.empty ())
		result = iS.asString (_defSeparator, _defEqual, "", "", 
			_printFirst, _whenEmpty, _blockSize); // a very basic conversion is defined...
	// Otherwise it has to be treaten piece by piece...
	else
		for (const auto& i : _pieces)
			result += i -> format (iS);

	return (result);
}

// ---
MCHEmul::Attributes MCHEmul::StdFormatter::Piece::attrsFromStrings (const MCHEmul::Strings& strs)
{
	MCHEmul::Attributes result;

	for (const auto& i : strs)
	{
		size_t p = i.find ('=');
		if (p == std::string::npos)
			continue;

		std::string aV = i.substr (p + 1);
		aV = MCHEmul::replaceAll (aV, "\\&2C", ","); // The comma is put back...
		aV = MCHEmul::replaceAll (aV, "\\&20", " "); // ...and the same with the space...
		aV = MCHEmul::replaceAll (aV, "\\&13", "\n"); // ...and the same with the return...
		aV = MCHEmul::replaceAll (aV, "\\&11", "\t"); // ...and the same with the tab...
		result.insert (MCHEmul::Attributes::value_type (MCHEmul::trim (i.substr (0, p)), aV));
	}

	return (result);
}

// ---
MCHEmul::StdFormatter::FixTextPiece::FixTextPiece (const std::string& n)
	: MCHEmul::StdFormatter::Piece (Type::_TEXT, n, { }, "")
{ 
	_name = MCHEmul::replaceAll (_name, "\\n", "\n");
	_name = MCHEmul::replaceAll (_name, "\\t", "\t");
}

// ---
std::string MCHEmul::StdFormatter::TablePiece::format (const MCHEmul::InfoStructure& iS) const
{
	std::string by = iS.attribute (_name);

	size_t sb = std::numeric_limits <size_t>::max ();
	std::string sbs = attribute ("blocksize");
	if (sbs != "") sb = (size_t) std::atoi (sbs.c_str ());

	// How the original elements are separated...
	char sp = ',';
	std::string sps = attribute ("listsep");
	if (sps != "") sp = sps [0];

	// The element used to separate the elements in the table...
	// If the attribute is not defined, then the default one is used...
	// ..and this is ""...
	std::string tsp = attribute ("tablesep");

	// The minimum size that every element in the table has to have...
	size_t es = 0;
	std::string ess = attribute ("minsize");
	if (ess != "") es = (size_t) std::atoi (ess.c_str ());

	// The max number of elements to print out...
	// If the table has more than this number of elements, 
	// only the last element (max size) will be printed out
	// If the parameter is not defined all will be printed out... (-1)
	int ne = -1;
	std::string nes = attribute ("maxelements");
	if (nes != "") ne = std::atoi (nes.c_str ());

	return (MCHEmul::tableFormat (MCHEmul::getElementsFrom (by, sp), tsp, es, sb, ne));
}

// ---
std::string MCHEmul::StdFormatter::ArrayPiece::format (const MCHEmul::InfoStructure& iS) const
{
	if (!iS.existsInfoStructure (_name))
		return ("");

	MCHEmul::InfoStructure sIS = iS.infoStructure (_name);

	std::string result = "";

	auto basicFmt = [=](const MCHEmul::InfoStructure& iS) -> std::string
		{
			std::string r = "";
			std::shared_ptr <MCHEmul::StdFormatter> dF = std::static_pointer_cast <MCHEmul::StdFormatter> 
				(FormatterBuilder::instance () -> defaultFormatter ());
			if (dF != nullptr)
			{ 
				dF -> setDefFormatElements 
					(_post, 
					 attribute ("equal"), 
					 attribute ("key") == MCHEmul::_YES, 
					 attribute ("empty"),
					 existAttribute ("blocksize") 
						? std::atoi (attribute ("blocksize").c_str ()) : -1); // The size of the block can be defined...

				r = dF -> format (iS);
			}
			else
				r = iS.asString (); // ...a very very default one might be also used...

			return (r);
		};

	// If there is an arrau of elements to print out...
	if (!sIS.infoStructures ().empty ())
	{ 
		int ct = 0;
		std::shared_ptr <MCHEmul::StdFormatter> sDF = nullptr;
		for (const auto& i : sIS.infoStructures ())
		{
			// To indicate whether a special formatter was finally used...
			bool sft = false;
			std::string fmter = attribute ("fmter");
			if (fmter != "")
			{ 
				if (fmter == ".CLASS") // The formatter aligned with the class name is taken.
					sDF = std::static_pointer_cast <MCHEmul::StdFormatter> 
						(FormatterBuilder::instance () -> formatter (i.second.attribute	("CLASSNAME")));
				else // otherwise the formatter indicated is taken...
					sDF = std::static_pointer_cast <MCHEmul::StdFormatter> (FormatterBuilder::instance () -> formatter (fmter));

				if (sDF != nullptr)
				{
					result +=  ((ct == 0) ? "" : _post) + sDF -> format (i.second);

					sft = true; // A formatter was used...
				}
			}

			// If no special formatter was used,
			// the default one will be used insted, if exists, and if not the most basic one...
			if (!sft)
				result = basicFmt (i.second);

			ct++;
		}
	}
	else
		result = basicFmt (sIS);

	return (result);
}

// ---
std::string MCHEmul::StdFormatter::InvokePiece::format (const MCHEmul::InfoStructure& iS) const
{
	std::string result = "";

	// If the redirection is over .CLASS
	// Look for a formatter with the name of the class and then print it out!
	if (_name == ".CLASS")
	{
		const std::shared_ptr <MCHEmul::StdFormatter>& sFmter = 
			std::static_pointer_cast <MCHEmul::StdFormatter> (MCHEmul::FormatterBuilder::instance () -> 
				formatter (iS.attribute (".CLASSNAME")));
		if (sFmter != nullptr)
			result = sFmter -> format (iS) + _post;
	}
	// ...otherwise the redirection is applied over the attribute pointed out!
	else
	{
		if (iS.existsInfoStructure (_name))
		{
			MCHEmul::InfoStructure sIS = iS.infoStructure (_name);
			const std::shared_ptr <MCHEmul::StdFormatter>& sFmter = 
				std::static_pointer_cast <MCHEmul::StdFormatter> (MCHEmul::FormatterBuilder::instance () -> formatter (_name));
			if (sFmter != nullptr)
				result = sFmter -> format (sIS) + _post;
		}
	}

	return (result);
}

// ---
MCHEmul::StdFormatter::Piece* MCHEmul::StdFormatter::createPiece
	(MCHEmul::StdFormatter::Piece::Type tp, const std::string& n, const MCHEmul::Strings& prms, const std::string& pt) const
{
	MCHEmul::StdFormatter::Piece* result = nullptr;

	switch (tp)
	{
		case MCHEmul::StdFormatter::Piece::Type::_TEXT:
			result = new MCHEmul::StdFormatter::FixTextPiece (n);
			break;

		case MCHEmul::StdFormatter::Piece::Type::_DATA:
			result = (n == "BYTES") 
				? (MCHEmul::StdFormatter::Piece*) new MCHEmul::StdFormatter::TablePiece (n, prms, pt) 
				: (MCHEmul::StdFormatter::Piece*) new MCHEmul::StdFormatter::AttributePiece (n, prms, pt);
			break;

		case MCHEmul::StdFormatter::Piece::Type::_ARRAY:
			result = new MCHEmul::StdFormatter::ArrayPiece (n, prms, pt);
			break;

		case MCHEmul::StdFormatter::Piece::Type::_INVOKE:
			result = new MCHEmul::StdFormatter::InvokePiece (n, prms, pt);
			break;

		default:
			break;
	}

	return (result);
}
