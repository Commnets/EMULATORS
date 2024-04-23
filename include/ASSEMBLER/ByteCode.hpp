/** \ingroup ASSEMBLER */
/*@{*/

/**	
*	@file	
*	File: ByteCodeLine.hpp \n
*	Framework: CPU Emulators library \n
*	Author: Ignacio Cea Fornies (EMULATORS library) \n
*	Creation Date: 27/06/2021 \n
*	Description: To manage a line of code.
*	Versions: 1.0 Initial
*/

#ifndef __ASSEMBLER_BYTECODE__
#define __ASSEMBLER_BYTECODE__

#include <CORE/incs.hpp>

namespace MCHEmul
{
	namespace Assembler
	{
		/** To manage the set of bytecode lines. \n
			The ByteCode structure is what the many actions over the memory produces. \n
			It can be load from the memory and also saved into files with the two basic formats supported
			by the emulation: Binary and Set of Blocks. */
		struct ByteCode final
		{
			// To returns the ByeCode in things that can be loaded / saved into the memory or files...
			/** Returns the initial address in the iA variable. 
				The addresses with no info are filled with 0x00. */
			std::vector <UByte> asSetOfBytes (Address& iA) const;
			/** Returns the info like a memory block. */
			DataMemoryBlock asDataMemoryBlock () const;
			/** Returns a set of code blocks. */
			DataMemoryBlocks asDataMemoryBlocks () const;

			/** To get the list of actions per address. 
				Action == 0 are not actions! */
			std::map <Address, unsigned int> listOfActions () const;

			/** To load the info into the memory. */
			void loadIntoMemory (Memory* m);

			/** To load from a file is not possible from here,
				because there is no format to keep ByteCodeLine. */

			// Save to a file...
			/** To save the info into a binary file. \n
				The starting address is defined at the header of the file. */
			bool saveAsBinary (const std::string& fN, const CPU* cpu) const;
			/** To save the ByteCode as a set of blocks. \n
				Every block defines its starting address and its size at the header. */
			bool saveAsBlocks (const std::string& fN, const CPU* cpu) const;
			/** Save its data blocks as per defined in formatter block file. */
			bool saveAsBlocksFormatter (const std::string& fN, const std::string& fF = ".\\blkFormatter.fmt") const;

			/** To create an object from the memory. \n 
			 	It is really to decompile the memory! (@see MCHEmul::Instruction). */
			static ByteCode createFromMemory (const Address& a, unsigned int b, Memory* m, Computer* cpu);

			std::vector <ByteCodeLine> _lines;
		};
	}
}

#endif

// End of the file
/*@}*/