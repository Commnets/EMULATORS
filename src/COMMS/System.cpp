#include <COMMS/System.hpp>
#include <COMMS/IPAddress.hpp>

// ---
bool MCHEmul::CommunicationSystem::processMessagesOn (MCHEmul::Computer* c)
{
	MCHEmul::CommandExecuter::executePendingCommands ();

	assert (c != nullptr);

	// The system hasn't been initialized well!
	if (!*this)
		return (false);

	std::string str;

	// Error sending the pending messages?
	if (_communicationChannel -> sendPendingMessages ())
		return (false);
	// Error receiving new messages?
	if (!_communicationChannel -> receive (str, _lastSender /** To store ho send the message. */))
		return (false); 

	// Nothing in the content received
	if (str == "")
		return (true); // Nothing received, but executed...

	// The received command can't be interpreted, 
	// so an error will be generated...
	str = MCHEmul::upper (MCHEmul::trim (str));
	MCHEmul::Command* cmd = commandBuilder () -> command (str);
	if (cmd == nullptr)
		return (false); 

	return (executeCommandNow (cmd, c));
}

// ---
void MCHEmul::CommunicationSystem::manageAnswer (MCHEmul::Command* c, const MCHEmul::InfoStructure& rst)
{
	if (!rst.empty ())
		_communicationChannel ->
			send (MCHEmul::FormatterBuilder::instance () -> formatter (_messageFormatter) -> format (rst), _lastSender);
}
