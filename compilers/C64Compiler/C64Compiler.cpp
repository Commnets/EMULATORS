#include "stdafx.h"

#include <ASSEMBLER/incs.hpp>
#include <F6500/incs.hpp>

/** Execute with /h to get info about the command available. */
int _tmain (int argc, _TCHAR *argv [])
{
	bool help = false;
	std::string originFile;
	std::string destinationFile;
	enum { _BINARY = 0, _BLOCK } typeFile = _BINARY;

	MCHEmul::Strings prms = MCHEmul::convertIntoStrings (argc, argv);
	for (const auto& i : prms)
	{
		std::map <unsigned char, std::string>::const_iterator p;
		if (i.length () < 2 || i [0] != '/')
			continue; // Not valid argument...
		else
		{
			if (i [1] == 'h') help = true;
			else if (i [1] == 'o') originFile = MCHEmul::trim (i.substr (2));
			else if (i [1] == 'd') destinationFile = MCHEmul::trim (i.substr (2));
			else if (i [1] == 't') typeFile = 
				(std::atoi (MCHEmul::trim (i.substr (2)).c_str ()) == 0) ? _BINARY : _BLOCK;
		}
	}

	if (help || 
		originFile == "" || destinationFile == "")
	{
		std::cout << "Copyright (2) 2022 by Ignacio Cea" << std::endl << std::endl;

		if (!help)
			std::cout << "Lack of parameters" << std::endl;

		std::cout << "/h\t\tFor help." << std::endl;
		std::cout << "/o[FILENAME]\tName of the file to compile." << std::endl;
		std::cout << "/d[FILENAME]\tName of the file to save the result if everything goes ok." << std::endl;
		std::cout << "/t\t\t0 for BINARY output, 1 for BLOCK output." << std::endl;

		return (0);
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

		default:
			std::cout << "Type of output format nor supported" << std::endl;
	}

	if (!cR)
		std::cout << "Error creating file" << std::endl;
	else
		std::cout << "File created with no errors" << std::endl;

	delete (prs);
	delete (c);
	delete (cpu);

	return (0);
}
