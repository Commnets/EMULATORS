#include <CONSOLE/Console.hpp>
#include <fstream>

// ---
Console::Console::Console (Emuls::Emulator* e, 
		MCHEmul::CommandBuilder* cB, const std::string& cF, std::ostream& oS)
	: _emulator (e), 
	  _commandBuilder (cB),
	  _outputStream (oS),
	  _command (""), _cursorPosition (0),
	  _commandErrorTxt (""), _commandDoesnExitTxt (""), _welcomeTxt (""), _commandPrompt ("")
{ 
	assert (_emulator != nullptr && _commandBuilder != nullptr); 

	_commandErrorTxt = "Command can't be executed";
	_commandDoesnExitTxt = "Command doesn't exist";
	_welcomeTxt = "";
	_commandPrompt = "?:";

	MCHEmul::Strings ls;
	std::ifstream cFile (cF, std::ios_base::in);
	if (cFile.is_open ())
	{
		char nl [255];
		while (!cFile.eof ())
		{
			cFile.getline (nl, 255);
			std::string l = MCHEmul::trim (std::string (nl));
			if (l [0] != '#') ls.push_back (l);
		}

		cFile.close ();
	}

	if (ls.size () > 0) _commandErrorTxt = ls [0];
	if (ls.size () > 1) _commandDoesnExitTxt = ls [1];
	if (ls.size () > 2) _commandPrompt = ls  [2];
	if (ls.size () > 3) for (size_t i = 3; i < ls.size (); _welcomeTxt += ls [i++] + '\n');
}

// ----
void Console::Console::run ()
{
	bool exit = false;

	std::cout << _welcomeTxt;
	std::cout << _commandPrompt;

	while (!exit)
	{
		if ((exit = readAndExecuteCommand ()))
			continue;
		exit = !_emulator -> runCycle ();
	}
}

// ---
bool Console::Console::readAndExecuteCommand () 
{
	if (!readCommand ())
		return (false); // No command ready...

	std::cout << std::endl;
	_command = MCHEmul::upper (MCHEmul::trim (_command));
	if (_command == "QUIT" || _command == "Q")
		return (true);

	MCHEmul::Command* cmd = _commandBuilder -> command (_command);
	if (cmd == nullptr)
		std::cout << _command << ":" << _commandDoesnExitTxt << std::endl;
	else
	{
		MCHEmul::Attributes rst; // The results...
		if (cmd -> execute (_emulator -> computer (), rst))
			for (const auto i : rst)
				std::cout << i.second << std::endl;
		else
			std::cout << _command << ":" << _commandErrorTxt << std::endl;
	}

	// The comand has not to be destroyed
	// The command builder own all off them...

	_command = ""; _cursorPosition = 0;
	std::cout << _commandPrompt;

	return (false);
}

// ---
bool Console::Console::readCommand ()
{
	static const std::string bk (200, '\b');

	bool result = false;

	char chr;
	if (readChar (chr))
	{
		size_t oPos = _cursorPosition;

		switch (chr)
		{
			case _LEFTKEY:
				if (_cursorPosition != 0) 
					_cursorPosition--;
				break;

			case _RIGHTKEY:
				if (_cursorPosition < _command.length ()) 
					_cursorPosition++;
				break;

			case _BACKKEY:
				if (_cursorPosition != 0)
				{
					_command = _command.substr (0, _cursorPosition - 1) + _command.substr (_cursorPosition);

					_cursorPosition--;
				}

				break;

			case _DELETEKEY:
				if (_cursorPosition != _command.length ())
					_command = _command.substr (0, _cursorPosition) + _command.substr (_cursorPosition + 1);
				break;

			case _ENTERKEY:
				result = true;
				break;

			default:
				if (chr >= 0 && 
					(std::isalnum ((int) chr) || chr == ' ' || chr == '$'))
				{
					if (_cursorPosition == _command.length ()) _command += chr;
					else if (_cursorPosition == 0) _command = chr + _command;
					else _command = _command.substr (0, _cursorPosition) + chr + _command.substr (_cursorPosition);

					_cursorPosition++;
				}

				break;
		}

		std::cout << bk.substr (0, oPos) << _command 
			<< ' ' /** to support the deletion. */ << bk.substr (0, _command.length () - _cursorPosition + 1);
	}

	return (result);
}

#ifdef _WIN32
#include <conio.h>
// ---
bool Console::Win32Console::readChar (char& chr) const 
{ 
	bool result = false;
	if (result = (_kbhit () != 0))
	{
		int k = _getch ();
		if (k == 0 || k == 0xe0)
			k += _getch ();

		switch (k)
		{
			case 299:
				chr = _LEFTKEY;
				break;

			case 301:
				chr = _RIGHTKEY;
				break;

			case 13:
				chr = _ENTERKEY;
				break;

			case 8:
				chr = _BACKKEY;
				break;

			case 83: // The one in the key pad...
			case 307: // The normal one...
				chr = _DELETEKEY;
				break;

			default:
				chr = static_cast <char> (k);
		}
	}

	return (result); 
}
#endif