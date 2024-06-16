#include <CORE/StdCommands.hpp>
#include <CORE/Computer.hpp>
#include <CORE/Stack.hpp>
#include <CORE/Instruction.hpp>
#include <CORE/ByteCodeLine.hpp>
#include <CORE/CmdExecuter.hpp>
#include <fstream>

// ---
const std::string MCHEmul::HelpCommand::_NAME = "CHELP";
const std::string MCHEmul::AuthorInfoCommand::_NAME = "CAUTHOR";
const std::string MCHEmul::StatusRegisterStatusCommand::_NAME =  "CSTATUS";
const std::string MCHEmul::RegistersStatusCommand::_NAME = "CREGISTERS";
const std::string MCHEmul::RegisterChangeCommand::_NAME = "CCHANGEREG";
const std::string MCHEmul::ProgramCounterStatusCommand::_NAME = "CPC";
const std::string MCHEmul::StackStatusCommand::_NAME = "CSTACK";
const std::string MCHEmul::CPUStatusCommand::_NAME = "CCPUSTATUS";
const std::string MCHEmul::CPUSimpleStatusCommand::_NAME = "CCPUSSTATUS";
const std::string MCHEmul::CPUInfoCommand::_NAME = "CCPUINFO";
const std::string MCHEmul::MemoryStatusCommand::_NAME = "CMEMORY";
const std::string MCHEmul::SetMemoryValueCommand::_NAME = "CSETMEMORY";
const std::string MCHEmul::StopCPUCommand::_NAME = "CSTOP";
const std::string MCHEmul::RunCPUCommand::_NAME = "CRUN";
const std::string MCHEmul::SetProgramCounterCommand::_NAME = "CSETPC";
const std::string MCHEmul::NextInstructionCommand::_NAME = "CNEXT";
const std::string MCHEmul::ShowNextInstructionCommand::_NAME = "CSHOWNEXT";
const std::string MCHEmul::LastIntructionCPUCommand::_NAME = "CINST";
const std::string MCHEmul::ListOfBreakPointsCommand::_NAME = "CBREAKS";
const std::string MCHEmul::SetBreakPointCommand::_NAME = "CSETBREAK";
const std::string MCHEmul::RemoveBreakPointCommand::_NAME = "CREMOVEBREAK";
const std::string MCHEmul::RemoveAllBreakPointsCommand::_NAME = "CREMOVEBREAKS";
const std::string MCHEmul::CPUSpeedCommand::_NAME = "CSPEED";
const std::string MCHEmul::LoadBinCommand::_NAME = "CLOADBIN";
const std::string MCHEmul::MoveParametersToAnswerCommand::_NAME = "CMOVEPARAMS";
const std::string MCHEmul::SaveBinCommand::_NAME = "CSAVEBIN";
const std::string MCHEmul::ActivateDeepDebugCommand::_NAME = "CACTIVATEDEEPDEBUG";
const std::string MCHEmul::DesactivateDeepDebugCommand::_NAME = "CDESACTIVATEDEEPDEBUG";
const std::string MCHEmul::RestartComputerCommand::_NAME = "CRESTART";
const std::string MCHEmul::IODevicesCommand::_NAME = "CDEVICES";
const std::string MCHEmul::PeripheralsCommand::_NAME = "CPERIPHERALS";
const std::string MCHEmul::PeripheralInstructionCommand::_NAME = "CPERCMD";
const std::string MCHEmul::AssignJoystickNameCommand::_NAME = "CASSIGNJ";
const std::string MCHEmul::ChangeCPUClockCommand::_NAME = "CCLOCKFACTOR";
const std::string MCHEmul::SoundOnCommand::_NAME = "CSOUNDON";
const std::string MCHEmul::SoundOffCommand::_NAME = "CSOUNDOFF";
const std::string MCHEmul::InterruptsCommand::_NAME = "CINTERRUPTS";
const std::string MCHEmul::InterruptSetCommand::_NAME = "CINTSET";
const std::string MCHEmul::InterruptDebugOnCommand::_NAME = "CIDEBUGON";
const std::string MCHEmul::InterruptDebugOffCommand::_NAME = "CIDEBUGOFF";
const std::string MCHEmul::ChipsListCommand::_NAME = "CCHIPS";
const std::string MCHEmul::CRTEffectOnCommand::_NAME = "CCRTON";
const std::string MCHEmul::CRTEffectOffCommand::_NAME = "CCRTOFF";
const std::string MCHEmul::DatasetteStatusCommand::_NAME = "CDATASETTE";

