#include <CORE/global.hpp>
#include <algorithm>
#include <cctype>

// ---
std::ostream& MCHEmul::operator << (std::ostream& o, const MCHEmul::Attributes& attrs)
{ 
	bool f = true;
	for (auto i : attrs)
	{
		if (!f)
			o << std::endl;
		o << i.first << ":" << i.second;

		f = false;
	}

	return (o);
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
std::string MCHEmul::removeAllFrom (const std::string& s, std::vector <std::string>& strs)
{
	std::string result = s;

	size_t pos;
	for (auto i : strs)
		if ((pos = result.find (i)) != std::string::npos)
			result.erase (pos, i.length ());

	return (result);
}

// ---
std::vector <std::string> MCHEmul::getElementsFrom (const std::string& txt, unsigned char ch, size_t nE)
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
bool MCHEmul::validLabel (const std::string& s)
{
	return (s.length () >= 1 && 
			std::find_if (s.begin (), s.end (), 
				[](unsigned ch) -> bool { return (!std::isalnum (ch)); }) == s.end () &&
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
