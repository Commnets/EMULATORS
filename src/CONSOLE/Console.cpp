#include <CONSOLE/Console.hpp>
#include <CORE/CommandBuilder.hpp>
#include <fstream>

// ---
MCHEmul::Console::Console (MCHEmul::Emulator* e,
		MCHEmul::CommandBuilder* cB, const std::string& cF, std::ostream& oS, size_t cK)
	: MCHEmul::CommandExecuter (_ID, cB), 
	  _emulator (e), 
	  _outputStream (oS),
	  _command (""), _lastCommands (), 
	  _lastCommandPosition (0), _cursorPosition (0),
	  _commandErrorTxt (""), _commandDoesnExitTxt (""), _welcomeTxt (""), _commandPrompt ("")
{ 
	assert (_emulator != nullptr); 

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
			if (l != "" && l [0] == '#') continue; // Comments are not taken into account...
			ls.push_back (l);
		}

		cFile.close ();
	}

	if (ls.size () > 0) _commandErrorTxt = ls [0];
	if (ls.size () > 1) _commandDoesnExitTxt = ls [1];
	if (ls.size () > 2) _commandPrompt = ls  [2];
	if (ls.size () > 3) for (size_t i = 3; i < ls.size (); _welcomeTxt += ls [i++] + '\n');
}

// ----
void MCHEmul::Console::run ()
{
	bool exit = false;

	/** Print out basic info about the computer managed. */
	_outputStream << "---- CPU ---" << std::endl
				  << *_emulator -> computer () -> cpu () << std::endl;
	_outputStream << "---- STACK -" << std::endl
				  << *_emulator -> computer () -> memory () -> stack () << std::endl << std::endl;

	/** Start to introduce data!. */
	std::cout << _welcomeTxt;
	std::cout << _commandPrompt;

	while (!exit)
	{
		if (isPendingCommands ())
		{
			if (!MCHEmul::CommandExecuter::executePendingCommands ())
				_outputStream << _commandErrorTxt << std::endl;
			_outputStream << std::endl;

			std::cout << _commandPrompt;
		}

		if ((exit = readAndExecuteCommand ()))
			continue;

		exit = !_emulator -> runCycle ();
	}
}

// ---
bool MCHEmul::Console::readAndExecuteCommand ()
{
	if (!readCommand ())
		return (false); // No command ready, no quit...

	_outputStream << std::endl;

	_command = MCHEmul::upper (MCHEmul::trim (_command));
	if (_command == "QUIT" || _command == "Q")
		return (true);

	// Saves the last command...
	// Only it is not the same than just the one before!
	if (_lastCommands.empty () || 
		(!_lastCommands.empty () && _lastCommands [_lastCommands.size () - 1] != _command))
	{
		_lastCommands.push_back (_command);
		if (_lastCommands.size () >= _maxCommandsKept) // No more than the max commands allowed to kept!
			_lastCommands = std::vector <std::string> (_lastCommands.begin ()++, _lastCommands.end ());
		_lastCommandPosition = _lastCommands.size (); // One the enters is pressed...it will be the last new element...
	}

	MCHEmul::Command* cmd = commandBuilder () -> command (_command);
	if (cmd == nullptr) _outputStream << _command << ":" << _commandDoesnExitTxt << std::endl;
	else executeCommandNow (cmd, _emulator -> computer ());

	// The comand has not to be destroyed
	// The command builder own all off them...
	// The same command is reused many times for performance reasons

	_command = ""; _cursorPosition = 0;

	_outputStream << std::endl;
	if (!isPendingCommands ())
		std::cout << _commandPrompt;

	// The command was executed, no quit...
	return (false);
}

// ---
bool MCHEmul::Console::readCommand ()
{
	static const std::string bk (200, '\b');

	bool result = false;

	char chr;
	if (readChar (chr))
	{
		size_t oPos = _cursorPosition;

		auto delCurrentCommand = [&]() -> void
			{	std::cout << bk.substr (0, _cursorPosition) // From the current position...
						  << MCHEmul::_SPACES.substr (0, _command.length ()) << bk.substr (0, _command.length ());
				oPos = 0; };

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

			case _UPKEY:
				if (_lastCommandPosition > 0)
				{ 
					delCurrentCommand ();
					_command = _lastCommands [--_lastCommandPosition];
					_cursorPosition = _command.length ();
				}

				break;

			case _DOWNKEY:
				if (_lastCommandPosition < (_lastCommands.size () - 1))
				{
					delCurrentCommand ();
					_command = _lastCommands [++_lastCommandPosition];
					_cursorPosition = _command.length ();
				}

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

// ---
void MCHEmul::Console::manageAnswer (MCHEmul::Command* c, const MCHEmul::InfoStructure& rst)
{
	_outputStream << MCHEmul::FormatterBuilder::instance () ->
		formatter (c -> name ()) -> format (rst) << std::endl; // It is used the formatter with the name of the command!
}

// ---
void MCHEmul::Console::manageErrorInExecution (MCHEmul::Command* c, const MCHEmul::InfoStructure& rst)
{
	_outputStream << _command << ":" << _commandErrorTxt << std::endl;
}

#ifdef _WIN32
#include <conio.h>
// ---
bool MCHEmul::Win32Console::readChar (char& chr) const
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

			case 296:
				chr = _UPKEY;
				break;

			case 304:
				chr = _DOWNKEY;
				break;

			case 13:
				chr = _ENTERKEY;
				break;

			case 8:
				chr = _BACKKEY;
				break;

			case 307:
				chr = _DELETEKEY;
				break;

			default:
				chr = static_cast <char> (k);
		}
	}

	return (result); 
}
#endif