// ---
MCHEmul::HelpCommand::HelpCommand (const std::string& hF)
	: Command (_ID, _NAME), 
	  _helpInfo ()
{
	MCHEmul::Strings hls = loadHelpFile (hF);
	MCHEmul::Strings::const_iterator i = hls.begin ();
	while (i != hls.end ())
	{
		if ((*i)[0] != ';')
			continue; // A line to define a command is expected, otherwise ignored!
		else
		{
			// Gets the command line...
			std::string cN = MCHEmul::upper (MCHEmul::trim ((*i++).substr (1)));
			// ...extract just the name of the command...
			auto ecN = std::find_if (cN.begin (), cN.end (), std::isspace);
			std::string cNS = (ecN == cN.end ()) ? cN : cN.substr (0, cN.find (*ecN)); // Only the main word is inserted as key...
			// Insert the full command as part of the information to show...
			MCHEmul::Strings hI;
			hI.push_back (cN);
			// ...and then insert the rest of the instructions associated
			while (i != hls.end () && (*i)[0] != ';')
				hI.push_back ((*i++));
			_helpInfo.insert (std::pair <std::string, MCHEmul::Strings> (cNS, hI));
		}
	}
}

// ---
void MCHEmul::HelpCommand::executeImpl (MCHEmul::CommandExecuter* cE, MCHEmul::Computer* c, MCHEmul::InfoStructure& rst)
{
	// No computer is needed...

	MCHEmul::InfoStructure iS;

	auto helpInfoCommand = [&](const std::string& cmd, bool sp) -> void
		{
			bool fL = true;
			MCHEmul::HelpCommand::HelpInfo::const_iterator i;
			auto eCmd = std::find_if (cmd.begin (), cmd.end (), std::isspace);
			if ((i = _helpInfo.find (cmd)) != _helpInfo.end ())
			{
				std::string h = "";
				for (size_t j = 0; j < (*i).second.size (); j++)
					h += ((j == 0) ? '\0' : '\n') + (*i).second [j];
				iS.add (((sp) ? "---\n" : "") + std::string ("->") + cmd, MCHEmul::removeAll0 (h));
			}
		};

	if (_parameters.size () == 0)
	{
		unsigned int ct = 0;
		for (const auto& i : _helpInfo)
			helpInfoCommand (i.first, (ct++ != 0) ? true : false);
	}
	else
		helpInfoCommand (parameter ("00"), false);

	rst.add ("HELP", std::move (iS));
}

// ---
MCHEmul::Strings MCHEmul::HelpCommand::loadHelpFile (const std::string& hF)
{
	MCHEmul::Strings result;

	std::fstream hFile (hF, std::ios_base::in);
	if (hFile.is_open ())
	{
		char l [255];
		while (!hFile.eof ())
		{
			hFile.getline (l, 255);
			std::string ls = MCHEmul::trim (l);
			if (ls != "" && ls [0] == '#') continue; // Comments are not taken into account...
			if (ls != "" && ls [0] == '?') // It means an include command!
			{
				MCHEmul::Strings hls = loadHelpFile (MCHEmul::trim (ls.substr (1)));

				result.insert (result.end (), hls.begin (), hls.end ());
			}
			else
				result.emplace_back (ls);
		}

		hFile.close ();
	}

	return (result);
}

// ---
void MCHEmul::AuthorInfoCommand::executeImpl (MCHEmul::CommandExecuter* cE, MCHEmul::Computer* c, MCHEmul::InfoStructure& rst)
{
	// No computer is needed...

	MCHEmul::InfoStructure dt;
	dt.add ("NAME", std::string ("Ignacio Cea Fornies"));
	dt.add ("COUNTRY", std::string ("Spain"));
	dt.add ("CITY", std::string ("Madrid"));
	dt.add ("DATE", std::string ("October 2022"));
	dt.add ("EMAIL", std::string ("ignacio.cea.fornies@telefonica.net"));
	dt.add ("LOVE", std::string ("my wife, my daugthers, my family and my friends"));

	rst.add ("Author", dt);
}

