/** \ingroup ASSEMBLER */
/*@{*/

/**	
 *	@file	
 *	File: FormulaParser.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Forniés (EMULATORS library) \n
 *	Creation Date: 15/11/2022 \n
 *	Description: To parser an Operation and creates the tree of OperationElements. 
 *	Versions: 1.0 Initial
 */

#ifndef __ASSEMBLER_FORMULAPARSER__
#define __ASSEMBLER_FORMULAPARSER__

#include <CORE/incs.hpp>
#include <ASSEMBLER/Operation.hpp>

namespace MCHEmul
{
	namespace Assembler
	{
		class OperationParser
		{
			public:
			OperationParser () = default;

			OperationParser (const OperationParser&) = default;

			virtual ~OperationParser ()
							{ }

			OperationParser& operator = (const OperationParser&) = default;

			/** To verify whether a string is or not valid. */
			bool valid (const std::string& o) const;
			/** nullptr is returned when error. */
			OperationElement* parser (const std::string& o) const;

			protected:
			// To create the different elements of the operation...
			/** To create the right BinaryFunctionOperationElement. */
			virtual OperationElement* createBinaryOperationElement (unsigned char s,
				OperationElement* l, OperationElement* r) const;
			/** To create the right MonoFunctionOperationElement. */
			virtual OperationElement* createUnaryOperationElement (unsigned char s, OperationElement* o) const;
			/** To create the right function. By default there is nothing to create. */
			virtual OperationElement* createFunctionOperationElement (const OperationElements& oE) const
							{ return (nullptr); }

			/** To get the different possibilities of the different type of elements
				managed by this parser. They can be overloaded for specific purposes. */
			virtual std::string validBinarySymbols () const // The order marks the priority...
							{ return ("+-*/"); }
			virtual std::string validUnarySymbols () const // The order marks the priority...
							{ return ("><!"); }
			virtual std::vector <std::string> validFunctionNames () const // The order marks the priority. There is none by default!
							{ return (std::vector <std::string> ()); }

			private:
			/** When parsing the parenthesis 
				this intermediate structure is generated. */
			struct tOpElement
			{
				tOpElement () = default;

				tOpElement (const std::string& c, const OperationElements& elmnts)
					: _code (c), _elements (elmnts)
							{ }

				tOpElement (const tOpElement&) = default;

				tOpElement& operator = (const tOpElement&) = default;

				/** The code assigned to the intermediate OperatationElements generated. */
				std::string _code; // In the way ##XXXX## always...
				/** There would be more than 1 when the parenthesis is the list of
					arguments for a function. */
				OperationElements _elements;
			};

			using tOpElements = std::map <std::string, tOpElement>;

			void removeAndParserParenthesis (std::string& f, tOpElements& tmp, unsigned int& ct, bool& e) const;
			OperationElements parserPiece (const std::string& o, tOpElements& r, unsigned int& ct, bool& e) const;
			OperationElements parserListPiece (const std::string& o, tOpElements& r, unsigned int& ct, bool& e) const;
			OperationElements parserBinaryPiece (const std::string& o, tOpElements& r, unsigned int& ct, bool& e) const;
			OperationElements parserUnaryPiece (const std::string& o, tOpElements& r, unsigned int& ct, bool& e) const;
			OperationElements parserFunctionPiece (const std::string& o, tOpElements& r, unsigned int& ct, bool& e) const;
			void deleteTOpElements () const;
			void deleteOperationElements (const OperationElements& oE) const
							{ for (const auto& i : oE) delete (i); }

			private:
			// Implementation
			mutable tOpElements _tOpElements;
		};
	}
}

#endif
  
// End of the file
/*@}*/