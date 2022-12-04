#include <COMMS/AnsSysCommand.hpp>

const std::string MCHEmul::CommsSystemAnswerCommand::_NAME = "CCOMMSANSWER";
const std::string MCHEmul::CommsSystemAnswerCommand::_PARFORMATTER = "FMTTER";
const std::string MCHEmul::CommsSystemAnswerCommand::_PARORIGINALCMMD = "CMMD";
const std::string MCHEmul::CommsSystemAnswerCommand::_PARANSWER = "ANS";

// ---
std::string MCHEmul::CommsSystemAnswerCommand::replaceCharsForComms (const std::string& str)
{
	std::string result = str;

	result = MCHEmul::replaceAll (result, "\t", "\\&09");
	result = MCHEmul::replaceAll (result, "\n", "\\&10");
	result = MCHEmul::replaceAll (result, "\v", "\\&11");
	result = MCHEmul::replaceAll (result, "\f", "\\&12");
	result = MCHEmul::replaceAll (result, "\r", "\\&13");
	result = MCHEmul::replaceAll (result, " " , "\\&32");	
	result = MCHEmul::replaceAll (result, "=" , "\\&61");

	return (result);
}

// ---
std::string MCHEmul::CommsSystemAnswerCommand::restablishCharsFromComms (const std::string& str) 
{
	std::string result = str;

	result = MCHEmul::replaceAll (result, "\\&09", "\t");
	result = MCHEmul::replaceAll (result, "\\&10", "\n");
	result = MCHEmul::replaceAll (result, "\\&11", "\v");
	result = MCHEmul::replaceAll (result, "\\&12", "\f");
	result = MCHEmul::replaceAll (result, "\\&13", "\r");
	result = MCHEmul::replaceAll (result, "\\&32", " " );	
	result = MCHEmul::replaceAll (result, "\\&61", "=" );

	return (result);
}

// ---
void MCHEmul::CommsSystemAnswerCommand::executeImpl
	(MCHEmul::CommandExecuter* cE, MCHEmul::Computer* c, MCHEmul::InfoStructure& rst)
{
	_lastCommandAnswerReceived = parameter (_PARORIGINALCMMD);
	rst = MCHEmul::FormatterBuilder::instance () -> 
		formatter (parameter (_PARFORMATTER)) -> unFormat (restablishCharsFromComms (parameter (_PARANSWER)));
}
