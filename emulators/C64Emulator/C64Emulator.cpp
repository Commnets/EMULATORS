#include "stdafx.h"

#include <iostream>

#include <C64/C64.hpp>
#include <ASSEMBLER/Compiler.hpp>

using namespace C64;

#ifndef _CONSOLE
#include <SDL.h>
#ifdef __cplusplus
#define C_LINKAGE "C"
#else
#define C_LINKAGE
#endif /* __cplusplus */
#if _MSC_VER >= 1900
extern C_LINKAGE FILE __iob_func[3] = { *stdin,*stdout,*stderr };
#endif
extern C_LINKAGE int main(int argc, char *argv[])
#else
int _tmain (int argc, char *argv [])
#endif /* _CONSOLE */
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
			MCHEmul::Address iA; 
			std::vector <MCHEmul::UByte> bt = cL.asSetOfBytes (iA /** It is set. */);
			myComputer.memory () -> set (iA, bt);
			myComputer.startProgramAt (iA);
		}
	}

	return (0);
}
