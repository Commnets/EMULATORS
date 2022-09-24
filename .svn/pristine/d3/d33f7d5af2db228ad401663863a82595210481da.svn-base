#include <C64/C64.hpp>

#include <iostream>

using namespace C64;

int main()
{
	Commodore64 myComputer;

	if (myComputer.lastError () != MCHEmul::_NOERROR)
		return (1);

	std::cout << myComputer << std::endl;

	myComputer.run ();

	return (0);
}
