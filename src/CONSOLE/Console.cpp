#include <CONSOLE/Console.hpp>
#include <CORE/CommandBuilder.hpp>
#include <fstream>

// ---
MCHEmul::Console::Console (MCHEmul::CommandBuilder* cB, 
		MCHEmul::ConsoleKeys* k, const std::string& cF, std::ostream& oS, size_t cK)
	: MCHEmul::CommandExecuter (_ID, cB),
	  _consoleKeys (k),
	  _outputStream (oS),
	  _maxCommandsKept (cK),
	  _clock (25), // Just 25 times per second is enough!
	  _command (""), _lastCommands (), 
	  _lastCommandPosition (0), _cursorPosition (0),
	  _consoleHold (false), // The console can be hold for a while...
	  _commandErrorTxt (""), _commandDoesnExitTxt (""), _welcomeTxt (""), _commandPrompt ("")
{ 
	assert (_consoleKeys != nullptr);

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
			ls.emplace_back (l);
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

	/** Start to introduce data!. */
	std::cout << _welcomeTxt;
	std::cout << _commandPrompt;

	bool lCH = false;
	while (!exit)
	{
		if (exit = runPerCycle ())
			continue;
		
		if (_consoleHold)
		{ 
			lCH = true;

			continue;
		}

		if (lCH)
		{
			lCH = false;
			// First time after being hold, the promt hast to be shown!...

			_outputStream << std::endl;

			std::cout << _commandPrompt;
		}

		if (isPendingCommands ())
		{
			if (!MCHEmul::CommandExecuter::executePendingCommands ())
				_outputStream << _commandErrorTxt << std::endl;

			_outputStream << std::endl;

			std::cout << _commandPrompt;
		}

		exit = readAndExecuteCommand ();

		lCH = _consoleHold;
	}
}

// ---
void MCHEmul::Console::manageAnswer (MCHEmul::Command* c, const MCHEmul::InfoStructure& rst)
{
	if (rst.empty ())
		return;

	_outputStream << MCHEmul::FormatterBuilder::instance () ->
		formatter (c -> name ()) -> format (rst) << std::endl; // It is used the formatter with the name of the command!
}

// ---
void MCHEmul::Console::manageErrorInExecution (MCHEmul::Command* c, const MCHEmul::InfoStructure& rst)
{
	_outputStream << _command << ":" << _commandErrorTxt << std::endl;
}

// ---
bool MCHEmul::Console::readAndExecuteCommand ()
{
	/** 
		When testing the speed, just to show that information periodically:
		static unsigned int _CC = 0;
		if (_CC++ < 300000)
			return (false);
		_CC = 0;
		_command = "SPEED";
	  */

	if (!readCommand ())
		return (false); // No command ready, no quit...

	_outputStream << std::endl;

	_command = MCHEmul::upper (MCHEmul::trim (_command));

	// The very standard commands...
	if (_command == "QUIT" || _command == "Q")
		return (true);

	// Then the rest of the commands that can be extended...
	// Saves the last command...
	// Only it is not the same than just the one before!
	if (_lastCommands.empty () || 
		(!_lastCommands.empty () && _lastCommands [_lastCommands.size () - 1] != _command))
	{
		_lastCommands.push_back (_command);
		if (_lastCommands.size () >= _maxCommandsKept) // No more than the max commands allowed to kept!
			_lastCommands = std::vector <std::string> (_lastCommands.begin ()++, _lastCommands.end ());
	}

	// One the enters is pressed...it will be the last new element...
	_lastCommandPosition = _lastCommands.size (); 

	createAndExecuteCommand ();

	// The comand has not to be destroyed
	// The command builder own all off them...
	// The same command is reused many times for performance reasons

	_command = ""; _cursorPosition = 0;

	if (!isPendingCommands () && !_consoleHold)
	{ 
		_outputStream << std::endl;

		std::cout << _commandPrompt;
	}

	// The command was executed, no quit...
	return (false);
}

// ---
bool MCHEmul::Console::readCommand ()
{
	static const std::string alKey (" ./$\\_");

	if (_clock.tooQuick ())
	{ 
		_clock.countCycles (0);

		return (false); // No command still read...
	}

	bool result = false;

	char chr;
	if (_consoleKeys -> readKey (chr))
	{
		size_t oPos = _cursorPosition;

		auto delCurrentCommand = [&]() -> void
			{	std::cout << MCHEmul::_BACKS.substr (0, _cursorPosition) // From the current position...
						  << MCHEmul::_SPACES.substr (0, _command.length ()) 
						  << MCHEmul::_BACKS.substr (0, _command.length ());
				oPos = 0; };

		switch (chr)
		{
			case MCHEmul::ConsoleKeys::_LEFTKEY:
				if (_cursorPosition != 0) 
					_cursorPosition--;
				break;

			case MCHEmul::ConsoleKeys::_RIGHTKEY:
				if (_cursorPosition < _command.length ()) 
					_cursorPosition++;
				break;

			case MCHEmul::ConsoleKeys::_UPKEY:
				if (_lastCommandPosition > 0)
				{ 
					delCurrentCommand ();
					_command = _lastCommands [--_lastCommandPosition];
					_cursorPosition = _command.length ();
				}

				break;

			case MCHEmul::ConsoleKeys::_DOWNKEY:
				if (_lastCommandPosition < (_lastCommands.size () - 1))
				{
					delCurrentCommand ();
					_command = _lastCommands [++_lastCommandPosition];
					_cursorPosition = _command.length ();
				}

				break;

			case MCHEmul::ConsoleKeys::_BACKKEY:
				if (_cursorPosition != 0)
				{
					_command = _command.substr (0, _cursorPosition - 1) + _command.substr (_cursorPosition);

					_cursorPosition--;
				}

				break;

			case MCHEmul::ConsoleKeys::_DELETEKEY:
				if (_cursorPosition != _command.length ())
					_command = _command.substr (0, _cursorPosition) + _command.substr (_cursorPosition + 1);
				break;

			case MCHEmul::ConsoleKeys::_ENTERKEY:
				result = true;
				break;

			case MCHEmul::ConsoleKeys::_BEGINKEY:
				_cursorPosition = 0;
				break;

			case MCHEmul::ConsoleKeys::_ENDKEY:
				_cursorPosition = _command.length ();
				break;

			default:
				if (chr >= 0 && 
					(std::isalnum ((int)chr) || alKey.find (chr) != std::string::npos))
				{
					if (_cursorPosition == _command.length ()) _command += chr;
					else if (_cursorPosition == 0) _command = chr + _command;
					else _command = _command.substr (0, _cursorPosition) + chr + _command.substr (_cursorPosition);

					_cursorPosition++;
				}

				break;
		}

		std::cout << MCHEmul::_BACKS.substr (0, oPos) 
				  << _command
				  << ' ' /** to support the deletion. */ 
				  << MCHEmul::_BACKS.substr (0, _command.length () - _cursorPosition + 1);
	}

	_clock.countCycles (1);

	return (result);
}
