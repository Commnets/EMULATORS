#include <CORE/StdCommands.hpp>
#include <CORE/Computer.hpp>
#include <CORE/Stack.hpp>
#include <CORE/Instruction.hpp>
#include <fstream>
#include <sstream>

// ---
MCHEmul::HelpCommand::HelpCommand (const std::string& hF)
	: Command (_ID), 
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
			MCHEmul::Strings hI;
			while (i != hls.end () && (*i)[0] != ';')
				hI.push_back ((*i++));
			_helpInfo.insert (std::pair <std::string, MCHEmul::Strings> (cN, hI));
		}
	}
}

// ---
void MCHEmul::HelpCommand::executeImpl (MCHEmul::Computer* c, MCHEmul::InfoStructure& rst)
{
	auto helpInfoCommand = [&](const std::string& cmd) -> void
		{
			bool fL = true;
			MCHEmul::HelpCommand::HelpInfo::const_iterator i;
			if ((i = _helpInfo.find (cmd)) != _helpInfo.end ())
			{
				std::string h = "";
				for (size_t j = 0; j < (*i).second.size (); j++)
					h += ((j == 0) ? '\0' : '\n') + (*i).second [j];
				rst.add (cmd, h);
			}
		};

	if (_parameters.size () == 0)
	{
		for (const auto& i : _helpInfo)
			helpInfoCommand (i.first);
	}
	else
		helpInfoCommand ((*_parameters.begin ()).first);
}

// ---
void MCHEmul::StatusRegisterStatusCommand::executeImpl (MCHEmul::Computer* c,  MCHEmul::InfoStructure& rst)
{
	rst = { }; rst.add ("SR", c -> cpu () -> statusRegister ().asString ());
}

// ---
void MCHEmul::RegistersStatusCommand::executeImpl (MCHEmul::Computer* c, MCHEmul::InfoStructure& rst)
{
	rst = c -> cpu () -> getInfoStructure ().infoStructure ("REGS");
}

// ---
void MCHEmul::ProgramCounterStatusCommand::executeImpl (MCHEmul::Computer* c, MCHEmul::InfoStructure& rst)
{
	rst = { }; rst.add ("PC", c -> cpu () -> programCounter ().asString ());
}

// ---
void MCHEmul::StackStatusCommand::executeImpl (MCHEmul::Computer* c, InfoStructure& rst)
{
	rst = c -> memory () -> stack () -> getInfoStructure ();
	if (_parameters.size () == 0 || (_parameters.size () == 1 && (*_parameters.find ("ALL")).second != "YES"))
		rst.remove ("MEMORY"); // Reduce the size...
}

// ---
MCHEmul::CPUStatusCommand::CPUStatusCommand ()
	: MCHEmul::ComplexCommand (_ID,
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
void MCHEmul::CPUInfoCommand::executeImpl (MCHEmul::Computer* c, MCHEmul::InfoStructure& rst)
{
	rst = c -> cpu () -> getInfoStructure ();
}

// ---
void MCHEmul::MemoryStatusCommand::executeImpl (MCHEmul::Computer* c, MCHEmul::InfoStructure& rst)
{
	rst = { };

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
void MCHEmul::StopCPUCommand::executeImpl (MCHEmul::Computer* c, MCHEmul::InfoStructure& rst)
{
	c -> setActionForNextCycle (MCHEmul::Computer::_ACTIONSTOP);

	MCHEmul::CPUStatusCommand ().execute (c, rst);
}

// ---
void MCHEmul::RunCPUCommand::executeImpl (MCHEmul::Computer* c, MCHEmul::InfoStructure& rst)
{
	c -> setActionForNextCycle (MCHEmul::Computer::_ACTIONCONTINUE);

	MCHEmul::CPUStatusCommand ().execute (c, rst);
}

// ---
void MCHEmul::NextInstructionCommand::executeImpl (MCHEmul::Computer* c, MCHEmul::InfoStructure& rst)
{
	c -> setActionForNextCycle (MCHEmul::Computer::_ACTIONNEXT);

	MCHEmul::CPUStatusCommand ().execute (c, rst);
}

// ---
void MCHEmul::LastIntructionCPUCommand::executeImpl (MCHEmul::Computer* c, MCHEmul::InfoStructure& rst)
{
	if (c -> cpu () -> lastInstruction () == nullptr)
		return; // It could happen when nothing has been executed yet...

	rst.add ("INST", c -> cpu () -> lastInstruction () -> asString ());
}
