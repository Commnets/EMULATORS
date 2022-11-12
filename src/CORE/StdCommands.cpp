#include <CORE/StdCommands.hpp>
#include <CORE/Computer.hpp>
#include <CORE/Stack.hpp>
#include <CORE/Instruction.hpp>
#include <CORE/CmdExecuter.hpp>
#include <fstream>

// ---
const std::string MCHEmul::HelpCommand::_NAME = "CHELP";
const std::string MCHEmul::AuthorInfoCommand::_NAME = "CAUTHOR";
const std::string MCHEmul::StatusRegisterStatusCommand::_NAME =  "CSTATUS";
const std::string MCHEmul::RegistersStatusCommand::_NAME = "CREGISTERS";
const std::string MCHEmul::ProgramCounterStatusCommand::_NAME = "CPC";
const std::string MCHEmul::StackStatusCommand::_NAME = "CSTACK";
const std::string MCHEmul::CPUStatusCommand::_NAME = "CCPUSTATUS";
const std::string MCHEmul::CPUSimpleStatusCommand::_NAME = "CCPUSSTATUS";
const std::string MCHEmul::CPUInfoCommand::_NAME = "CCPUINFO";
const std::string MCHEmul::MemoryStatusCommand::_NAME = "CMEMORY";
const std::string MCHEmul::SetMemoryValueCommand::_NAME = "CSETMEMORY";
const std::string MCHEmul::StopCPUCommand::_NAME = "CSTOP";
const std::string MCHEmul::RunCPUCommand::_NAME = "CRUN";
const std::string MCHEmul::NextInstructionCommand::_NAME = "CNEXT";
const std::string MCHEmul::LastIntructionCPUCommand::_NAME = "CINST";
const std::string MCHEmul::ListOfBreakPointsCommand::_NAME = "CBREAKS";
const std::string MCHEmul::SetBreakPointCommand::_NAME = "CSETBREAK";
const std::string MCHEmul::RemoveBreakPointCommand::_NAME = "CREMOVEBREAK";
const std::string MCHEmul::RemoveAllBreakPointsCommand::_NAME = "CREMOVEBREAKS";
const std::string MCHEmul::CPUSpeedCommand::_NAME = "CSPEED";

// ---
MCHEmul::HelpCommand::HelpCommand (const std::string& hF)
	: Command (_ID, _NAME), 
	  _helpInfo ()
{
	MCHEmul::Strings hls;
	std::fstream hFile (hF, std::ios_base::in);
	if (hFile.is_open ())
	{
		char l [255];
		while (!hFile.eof ())
		{
			hFile.getline (l, 255);
			std::string ls = MCHEmul::trim (l);
			if (ls != "" && ls [0] == '#') continue; // Comments are not taken into account...
			hls.push_back (ls);
		}

		hFile.close ();
	}

	MCHEmul::Strings::const_iterator i = hls.begin ();
	while (i != hls.end ())
	{
		if ((*i)[0] != ';')
			continue; // A line to define a command is expected, otherwise ignored!
		else
		{
			std::string cN = MCHEmul::upper (MCHEmul::trim ((*i++).substr (1)));
			auto ecN = std::find_if (cN.begin (), cN.end (), std::isspace);
			cN = (ecN == cN.end ()) ? cN : cN.substr (0, cN.find (*ecN)); // Only the main word is inserted as key...
			MCHEmul::Strings hI;
			while (i != hls.end () && (*i)[0] != ';')
				hI.push_back ((*i++));
			_helpInfo.insert (std::pair <std::string, MCHEmul::Strings> (cN, hI));
		}
	}
}

