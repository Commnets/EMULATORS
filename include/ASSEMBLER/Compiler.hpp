/** \ingroup ASSEMBLER */
/*@{*/

/**	
 *	@file	
 *	File: Compiler.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Forniés (EMULATORS library) \n
 *	Creation Date: 04/06/2021 \n
 *	Description: To convert an assembler semantic into byte codes.
 *	Versions: 1.0 Initial
 */

#ifndef __ASSEMBLER_COMPILER__
#define __ASSEMBLER_COMPILER__

#include <CORE/incs.hpp>
#include <ASSEMBLER/Grammar.hpp>
#include <ASSEMBLER/Parser.hpp>
#include <ASSEMBLER/StdOperationParser.hpp>

namespace MCHEmul
{
	namespace Assembler
	{
		/** The compiler reads the semantic and transform it into lines of bytes. */
		struct ByteCodeLine final
		{
			ByteCodeLine ()
				: _address ({ 0x00 }), _bytes (), _label (""), _instruction (nullptr), _actionOn (0)
							{ }

			ByteCodeLine (const Address& a, const std::vector <UByte>& b, const std::string& n, 
					const Instruction* i, unsigned int act)
				: _address (a), _bytes (b), _label (n), _instruction (i), _actionOn (act)
							{ }

			ByteCodeLine (const ByteCodeLine&) = default;

			ByteCodeLine& operator = (const ByteCodeLine&) = default;

			friend std::ostream& operator << (std::ostream& o, const ByteCodeLine& c);

			Address _address;
			std::vector <UByte> _bytes;
			std::string _label;
			const Instruction* _instruction;
			unsigned int _actionOn;
		};

		/** To manage the set of bytecode lines. \n
			The ByteCode structure is what the compilation process produces. \n
			It can be load into the memory and saved into files with the two basic formats supported
			by the emulation: Binary and Set of Blocks. */
		struct ByteCode final
		{
			ByteCode () = default;

			ByteCode (const ByteCode&) = default;

			ByteCode& operator = (const ByteCode&) = default;

			// To returns the ByeCode in things that can be lodaded / saved into the memory....
			/** Returns the initial address in the iA variable. 
				The addresses with no info are filled with 0x00. */
			std::vector <UByte> asSetOfBytes (Address& iA) const;
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

			static ByteCode createFromMemory (const Address& a, unsigned int b, Memory* m, CPU* cpu);

			std::vector <ByteCodeLine> _lines;
		};

		/** The Compiler just to do the work. \n
			The Compiler is defined for a CPU too (the same than the Parser). \n
			The Compiler doesn't own the Parser. \n
			The Compiler can be overloaded. \n
			To generate the code sometimes a OperationParser is needed. \n
			The Operation Parser is own by the Compiler. */
		class Compiler
		{
			public:
			Compiler (Parser* p)
				: _parser (p),
				  _operationParser (nullptr),
				  _errors ()
							{ assert (p != nullptr); }

			~Compiler ()
							{ delete (_operationParser); }

			/** The key method. Generic the Byte Codes to by load and executed.
				It receives the file with the instructions as parameter.
				A file with the extension ".act" from the previous one will be looked for. 
				This file could have commands on the code like stop it (break points). */
			ByteCode compile (const std::string& fN) const;

			const CPU* cpu () const
							{ return (_parser -> cpu ()); }

			const OperationParser* operationParser () const
							{ return ((_operationParser == nullptr) 
								? _operationParser = createOperationParser () : _operationParser); }
			OperationParser* operationParser ()
							{ ((OperationParser*) ((const Compiler *) this) -> operationParser ()); }

			Errors errors () const
							{ return (_errors); }
			/** To simplify checking whether there was or not an error compiling. */
			bool operator ! () const
							{ return (!_errors.empty ()); }

			protected:
			virtual OperationParser* createOperationParser () const
							{ return (new StdOperationParser); }

			protected:
			Parser* _parser;

			// Implementation
			mutable OperationParser* _operationParser;
			mutable Errors _errors;
		};
	}
}

#endif
  
// End of the file
/*@}*/