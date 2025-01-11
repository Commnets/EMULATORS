#include <SINCLAIR/Commands.hpp>
#include <SINCLAIR/Computer.hpp>

const std::string SINCLAIR::SystemVariablesCommand::_NAME = "CSYSVARS";
const std::string SINCLAIR::SystemVariableCommand::_NAME = "CSYSVAR";

// ---
void SINCLAIR::SystemVariablesCommand::executeImpl (MCHEmul::CommandExecuter* cE,
	MCHEmul::Computer* c, MCHEmul::InfoStructure& rst)
{
	SINCLAIR::Computer* sc = dynamic_cast <SINCLAIR::Computer*> (c);

	// Only with a valid computer, but also a SINCLAIR one. 
	if (sc == nullptr)
		return;

	rst.add ("SYSVARS", dynamic_cast <SINCLAIR::Computer*> (c) -> 
		getSystemVariablesAsInfoStructure (c -> memory ()));
}

// ---
void SINCLAIR::SystemVariableCommand::executeImpl (MCHEmul::CommandExecuter* cE,
	MCHEmul::Computer* c, MCHEmul::InfoStructure& rst)
{
	SINCLAIR::Computer* sc = dynamic_cast <SINCLAIR::Computer*> (c);

	// Only with a valid computer, but also a SINCLAIR one. 
	if (sc == nullptr)
		return;

	std::string nV = parameter ("00");
	if (sc -> existSystemVariable (nV))
		rst.add ("SysVar", sc -> systemVariable (nV).getInfoStructure (c -> memory ()));
	else
		rst.add ("ERROR", "Variable " + nV + " not defined");
}
