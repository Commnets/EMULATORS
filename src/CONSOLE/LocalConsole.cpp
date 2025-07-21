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
	outputStream () << "---- CPU ---" << std::endl
		<< *_emulator -> computer () -> cpu () << std::endl;
	outputStream () << "---- STACK -" << std::endl
		<< *_emulator -> computer () -> memory () -> stack () << std::endl << std::endl;

	MCHEmul::Console::run ();

	// When exiting a dump is done if either there was an error 
	// or the right debug level was selected...
	if (_emulator -> debugLevel () >= MCHEmul::_DUMPATEXIT || 
		_emulator -> computer () -> error () != MCHEmul::_NOERROR)
		_LOG (MCHEmul::removeAll0 (MCHEmul::FormatterBuilder::instance () -> // To the log file when there is a big error...
			formatter ("Computer") -> format (_emulator -> computer () -> getInfoStructure ())));
}

// ---
void MCHEmul::LocalConsole::createAndExecuteCommand ()
{
	auto nameFor = [](const std::string& cmd) -> std::string 
		{ std::string::const_iterator i = 
			std::find_if (cmd.begin (), cmd.end (), [](char c) -> bool { return (std::isspace (c)); });
		  return ((i == cmd.end ()) ? cmd : MCHEmul::trim (cmd.substr (0, cmd.find (*i)))); };
	auto prmsFor = [](const std::string& cmd, const std::string& name) -> std::string
		{ return (MCHEmul::trim (cmd.substr (cmd.find (name) + std::string (name).length ()))); };

	// LoadPrg, LoadBinary and LoadBlocks... are special, 
	// as it is related with the emulation and not with the computer...
	std::string cmdLoadPrg ("LOADPRG");
	std::string cmdLoadBinary ("LOADBINARY");
	std::string cmdLoadBlocks ("LOADBLOCKS");
	std::string cmdDecompileMemory ("DECOMPILE");
	std::string cmdConnectPeripheral ("CONNECTPER");
	std::string cmdDisconnectPeripherals ("DISCONNECTPERS");
	std::string cmdLoadPeripheralData ("LOADPERDATA");
	std::string cmdEmptyPeripheralData ("EMPTYPERDATA");
	std::string cmdSavePeripheralData ("SAVEPERDATA");

	std::string cmdName = nameFor (_command);

	if (cmdName == cmdLoadPrg)
		outputStream () << MCHEmul::FormatterBuilder::instance () ->
			formatter ("CLOADPRG") -> format (loadProgram (prmsFor (_command, cmdLoadPrg))) << std::endl;
	else
	if (cmdName == cmdLoadBinary)
		outputStream () << MCHEmul::FormatterBuilder::instance () ->
			formatter ("C" + cmdLoadBinary) -> format (loadBinaryFile (prmsFor (_command, cmdLoadBinary))) << std::endl;
	else
	if (cmdName == cmdLoadBlocks)
		outputStream () << MCHEmul::FormatterBuilder::instance () ->
			formatter ("C" + cmdLoadBlocks) -> format (loadBlocksFile (prmsFor (_command, cmdLoadBlocks))) << std::endl;
	else
	if (cmdName == cmdDecompileMemory)
		outputStream () << MCHEmul::FormatterBuilder::instance () ->
			formatter ("C" + cmdDecompileMemory) -> format (decompileMemory 
				(prmsFor (_command, cmdDecompileMemory))) << std::endl;
	else
	if (cmdName == cmdConnectPeripheral)
		outputStream () << MCHEmul::FormatterBuilder::instance () ->
			formatter ("C" + cmdConnectPeripheral) -> format (connectPeripheral
				(prmsFor (_command, cmdConnectPeripheral))) << std::endl;
	else
	if (cmdName == cmdDisconnectPeripherals)
		outputStream () << MCHEmul::FormatterBuilder::instance () ->
			formatter ("C" + cmdDisconnectPeripherals) -> format (disconnectPeripherals
				(prmsFor (_command, cmdDisconnectPeripherals))) << std::endl;
	else
	if (cmdName == cmdLoadPeripheralData)
		outputStream () << MCHEmul::FormatterBuilder::instance () ->
			formatter ("C" + cmdLoadPeripheralData) -> format (loadPeripheralData 
				(prmsFor (_command, cmdLoadPeripheralData))) << std::endl;
	else
	if (cmdName == cmdEmptyPeripheralData)
		outputStream () << MCHEmul::FormatterBuilder::instance () ->
			formatter ("C" + cmdEmptyPeripheralData) -> format (emptyDataInPeripheral 
				(prmsFor (_command, cmdEmptyPeripheralData))) << std::endl;
	else
	if (cmdName == cmdSavePeripheralData)
		outputStream () << MCHEmul::FormatterBuilder::instance () ->
			formatter ("C" + cmdSavePeripheralData) -> format (savePeripheralData 
				(prmsFor (_command, cmdSavePeripheralData))) << std::endl;
	else
	{
		MCHEmul::Command* cmd = commandBuilder () -> command (_command);
		if (cmd == nullptr) outputStream () << _command << ":" << _commandDoesnExitTxt << std::endl;
		else executeCommandNow (cmd, _emulator -> computer ());
	}
}

