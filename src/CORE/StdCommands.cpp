#include <CORE/StdCommands.hpp>
#include <CORE/Computer.hpp>

// ---
void MCHEmul::StatusRegisterCommand::executeImpl (MCHEmul::Computer* c,  MCHEmul::Attributes& rst)
{
	rst.insert (std::pair <std::string, std::string> 
		("RESULT", c -> cpu () -> statusRegister ().asString ()));
}