// ---
void MCHEmul::StatusRegisterStatusCommand::executeImpl (MCHEmul::CommandExecuter* cE, MCHEmul::Computer* c,  MCHEmul::InfoStructure& rst)
{
	if (c == nullptr)
		return;

	rst.add ("SR", c -> cpu () -> statusRegister ().asString ());
}

// ---
void MCHEmul::RegistersStatusCommand::executeImpl (MCHEmul::CommandExecuter* cE, MCHEmul::Computer* c, MCHEmul::InfoStructure& rst)
{
	if (c == nullptr)
		return;

	rst.add ("REGS", c -> cpu () -> getInfoStructure ().infoStructure ("REGS"));
}

// ---
void MCHEmul::RegisterChangeCommand::executeImpl (MCHEmul::CommandExecuter* cE, MCHEmul::Computer* c, MCHEmul::InfoStructure& rst)
{
	if (c == nullptr)
		return;

	// First of all, the register has to exists...
	if (c -> cpu () -> existsInternalRegister (parameter ("00")))
	{ 
		std::string nR = parameter ("00");
		MCHEmul::UBytes by = MCHEmul::UInt::fromStr (parameter ("01")).bytes ();
		// ...but it also has to accept the bytes (size mainly) as parameter...
		if (c -> cpu () -> internalRegister (nR).accept (by))
			c -> cpu () -> internalRegister (nR).set (by);
		else
			rst.add ("ERROR", std::string ("The register doesn't accept the bytes"));
	}
	else
		rst.add ("ERROR", std::string ("The register doesn't exist"));
	// If hansn't (any of both conditions) and error is recordered...
}

// ---
void MCHEmul::ProgramCounterStatusCommand::executeImpl (MCHEmul::CommandExecuter* cE, MCHEmul::Computer* c, MCHEmul::InfoStructure& rst)
{
	if (c == nullptr)
		return;

	rst.add ("PC", c -> cpu () -> programCounter ().asString ());
}

// ---
void MCHEmul::StackStatusCommand::executeImpl (MCHEmul::CommandExecuter* cE, MCHEmul::Computer* c, InfoStructure& rst)
{
	if (c == nullptr)
		return;

	MCHEmul::InfoStructure iS = c -> memory () -> stack () -> getInfoStructure ();
	if (_parameters.size () == 0 || (_parameters.size () == 1 && parameter ("00") != "ALL"))
		iS.remove ("Memory"); // Reduce the size...

	rst.add ("Stack", std::move (iS));
}

// ---
MCHEmul::CPUStatusCommand::CPUStatusCommand ()
	: MCHEmul::ComplexCommand (_ID, _NAME,
			MCHEmul::Commands (
				{ new StatusRegisterStatusCommand,
				  new RegistersStatusCommand,
				  new ProgramCounterStatusCommand,
				  new LastIntructionCPUCommand,
				  new StackStatusCommand,
				  new InterruptsCommand
				}))
{
	// Nothing else...
}

// ---
MCHEmul::CPUSimpleStatusCommand::CPUSimpleStatusCommand ()
	: MCHEmul::ComplexCommand (_ID, _NAME,
			MCHEmul::Commands (
				{ new StatusRegisterStatusCommand,
				  new RegistersStatusCommand,
				  new ProgramCounterStatusCommand,
				  new LastIntructionCPUCommand,
				  new InterruptsCommand
				}))
{
	// Nothing else...
}

// ---
void MCHEmul::CPUInfoCommand::executeImpl (MCHEmul::CommandExecuter* cE, MCHEmul::Computer* c, MCHEmul::InfoStructure& rst)
{
	if (c == nullptr)
		return;

	rst.add ("CPU", std::move (c -> cpu () -> getInfoStructure ()));
}

