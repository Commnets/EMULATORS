#include <COMMS/StdMessages.hpp>

// ---
MCHEmul::CommunicationMessage* MCHEmul::StandardMessageBuilder::createMessage (const std::string& str)
{
	if (!verifyStructure (str))
		return (nullptr); // No message possible...

	MCHEmul::CommunicationMessage* result = nullptr;
	MCHEmul::Attributes attrs = attributesFromStr (std::stoi (str.substr (1, 1)), str.substr (2)); 
	switch (str [0])
	{
		case 'A':
			result = new MCHEmul::GetRegisterStatusMessage (attrs);
			break;

		case 'B':
			result = new MCHEmul::GetMemoryDataMessage (attrs);
			break;

		default:
			assert (false);
	}

	return (result);
}

// ---
bool MCHEmul::GetRegisterStatusMessage::executeOn (MCHEmul::Computer*)
{
	return (true);
}

// ---
bool MCHEmul::GetMemoryDataMessage::executeOn (MCHEmul::Computer*)
{
	return (true);
}
