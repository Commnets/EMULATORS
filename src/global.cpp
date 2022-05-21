#include <global.hpp>
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

size_t MCHEmul::isIn(const std::string& s1, const std::string& s2)
{ 
	size_t counter = 0; size_t i = 0;
	for(; i < s1.length (); i++)
	{
		if (counter == s2.length ()) break;
		if (s2 [counter] == s1 [i]) counter++;
		else
		{
			if (counter > 0) i -= counter;
			counter = 0;
		}
	}

	return (counter < s2.length () ? std::string::npos : (i - counter));
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
