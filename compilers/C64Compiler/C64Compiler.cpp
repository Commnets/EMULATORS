#include "stdafx.h"

#include <ASSEMBLER/incs.hpp>
#include <F6500/incs.hpp>

/** Execute with /h to get info about the command available. */
int _tmain (int argc, _TCHAR *argv [])
{
	bool help = false;
	std::string originFile;
	std::string destinationFile;
	std::string formatterFile;
	enum { _BINARY = 0, _BLOCK, _DATATEXT } typeFile = _BINARY;

	std::cout << "C64Compiler" << std::endl;
	std::cout << "Copyright (2) 2022 by Ignacio Cea" << std::endl << std::endl;

	// Read the parameters of the command line...
	MCHEmul::CommandLineArguments cmdArgs (argc, argv);
	help = cmdArgs.existsArgument ('h');
	if (cmdArgs.existsArgument ('o')) originFile = cmdArgs.argumentAsString ('o');
	if (cmdArgs.existsArgument ('d')) destinationFile = cmdArgs.argumentAsString ('d');
	if (cmdArgs.existsArgument ('t')) typeFile = (cmdArgs.argumentAsInt ('t') == 0) 
			? _BINARY : ((cmdArgs.argumentAsInt ('t') == 1) ? _BLOCK : _DATATEXT);
	if (cmdArgs.existsArgument ('f')) formatterFile = cmdArgs.argumentAsString ('f');
	if (formatterFile == "" && typeFile == _DATATEXT) formatterFile = ".\\blkFormatter.fmt"; // It is ncessesary
	
	// If either the mandatory arguments hasn't be provided or help is requested...
	if (help || 
		originFile == "" || destinationFile == "")
	{
		if (!help)
			std::cout << "Lack of parameters" << std::endl;

		std::cout << "/h\t\tFor help" << std::endl;
		std::cout << "/o[FILENAME]\tName of the file to compile" << std::endl;
		std::cout << "/d[FILENAME]\tName of the file to save the result if everything goes ok" << std::endl;
		std::cout << "/t[TYPE]\t0 for BINARY output, 1 for BLOCK output, 2 for FORMATTED BLOCK output" << std::endl;
		std::cout << "/f[FILENAME]\tName of the formatter file when /t=2. blkFormatter.fmt if not defined" << std::endl;

		return (0); // ...and does nothing...
	}

	// Creates the environment...
	// Creates un compiler for the 6510 instruction, and it is not right, then finishes...
	F6500::C6510* cpu = new F6500::C6510;
	MCHEmul::Assembler::Parser* prs = new MCHEmul::Assembler::Parser (cpu);
	prs -> setPrintOutProcess (true); // To print out the progress...
	MCHEmul::Assembler::Compiler* c = new MCHEmul::Assembler::Compiler (prs);

	// Compile...
	std::cout << "Parsing & Compiling:" << std::endl;
	MCHEmul::Assembler::ByteCode rst = c -> compile (originFile);
	if (!*c)
	{
		std::cout << "There were errors:" << std::endl;
		for (const auto& i : c -> errors ())
			std::cout << i << std::endl;

		delete (c);
		delete (cpu);

		return (0);
	}
	else
		std::cout << "Compilation finishes with no errors" << std::endl;

	// Save the ourcome attending to the type...
	bool cR = false;
	switch (typeFile)
	{
		case _BINARY:
			std::cout << "Saving the result as a binary file: " << destinationFile << std::endl;
			cR = rst.saveAsBinary (destinationFile, cpu);
			break;

		case _BLOCK:
			std::cout << "Saving the result as a block file: " << destinationFile << std::endl;
			cR = rst.saveAsBlocks (destinationFile, cpu);
			break;

		case _DATATEXT:
			std::cout << "Saving the result as a block formatter file: " << destinationFile << std::endl;
			cR = rst.saveAsBlocksFormatter (destinationFile, formatterFile);
			break;

		default:
			std::cout << "Type of output format nor supported" << std::endl;
	}

	std::cout << 
		((!cR) ? "Error creating file" : "File created with no errors") << std::endl;

	delete (prs);
	delete (c);
	delete (cpu);

	return (0);
}
