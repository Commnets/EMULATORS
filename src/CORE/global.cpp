#include <CORE/global.hpp>
#include <algorithm>
#include <cctype>
#include <locale>
#include <codecvt>

const MCHEmul::ClockTime MCHEmul::_STARTINGTIME = 
	std::chrono::time_point_cast <MCHEmul::ClockDurationType> (ClockType::now ());
MCHEmul::ClockTime MCHEmul::_NOW = std::chrono::time_point_cast <MCHEmul::ClockDurationType> (MCHEmul::ClockType::now ());
MCHEmul::ClockDurationType MCHEmul::_MILLISECONDSPAST = ClockDurationType (0);

// ---
std::ostream& MCHEmul::operator << (std::ostream& o, const MCHEmul::Strings& strs)
{
	bool f = true;
	for (const auto& i : strs)
	{
		if (!f)	o << std::endl;
		o << i;
		f = false;
	}

	return (o);
}

// ---
std::ostream& MCHEmul::operator << (std::ostream& o, const MCHEmul::Attributes& attrs)
{ 
	bool f = true;
	for (const auto& i : attrs)
	{
		if (!f)	o << std::endl;
		o << i.first << ":" << i.second;
		f = false;
	}

	return (o);
}

// ---
size_t MCHEmul::firstSpaceIn (const std::string& s)
{
	std::string::const_iterator i = 
		std::find_if (s.begin (), s.end (), [](unsigned char c) -> bool { return (std::isspace (c)); });
	return (i == s.end () ? std::string::npos : s.find (*i));
}

// ---
size_t MCHEmul::firstOf (const std::string& s, const std::string& c)
{
	size_t r = std::string::npos; 
	for (size_t i = 0; i < c.length () && r == std::string::npos; r = s.find (c [i++]));
	return (r);
}

// ---
size_t MCHEmul::firstOf (const std::string& s, const MCHEmul::Strings& strs, size_t& r)
{
	size_t pos = std::string::npos;
	for (r = 0; r < strs.size () && pos == std::string::npos; pos = s.find (strs [r++]));
	if (pos != std::string::npos) r--; // If something was found the selected string is the previous one...
	return (pos);
}

// ---
std::string MCHEmul::ltrim (const std::string& s)
{
	std::string r = s; 
	r.erase (r.begin (), std::find_if (r.begin (), r.end (), 
		[](unsigned char ch) -> bool { return (!std::isspace (ch)); })); 

	return (r); 
}

// ---
std::string MCHEmul::rtrim (const std::string& s)
{
	std::string r = s; 
	r.erase (std::find_if (r.rbegin (), r.rend (), 
		[](unsigned char ch) -> bool { return (!std::isspace (ch)); }).base (), r.end ()); 

	return (r); 
}

// ---
std::string MCHEmul::trim (const std::string& s)
{
	return (MCHEmul::rtrim (MCHEmul::ltrim (s))); 
}

// ---
std::string MCHEmul::upper (const std::string& s)
{ 
	std::string r = s; 
	for (auto& i : r) 
		i = std::toupper (i); 
	
	return (r);
}

// ---
std::string MCHEmul::upperExcept (const std::string& s)
{
	std::string r = s; 

	bool q = false;
	for (auto& i : r)
	{
		if (i == '\'' || i == '\"')
		{
			q = !q;

			continue;
		}

		if (!q)
			i = std::toupper (i);
	}
	
	return (r);

}

// ---
std::string MCHEmul::lower (const std::string& s)
{
	std::string r = s; 
	for (auto& i : r) 
		i = std::tolower (i); 
	
	return (r);
}

// ---
std::string MCHEmul::lowerExcept (const std::string& s)
{
	std::string r = s; 

	bool q = false;
	for (auto& i : r)
	{
		if (i == '\'' || i == '\"')
		{
			q = !q;

			continue;
		}

		if (!q)
			i = std::tolower (i);
	}
	
	return (r);

}

