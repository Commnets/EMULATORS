/** \ingroup ASSEMBLER */
/*@{*/

/**	
 *	@file	
 *	File: Operation.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 14/08/2022 \n
 *	Description: Many data can be represented as an Operation in an assembler program. \n
 *               The outcome after the execution of an Operation is here always an UInt object. \n
 *               That UInt could be managed later as an Address or simply as a number. 
 *	Versions: 1.0 Initial
 */

#ifndef __ASSEMBLER_OPERATIONELMNT__
#define __ASSEMBLER_OPERATIONELMNT__

#include <CORE/incs.hpp>
#include <ASSEMBLER/Error.hpp>
#include <ASSEMBLER/Grammar.hpp>

namespace MCHEmul
{
	namespace Assembler
	{
		/** Any Formula can be represented as a tree
		made up of a set of related "OperationElement". */
		class OperationElement
		{
			public:
			OperationElement ()
				: _error (ErrorType::_NOERROR)
							{ }
		
			OperationElement (const OperationElement&) = delete;
		
			OperationElement& operator = (const OperationElement&) = delete;
		
			/** Just in the case of any extension. */
			virtual ~OperationElement ()
							{ }

			OperationElement (OperationElement&&) = delete;

			OperationElement& operator = (OperationElement&&) = delete;

			/** To get the value of the OperationElement. \n 
				The value will depend on the type OperationElement itself. */
			virtual UInt value (const Macros& ms, const OperationParser* oP) const = 0;
		
			/** To get the OperationElement as a string. \n
				It could be useful to represent externally the OperationElement. \n
				Given the string representation of an OperationElement using a factory. */
			virtual std::string asString () const = 0;
		
			friend std::ostream& operator << (std::ostream& o, const OperationElement& fE)
							{ return (o << fE.asString ()); }
		
			/** When the value method is invoked, and error can be generated. \n 
				The next two methods are useful to check that situation. */ 
			ErrorType error () const
							{ return (_error); }
			bool operator ! () const
							{ return (_error != ErrorType::_NOERROR); }
		
			protected:
			mutable ErrorType _error; 
		
			// Implementation            
			/** The value to return when there is an error in a OperationElement. */
			static UInt _whenError;
		};

		/** To simplify the use of a list of Operation Elements. */
		using OperationElements = std::vector <OperationElement*>;
	
		// The very basic elements that any type of parser should understand:
		// One to represent a number and another one to represent a variable.
		/** Just a value. \n
			Its value is the number itself. */
		class ValueOperationElement final : public OperationElement
		{
			public:
			ValueOperationElement (const std::string& n)
				: OperationElement (),
				  _value (UInt::fromStr (n, UInt::_BINARY /** The most common. */))
							{ }
		
			virtual UInt value (const Macros&, const OperationParser*) const override
							{ return (_value); }
		
			/** In hexa, that is the format of the assembler!. */
			virtual std::string asString () const
							{ return (_value.asString (UByte::OutputFormat::_HEXA, ' ', 2)); }
		
			private:
			const UInt _value; // Set at construction time...
		};

		/** A variable. \n 
			Its value will get from the macro comtext. */
		class VariableOperationElement final : public OperationElement
		{
			public:
			VariableOperationElement (const std::string& n)
				: OperationElement (),
				  _name (n)
							{ }
		
			virtual UInt value (const Macros& ms, const OperationParser* oP) const override;
		
			virtual std::string asString () const
							{ return (_name); }
		
			private:
			const std::string _name; // Set at construction time...
		};
	}
}

#endif
  
// End of the file
/*@}*/