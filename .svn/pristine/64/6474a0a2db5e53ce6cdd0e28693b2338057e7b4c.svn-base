#include <CONSOLE/Console.hpp>

// ---
Console::Console::Console (Emuls::Emulator* e, MCHEmul::CommandBuilder* cB)
	: _emulator (e), 
	  _commandBuilder (cB),
	  _command ("")
{ 
	assert (_emulator != nullptr && _commandBuilder != nullptr); 
}

// ----
void Console::Console::run ()
{
	bool exit = false;

	std::cout << "?:";
	while (!exit)
	{
		// Any command ready?
		if (readCommand ())
		{
			std::cout << std::endl;
			_command = MCHEmul::upper (MCHEmul::trim (_command));
			if (exit = (_command == "QUIT")) break; // The very common command to any interpreter...
			else
			{
				MCHEmul::Attributes prms;
				MCHEmul::Command* cmd = _commandBuilder -> createCommand (_command);
				if (cmd != nullptr /** good? */ && cmd -> canBeExecuted () /** possible? */)
				{
					MCHEmul::Attributes rst; // The results...
					if (!cmd -> execute (_emulator -> computer (), rst)) // error?
					{
					}
					else
					{
					}
				}
				else
				{
				}

				delete (cmd);
			}

			// Starts back!
			_command = "";
			std::cout << std::endl << "?:";
		}

		exit = !_emulator -> runCycle ();
	}
}

// ---
bool Console::Console::readCommand ()
{
	bool result = false;

	char chr;
	if (readChar (chr))
	{
		if (chr == 0x0d /** enter key code */) result = true; 
		else
		{
			std::cout << chr;

			_command += chr;
		}
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
		chr = static_cast <char> (_getch ());
	return (result); 
}
#endif