// ---
std::string MCHEmul::noSpaces (const std::string& s)
{ 
	std::string r = s; 
	r.erase (std::remove_if (r.begin (), r.end (), 
		[](unsigned char ch) -> bool { return (std::isspace (ch)); }), r.end ());
	
	return (r);
}

// ---
std::string MCHEmul::replaceAllSpacesPerEquivalent (const std::string& s)
{
	std::string result = s;

	result = MCHEmul::replaceAll (result, "\t", "\\&09");
	result = MCHEmul::replaceAll (result, "\n", "\\&10");
	result = MCHEmul::replaceAll (result, "\v", "\\&11");
	result = MCHEmul::replaceAll (result, "\f", "\\&12");
	result = MCHEmul::replaceAll (result, "\r", "\\&13");
	result = MCHEmul::replaceAll (result, " " , "\\&32");

	return (result);
}

// ---
std::string MCHEmul::restoreSpacesFromEquivalent (const std::string& s)
{
	std::string result = s;

	result = MCHEmul::replaceAll (result, "\\&09", "\t");
	result = MCHEmul::replaceAll (result, "\\&10", "\n");
	result = MCHEmul::replaceAll (result, "\\&11", "\v");
	result = MCHEmul::replaceAll (result, "\\&12", "\f");
	result = MCHEmul::replaceAll (result, "\\&13", "\r");
	result = MCHEmul::replaceAll (result, "\\&32", " " );

	return (result);
}

// ---
std::string MCHEmul::onlyAlphanumeric (const std::string& s)
{
	std::string r = s; 
	r.erase (std::remove_if (r.begin (), r.end (), 
		[](unsigned char ch) -> bool { return (!std::isalnum (ch)); }), r.end ());
	
	return (r);
}

// ---
std::string MCHEmul::noneOf (const std::string& s, const std::string& chrs)
{
	std::string r = s; 
	r.erase (std::remove_if (r.begin (), r.end (), 
		[=](unsigned char chr) -> bool { return (chrs.find (chr) != std::string::npos); }), r.end ());
	
	return (r);
}

// ---
std::string MCHEmul::removeAllFrom (const std::string& s, const MCHEmul::Strings& strs)
{
	std::string r = s;

	size_t pos;
	for (const auto& i : strs)
		if ((pos = r.find (i)) != std::string::npos)
			r.erase (pos, i.length ());

	return (r);
}

// ---
std::string MCHEmul::removeAll0 (const std::string& s)
{
	std::string r = s; 
	r.erase (std::remove_if (r.begin (), r.end (), 
		[](unsigned char ch) -> bool { return (ch == '\0'); }), r.end());
	
	return (r);
}

// ---
std::string MCHEmul::replaceAll (const std::string& s, const std::string& o, const std::string& d)
{
	std::string r = s;

	size_t p = 0;
	while (p != std::string::npos)
	{
		if ((p = r.find (o, p)) == std::string::npos)
			continue;

		r.replace (p, o.length (), d);
	}

	return (r);
}

// ---
std::string MCHEmul::fixLenStr (const std::string& s, size_t sz, bool l, const std::string& as)
{
	std::string result;

	// If the length of the string were bigger than the max size allowed...
	// it would need to cut part of the string...
	if (s.length () > sz)
	{ 
		// if that length of the string were bigger than 3
		if (s.length () > 3)
			result = s.substr (0, sz - 3) + "..."; // ...and indication of the cut is aded...
		else
			result = s.substr (0, sz); // ...otherwise it is not possible!
	}
	// But if it was smaller....
	// ...it would need to add some chars...
	else
	{ 
		// if the requested size were smaller than the max chars to add...
		if (sz < as.length ())
			result = l // ...add them either in the right or in the left, depend on the spefication...
				? as.substr (0, sz - s.length ()) + s
				: s + as.substr (0, sz - s.length ());
		// ...if not, no action would be possible...
		else
			result = s;
	}

	return (result);
}