// ---
void MCHEmul::MemoryStatusCommand::executeImpl (MCHEmul::CommandExecuter* cE, MCHEmul::Computer* c, MCHEmul::InfoStructure& rst)
{
	if (c == nullptr)
		return;

	MCHEmul::Address a1 = MCHEmul::Address::fromStr (parameter ("00"));
	if (a1 > c -> cpu () -> architecture ().longestAddressPossible ()) 
		return;
	if (_parameters.size () == 2)
	{
		MCHEmul::Address a2 = MCHEmul::Address::fromStr (parameter ("01"));
		if (a2 > c -> cpu () -> architecture ().longestAddressPossible ())
			return;

		MCHEmul::Address iA = ((a1 <= a2) ? a1 : a2);
		rst.add ("BYTES", c -> cpu () -> memoryRef () -> values (iA, (((a2 >= a1) ? a2 : a1) - iA) + 1).
				asString (MCHEmul::UByte::OutputFormat::_HEXA, ',', 2));
	}
	else
		rst.add ("BYTES", c -> cpu () -> memoryRef () -> value (a1).asString (MCHEmul::UByte::OutputFormat::_HEXA, 2));
}

// ---
void MCHEmul::SetMemoryValueCommand::executeImpl (MCHEmul::CommandExecuter* cE, MCHEmul::Computer* c, MCHEmul::InfoStructure& rst)
{
	if (c == nullptr)
		return;

	MCHEmul::Address a1 = MCHEmul::Address::fromStr (parameter ("00"));
	if (a1 > c -> cpu () -> architecture ().longestAddressPossible ()) 
		return;
	
	MCHEmul::Address a2 = a1;
	std::vector <MCHEmul::UByte> v;
	if (_parameters.size () == 3)
	{
		a2 = MCHEmul::Address::fromStr (parameter ("01"));
		if (a2 > c -> cpu () -> architecture ().longestAddressPossible ())
			return;

		v = MCHEmul::UInt::fromStr (parameter ("02")).bytes ();
	}
	else
		v = MCHEmul::UInt::fromStr (parameter ("01")).bytes ();

	MCHEmul::Address iA = (a1 <= a2) ? a1 : a2;
	MCHEmul::Address fA = (a2 >= a1) ? a2 : a1;
	for (size_t i = 0; i <= (size_t) (fA - iA); i += v.size ())
		c -> cpu () -> memoryRef () -> set (iA + i, v); // Without force it!
}

// ---
void MCHEmul::StopCPUCommand::executeImpl (MCHEmul::CommandExecuter* cE, MCHEmul::Computer* c, MCHEmul::InfoStructure& rst)
{
	if (c == nullptr)
		return;

	c -> setActionForNextCycle (MCHEmul::Computer::_ACTIONSTOP);

	MCHEmul::CPUStatusCommand ().execute (cE, c, rst);
}

// ---
void MCHEmul::RunCPUCommand::executeImpl (MCHEmul::CommandExecuter* cE, MCHEmul::Computer* c, MCHEmul::InfoStructure& rst)
{
	if (c == nullptr)
		return;

	if (_parameters.size () == 1)
		c -> cpu () -> programCounter ().setAddress (MCHEmul::Address::fromStr (parameter ("00")));

	c -> setActionForNextCycle (MCHEmul::Computer::_ACTIONCONTINUE);

	MCHEmul::CPUStatusCommand ().execute (cE, c, rst);
}

// ---
void MCHEmul::SetProgramCounterCommand::executeImpl
	(MCHEmul::CommandExecuter* cE, MCHEmul::Computer* c, MCHEmul::InfoStructure& rst)
{
	if (c == nullptr)
		return;

	c -> cpu () -> programCounter ().setAddress (MCHEmul::Address::fromStr (parameter ("00")));
}

// ---
void MCHEmul::NextInstructionCommand::executeImpl (MCHEmul::CommandExecuter* cE, MCHEmul::Computer* c, MCHEmul::InfoStructure& rst)
{
	if (c == nullptr)
		return;

	c -> setActionForNextCycle (MCHEmul::Computer::_ACTIONNEXT);

	// To show status after execution...
	if (cE != nullptr)
		cE -> executeCommand (cE -> commandBuilder () -> command ("CPUSSTATUS"), c);
}

