/** Program to get a set of VIC20 CRT dump and create a VIC20CRTFormat.
	That VIC20Format is like C64 CRT format.
	Take a look to VICE documentation. */

#include "CRTGenerator.hpp"

using namespace MCHEmul;
int main (int ac, char** av)
{
	std::cout << "VIC20 CRT file format generator" << std::endl
			  << "Created by Ignacio Cea Fornies" << std::endl
			  << "(C) 2023 - 2024" << std::endl
			  << "/h for help" << std::endl << std::endl;

	// Try to generate the file and... 
	// ...if something is wrong with that, all errors are printed out...
	// If -h were selected nothing else will be done...
	VIC20::CRTGenerator generator;
	bool result = generator.run (ac, av);
	if (!result) 
		std::cout << "Errors:" << std::endl
				  << generator.errorNames () << std::endl;
	else
		std::cout << "Steps:" << std::endl
				  << generator.steps () << std::endl;

	return (result ? 0 : 1);
}