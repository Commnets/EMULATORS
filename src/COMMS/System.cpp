#include <COMMS/System.hpp>
#include <COMMS/IPAddress.hpp>

// ---
bool MCHEmul::CommunicationSystem::processMessagesOn (MCHEmul::Computer* c)
{
	assert (c != nullptr);

	// The system hasn't been initialized well!
	if (!*this)
		return (false);

	std::string str;
	MCHEmul::IPAddress addr;

	// Error receiving?
	if (!_communicationChannel -> receive (str, addr))
		return (false); 

	// Nothing in the content received
	if (str == "")
		return (true); 

	// The received command can't be interpreted, 
	// so an error will be generated...
	MCHEmul::Command* cmd = _commandBuilder -> command (str);
	if (cmd == nullptr)
		return (false); 

	bool result = true;
	MCHEmul::InfoStructure rst;
	if (cmd -> execute (c, rst))
		if (!rst.empty ()) // Is there is something to sent back?
			result = _communicationChannel -> 
				send (MCHEmul::FormatterBuilder::instance () -> formatter (_messageFormatter) -> format (rst), addr);

	delete (cmd);

	return (result);
}
