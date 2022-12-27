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
#include <ASSEMBLER/ByteCode.hpp>
#include <ASSEMBLER/Grammar.hpp>
#include <ASSEMBLER/Parser.hpp>
#include <ASSEMBLER/StdOperationParser.hpp>

namespace MCHEmul
{
	namespace Assembler
	{
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