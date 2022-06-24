#include <COMMS/System.hpp>
#include <COMMS/IPAddress.hpp>
#include <CORE/Computer.hpp>

// ---
bool MCHEmul::CommunicationSystem::processMessagesOn (MCHEmul::Computer* c)
{
	assert (c != nullptr);

	if (!*this)
		return (false);

	std::string str;
	MCHEmul::IPAddress addr;
	bool rS;
	
	if (!(rS = _communicationChannel -> receive (str, addr)))
		return (false);
	if (str == "")
		return (true);

	MCHEmul::CommunicationMessage* msg = _messageBuilder -> createMessage (str);
	if (msg == nullptr)
		return (false);

	bool result = true;
	MCHEmul::CommunicationMessage* answ = nullptr;
	if ((result = msg -> executeOn (c, answ)) && answ != nullptr)
		result = _communicationChannel -> send (answ -> toString (), addr);

	delete (answ);
	delete (msg);

	return (result);
}
