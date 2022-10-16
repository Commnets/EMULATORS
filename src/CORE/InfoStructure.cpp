#include <CORE/InfoStructure.hpp>

// ---
bool MCHEmul::InfoStructure::existsAttribute (const std::string& n) const
{
	bool result = false;

	size_t posp = n.find ('.');
	if (posp == std::string::npos)
		result = (_attributes.find (n) != _attributes.end ());
	else
	{
		MCHEmul::InfoStructures::const_iterator i = _infoStructures.find (n.substr (0, posp));
		if (i != _infoStructures.end ())
			result = i -> second.existsAttribute (n.substr (posp + 1));
	}

	return (result);
}

// ---
std::string MCHEmul::InfoStructure::attribute (const std::string& n) const
{
	std::string result = "";

	size_t posp = n.find ('.');
	if (posp == std::string::npos)
	{
		MCHEmul::Attributes::const_iterator i = _attributes.find (n);
		if (i != _attributes.end ())
			result = i -> second;
	}
	else
	{
		MCHEmul::InfoStructures::const_iterator i = _infoStructures.find (n.substr (0, posp));
		if (i != _infoStructures.end ())
			result = i -> second.attribute (n.substr (posp + 1));
	}

	return (result);
}

// ---
bool MCHEmul::InfoStructure::existsInfoStructure (const std::string& n) const
{
	bool result = false;

	size_t posp = n.find ('.');
	if (posp == std::string::npos)
		result = (_infoStructures.find (n) != _infoStructures.end ());
	else
	{
		MCHEmul::InfoStructures::const_iterator i = _infoStructures.find (n.substr (0, posp));
		if (i != _infoStructures.end ())
			result = i -> second.existsInfoStructure (n.substr (posp + 1));
	}

	return (result);
}

// ---
MCHEmul::InfoStructure MCHEmul::InfoStructure::infoStructure (const std::string& n) const
{
	MCHEmul::InfoStructure result;

	size_t posp = n.find ('.');
	if (posp == std::string::npos)
	{
		MCHEmul::InfoStructures::const_iterator i = _infoStructures.find (n);
		if (i != _infoStructures.end ())
			result = i -> second;
	}
	else
	{
		MCHEmul::InfoStructures::const_iterator i = _infoStructures.find (n.substr (0, posp));
		if (i != _infoStructures.end ())
			result = i -> second.infoStructure (n.substr (posp + 1));
	}

	return (result);
}

// ---
void MCHEmul::InfoStructure::remove (const std::string& an)
{
	size_t posp = an.find (".");
	if (posp == std::string::npos)
	{
		MCHEmul::Attributes::const_iterator i = _attributes.find (an);
		if (i != _attributes.end ())
			_attributes.erase (i);
		else
		{
			MCHEmul::InfoStructures::const_iterator j = _infoStructures.find (an);
			if (j != _infoStructures.end ())
				_infoStructures.erase (j);
		}
	}
	else
	{
		MCHEmul::InfoStructures::const_iterator i = _infoStructures.find (an.substr (0, posp)); // Recursive...
		if (i != _infoStructures.end ())
			remove (an.substr (posp + 1));
	}
}

// ---
std::string MCHEmul::InfoStructure::asString (const std::string& s, const std::string& sa, 
	const std::string& bI, const std::string& bF, bool pF) const
{
	std::string result = "";

	size_t ct = 0;
	for (const auto& i : _attributes)
		result += ((ct++ == 0) ? "" : s) + (pF ? (i.first + sa) : "") + i.second;
	for (const auto& i : _infoStructures)
		result += ((ct++ == 0) ? "" : s) + bI + (pF ? (i.first + sa) : "") + i.second.asString (s, sa, bI, bF) + bF;

	return (result);
}

// ---
MCHEmul::InfoStructure MCHEmul::InfoStructure::fromStr (const std::string& str, char s, char sa, char bI, char bF)
{
	MCHEmul::InfoStructure result;

	MCHEmul::Strings elmts = MCHEmul::getElementsFrom (str, s);
	for (const auto& i : elmts)
	{
		size_t saP = i.find (sa);
		if (saP == std::string::npos)
			continue; // There is an error...
					  // but let's try to recover from it going to the next one!

		std::string n = i.substr (0, saP);
		std::string v = i.substr (saP + 1);
		if (v [0] == bI && v [v.length () - 1] == bF) result._infoStructures [n] = MCHEmul::InfoStructure::fromStr (v, s, sa, bI, bF);
		else result._attributes [n] = v;
	}

	return (result);
}
