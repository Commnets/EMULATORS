#include <COMMS/System.hpp>
#include <COMMS/IPAddress.hpp>
#include <CORE/Computer.hpp>

// ---
unsigned int MCHEmul::CommunicationSystem::processMessagesOn (MCHEmul::Computer* c)
{
	assert (c != nullptr);

	if (!*this)
		return (0);

	std::string str;
	MCHEmul::IPAddress addr;
	bool rS;
	
	if (!(rS = _communicationChannel -> receive (str, addr)))
		return (0);

	if (str == "")
		return (0);

	MCHEmul::CommunicationMessage* msg = _messageBuilder -> createMessage (str);
	if (msg == nullptr)
		return (0);

	unsigned int result = 0;
	MCHEmul::CommunicationMessage* answ = nullptr;
	if ((result = msg -> executeOn (c, answ)) == 0 /** Meaning continue. */ && answ != nullptr)
		result = _communicationChannel -> send (answ -> toString (), addr);

	delete (answ);
	delete (msg);

	return (result);
}
