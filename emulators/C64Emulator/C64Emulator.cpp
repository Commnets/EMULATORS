#include <iostream>

#include <C64/C64.hpp>
#include <ASSEMBLER/Compiler.hpp>

using namespace C64;

int main ()
{
	// PAL by default...
	Commodore64 myComputer;

	if (myComputer.lastError () != MCHEmul::_NOERROR)
		return (1);

	// The other type of parser...
	MCHEmul::Assembler::Compiler compiler (new MCHEmul::Assembler::Parser (myComputer.cpu ()));
	MCHEmul::Assembler::ByteCode cL = compiler.compile ("./test.asm");
	if (!compiler)
	{
		for (auto i : compiler.errors ())
			std::cout << i << std::endl;
	}
	else
	{
		for (auto i : cL._lines)
			std::cout << i << std::endl;

		if (myComputer.initialize ())
		{
			cL.loadIntoMemory (myComputer.cpu () -> memoryRef ());
			myComputer.runFrom (MCHEmul::Address ({ 0x00, 0xc0 }, false));
		}
	}

	return (0);
}
