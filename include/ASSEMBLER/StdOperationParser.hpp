/** \ingroup ASSEMBLER */
/*@{*/

/**	
 *	@file	
 *	File: FormulaParser.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Forniés (EMULATORS library) \n
 *	Creation Date: 15/11/2022 \n
 *	Description: The standard OperationParser. It supports the StdOperationElements (@see StdOperation.hpp file).
 *	Versions: 1.0 Initial
 */

#ifndef __ASSEMBLER_STDOPERATIONPARSER__
#define __ASSEMBLER_STDOPERATIONPARSER__

#include <ASSEMBLER/OperationParser.hpp>

namespace MCHEmul
{
	namespace Assembler
	{
		/** Just to manage the very basic operations. No functions are considered. */
		class StdOperationParser : public OperationParser
		{
			public:
			StdOperationParser () = default;

			protected:
			virtual OperationElement* createBinaryOperationElement (unsigned char s,
				OperationElement* l, OperationElement* r) const override;
			virtual OperationElement* createUnaryOperationElement (unsigned char s, OperationElement* o) const override;
			virtual OperationElement* createFunctionOperationElement (const OperationElements& oE) const override
							{ return (nullptr); }

			virtual std::string validBinarySymbols () const override 
							{ return ("+-*/"); }
			virtual std::string validUnarySymbols () const override 
							{ return ("><!"); }
			virtual std::vector <std::string> validFunctionNames () const override 
							{ return (std::vector <std::string> ()); }
		};
	}
}

#endif
  
// End of the file
/*@}*/