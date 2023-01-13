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
	std::string cmdDecompileMemory ("DECOMPILE");
	if (_command.find (cmdLoadPrg) != std::string::npos)
		_outputStream << MCHEmul::FormatterBuilder::instance () ->
			formatter ("CLOADPRG") -> format (loadProgram (MCHEmul::trim (_command.substr 
				(_command.find (cmdLoadPrg) + std::string (cmdLoadPrg).length ())))) << std::endl;
	else
	if (_command.find (cmdLoadBinary) != std::string::npos)
		_outputStream << MCHEmul::FormatterBuilder::instance () ->
			formatter ("CLOADBINARY") -> format (loadBinaryFile (MCHEmul::trim (_command.substr 
				(_command.find (cmdLoadBinary) + std::string (cmdLoadBinary).length ())))) << std::endl;
	else
	if (_command.find (cmdLoadBlocks) != std::string::npos)
		_outputStream << MCHEmul::FormatterBuilder::instance () ->
			formatter ("CLOADBLOCKS") -> format (loadBlocksFile (MCHEmul::trim (_command.substr 
				(_command.find (cmdLoadBlocks) + std::string (cmdLoadBlocks).length ())))) << std::endl;
	else
	if (_command.find (cmdDecompileMemory) != std::string::npos)
		_outputStream << MCHEmul::FormatterBuilder::instance () ->
			formatter ("CDECOMPILE") -> format (decompileMemory (MCHEmul::trim (_command.substr 
				(_command.find (cmdDecompileMemory) + std::string (cmdDecompileMemory).length ())))) << std::endl;
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
	bool exit = !_emulator -> runCycle ();

	if (exit && _emulator -> computer () -> restartAfterExit ())
	{
		// To continue or not will really depend on the intialization process...
		exit = !_emulator -> computer () -> restart ();
		// The instruction will not longer be executed. Just once!
		_emulator -> computer () -> setRestartAfterExit (false /** level 0 back. */);
	}

	return (exit);
}

// ---
MCHEmul::InfoStructure MCHEmul::LocalConsole::loadBinaryFile (const std::string& nP) const
{
	bool e;
	MCHEmul::DataMemoryBlock dM = _emulator -> loadBinaryFile (nP, e);

	MCHEmul::InfoStructure result;
	if (e)
		return (result);
	else
		result.add ("CODE", std::string ("Binary file (") + 
			std::to_string (dM.bytes ().size ()) + " bytes) loaded at:" +
			dM.startAddress ().asString (MCHEmul::UByte::OutputFormat::_HEXA, '\0', 2));

	return (result);
}

// ---
MCHEmul::InfoStructure MCHEmul::LocalConsole::loadProgram (const std::string& nP) const
{
	MCHEmul::Assembler::Errors e;
	MCHEmul::Assembler::ByteCode cL = _emulator -> loadProgram (nP, e);

	MCHEmul::InfoStructure result;
	if (!e.empty ())
	{
		MCHEmul::InfoStructure errs;
		for (size_t i = 0; i < e.size (); i++)
			errs.add (std::to_string (i), e [i].asString ());
		result.add ("ERRORS", errs);
	}
	else
	{
		MCHEmul::InfoStructure lns;
		for (size_t i = 0; i < cL._lines.size (); i++)
		{ 
			std::string iP = std::to_string (i);
			iP = MCHEmul::_CEROS.substr (0, 5 - iP.length ()) + iP;
			lns.add (iP, cL._lines [i].asString (MCHEmul::UByte::OutputFormat::_HEXA, ' ', 2));
		}

		result.add ("CODELINES", lns);
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
		return (result);
	else
		result.add ("CODE", std::string ("Binary file (") + 
			(mB.empty () ? "0" : std::to_string (mB [0].bytes ().size ())) + " bytes) loaded at:" +
			(mB.empty () ? "-" : mB [0].startAddress ().asString (MCHEmul::UByte::OutputFormat::_HEXA, '\0', 2)));

	return (result);
}

// ---
MCHEmul::InfoStructure MCHEmul::LocalConsole::decompileMemory (const std::string& prms)
{
	MCHEmul::InfoStructure result;

	size_t pS = MCHEmul::firstSpaceIn (prms);
	if (pS == std::string::npos)
		return (result);

	MCHEmul::Address iA = MCHEmul::Address::fromStr (MCHEmul::trim (prms.substr (0, pS)));
	if (iA > _emulator -> computer () -> cpu () -> architecture ().longestAddressPossible ())
		return (result);

	size_t nB = (size_t) std::atoi (MCHEmul::trim (prms.substr (pS)).c_str ());
	size_t nBA = _emulator -> computer () -> cpu () -> architecture ().numberBytes ();
	if (iA.size () > nBA || nB > ((size_t) 1 << (MCHEmul::UByte::sizeBits () * nBA)))
		return (result);

	MCHEmul::Assembler::ByteCode cL = MCHEmul::Assembler::ByteCode::createFromMemory 
		(iA, (unsigned int) nB, _emulator -> computer () -> memory (), _emulator -> computer ());
	MCHEmul::InfoStructure lns;
	for (size_t i = 0; i < cL._lines.size (); i++)
	{ 
		std::string iP = std::to_string (i);
		iP = MCHEmul::_CEROS.substr (0, 5 - iP.length ()) + iP;
		lns.add (iP, cL._lines [i].asString (MCHEmul::UByte::OutputFormat::_HEXA, ' ', 2));
	}

	result.add ("CODELINES", lns);
	return (result);
}
