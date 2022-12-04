#include <CORE/JSONFormatter.hpp>

// ---
std::string MCHEmul::JSONFormatter::format (const MCHEmul::InfoStructure& a) const
{
	std::string result = "{";

	unsigned int idt = 0;

	for (const auto& i : a.attributes ())
		result += "\n\"" + i.first + "\":\"" + MCHEmul::replaceAll (i.second, "\"", "\\&34") + "\"";
	for (const auto& i : a.infoStructures ())
		result += "\n\"" + i.first + "\":" + format (i.second); // Recursive...

	result += "\n}";

	return (result);
}

// ---
MCHEmul::InfoStructure MCHEmul::JSONFormatter::unFormat (const std::string& str) const
{
	MCHEmul::InfoStructure result;

	std::string cStr = MCHEmul::noSpaces (str);
	if (cStr [0] != '{' && cStr [cStr.length () - 1] != '}')
		return (result);
	cStr = cStr.substr (1, cStr.length () - 2);

	size_t p = 0;
	while (p != cStr.length ())
	{
		size_t pI = cStr.find (':', p);
		if (pI == std::string::npos || pI == cStr.length () || pI < (p + 2) ||
			(cStr [p] != '"' && cStr [pI - 1] != '"') || // The name of the attribute between quotes...
			(cStr [pI + 1] != '{' && cStr [pI + 1] != '"')) // The parameter mas start with either a parenthesys or an equal...
			break; // otherwise a mistake...

		if (cStr [pI + 1] == '"')
		{
			size_t pF = pI;
			if ((pI + 2) >= cStr.length () ||
				(pF = cStr.find ('"', pI + 2)) == std::string::npos)
				break; // A mistake...

			// Add the attribute made up of a name and the value without quotes...
			result.add (cStr.substr (p + 1, pI - p - 2), 
				MCHEmul::replaceAll (cStr.substr (pI + 2, pF - pI - 2), "\\&34", "\""));

			p = pF;
		}
		else
		{
			size_t pF = pI + 2;
			unsigned int nP = 1;
			for (;nP != 0 && pF < cStr.length (); pF++)
			{ 
				if (cStr [pF] == '{') nP++;
				if (cStr [pF] == '}') nP--;
			}

			if (nP != 0)
				break; // Mistake...

			// Add an internal attribute...
			result.add (cStr.substr (p + 1, pI - p - 2), 
				unFormat (cStr.substr (pI + 1, pF - pI - 1)) /** recursive. */);

			p = pF - 1;
		}

		p++;
	}

	return (result);
}
