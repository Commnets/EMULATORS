#include <COMMS/Message.hpp>
#include <CORE/Computer.hpp>

// ---
std::string MCHEmul::CommunicationMessage::toString () const
{
	std::string result;

	result += MCHEmul::CommunicationMessage::_MESSAGEID;
	result += _type;

	bool f = true;
	result += _type;
	for (auto i : _attributes)
	{
		result += ((f) ? "" : ",") + i.first + "=" + i.second;

		f = false;
	}

	return (result);
}

// ---
MCHEmul::Attributes MCHEmul::MessageBuilder::attributesFromStr (const std::string& str) const
{
	MCHEmul::Attributes result;

	std::vector <std::string> attrs = 
		MCHEmul::getElementsFrom (str.substr (1) /** First char defined the type. */, ',');
	for (auto i : attrs)
	{
		std::vector <std::string> attr = MCHEmul::getElementsFrom (i, '=');
		std::string n, v;
		if ((attr.size () == (size_t) 2) && // Only 2 elements!
			((n = MCHEmul::onlyAlphanumeric (attr [0])) == attr [0]) && // Name & Value have to be alphanumeric...
			((v = MCHEmul::onlyAlphanumeric (attr [1])) == attr [1]))
			result [n] = v;
	}

	return (result);
}

// ---
bool MCHEmul::MessageBuilder::verifyStructure (const std::string& str)
{
	if (str == "")
		return (false);

	bool result = true;
	std::vector <std::string> attrs = 
		MCHEmul::getElementsFrom (str.substr (1) /** First char defined the type. */, ',');
	for (std::vector <std::string>::const_iterator i = attrs.begin (); i != attrs.end () && result; i++)
	{
		std::vector <std::string> attr = MCHEmul::getElementsFrom ((*i), '=');
		result = (attr.size () == (size_t) 2) && // Only 2 elements!
			(MCHEmul::onlyAlphanumeric (attr [0]) == attr [0]) && // Name & Value have to be alphanumeric...
			(MCHEmul::onlyAlphanumeric (attr [1]) == attr [1]);
	}

	return (result);
}
