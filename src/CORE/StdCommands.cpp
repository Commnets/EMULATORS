#include <CORE/StdCommands.hpp>
#include <CORE/Computer.hpp>
#include <CORE/Stack.hpp>
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
void MCHEmul::HelpCommand::executeImpl (MCHEmul::Computer* c, MCHEmul::Attributes& rst)
{
	std::stringstream ss;

	auto helpInfoCommand = [&](const std::string& cmd) -> void
		{
			bool fL = true;
			MCHEmul::HelpCommand::HelpInfo::const_iterator i;
			if ((i = _helpInfo.find (cmd)) != _helpInfo.end ())
			{
				ss << "->" << (*i).first << std::endl;
				for (const auto& j : (*i).second)
				{
					if (!fL) ss << std::endl;
					ss << j;
					fL = false;
				}
			}
		};

	if (_parameters.size () == 0)
	{
		bool fH = true;
		for (const auto& i : _helpInfo)
		{
			if (!fH) ss << std::endl << std::endl;
			helpInfoCommand (i.first);
			fH = false;
		}
	}
	else
		helpInfoCommand ((*_parameters.begin ()).first);

	rst.insert (std::pair <std::string, std::string>  ("1", ss.str ()));
}

// ---
void MCHEmul::StatusRegisterStatusCommand::executeImpl (MCHEmul::Computer* c,  MCHEmul::Attributes& rst)
{
	std::stringstream ss;
	ss << c -> cpu () -> statusRegister ();
	rst.insert (std::pair <std::string, std::string>  ("1", ss.str ()));
}

// ---
void MCHEmul::RegistersStatusCommand::executeImpl (MCHEmul::Computer* c, MCHEmul::Attributes& rst)
{
	std::stringstream ss;

	bool fR = true;
	for (const auto& i : c -> cpu () -> internalRegisters ())
	{
		if (!fR) ss << std::endl;
		ss << i;
		fR = false;
	}

	rst.insert (std::pair <std::string, std::string>  ("1", ss.str ()));
}

// ---
void MCHEmul::ProgramCounterStatusCommand::executeImpl (MCHEmul::Computer* c, MCHEmul::Attributes& rst)
{
	std::stringstream ss;
	ss << c -> cpu () -> programCounter ();
	rst.insert (std::pair <std::string, std::string>  ("1", ss.str ()));
}

// ---
void MCHEmul::StackStatusCommand::executeImpl (MCHEmul::Computer* c, Attributes& rst)
{
	std::stringstream ss;

	// All information about the stack ins requested, including all values...
	if (_parameters.size () == 1 && (*_parameters.find ("ALL")).second == "YES")
		ss << (*c -> memory () -> stack ()); 
	// Just info about where it is defined, size and current position is requested...
	else
	{
		MCHEmul::Stack* stk = c -> memory () -> stack ();
		ss << "Stack Initial:" << stk -> initialAddress () << 
			" (Size:" << stk -> size () << ", Position:" << stk -> position () << ")";
	}

	rst.insert (std::pair <std::string, std::string>  ("1", ss.str ()));
}

// ---
void MCHEmul::CPUInfoCommand::executeImpl (MCHEmul::Computer* c, MCHEmul::Attributes& rst)
{
	std::stringstream ss;
	ss << *c -> cpu ();
	rst.insert (std::pair <std::string, std::string>  ("1", ss.str ()));
}

// ---
void MCHEmul::MemoryStatusCommand::executeImpl (MCHEmul::Computer* c, MCHEmul::Attributes& rst)
{
	std::stringstream ss;

	MCHEmul::Address a1 = MCHEmul::Address::fromStr ((*_parameters.begin ()).first);
	if (_parameters.size () == 2)
	{
		MCHEmul::Address a2 = MCHEmul::Address::fromStr ((*++_parameters.begin ()).first);
		MCHEmul::Address iA = ((a1 <= a2) ? a1 : a2);

		bool fB = true;
		size_t d = (((a2 >= a1) ? a2 : a1) - iA) + 1;
		for (size_t i = 0; i < d; i += 0x10, fB = false) // Blocks of 16 elements...
		{
			if (!fB) ss << std::endl;

			bool fE = true;
			for (size_t j = i; j < (i + 0x10) && j < d; j++, fE = false) // Every element in the block is seperated by an space...
				ss << (fE ? "" : " ") << c -> cpu () -> memoryRef () -> value (iA + j);
		}
	}
	else
		ss << c -> cpu () -> memoryRef () -> value (a1);

	rst.insert (std::pair <std::string, std::string>  ("1", ss.str ()));
}

// ---
void MCHEmul::StopCPUCommand::executeImpl (MCHEmul::Computer* c, MCHEmul::Attributes& rst)
{
	c -> setActionForNextCycle (MCHEmul::Computer::_ACTIONSTOP);
}

// ---
void MCHEmul::RunCPUCommand::executeImpl (MCHEmul::Computer* c, MCHEmul::Attributes& rst)
{
	c -> setActionForNextCycle (MCHEmul::Computer::_ACTIONCONTINUE);
}

// ---
void MCHEmul::NextInstructionCommand::executeImpl (MCHEmul::Computer* c, MCHEmul::Attributes& rst)
{
	c -> setActionForNextCycle (MCHEmul::Computer::_ACTIONNEXT);
}

// ---
void MCHEmul::LastIntructionCPUCommand::executeImpl (MCHEmul::Computer* c, MCHEmul::Attributes& rst)
{
	if (c -> cpu () -> lastInstruction () == nullptr)
		return; // It could happen when nothing has been executed yet...

	std::stringstream ss;
	ss << *c -> cpu () -> lastInstruction () << "(" << c -> cpu () -> programCounter () << ")";
	rst.insert (std::pair <std::string, std::string>  ("1", ss.str ()));
}
