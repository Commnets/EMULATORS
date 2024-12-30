#include "stdafx.h"

#include "Tests.hpp"
#include <CORE/incs.hpp>
#include <F6500/incs.hpp>

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

	std::cout << "TestInst6500" << std::endl;
	std::cout << "Copyright (C) 2024 by Ignacio Cea" << std::endl;
	std::cout << "To test (against results) the different instructions of the 6500 family" << std::endl;
	std::cout << "/h For help" << std::endl;
	std::cout << "/n[FILENAME]: For a bulk of tests. Tests.dt as devault value" << std::endl;
	std::cout << "/t[TESTPERINST]: With the max number of tests to do per instruction. 10 as default value" << std::endl << std::endl;

	bool h = false;
	std::string tId = "Tests.dt";
	unsigned int mTst = 10;
	if (argc > 1)
	{
		int ct = 0;
		MCHEmul::Strings ar = MCHEmul::convertIntoStrings (argc, argv);
		while (++ct < argc)
		{
			if (h = (ar [ct].substr (0, 2) == "/h"))
				break;
			if (ar [ct].substr (0, 3) == "/n:")
				tId = ar [ct].substr (3);
			else
			if (ar [ct].substr (0, 3) == "/t:")
				mTst = (unsigned int) std::atoi (ar [ct].substr (3).c_str ());
		}
	}

	if (h)
		return (0);

	Test test (tId, mTst);
	if (!test)
		std::cout << "Error in input files" << std::endl;
	else
		test.runTest (new F6500::Test6500 (new F6500::PlainMemoryTest));

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
