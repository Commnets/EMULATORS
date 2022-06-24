#include <COMMS/StdMessages.hpp>

// ---
MCHEmul::CommunicationMessage* MCHEmul::StandardMessageBuilder::createMessage (const std::string& str)
{
	if (!verifyStructure (str))
		return (nullptr); // No message possible...

	MCHEmul::CommunicationMessage* result = nullptr;
	MCHEmul::Attributes attrs = attributesFromStr (str.substr (1)); 
	switch (str [0])
	{
		case MCHEmul::GetRegisterStatusMessage::_ID:
			result = new MCHEmul::GetRegisterStatusMessage (attrs);
			break;

		case MCHEmul::GetMemoryDataMessage::_ID:
			result = new MCHEmul::GetMemoryDataMessage (attrs);
			break;

		default:
			assert (false);
	}

	return (result);
}

// ---
bool MCHEmul::GetRegisterStatusMessage::executeOn (MCHEmul::Computer* c, MCHEmul::CommunicationMessage*& ans)
{
	return (true);
}

// ---
bool MCHEmul::GetMemoryDataMessage::executeOn (MCHEmul::Computer*, MCHEmul::CommunicationMessage*& ans)
{
	return (true);
}
