#include <CONSOLE/LocalConsole.hpp>
#include <sstream>

// ---
MCHEmul::LocalConsole::LocalConsole (MCHEmul::Emulator* e, MCHEmul::CommandBuilder* cB, 
		MCHEmul::ConsoleKeys* k, const std::string& cF, std::ostream& oS, size_t cK)
	: MCHEmul::Console (cB, k, cF, oS, cK),
	  _emulator (e)
{ 
	assert (_emulator != nullptr); 
}

// ---
void MCHEmul::LocalConsole::run ()
{
	/** Print out basic info about the computer managed. */
	_outputStream << "---- CPU ---" << std::endl
		<< *_emulator -> computer () -> cpu () << std::endl;
	_outputStream << "---- STACK -" << std::endl
		<< *_emulator -> computer () -> memory () -> stack () << std::endl << std::endl;

	MCHEmul::Console::run ();
}

// ---
void MCHEmul::LocalConsole::createAndExecuteCommand ()
{
	// LoadPrg is special, 
	// as it is related with the emulation and not with the computer...
	std::string cmdLoad ("LOADPRG");
	if (_command.find (cmdLoad) != std::string::npos)
		_outputStream << MCHEmul::FormatterBuilder::instance () ->
			formatter ("CLOADPRG") -> format (loadProgram (_command.substr 
				(_command.find (cmdLoad) + std::string (cmdLoad).length ()))) << std::endl;
	else
	{
		MCHEmul::Command* cmd = commandBuilder () -> command (_command);
		if (cmd == nullptr) _outputStream << _command << ":" << _commandDoesnExitTxt << std::endl;
		else executeCommandNow (cmd, _emulator -> computer ());
	}
}

// ---
bool MCHEmul::LocalConsole::runPerCycle ()
{
	return (!_emulator -> runCycle ());
}

// ---
MCHEmul::InfoStructure MCHEmul::LocalConsole::loadProgram (const std::string& nP) const
{
	MCHEmul::Assembler::Errors e;
	MCHEmul::Assembler::ByteCode cL = _emulator -> loadProgram (nP, e);

	MCHEmul::InfoStructure result;

	bool fL = true;
	std::string ln;
	std::stringstream ss; 
	if (!e.empty ())
	{
		for (const auto& i : e)
		{
			std::stringstream ss; ss << i;
			ln += (fL ? "" : "\n") + ss.str();

			fL = false;
		}

		result.add ("CODE", std::string ("No code loaded"));
		result.add ("ERRORS", ln);
	}
	else
	{
		_emulator -> computer () -> setActionForNextCycle(MCHEmul::Computer::_ACTIONSTOP);

		for (const auto& i : cL._lines)
		{
			ss << i;
			ln += (fL ? "" : "\n") + ss.str();

			fL = false;
		}

		result.add ("CODE", ln);
		result.add ("ERRORS", std::string ("No errors"));
	}

	return (result);
}

