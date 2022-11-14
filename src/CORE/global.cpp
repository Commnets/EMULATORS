#include <CORE/global.hpp>
#include <algorithm>
#include <cctype>
#include <locale>
#include <codecvt>

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
std::string MCHEmul::lower (const std::string& s)
{
	std::string r = s; 
	for (auto& i : r) 
		i = std::tolower (i); 
	
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
	std::string result = s;

	size_t i = 0;
	while (i < result.length ())
	{
		if (result [i] == 0)
			result = result.substr (0, i) + result.substr (i + 1);
		i++;
	}

	return (result);
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
		result.push_back (MCHEmul::trim (prt));
	}

	// Adjust the size...
	if (result.size () < nE && nE != std::numeric_limits <size_t>::max ()) 
		for (size_t i = result.size (); i < nE; result.push_back (std::string ("")), i++);
	if (result.size () > nE && nE != std::numeric_limits <size_t>::max ())
		for (size_t i = result.size (); i > nE; result.pop_back (), i++);

	return (result);
}

// ---
std::string MCHEmul::tableFormat (const MCHEmul::Strings& s, const std::string& sp, size_t l, size_t sb)
{
	std::string result;

	for (size_t i = 0; i < s.size (); i += sb)
	{
		if (i != 0) result += '\n';
		for (size_t j = i; j < (i + sb) && j < s.size (); j++)
		{
			if (j != i) result += sp;
			result += s [j] + 
				((s [j].length () < l) ? MCHEmul::_SPACES.substr (0, l - s [j].length ()) : "");
		}
	}

	return (result);
}

// ---
MCHEmul::Strings MCHEmul::convertIntoStrings (int n, char** dt)
{
	MCHEmul::Strings result;
	for (int i = 0; i < n; result.push_back (dt [i++]));
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
bool MCHEmul::validLabel (const std::string& s)
{
	return (s.length () >= 1 && 
			std::find_if (s.begin (), s.end (), 
				[](unsigned ch) -> bool { return (!std::isalnum (ch) && ch != '_'); }) == s.end () &&
			!std::isdigit (s [0]));
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
	return (MCHEmul::validBytesOctal (s) || 
			MCHEmul::validBytesHexadecimal (s) ||
			MCHEmul::validBytesDecimal (s));
}

// ---
bool MCHEmul::validFunction (const std::string& s)
{
	static const std::string vF ("><+-*/");

	if (s == "")
		return (true);

	bool result = false;
	std::string::const_iterator i = std::find_if (s.begin (), s.end (),
		[](unsigned char ch) -> bool { return (vF.find (ch) != std::string::npos); });
	if (i == s.end ())
		return (MCHEmul::validLabel (s) || MCHEmul::validBytes (s));

	size_t fP = s.find (*i);
	std::string sL = MCHEmul::trim (s.substr (0, fP));
	std::string sR = MCHEmul::trim (s.substr (fP + 1));
	return (validFunction (sL) && validFunction (sR));
}