// ---
void MCHEmul::ShowNextInstructionCommand::executeImpl 
	(MCHEmul::CommandExecuter* cE, MCHEmul::Computer* c, MCHEmul::InfoStructure& rst)
{
	if (c == nullptr)
		return;

	unsigned int nI = 1;
	if (_parameters.size () != 0)
		nI = std::stoi (parameter ("00").c_str ());

	MCHEmul::ProgramCounter& pC = c -> cpu () -> programCounter ();
	MCHEmul::Address oA = pC.asAddress ();

	bool e = false;
	MCHEmul::InfoStructure iS;
	for (unsigned int i = 0; i < nI && !e; i++)
	{ 
		const MCHEmul::Instruction* nI = c -> cpu () -> nextInstruction ();
		if (e = (nI == nullptr))
			continue; // Not possible to continue...

		MCHEmul::Address pCA = pC.asAddress ();
		std::string iP = std::to_string (i);
		iP = MCHEmul::_CEROS.substr (0, 5 - iP.length ()) + iP;
		iS.add (std::move (iP),
			MCHEmul::ByteCodeLine (pCA, nI -> parameters ().bytes (), "", nI, c -> action (pCA)).asString
				(MCHEmul::UByte::OutputFormat::_HEXA, ' ', 2));
		
		pC.increment (nI -> memoryPositions (c -> memory (), pCA)); // as it hasn't been executed, 
																	// memoryPositionsExecuted can not be used...
	}

	rst.add ("CODELINES", std::move (iS));

	// The program counter is restored!
	pC.setAddress (oA);
}

// ---
void MCHEmul::LastIntructionCPUCommand::executeImpl (MCHEmul::CommandExecuter* cE, MCHEmul::Computer* c, MCHEmul::InfoStructure& rst)
{
	if (c == nullptr)
		return;

	/** The instruction is added in the format o a bye code line. */
	rst.add ("INST", c -> cpu () -> lastInstruction () == nullptr 
		? "-" 
		: MCHEmul::ByteCodeLine (
			c -> cpu () -> lastInstruction () -> programCounter ().asAddress (), // Before the execution
			c -> cpu () -> lastInstruction () -> parameters ().bytes (), // The bytes of the instruction
			"", /** No label recognized. */
			c -> cpu () -> lastInstruction (), 
			c -> action (c -> cpu () -> programCounter ().asAddress ())).
				asString (MCHEmul::UByte::OutputFormat::_HEXA, ' ', 2));
}

// ---
void MCHEmul::ListOfBreakPointsCommand::executeImpl (MCHEmul::CommandExecuter* cE, MCHEmul::Computer* c, MCHEmul::InfoStructure& rst)
{
	if (c == nullptr)
		return;

	int ct = 0;
	std::string lst;
	for (const auto& i : c -> actions ())
		if (i.second == MCHEmul::Computer::_ACTIONSTOP) // Only if stopped...
			lst += ((ct++ != 0) ? "," : "\0") + std::string ("$") + 
				i.first.asString (MCHEmul::UByte::OutputFormat::_HEXA, '\0', 2);
	lst = MCHEmul::removeAll0 (lst);

	rst.add ("BREAKPOINTS", std::move (lst));
}

// ---
void MCHEmul::SetBreakPointCommand::executeImpl (MCHEmul::CommandExecuter* cE, MCHEmul::Computer* c, MCHEmul::InfoStructure& rst)
{
	if (c == nullptr)
		return;

	for (const auto& i : _parameters)
		c -> addAction (MCHEmul::Address::fromStr (i.second), MCHEmul::Computer::_ACTIONSTOP);
}

// ---
void MCHEmul::RemoveBreakPointCommand::executeImpl (MCHEmul::CommandExecuter* cE, MCHEmul::Computer* c, MCHEmul::InfoStructure& rst)
{
	if (c == nullptr)
		return;

	for (const auto& i : _parameters)
	{
		MCHEmul::Address bP = MCHEmul::Address::fromStr (i.second);
		if (c -> action (bP) == MCHEmul::Computer::_ACTIONSTOP) // Only if stopped...
			c -> addAction (bP, MCHEmul::Computer::_ACTIONNOTHING);
	}
}

// ---
void MCHEmul::RemoveAllBreakPointsCommand::executeImpl (MCHEmul::CommandExecuter* cE, MCHEmul::Computer* c, MCHEmul::InfoStructure& rst)
{
	if (c == nullptr)
		return;

	c -> removeAllActions (MCHEmul::Computer::_ACTIONSTOP);
}

