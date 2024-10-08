#include "stdafx.h"

#include <iostream>

#include <FZ80/incs.hpp>

int _tmain (int argc, _TCHAR *argv [])
{
	// Notice that the simulation is executed not taking into account any buffering behaviour...
	// The buffering for the set command at memory level is not activated...

	MCHEmul::CommandLineArguments cmdArgs (argc, argv);
	unsigned int nTimes = (cmdArgs.existsArgument ('t')) ? cmdArgs.argumentAsInt ('t') : 1;
	bool output = (cmdArgs.existsArgument ('o')) ? cmdArgs.argumentAsBool ('o') : false;
	bool accessM = (cmdArgs.existsArgument ('m')) ? cmdArgs.argumentAsBool ('m') : false;

	std::cout << "TestZ80" << std::endl;
	std::cout << "Copyright (C) 2023 by Ignacio Cea" << std::endl;
	std::cout << "To test the different instructions of the Z80 family" << std::endl;
	std::cout << "/t[TIMES]\tNumber of times to test every instruction" << std::endl;
	std::cout << "/o\t\tTo print out the status of the CPU after every test" << std::endl;
	std::cout << "/m\t\tTo simulate the access to the memory." << std::endl << std::endl;

	MCHEmul::TestCPUSpeed (new FZ80::TestZ80 (new FZ80::FragmentatedMemoryTest)).
		testAllInstructionSet (std::cout, nTimes, accessM, output);

	return (0);
}

