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
		/** The compiler reads the semantic and transform it into codelines bytes. */
		struct CodeLine
		{
			CodeLine ()
				: _address ({ 0x00 }), _bytes (), _label (""), _instruction (nullptr)
							{ }

			CodeLine (const Address& a, const std::vector <UByte>& b, const std::string& n, const Instruction* i)
				: _address (a), _bytes (b), _label (n), _instruction (i)
							{ }

			CodeLine (const CodeLine&) = default;

			CodeLine& operator = (const CodeLine&) = default;

			friend std::ostream& operator << (std::ostream& o, const CodeLine& c);

			const Address _address;
			const std::vector <UByte> _bytes;
			const std::string _label;
			const Instruction* _instruction;
		};

		/** The code will be just a set of code lines. */
		using CodeLines = std::vector <CodeLine>;

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

			CodeLines compile (const std::string& fN) const;

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