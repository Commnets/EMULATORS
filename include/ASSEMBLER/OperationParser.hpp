/** \ingroup ASSEMBLER */
/*@{*/

/**	
 *	@file	
 *	File: OperationParser.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 15/11/2022 \n
 *	Description: To parser an Operation and creates the tree of OperationElements. 
 *	Versions: 1.0 Initial
 */

#ifndef __ASSEMBLER_OPERATIONPARSER__
#define __ASSEMBLER_OPERATIONPARSER__

#include <CORE/incs.hpp>
#include <ASSEMBLER/Operation.hpp>

namespace MCHEmul
{
	namespace Assembler
	{
		/** It can be overloaded later to support additional functions. */
		class OperationParser
		{
			public:
			OperationParser () = default;

			OperationParser (const OperationParser&) = delete;

			OperationParser& operator = (const OperationParser&) = delete;

			virtual ~OperationParser ()
							{ }

			OperationParser (OperationParser&&) = delete;

			OperationParser& operator = (OperationParser&&) = delete;

			/** To verify whether a string is or not valid. */
			bool valid (const std::string& o) const;
			/** nullptr is returned when error. */
			OperationElement* parser (const std::string& o) const;

			protected:
			// To create the different elements of the operation...
			/** To create the right BinaryFunctionOperationElement. */
			virtual OperationElement* createBinaryOperationElement (unsigned char s,
				OperationElement* l, OperationElement* r) const = 0;
			/** To create the right MonoFunctionOperationElement. */
			virtual OperationElement* createUnaryOperationElement (unsigned char s, OperationElement* o) const = 0;
			/** To create the right function. By default there is nothing to create. */
			virtual OperationElement* createFunctionOperationElement (const OperationElements& oE) const = 0;

			/** To get the different possibilities of the different type of elements
				managed by this parser. They can be overloaded for specific purposes. */
			virtual std::string validBinarySymbols () const = 0; // The order marks the priority...
			virtual std::string validUnarySymbols () const = 0; // The order marks the priority...
			virtual std::vector <std::string> validFunctionNames () const = 0; // The order marks the priority. There is none by default!

			private:
			/** When parsing the parenthesis 
				this intermediate structure is generated. */
			struct tOpElement
			{
				tOpElement () = default;

				tOpElement (const std::string& c, const OperationElements& elmnts)
					: _code (c), _elements (elmnts)
							{ }

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