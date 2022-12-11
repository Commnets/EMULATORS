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
	// LoadPrg, LoadBinary and LoadBlocks are special, 
	// as it is related with the emulation and not with the computer...
	std::string cmdLoadPrg ("LOADPRG");
	std::string cmdLoadBinary ("LOADBINARY");
	std::string cmdLoadBlocks ("LOADBLOCKS");
	if (_command.find (cmdLoadPrg) != std::string::npos)
		_outputStream << MCHEmul::FormatterBuilder::instance () ->
			formatter ("CLOADPRG") -> format (loadProgram (_command.substr 
				(_command.find (cmdLoadPrg) + std::string (cmdLoadPrg).length ()))) << std::endl;
	else
	if (_command.find (cmdLoadBinary) != std::string::npos)
		_outputStream << MCHEmul::FormatterBuilder::instance () ->
			formatter ("CLOADBINARY") -> format (loadProgram (_command.substr 
				(_command.find (cmdLoadBinary) + std::string (cmdLoadBinary).length ()))) << std::endl;
	else
	if (_command.find (cmdLoadBlocks) != std::string::npos)
		_outputStream << MCHEmul::FormatterBuilder::instance () ->
			formatter ("CLOADBLOCKS") -> format (loadProgram (_command.substr 
				(_command.find (cmdLoadBlocks) + std::string (cmdLoadBlocks).length ()))) << std::endl;
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

	// The status of the console (hols) is not touched!
}

// ---
MCHEmul::InfoStructure MCHEmul::LocalConsole::loadBinaryFile (const std::string& nP) const
{
	bool e;
	MCHEmul::DataMemoryBlock dM = _emulator -> loadBinaryFile (nP, e);

	MCHEmul::InfoStructure result;
	if (e)
		result.add ("CODE", std::string ("No code loaded"));
	else
	{
		_emulator -> computer () -> setActionForNextCycle (MCHEmul::Computer::_ACTIONSTOP);

		result.add ("CODE", std::string ("Binary file (") + 
			std::to_string (dM._bytes.size ()) + " bytes) loaded at:" + 
			dM._startAddress.asString (MCHEmul::UByte::OutputFormat::_HEXA, '\0', 2));
	}

	return (result);
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
			ln += (fL ? "" : "\n") + ss.str ();

			fL = false;
		}

		result.add ("CODE", std::string ("No code loaded"));
		result.add ("ERRORS", ln);
	}
	else
	{
		_emulator -> computer () -> setActionForNextCycle (MCHEmul::Computer::_ACTIONSTOP);

		for (const auto& i : cL._lines)
		{
			ss << i;
			ln += (fL ? "" : "\n") + ss.str ();

			fL = false;
		}

		result.add ("CODE", ln);
		result.add ("ERRORS", std::string ("No errors"));
	}

	return (result);
}

// ---
MCHEmul::InfoStructure MCHEmul::LocalConsole::loadBlocksFile (const std::string& nP) const
{
	bool e;
	MCHEmul::DataMemoryBlocks mB = _emulator -> loadBlocksFile (nP, e);

	MCHEmul::InfoStructure result;
	if (e)
		result.add ("CODE", std::string ("No code loaded"));
	else
	{
		_emulator -> computer () -> setActionForNextCycle (MCHEmul::Computer::_ACTIONSTOP);

		result.add ("CODE", std::string ("Binary file (") + 
			(mB.empty () ? "0" : std::to_string (mB[0]._bytes.size ())) + " bytes) loaded at:" +
			(mB.empty () ? "-" : mB[0]._startAddress.asString (MCHEmul::UByte::OutputFormat::_HEXA, '\0', 2)));
	}

	return (result);
}