// ---
void MCHEmul::HelpCommand::executeImpl (MCHEmul::CommandExecuter* cE, MCHEmul::Computer* c, MCHEmul::InfoStructure& rst)
{
	MCHEmul::InfoStructure iS;

	auto helpInfoCommand = [&](const std::string& cmd) -> void
		{
			bool fL = true;
			MCHEmul::HelpCommand::HelpInfo::const_iterator i;
			auto eCmd = std::find_if (cmd.begin (), cmd.end (), std::isspace);
			if ((i = _helpInfo.find (cmd)) != _helpInfo.end ())
			{
				std::string h = "";
				for (size_t j = 0; j < (*i).second.size (); j++)
					h += ((j == 0) ? '\0' : '\n') + (*i).second [j];
				iS.add ("->" + cmd, MCHEmul::removeAll0 (h));
			}
		};

	if (_parameters.size () == 0)
	{
		for (const auto& i : _helpInfo)
			helpInfoCommand (i.first);
	}
	else
		helpInfoCommand ((*_parameters.begin ()).first);

	rst.add ("HELP", iS);
}

// ---
void MCHEmul::AuthorInfoCommand::executeImpl (MCHEmul::CommandExecuter* cE, MCHEmul::Computer* c, MCHEmul::InfoStructure& rst)
{
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
	rst.add ("SR", c -> cpu () -> statusRegister ().asString ());
}

// ---
void MCHEmul::RegistersStatusCommand::executeImpl (MCHEmul::CommandExecuter* cE, MCHEmul::Computer* c, MCHEmul::InfoStructure& rst)
{
	rst.add ("REGS", c -> cpu () -> getInfoStructure ().infoStructure ("REGS"));
}

// ---
void MCHEmul::ProgramCounterStatusCommand::executeImpl (MCHEmul::CommandExecuter* cE, MCHEmul::Computer* c, MCHEmul::InfoStructure& rst)
{
	rst.add ("PC", c -> cpu () -> programCounter ().asString ());
}

// ---
void MCHEmul::StackStatusCommand::executeImpl (MCHEmul::CommandExecuter* cE, MCHEmul::Computer* c, InfoStructure& rst)
{
	MCHEmul::InfoStructure iS = c -> memory () -> stack () -> getInfoStructure ();
	if (_parameters.size () == 0 || (_parameters.size () == 1 && (*_parameters.find ("ALL")).second != "YES"))
		iS.remove ("Memory"); // Reduce the size...

	rst.add ("Stack", iS);
}

// ---
MCHEmul::CPUStatusCommand::CPUStatusCommand ()
	: MCHEmul::ComplexCommand (_ID, _NAME,
			MCHEmul::Commands (
				{ new StatusRegisterStatusCommand,
				  new RegistersStatusCommand,
				  new ProgramCounterStatusCommand,
				  new LastIntructionCPUCommand,
				  new StackStatusCommand
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
				}))
{
	// Nothing else...
}

// ---
void MCHEmul::CPUInfoCommand::executeImpl (MCHEmul::CommandExecuter* cE, MCHEmul::Computer* c, MCHEmul::InfoStructure& rst)
{
	rst.add ("CPU", c -> cpu () -> getInfoStructure ());
}