// ---
bool MCHEmul::LocalConsole::runPerCycle ()
{
	// Exist when something is wrong...
	bool exit = !_emulator -> runCycle ();
	// ...and if it has been defined to restart when finished tryit bac...
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

	MCHEmul::Strings prmsL = parametersListFrom (prms);
	if (prmsL.size () != 2)
	{ 
		result.add (std::string ("ERROR"), std::string ("Insuficient numberof arguments"));

		return (result);
	}

	MCHEmul::Address iA = MCHEmul::Address::fromStr (prmsL [0]);
	if (iA > _emulator -> computer () -> cpu () -> architecture ().longestAddressPossible ())
	{ 
		result.add (std::string ("ERROR"), std::string ("Initial address out of memory"));

		return (result);
	}

	size_t nB = (size_t) std::atoi (prmsL [1].c_str ());
	size_t nBA = _emulator -> computer () -> cpu () -> architecture ().numberBytes ();
	if (iA.size () > nBA || nB > ((size_t) 1 << (MCHEmul::UByte::sizeBits () * nBA)))
	{ 
		result.add (std::string ("ERROR"), std::string ("Final address out of memory"));

		return (result);
	}

	MCHEmul::Assembler::ByteCode cL = MCHEmul::Assembler::ByteCode::createFromMemory 
		(iA, (unsigned int) nB, _emulator -> computer () -> memory (), _emulator -> computer ());
	MCHEmul::InfoStructure lns;
	for (size_t i = 0; i < cL._lines.size (); i++)
	{ 
		std::string iP = std::to_string (i);
		iP = MCHEmul::_CEROS.substr (0, 5 - iP.length ()) + iP;
		lns.add (iP, cL._lines [i].asString (MCHEmul::UByte::OutputFormat::_HEXA, ' ', 2));
	}

	result.add (std::string ("ERROR"), std::string ("no errors"));
	result.add ("CODELINES", lns);
	return (result);
}

// ---
MCHEmul::InfoStructure MCHEmul::LocalConsole::connectPeripheral (const std::string& prms)
{
	MCHEmul::InfoStructure result;

	MCHEmul::Strings prmsL = parametersListFrom (prms);
	if (prmsL.size () == 0)
	{ 
		result.add (std::string ("ERROR"), std::string ("Insuficient numberof arguments"));

		return (result);
	}

	int prhId = std::atoi (prmsL [0].c_str ());
	MCHEmul::Attributes prhAttrs;
	if (prmsL.size () > 1)
	{ 
		int ct = 0;
		for (size_t i = 1; i < prmsL.size (); i++, ct++)
			prhAttrs [std::to_string (ct)] = prmsL [i];
	}

	result.add (std::string ("ERROR"), 
		!_emulator -> connectPeripheral (prhId, prhAttrs)
			? std::string ("Peripheral not connected. Maybe it does exist")
			: "No errors");

	return (result);
}

