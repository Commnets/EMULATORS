#include <C64/C64.hpp>
#include <language/Parser.hpp>

#include <iostream>

using namespace C64;

int main ()
{
	// PAL by default...
	Commodore64 myComputer;

	if (myComputer.lastError () != MCHEmul::_NOERROR)
		return (1);

	MCHEmul::Parser parser (myComputer.cpu ());
	MCHEmul::Parser::Code c = parser.parse ("./test.asm");
	if (!parser)
		for (auto i : c)
			std::cout << i <<std::endl;
	else
		std::cout << "error" << std::endl;

	std::cout << myComputer << std::endl;

	myComputer.run ();

	return (0);
}