// ---
void MCHEmul::MemoryStatusCommand::executeImpl (MCHEmul::CommandExecuter* cE, MCHEmul::Computer* c, MCHEmul::InfoStructure& rst)
{
	MCHEmul::Address a1 = MCHEmul::Address::fromStr ((*_parameters.begin ()).first);
	if (_parameters.size () == 2)
	{
		MCHEmul::Address a2 = MCHEmul::Address::fromStr ((*++_parameters.begin ()).first);
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
	MCHEmul::Address a1 = MCHEmul::Address::fromStr ((*_parameters.begin ()).first);
	MCHEmul::Address a2 = a1;
	std::vector <MCHEmul::UByte> v;
	if (_parameters.size () == 3)
	{
		a2 = MCHEmul::Address::fromStr ((*++_parameters.begin ()).first);

		v = MCHEmul::UInt::fromStr ((*++++_parameters.begin ()).first).bytes ();
	}
	else
		v = MCHEmul::UInt::fromStr ((*++_parameters.begin ()).first).bytes ();

	MCHEmul::Address iA = (a1 <= a2) ? a1 : a2;
	MCHEmul::Address fA = (a2 >= a1) ? a2 : a1;
	for (size_t i = 0; i < fA - iA; i += v.size ())
		c -> cpu () -> memoryRef () -> set (iA + i, v); // Without force it!
}

// ---
void MCHEmul::StopCPUCommand::executeImpl (MCHEmul::CommandExecuter* cE, MCHEmul::Computer* c, MCHEmul::InfoStructure& rst)
{
	c -> setActionForNextCycle (MCHEmul::Computer::_ACTIONSTOP);

	MCHEmul::CPUStatusCommand ().execute (cE, c, rst);
}

// ---
void MCHEmul::RunCPUCommand::executeImpl (MCHEmul::CommandExecuter* cE, MCHEmul::Computer* c, MCHEmul::InfoStructure& rst)
{
	if (_parameters.size () == 1)
		c -> cpu () -> programCounter ().setAddress (MCHEmul::Address::fromStr ((*_parameters.begin ()).first));

	c -> setActionForNextCycle (MCHEmul::Computer::_ACTIONCONTINUE);

	MCHEmul::CPUStatusCommand ().execute (cE, c, rst);
}

// ---
void MCHEmul::NextInstructionCommand::executeImpl (MCHEmul::CommandExecuter* cE, MCHEmul::Computer* c, MCHEmul::InfoStructure& rst)
{
	c -> setActionForNextCycle (MCHEmul::Computer::_ACTIONNEXT);

	// To show status after execution...
	if (cE != nullptr)
		cE -> executeCommand (cE -> commandBuilder () -> command ("CPUSSTATUS"), c);
}

// ---
void MCHEmul::LastIntructionCPUCommand::executeImpl (MCHEmul::CommandExecuter* cE, MCHEmul::Computer* c, MCHEmul::InfoStructure& rst)
{
	rst.add ("INST", c -> cpu () -> lastInstruction () == nullptr 
		? "-" : c -> cpu () -> lastInstruction () -> asString ());
}

// ---
void MCHEmul::ListOfBreakPointsCommand::executeImpl (MCHEmul::CommandExecuter* cE, MCHEmul::Computer* c, MCHEmul::InfoStructure& rst)
{
	int ct = 0;
	std::string lst;
	for (const auto& i : c -> actions ())
		if (i.second == MCHEmul::Computer::_ACTIONSTOP) // Only if stopped...
			lst += ((ct++ != 0) ? "," : "\0") + i.first.asString (MCHEmul::UByte::OutputFormat::_HEXA, '\0', 2);
	lst = MCHEmul::removeAll0 (lst);

	rst.add ("BREAKPOINTS", lst);
}

// ---
void MCHEmul::SetBreakPointCommand::executeImpl (MCHEmul::CommandExecuter* cE, MCHEmul::Computer* c, MCHEmul::InfoStructure& rst)
{
	for (const auto& i : _parameters)
		c -> addAction (MCHEmul::Address::fromStr (i.first), MCHEmul::Computer::_ACTIONSTOP);
}

// ---
void MCHEmul::RemoveBreakPointCommand::executeImpl (MCHEmul::CommandExecuter* cE, MCHEmul::Computer* c, MCHEmul::InfoStructure& rst)
{
	for (const auto& i : _parameters)
	{
		MCHEmul::Address bP = MCHEmul::Address::fromStr (i.first);
		if (c -> action (bP) == MCHEmul::Computer::_ACTIONSTOP) // Only if stopped...
			c -> addAction (bP, MCHEmul::Computer::_ACTIONNOTHING);
	}
}

// ---
void MCHEmul::RemoveAllBreakPointsCommand::executeImpl (MCHEmul::CommandExecuter* cE, MCHEmul::Computer* c, MCHEmul::InfoStructure& rst)
{
	c -> removeAllActions (MCHEmul::Computer::_ACTIONSTOP);
}

// ---
void MCHEmul::CPUSpeedCommand::executeImpl (MCHEmul::CommandExecuter* cE, MCHEmul::Computer* c, MCHEmul::InfoStructure& rst)
{
	rst.add ("SPEED", c -> realCyclesPerSecond ());
}