// ---
MCHEmul::InfoStructure MCHEmul::LocalConsole::disconnectPeripherals (const std::string& prms)
{
	MCHEmul::InfoStructure result;

	MCHEmul::Strings prmsL = parametersListFrom (prms);
	if (prmsL.size () == 0)
	{ 
		result.add (std::string ("ERROR"), std::string ("Insuficient numberof arguments"));

		return (result);
	}

	bool e = true;
	for (const auto& i : prmsL)
		e &= _emulator -> disconnectPeripheral (std::atoi (i.c_str ()));

	result.add (std::string ("ERROR"), 
		!e ? std::string ("Some peripherals were not well disconnected. Verify.") : "No errors");

	return (result);
}

// ---
MCHEmul::InfoStructure MCHEmul::LocalConsole::loadPeripheralData (const std::string& prms) const
{
	MCHEmul::InfoStructure result;

	MCHEmul::Strings prmsL = parametersListFrom (prms);
	if (prmsL.size () != 2 && prmsL.size () != 3)
	{ 
		result.add (std::string ("ERROR"), std::string ("Insuficient number of arguments"));

		return (result);
	}

	MCHEmul::FileData* dt = _emulator -> connectDataToPeripheral (prmsL [1], std::atoi (prmsL [0].c_str ()));
	result.add (std::string ("ERROR"), 
		(dt == nullptr)
			? std::string ("The data was not connected to the peripheral.")
			: "No errors. " + dt -> asString ());

	// If there were a third parameter it could be the file were to trace...
	if (dt != nullptr && prmsL.size () == 3)
		_emulator -> computer () -> activateDeepDebug 
			(prmsL [2], 
				MCHEmul::Address (),
				_emulator -> computer () -> cpu () -> architecture ().longestAddressPossible (),
				true, { }, { }, { }, true);

	return (result);
}

// ---
MCHEmul::InfoStructure MCHEmul::LocalConsole::emptyDataInPeripheral (const std::string& prms) const
{
	MCHEmul::InfoStructure result;

	MCHEmul::Strings prmsL = parametersListFrom (prms);
	if (prmsL.size () != 2)
	{ 
		result.add (std::string ("ERROR"), std::string ("Insuficient number of arguments"));

		return (result);
	}

	MCHEmul::FileData* dt = 
		_emulator -> createEmptyDataInPeripheral (prmsL [1], std::atoi (prmsL [0].c_str ()));
	if (dt == nullptr)
		result.add (std::string ("ERROR"),
			std::string ("Impossible to create empty data in the peripheral."));
	else
		result = std::move (loadPeripheralData (prms));

	return (result);
}

// ---
MCHEmul::InfoStructure MCHEmul::LocalConsole::savePeripheralData (const std::string & prms) const
{
	MCHEmul::InfoStructure result;

	MCHEmul::Strings prmsL = parametersListFrom (prms);
	if (prmsL.size () != 2 && prmsL.size () != 1)
	{ 
		result.add (std::string ("ERROR"), std::string ("Insuficient number of arguments"));

		return (result);
	}

	result.add (std::string ("ERROR"), 
		(_emulator -> saveDataFromPeripheral 
			((prmsL.size () == 1) ? "" : prmsL [1], std::atoi (prmsL [0].c_str ()))
				? std::string ("No errors.")
				: std::string ("The data was not connected to the peripheral.")));

	return (result);
}

// ---
MCHEmul::Strings MCHEmul::LocalConsole::parametersListFrom (const std::string& cmd) const
{
	MCHEmul::Strings result;

	size_t pS = 0;
	std::string cmdC = MCHEmul::trim (cmd);
	while (cmdC != "")
	{
		pS = MCHEmul::firstSpaceIn (cmdC);
		if (pS == std::string::npos)
		{ 
			result.emplace_back (MCHEmul::trim (cmdC));

			cmdC = "";
		}
		else
		{
			result.emplace_back (cmdC.substr (0, pS));

			cmdC = (pS == (cmdC.length () - 1) /** The last one? */) ? "" : MCHEmul::trim (cmdC.substr (pS));
		}
	}

	return (result);
}