// ---
MCHEmul::Strings MCHEmul::getElementsFrom (const std::string& txt, unsigned char ch, size_t nE)
{
	std::vector <std::string> result;

	size_t i = 0;
	std::string cpTxt = txt;
	while (cpTxt != std::string ("") && i < nE)
	{
		std::string prt = cpTxt;
		size_t pC = cpTxt.find_first_of (ch);
		prt = cpTxt.substr (0, pC); 
		cpTxt = (pC == std::numeric_limits <size_t>::max ()) ? "" : cpTxt.substr (pC + 1);
		result.emplace_back (MCHEmul::trim (prt));
	}

	// Adjust the size...
	if (result.size () < nE && nE != std::numeric_limits <size_t>::max ()) 
		for (size_t i = result.size (); i < nE; result.emplace_back (std::string ("")), i++);
	if (result.size () > nE && nE != std::numeric_limits <size_t>::max ())
		for (size_t i = result.size (); i > nE; result.pop_back (), i++);

	return (result);
}

// ---
std::string MCHEmul::tableFormat (const MCHEmul::Strings& s, const std::string& sp, size_t l, size_t sb, int ne)
{
	std::string result;

	MCHEmul::Strings cs = s;
	if (ne != -1 && cs.size () > ne)
		cs = MCHEmul::Strings (cs.end () - ne, cs.end ());

	for (size_t i = 0; i < cs.size (); i += sb)
	{
		if (i != 0) result += '\n';
		for (size_t j = i; j < (i + sb) && j < cs.size (); j++)
		{
			if (j != i) result += sp;
			result += cs [j] + 
				((cs [j].length () < l) ? MCHEmul::_SPACES.substr (0, l - cs [j].length ()) : "");
		}
	}

	return (result);
}

// ---
MCHEmul::Strings MCHEmul::convertIntoStrings (int n, char** dt)
{
	MCHEmul::Strings result;
	for (int i = 0; i < n; result.push_back (std::string (dt [i++])));
	return (result);
}

// ---
MCHEmul::Strings MCHEmul::convertIntoStrings (int n, wchar_t** dt)
{
	MCHEmul::Strings result;
	std::wstring_convert <std::codecvt_utf8 <wchar_t>> converter;
	for (int i = 0; i < n; result.push_back (converter.to_bytes (dt [i++])));
	return (result);
}

// ---
std::string MCHEmul::concatenateStrings (const Strings& strs, const std::string& c)
{
	bool f = true;
	std::string result = "";
	for (const auto& i : strs)
	{
		result += ((!f) ? c : "") + i;

		f = false;
	}

	return (result);
}

// ---
bool MCHEmul::validLabel (const std::string& s)
{
	return (s.length () >= 1 && 
			std::find_if (s.begin (), s.end (), 
				[](unsigned ch) -> bool { return (!std::isalnum (ch) && ch != '_'); }) == s.end () &&
			!std::isdigit (s [0]));
}

// ---
bool MCHEmul::validBytesChar (const std::string& s)
{
	if (s.length () < 2 ||
		(s.length () >= 2 && (*s.begin () != '\'' || *s.rbegin () != '\'')))
		return (false);

	std::string sc = s.substr (1, s.length () - 2);
	return (std::find_if (sc.begin (), sc.end (),
			[](unsigned ch) -> bool { return (!std::isalnum (ch)); }) == sc.end ());
}

// ---
bool MCHEmul::validBytesBinary (const std::string& s)
{
	return (s.length () > 1 && 
			std::find_if (++s.begin (), s.end (), 
				[](unsigned ch) -> bool { return (ch != '0' && ch != '1'); }) == s.end () &&
			(s [0] == 'z' || s [0] == 'Z'));
}

// ---
bool MCHEmul::validBytesOctal (const std::string& s)
{ 
	static const std::string oS ("01234567");

	return (s.length () >= 1 && 
			std::find_if (s.begin (), s.end (), 
				[](unsigned ch) -> bool { return (oS.find (ch) == std::string::npos); }) == s.end () &&
			s [0] == '0'); 
}