// ---
void MCHEmul::CPUSpeedCommand::executeImpl (MCHEmul::CommandExecuter* cE, MCHEmul::Computer* c, MCHEmul::InfoStructure& rst)
{
	if (c == nullptr)
		return;

	rst.add ("SPEED",	c -> realCyclesPerSecond ());
	rst.add ("HERTZS",	c -> screen () -> realHertzs ());
}

// ---
void MCHEmul::LoadBinCommand::executeImpl (MCHEmul::CommandExecuter* cE, MCHEmul::Computer* c, MCHEmul::InfoStructure& rst)
{
	if (c == nullptr)
		return;

	rst.add	("RESULT", 
		(c -> loadInto (parameter ("00"), 
			MCHEmul::Address::fromStr (parameter ("01")))) ? std::string ("Ok") : std::string ("Error"));
}

// ---
void MCHEmul::SaveBinCommand::executeImpl (MCHEmul::CommandExecuter* cE, MCHEmul::Computer* c, MCHEmul::InfoStructure& rst)
{
	if (c == nullptr)
		return;

	rst.add	("RESULT", 
		(c -> saveFrom
			(parameter ("00"), std::stoi (parameter ("01").c_str ()), 
				MCHEmul::Address::fromStr (parameter ("02")))) ? std::string ("Ok") : std::string ("Error"));
}

// ---
void MCHEmul::MoveParametersToAnswerCommand::executeImpl 
	(MCHEmul::CommandExecuter* cE, MCHEmul::Computer* c, MCHEmul::InfoStructure& rst)
{
	for (const auto& i : _parameters)
		rst.add (i.first, i.second);
}

// ---
void MCHEmul::ActivateDeepDebugCommand::executeImpl 
	(MCHEmul::CommandExecuter* cE, MCHEmul::Computer* c, MCHEmul::InfoStructure& rst)
{
	if (c == nullptr)
		return;

	std::string fN = parameter ("00");	// The name of the file is always mandatory...
	std::vector <int> cId = { };		// None by default...
	std::vector <int> iId = { };		// None by default...
	bool a = false;						// Not to add by default...
	if (parameters ().size () >= 2)
	{
		a = (parameter ("01") == "YES") ? true : false;

		if (parameters ().size () >= 3)
		{
			int nP = 2;
			std::string nPStr = ((nP < 10) ? "0" : "") + std::to_string (nP);
			while (existParameter (nPStr))
			{
				std::string prm = parameter (nPStr);
				if (prm == "ALLCHIP")
					cId = { -1 };
				else
				if (prm == "ALLIO")
					iId = { -1 };
				else
				{
					if (prm.length () > 2)
					{
						if (prm.substr (0, 2) == "C:" && 
							(cId.size () == 0 || (cId.size () != 0 && cId [0] != -1))) 	// When a -1 is already in the list, nothing else makes sense...
							cId.emplace_back (std::atoi (prm.substr (2).c_str ()));
						if (prm.substr (0, 2) == "I:" && 
							(iId.size () == 0 || (iId.size () != 0 && iId [0] != -1)))
							iId.emplace_back (std::atoi (prm.substr (2).c_str ()));
					}
				}

				nP++;
				nPStr = ((nP < 10) ? "0" : "") + std::to_string (nP);
			}
		}
	}

	rst.add ("ERROR", 
		c -> activateDeepDebug (fN, cId, iId, a)
				? std::string ("No errors")
				: std::string ("Deep debugging activation error"));
}

// ---
void MCHEmul::DesactivateDeepDebugCommand::executeImpl 
	(MCHEmul::CommandExecuter* cE, MCHEmul::Computer* c, MCHEmul::InfoStructure& rst)
{
	if (c == nullptr)
		return;

	rst.add ("ERROR", 
		c -> desactivateDeepDebug ()
			? std::string ("No errors")
			: std::string ("Deep debugging desactivation error"));
}

// ---
void MCHEmul::RestartComputerCommand::executeImpl 
	(MCHEmul::CommandExecuter* cE, MCHEmul::Computer* c, MCHEmul::InfoStructure& rst)
{
	if (c == nullptr)
		return;

	unsigned int level = (_parameters.size () == 1) ? std::atoi (parameter ("00").c_str ()) : 0;

	// Ends the main loop...
	c -> setExit (true);
	// but starts back...
	c -> setRestartAfterExit (true, level); 
}

