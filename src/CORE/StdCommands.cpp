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
			if (ls [0] != '#')
				hls.push_back (ls);
		}
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
				for (auto j : (*i).second)
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
		for (const auto i : _helpInfo)
		{
			if (!fH) ss << std::endl;
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
	for (const auto i : c -> cpu () -> internalRegisters ())
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
		ss << "Stack Initial: " << stk -> initialAddress () << 
			"(Size:" << stk -> size () << ", Position:" << stk -> position () << ")";
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