// ---
bool MCHEmul::validBytesHexadecimal (const std::string& s) 
{ 
	static const std::string oS ("0123456789ABCDEFabcdef");

	return (s.length () > 1 && 
			std::find_if (++s.begin (), s.end (), 
				[](unsigned ch) -> bool { return (oS.find (ch) == std::string::npos); }) == s.end () &&
			s [0] == '$'); 
}

// ---
bool MCHEmul::validBytesDecimal (const std::string& s) 
{ 
	static const std::string oS ("0123456789");

	return (s.length () >= 1 && 
			std::find_if (s.begin (), s.end (), 
				[](unsigned ch) -> bool { return (oS.find (ch) == std::string::npos); }) == s.end () &&
			s [0] != '0' /** It will be octal instead */); 
}

// ---
bool MCHEmul::validBytes (const std::string& s)
{ 
	return (MCHEmul::validBytesChar (s) ||
			MCHEmul::validBytesBinary (s) ||
			MCHEmul::validBytesOctal (s) || 
			MCHEmul::validBytesHexadecimal (s) ||
			MCHEmul::validBytesDecimal (s));
}

// ---
bool MCHEmul::validOperation (const std::string& s)
{
	// If it is a valid label or a valid bytes it would also be a valid operation...
	if (validLabel (s) || validBytes (s)) 
		return (true);

	// Otherwise....could it be an operation?
	// There is no way at that point to try to "create" an operation
	// (the symbols e.g. will depend on the parser, etc...)
	// So it just a matter to try to determine if is potentially one...
	// How?

	static char _SYMBOL = '@';

	// A lambda method to extract the deeper (or rightest) parenthesis within a formula...
	// and to replace than context with a symbol (received as parameter)
	auto extractAndReplaceDeeperParenthesis = [&](std::string& s) -> std::string
	{
		size_t pO = s.find_last_of ("("); if (pO == std::string::npos) return ("");
		size_t pC = s.find_first_of (")", pO); if (pC == std::string::npos) return ("");
		std::string inner = s.substr (pO + 1, pC - pO - 1);
		s = s.substr (0, pO) + _SYMBOL + s.substr (pC + 1);
		return (inner);
	};

	// Replace the content of the parenthesis...
	// Every parenthesis content is stored to be analysed later!
	MCHEmul::Strings pL;
	std::string cS = s; std::string p = "";
	while ((p = extractAndReplaceDeeperParenthesis (cS)) != "") pL.emplace_back (p);
	// ..if there is still one at least, it is not a valis operation...
	if (cS.find ("(") != std::string::npos || cS.find (")") != std::string::npos)
		return (false);
	
	// In the final string, there couldn't be two o more _SYMBOL together...
	// Because It is suppossed that the parenthesis should be each other separated with a operation!
	int ct = 0;
	for (size_t i = 0; i < cS.length () && ct <= 1; 
			ct += (cS [i++] == _SYMBOL) ? 1 : -ct /** The counter back to 0 */);
	if (ct > 1)
		return (false);

	// Reaching this point it could be potentially a valid operation...
	// That is true that nothing regarding the structure of the parameters will have been analyzed!

	return (true);
}

// ---
double MCHEmul::linearInterpolation (double minx, double miny, double maxx, double maxy, double x)
{ 
	// When there is no length in x, the value returned is always the maxy...
	return ((maxx == minx) ? maxy : (miny + ((x - minx) * (maxy - miny) / (maxx - minx)))); 
};

// ---
void MCHEmul::actualizeGlobalTime ()
{
	MCHEmul::ClockTime n = 
		std::chrono::time_point_cast <MCHEmul::ClockDurationType> (MCHEmul::ClockType::now ());
	MCHEmul::_MILLISECONDSPAST = n - MCHEmul::_NOW;
	MCHEmul::_NOW = n;
}
