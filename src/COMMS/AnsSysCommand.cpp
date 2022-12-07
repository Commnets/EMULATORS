#include <COMMS/AnsSysCommand.hpp>

const std::string MCHEmul::CommsSystemAnswerCommand::_NAME = "CCOMMSANSWER";
const std::string MCHEmul::CommsSystemAnswerCommand::_PARFORMATTER = "FMTTER";
const std::string MCHEmul::CommsSystemAnswerCommand::_PARORIGINALCMMD = "CMMD";
const std::string MCHEmul::CommsSystemAnswerCommand::_PARANSWER = "ANS";

// ---
std::string MCHEmul::CommsSystemAnswerCommand::replaceCharsForComms (const std::string& str)
{
	std::string result = str;

	result = replaceAllSpacesPerEquivalent (result);
	result = MCHEmul::replaceAll (result, "=" , "\\&61");

	return (result);
}

// ---
std::string MCHEmul::CommsSystemAnswerCommand::restablishCharsFromComms (const std::string& str) 
{
	std::string result = str;

	result = restoreSpacesFromEquivalent (result);
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
