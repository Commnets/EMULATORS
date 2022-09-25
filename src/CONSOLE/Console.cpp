#include <CONSOLE/Console.hpp>

// ---
Console::Console::Console (Emuls::Emulator* e, MCHEmul::CommandBuilder* cB)
	: _emulator (e), 
	  _commandBuilder (cB),
	  _command (""), _cursorPosition (0)
{ 
	assert (_emulator != nullptr && _commandBuilder != nullptr); 
}

// ----
void Console::Console::run ()
{
	bool exit = false;

	// Introduction
	std::cout << "Console" << std::endl;
	std::cout << "(C) By Ignacio Cea Forniés" << std::endl;
	std::cout << "Introduce command (QUIT to exit)" << std::endl;
	std::cout << "?:";

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
	if (_command == "QUIT")
		return (true);

	MCHEmul::Command* cmd = _commandBuilder -> createCommand (_command);
	if (cmd == nullptr)
		std::cout << "Command:" << _command << ", is not allowed" << std::endl;
	else
	{
		MCHEmul::Attributes rst; // The results...
		if (cmd -> execute (_emulator -> computer (), rst))
		{
			for (auto i : rst)
				std::cout << i.first << "=" << i.second << std::endl;
		}
		else
			std::cout << "The command:" << _command << ", can't be executed" << std::endl;

		delete (cmd);
		_command = ""; _cursorPosition = 0;
		std::cout << "?:";
	}

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
				if (std::isalnum ((int) chr))
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