// ---
void MCHEmul::IODevicesCommand::executeImpl 
	(MCHEmul::CommandExecuter* cE, MCHEmul::Computer* c, MCHEmul::InfoStructure& rst)
{
	if (c == nullptr)
		return;

	MCHEmul::InfoStructure dvcs;
	for (const auto& i : c -> devices ())
		dvcs.add (std::to_string (i.first), std::move (i.second -> getInfoStructure ()));
	rst.add ("DEVICES", std::move (dvcs));
}

// ---
void MCHEmul::PeripheralsCommand::executeImpl 
	(MCHEmul::CommandExecuter* cE, MCHEmul::Computer* c, MCHEmul::InfoStructure& rst)
{
	if (c == nullptr)
		return;

	MCHEmul::InfoStructure prhsD;
	MCHEmul::IOPeripherals prhs = std::move (c -> peripherals ()); // The list has been built up...
	for (const auto& i : prhs)
		prhsD.add (std::to_string (i.first), std::move (i.second -> getInfoStructure ()));
	rst.add ("PERIPHERALS", std::move (prhsD));
}

// ---
void MCHEmul::PeripheralInstructionCommand::executeImpl 
	(MCHEmul::CommandExecuter* cE, MCHEmul::Computer* c, MCHEmul::InfoStructure& rst)
{
	if (c == nullptr)
		return;

	int pId = std::atoi (parameter ("00").c_str ());
	if (!c -> existsPeripheral (pId))
	{ 
		rst.add ("ERROR", std::string ("The peripheral doesn't exist"));

		return;
	}

	int cId = std::atoi (parameter ("01").c_str ());
	MCHEmul::Strings aPrms;
	for (const auto& i : parameters ())
		if (i.first != "00" && i.first != "01") // The two first parameters have been laready used...
			aPrms.emplace_back (i.second);

	rst.add ("ERROR", 
		!c -> peripheral (pId) -> executeCommand (cId, aPrms)
			? std::string ("The command can not be executed")
			: std::string ("No errors"));
}

// ---
void MCHEmul::AssignJoystickNameCommand::executeImpl 
	(MCHEmul::CommandExecuter* cE, MCHEmul::Computer* c, MCHEmul::InfoStructure& rst)
{
	if (c == nullptr)
		return;

	int jId = std::atoi (parameter ("00").c_str ());
	int jN = std::atoi (parameter ("01").c_str ());
	if (c -> inputOSSystem () == nullptr)
	{ 
		rst.add ("ERROR", std::string ("The peripheral doesn't exist"));

		return;
	}

	const_cast <MCHEmul::InputOSSystem*> (c -> inputOSSystem ()) -> addConversionJoystick (jId, jN);

	rst.add ("ERROR", std::string ("No errors"));
}

// ---
void MCHEmul::ChangeCPUClockCommand::executeImpl 
	(MCHEmul::CommandExecuter* cE, MCHEmul::Computer* c, MCHEmul::InfoStructure& rst)
{
	if (c == nullptr)
		return;

	c -> clock ().setFactor (std::atof (parameter ("00").c_str ()));
}

// ---
void MCHEmul::SoundOnCommand::executeImpl 
	(MCHEmul::CommandExecuter* cE, MCHEmul::Computer* c, MCHEmul::InfoStructure& rst)
{
	if (c == nullptr || c -> sound () == nullptr)
		return; // Nothing to do...

	c -> sound () -> setSilence (false);
}

// ---
void MCHEmul::SoundOffCommand::executeImpl 
	(MCHEmul::CommandExecuter* cE, MCHEmul::Computer* c, MCHEmul::InfoStructure& rst)
{
	if (c == nullptr || c -> sound () == nullptr)
		return; // Nothing to do...

	c -> sound () -> setSilence (true);
}

// ---
void MCHEmul::InterruptsCommand::executeImpl
	(MCHEmul::CommandExecuter* cE, MCHEmul::Computer* c, MCHEmul::InfoStructure& rst)
{
	if (c == nullptr)
		return; // Nothing to do...

	MCHEmul::InfoStructure intr;
	for (const auto& i : c -> cpu () -> interrupts ())
		intr.add (std::to_string (i.second -> id ()), std::move (i.second -> getInfoStructure ()));
	rst.add ("INTERRUPTS", std::move (intr));
}

