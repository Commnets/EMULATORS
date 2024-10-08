#include <ASSEMBLER/Error.hpp>

// ---
std::string MCHEmul::Assembler::Error::asString () const
{
	static std::map <MCHEmul::Assembler::ErrorType, std::string> _ERRORTEXT =
	{
		{ MCHEmul::Assembler::ErrorType::_NOERROR, "No error" },
		{ MCHEmul::Assembler::ErrorType::_MACROBADDEFINED, "Not valid Macro" },
		{ MCHEmul::Assembler::ErrorType::_MACRONOTDEFINED, "Macro not defined" },
		{ MCHEmul::Assembler::ErrorType::_DUPLICATEMACRO, "Duplicated Macro" },
		{ MCHEmul::Assembler::ErrorType::_BADARGUMENTFUNCTION, "Operation argument invalid" },
		{ MCHEmul::Assembler::ErrorType::_DIVISIONBYCERO, "Operation division by 0" },
		{ MCHEmul::Assembler::ErrorType::_TEMPLATENOTVALID, "Code Template not valid" },
		{ MCHEmul::Assembler::ErrorType::_TEMPLATENOTEFINED, "Code Template not defined" },
		{ MCHEmul::Assembler::ErrorType::_TEMPLATENOTCALCULATED, "Code Template not calculated" },
		{ MCHEmul::Assembler::ErrorType::_DUPLICATECODETEMPLATE, "Duplicated Code Template" },
		{ MCHEmul::Assembler::ErrorType::_BINARYFILENOTVALID, "Binary file nor valid" },
		{ MCHEmul::Assembler::ErrorType::_LABELNOTVALID, "Not valid Label" },
		{ MCHEmul::Assembler::ErrorType::_LABELNOTDEFINED, "Label not defined" },
		{ MCHEmul::Assembler::ErrorType::_BYTESNOTVALID, "Bytes definition not valid" },
		{ MCHEmul::Assembler::ErrorType::_INSTRUCTIONNOTVALID, "Instruction definition not valid" },
		{ MCHEmul::Assembler::ErrorType::_INSTRUCTIONNOTDEFINED, "Instruction not defined" },
		{ MCHEmul::Assembler::ErrorType::_STARTINGPOINTNOTVALID, "Not valid Starting Point" },
		{ MCHEmul::Assembler::ErrorType::_GRAMARELEMENTNOTVALID, "Not valid Grammatical Element" },
		{ MCHEmul::Assembler::ErrorType::_STARTINGPOINTNOTDEFINED, "Starting Point not defined yet" },
		{ MCHEmul::Assembler::ErrorType::_SEMANTICERROR, "Semantic wrong" },
		{ MCHEmul::Assembler::ErrorType::_FILEEMPTY, "Input file empty" },
		{ MCHEmul::Assembler::ErrorType::_PARSERNOTFOUND, "Intruction Parser not defined" }
	};

	std::string result = "Error:" 
		+ _ERRORTEXT [_type] 
		+ " at line " + std::to_string (_line) + ((_detail != "") ? "(" + _detail + ")" : "") 
		+ " in file " + _file;
	return (result);
}
