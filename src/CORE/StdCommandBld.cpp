#include <CORE/StdCommandBld.hpp>
#include <CORE/StdCommands.hpp>

// ---
MCHEmul::Command* MCHEmul::StandardCommandBuilder::createEmptyCommand (const std::string& cmdName) const
{
	MCHEmul::Command* result = nullptr;

	if (cmdName == "STATUS")
		result = new MCHEmul::StatusRegisterCommand ();

	return (result);
}
