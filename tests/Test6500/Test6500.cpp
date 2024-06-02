#include "stdafx.h"

#include <iostream>

#include <F6500/incs.hpp>

int _tmain (int argc, _TCHAR *argv [])
{
	MCHEmul::CommandLineArguments cmdArgs (argc, argv);
	unsigned int nTimes = (cmdArgs.existsArgument ('t')) ? cmdArgs.argumentAsInt ('t') : 1;
	bool output = (cmdArgs.existsArgument ('o')) ? cmdArgs.argumentAsBool ('o') : false;
	bool accessM = (cmdArgs.existsArgument ('m')) ? cmdArgs.argumentAsBool ('m') : false;

	std::cout << "Test6500" << std::endl;
	std::cout << "Copyright (C) 2022 by Ignacio Cea" << std::endl;
	std::cout << "To test the different instructions of the 6500 family" << std::endl;
	std::cout << "/t[TIMES]\tNumber of times to test every instruction" << std::endl;
	std::cout << "/o\t\tTo print out the status of the CPU after every test" << std::endl;
	std::cout << "/m\t\tTo simulate the access to the memory." << std::endl << std::endl;

	MCHEmul::TestCPUSpeed (new F6500::Test6500 (new F6500::FragmentatedMemoryTest)).
		testAllInstructionSet (std::cout, nTimes, accessM, output);

	return (0);
}
