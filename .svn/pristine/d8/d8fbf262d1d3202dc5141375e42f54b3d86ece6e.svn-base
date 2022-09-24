#include <COMMS/StdMessages.hpp>

// ---
MCHEmul::CommunicationMessage* MCHEmul::StandardMessageBuilder::createMessage (const std::string& str)
{
	if (!verifyStructure (str))
		return (nullptr); // No message possible...

	// The position 0 es the on indicating theer is a MCHEmul message
	MCHEmul::CommunicationMessage* result = nullptr;
	MCHEmul::Attributes attrs = attributesFromStr (str.substr (2)); 
	switch (str [1])
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
unsigned int MCHEmul::GetRegisterStatusMessage::executeOn (MCHEmul::Computer* c, MCHEmul::CommunicationMessage*& ans)
{
	// TODO

	return (0);
}

// ---
unsigned int MCHEmul::GetMemoryDataMessage::executeOn (MCHEmul::Computer*, MCHEmul::CommunicationMessage*& ans)
{
	// TODO

	return (0);
}
