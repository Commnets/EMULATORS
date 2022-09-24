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

#include <CPU/Incs.hpp>
#include <ASSEMBLER/Grammar.hpp>
#include <ASSEMBLER/Parser.hpp>

namespace MCHEmul
{
	namespace Assembler
	{
		/** The compiler reads the semantic and transform it into lines of bytes. */
		struct ByteCodeLine
		{
			ByteCodeLine ()
				: _address ({ 0x00 }), _bytes (), _label (""), _instruction (nullptr)
							{ }

			ByteCodeLine (const Address& a, const std::vector <UByte>& b, const std::string& n, const Instruction* i)
				: _address (a), _bytes (b), _label (n), _instruction (i)
							{ }

			ByteCodeLine (const ByteCodeLine&) = default;

			ByteCodeLine& operator = (const ByteCodeLine&) = default;

			friend std::ostream& operator << (std::ostream& o, const ByteCodeLine& c);

			const Address _address;
			const std::vector <UByte> _bytes;
			const std::string _label;
			const Instruction* _instruction;
		};

		/** To manage the set of bytecode lines. */
		struct ByteCode
		{
			ByteCode () = default;

			ByteCode (const ByteCode&) = default;

			ByteCode& operator = (const ByteCode&) = default;

			/** To load the info into the memory. */
			void loadIntoMemory (Memory* m);

			std::vector <ByteCodeLine> _lines;
		};

		/** The Compiler just to do the work. \n
			The Compiler is defined for a CPU too (the same than the Parser). \n
			The Compiler own the Parser. */
		class Compiler
		{
			public:
			Compiler (Parser* p)
				: _parser (p)
							{ assert (_parser != nullptr); }

			~Compiler ()
							{ delete (_parser); }

			ByteCode compile (const std::string& fN) const;

			const CPU* cpu () const
							{ return (_parser -> cpu ()); }
			CPU* cpu ()
							{ return (_parser -> cpu ()); }

			Errors errors () const
							{ return (_errors); }

			/** To simplify checking whether there was or not an error compiling. */
			bool operator ! () const
							{ return (!_errors.empty ()); }

			private:
			Parser* _parser;

			// Implementation
			mutable Errors _errors;
		};
	}
}

#endif
  
// End of the file
/*@}*/