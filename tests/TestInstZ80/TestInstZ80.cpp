#include "stdafx.h"

#include "Tests.hpp"
#include <CORE/incs.hpp>
#include <FZ80/incs.hpp>

// The main...
int _tmain (int argc, _TCHAR *argv [])
{
	// Notice that the simulation is executed not taking into account any buffering behaviour...
	// The buffering for the set command at memory level is not activated...

	auto printErrors = [&](const std::vector <std::string>& e) -> void
		{
			for (const auto& i : e)
				std::cout << i << std::endl;
		};

	std::cout << "TestInstZ80" << std::endl;
	std::cout << "Copyright (C) 2024 by Ignacio Cea" << std::endl;
	std::cout << "To test (against results) the different instructions of the Z80 family" << std::endl << std::endl;

	Test test ("tests.in", "tests.expected");
	if (!test)
		std::cout << "Error in input files" << std::endl;
	else
		test.runTest (new FZ80::TestZ80 (new FZ80::PlainMemoryTest));

	size_t nT = 0;
	std::cout << "Results:" << std::endl
				<< "Total Tests:" << test.totalTests () << std::endl;

	std::cout << "Tests not implemented:" << std::endl;
	printErrors (test.noimplemented ());
	std::cout << "Total: " << test.noimplemented ().size () << std::endl;
	std::cout << "----------" << std::endl;
	nT += test.noimplemented ().size ();

	std::cout << "Tests with errors:" << std::endl;
	printErrors (test.errors ());
	std::cout << "Total:" << test.errors ().size () << std::endl;
	std::cout << "----------" << std::endl;
	nT += test.errors ().size ();

	std::cout << "Tests with warnings:" << std::endl;
	printErrors (test.warnings ());
	std::cout << "Total:" << test.warnings ().size () << std::endl;
	std::cout << "----------" << std::endl;
	nT += test.warnings().size ();

	std::cout << "Tests Ok:" << (test.totalTests () - nT) << std::endl;

	return (test.errors ().empty () ? 0 : 1);
}
