#include <COMMS/AnsSysCommand.hpp>

const std::string MCHEmul::CommsSystemAnswerCommand::_NAME = "CCOMMSANSWER";

std::string MCHEmul::CommsSystemAnswerCommand::replaceCharsForComms (const std::string& str)
{
	std::string result = str;

	result = MCHEmul::replaceAll (result, " ",	"\\&32");	
	result = MCHEmul::replaceAll (result, "\n",	"\\&13");
	result = MCHEmul::replaceAll (result, "\t",	"\\&09");
	result = MCHEmul::replaceAll (result, "=",	"\\&61");

	return (result);
}

std::string MCHEmul::CommsSystemAnswerCommand::restablishCharsFromComms (const std::string& str) 
{
	std::string result = str;
	
	result = MCHEmul::replaceAll (result, "\\&32", " ");	
	result = MCHEmul::replaceAll (result, "\\&13", "\n");
	result = MCHEmul::replaceAll (result, "\\&09", "\t");
	result = MCHEmul::replaceAll (result, "\\&61", "=");

	return (result);
}

// ---
void MCHEmul::CommsSystemAnswerCommand::executeImpl
	(MCHEmul::CommandExecuter* cE, MCHEmul::Computer* c, MCHEmul::InfoStructure& rst)
{
	rst.add	("RESULT", restablishCharsFromComms ((*_parameters.begin ()).first));
}
