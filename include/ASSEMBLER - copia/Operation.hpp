/** \ingroup ASSEMBLER */
/*@{*/

/**	
 *	@file	
 *	File: Operatipon.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Forniés (EMULATORS library) \n
 *	Creation Date: 14/08/2022 \n
 *	Description: Many data can be represented as an Operation in an assembler program. \n
 *               The outcome after the execution of an Operation is here always an UInt object. \n
 *               That UInt could be managed later as an Address or simply as a number. 
 *	Versions: 1.0 Initial
 */

#ifndef __ASSEMBLER_FORMULA__
#define __ASSEMBLER_FORMULA__

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
		
			OperationElement (const OperationElement&) = default;
		
			OperationElement& operator = (const OperationElement&) = default;
		
			/** Just in the case of any extension. */
			virtual ~OperationElement ()
							{ }
		
			// Managing the value of the OperationElement
			/** To calculate the result of an OperationElement, a context is sometimes needed. 
				That context is given by the Macros defined in the Semantic object
				extracted from any Assembler program. */
			virtual void setMacros (const Macros& ms) const = 0;
		
			/** To get the value of the OperationElement. \n 
				The value will depend on the type OperationElement itself. */
			virtual UInt value () const = 0;
		
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
		
		using OperationElements = std::vector <OperationElement*>;
		
		/** A variable. \n 
			Its value will get from the macro comtext. */
		class VariableOperationElement final : public OperationElement
		{
			public:
			VariableOperationElement (const std::string& n)
				: OperationElement (),
				  _name (n)
							{ }
		
			virtual void setMacros (const Macros& ms) const override
							{ _macros = ms; }
		
			virtual UInt value () const override;
		
			virtual std::string asString () const
							{ return (_name); }
		
			private:
			const std::string _name; // Set at construction time...
			mutable Macros _macros;
		};
		
		/** Represents a function. \n
			That is, something to do with a couple of other operation elements. */
		class FunctionOperationElement : public OperationElement
		{
			public:
			FunctionOperationElement (const OperationElements& elmts)
				: OperationElement (),
				  _operationElements (elmts) // None should be null..
							{ for (const auto& i : _operationElements) assert (i != nullptr); }
		
			/** The function own the elements involved. */
			virtual ~FunctionOperationElement ()
							{ for (const auto& i : _operationElements) delete (i); }
		
			virtual void setMacros (const Macros& ms) const override
							{ for (const auto& i: _operationElements) i -> setMacros (ms); }
		
			virtual UInt value () const override;
		
			protected:
			/** To get the function symbol.
				It will be likely used in the method "asString". */
			virtual std::string functionSymbol () const = 0;
			/** Number parameters. -1 will mean not defined. */
			virtual int numberParameters () const = 0;
		
			// Implementation
			/** This method is invoked when there is no errors in any of the other 
				operation elements involved in the function when calling "value". 
				and it is the real one executing the operation among them. \n
				When this function is invoked the number of parameters is right!. (dont need to check it back) */
			virtual UInt doFunction (const std::vector <UInt>& v) const = 0;
		
			protected:
			OperationElements _operationElements;
		};
		
		/** Represents a binary function between two OperationElements. */
		class BinaryFunctionOperationElement : public FunctionOperationElement
		{
			public:
			BinaryFunctionOperationElement (OperationElement* l, OperationElement* r)
				: FunctionOperationElement ({ l, r })
							{ }
		
			virtual std::string asString () const
							{ return (_operationElements [0] -> asString () + functionSymbol () + 
								_operationElements [1] -> asString ()); }

			protected:
			virtual int numberParameters () const override
							{ return (2); }
		};
		
		/** Represents a "mono" argument function 
			taking just the element at its right as parameter*/
		class UnaryFunctionOperationElement : public FunctionOperationElement
		{
			public:
			UnaryFunctionOperationElement (OperationElement* o)
				: FunctionOperationElement ({ o })
							{ }
		
			virtual std::string asString () const
							{ return (functionSymbol () + 
								_operationElements [0] -> asString ()); }

			protected:
			virtual int numberParameters () const override
							{ return (1); }
		};
		
		/** A binary function to add two OperationElement. */
		class AddFunctionOperationElement final : public BinaryFunctionOperationElement
		{
			public:
			AddFunctionOperationElement (OperationElement* l, OperationElement* r)
				: BinaryFunctionOperationElement (l, r)
							{ }
		
			private:
			virtual std::string functionSymbol () const
							{ return ("+"); }

			virtual UInt doFunction (const std::vector <UInt>& v) const override
							{ return (v [0] + v [1]); }
		};
		
		/** A binary function to subtract two OperationElement. */
		class SubtractFunctionOperationElement final : public BinaryFunctionOperationElement
		{
			public:
			SubtractFunctionOperationElement (OperationElement* l, OperationElement* r)
				: BinaryFunctionOperationElement (l, r)
							{ }
		
			private:
			virtual std::string functionSymbol () const
							{ return ("-"); }

			virtual UInt doFunction (const std::vector <UInt>& v) const override
							{ return (v [0] - v [1]); }
		};
		
		/** A binary function to multiply two OperationElement. */
		class MultiplyFunctionOperationElement final : public BinaryFunctionOperationElement
		{
			public:
			MultiplyFunctionOperationElement (OperationElement* l, OperationElement* r)
				: BinaryFunctionOperationElement (l, r)
							{ }
		
			private:
			virtual std::string functionSymbol () const
							{ return ("*"); }

			virtual UInt doFunction (const std::vector <UInt>& v) const override
							{ return (v [0] * v [1]); }
		};
		
		/** A binary function to divide two OperationElement. */
		class DivideFunctionOperationElement final : public BinaryFunctionOperationElement
		{
			public:
			DivideFunctionOperationElement (OperationElement* l, OperationElement* r)
				: BinaryFunctionOperationElement (l, r)
							{ }
		
			private:
			virtual std::string functionSymbol () const
							{ return ("/"); }

			virtual UInt doFunction (const std::vector <UInt>& v) const override;
		};    

		/** A unary function to get the LSB of an UInt. */
		class LSBFunctionOperationElement final : public UnaryFunctionOperationElement
		{
			public:
			LSBFunctionOperationElement (OperationElement* o)
				: UnaryFunctionOperationElement (o)
							{ }

			private:
			virtual std::string functionSymbol () const
							{ return (">"); }

			virtual UInt doFunction (const std::vector <UInt>& v) const override;
		};    

		/** A unary function to get the LSB of an UInt. */
		class MSBFunctionOperationElement final : public UnaryFunctionOperationElement
		{
			public:
			MSBFunctionOperationElement (OperationElement* o)
				: UnaryFunctionOperationElement (o)
							{ }

			private:
			virtual std::string functionSymbol () const
							{ return (">"); }

			virtual UInt doFunction (const std::vector <UInt>& v) const override;
		};    
	}
}

#endif
  
// End of the file
/*@}*/