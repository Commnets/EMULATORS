#include <COMMS/System.hpp>
#include <COMMS/IPAddress.hpp>
#include <CORE/Computer.hpp>

// ---
unsigned int MCHEmul::CommunicationSystem::processMessagesOn (MCHEmul::Computer* c)
{
	assert (c != nullptr);

	if (!*this)
		return (MCHEmul::Computer::_ACTIONNOTHING);

	std::string str;
	MCHEmul::IPAddress addr;
	bool rS;
	
	if (!(rS = _communicationChannel -> receive (str, addr)))
		return (MCHEmul::Computer::_ACTIONNOTHING);

	if (str == "")
		return (MCHEmul::Computer::_ACTIONNOTHING);

	MCHEmul::Command* cmd = _commandBuilder -> command (str);
	if (cmd == nullptr)
		return (MCHEmul::Computer::_ACTIONNOTHING);

	unsigned int result = 0;
	MCHEmul::InfoStructure rst;
	if (cmd -> execute (c, rst))
		if (!rst.empty ())
			result = _communicationChannel -> 
				send (MCHEmul::FormatterBuilder::instance () -> formatter (_messageFormatter) -> format (rst), addr);

	delete (cmd);

	return (result);
}