// ---
void MCHEmul::InterruptSetCommand::executeImpl
	(MCHEmul::CommandExecuter* cE, MCHEmul::Computer* c, MCHEmul::InfoStructure& rst)
{
	if (c == nullptr)
		return; // Nothing to do...

	// Gets the interrupt id/ids affected...
	// ...and checj whether there is an error or not
	bool e = false;
	std::vector <int> iIds;
	if (parameter ("00") == "ALL")
		for (const auto& i : c -> cpu () -> interrupts ())
			iIds.emplace_back (i.first); 
			// The id of the interrupt...and it is sure that all of them will exist!
	else
	{
		int iId = std::atoi (parameter ("00").c_str ());
		if (!(e = !c -> cpu () -> existsInterrupt (iId)))
			iIds.emplace_back (iId);
	}

	if (!e)
	{
		bool acc = (parameter ("01") == "ON") ? true : false;
		for (const auto& i : iIds)
			c -> cpu () -> interrupt (i) -> setActive (acc);
	}
	else
		rst.add ("ERROR", std::string ("Interrupt doesn't exist"));
}

// ---
void MCHEmul::InterruptDebugOnCommand::executeImpl 
	(MCHEmul::CommandExecuter* cE, MCHEmul::Computer* c, MCHEmul::InfoStructure& rst)
{
	if (c == nullptr)
		return;

	rst.add ("ERROR", 
		MCHEmul::CPUInterrupt::activateDebug (c, parameter ("00"), 
			(existParameter ("01") ? ((parameter ("01") == "YES") ? true : false) : false /** default value. */))
				? std::string ("No errors")
				: std::string ("Interrupt debug activation error"));
}

// ---
void MCHEmul::InterruptDebugOffCommand::executeImpl 
	(MCHEmul::CommandExecuter* cE, MCHEmul::Computer* c, MCHEmul::InfoStructure& rst)
{
	if (c == nullptr)
		return;

	rst.add ("ERROR", 
		MCHEmul::CPUInterrupt::desactivateDebug (c)
			? std::string ("No errors")
			: std::string ("Interrupt debug desactivation error"));
}

// ---
void MCHEmul::ChipsListCommand::executeImpl
	(MCHEmul::CommandExecuter* cE, Computer* c, InfoStructure& rst)
{
	if (c == nullptr)
		return;

	auto attrToStr = [=](const MCHEmul::Attributes& attrs) -> std::string
	{
		bool f = true;
		std::string result = "";
		for (const auto& i : attrs)
		{
			result += (!f ? "," : "") + i.second;

			f = false;
		}

		return (result);
	};

	MCHEmul::InfoStructure ch;
	for (const auto& i : c -> chips ())
		ch.add (std::to_string (i.second -> id ()), std::move (attrToStr (i.second -> attributes ())));
	rst.add ("CHIPS", std::move (ch));
}

// ---
void MCHEmul::CRTEffectOnCommand::executeImpl
	(MCHEmul::CommandExecuter* cE, MCHEmul::Computer* c, InfoStructure& rst)
{
	if (c == nullptr)
		return;

	c -> screen () -> setCRTEffect (true);
}

// ---
void MCHEmul::CRTEffectOffCommand::executeImpl
	(MCHEmul::CommandExecuter* cE, MCHEmul::Computer* c, InfoStructure& rst)
{
	if (c == nullptr)
		return;

	c -> screen () -> setCRTEffect (false);
}

// ---
void MCHEmul::DatasetteStatusCommand::executeImpl 
	(MCHEmul::CommandExecuter* cE, MCHEmul::Computer* c, MCHEmul::InfoStructure& rst)
{
	if (c == nullptr)
		return;

	// Look for the datasette if any...
	MCHEmul::IOPeripherals prhs = std::move (c -> peripherals ()); // The list has been built up...
	MCHEmul::DatasettePeripheral* ds = nullptr;
	for (MCHEmul::IOPeripherals::const_iterator i = prhs.begin (); 
			i != prhs.end () && ds == nullptr; i++)
		ds = dynamic_cast <MCHEmul::DatasettePeripheral*> ((*i).second);

	if (ds != nullptr) 
		rst.add ("StdDatasette", std::move (ds -> getInfoStructure ()));
}
