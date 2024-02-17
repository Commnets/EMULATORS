/** \ingroup ASSEMBLER */
/*@{*/

/**	
 *	@file	
 *	File: StdOperations.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 09/12/2022 \n
 *	Description: The standard operations. The most common ones have been included.
 *	Versions: 1.0 Initial
 */

#ifndef __ASSEMBLER_STDOPERATIONS__
#define __ASSEMBLER_STDOPERATIONS__

#include <ASSEMBLER/Operation.hpp>

namespace MCHEmul
{
	namespace Assembler
	{
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
			virtual ~FunctionOperationElement () override
							{ for (const auto& i : _operationElements) delete (i); }
		
			virtual UInt value (const Macros& ms, const OperationParser* oP) const override;

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
		
			virtual std::string asString () const override
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
		
			virtual std::string asString () const override
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
			virtual std::string functionSymbol () const override
							{ return ("+"); }

			virtual UInt doFunction (const std::vector <UInt>& v) const override
							{ return (UInt::adjust (UInt::fromInt (v [0].asInt () + v [1].asInt ()))); }
		};
		
		/** A binary function to subtract two OperationElement. */
		class SubtractFunctionOperationElement final : public BinaryFunctionOperationElement
		{
			public:
			SubtractFunctionOperationElement (OperationElement* l, OperationElement* r)
				: BinaryFunctionOperationElement (l, r)
							{ }
		
			private:
			virtual std::string functionSymbol () const override
							{ return ("-"); }

			virtual UInt doFunction (const std::vector <UInt>& v) const override
							{ return (UInt::adjust (UInt::fromInt (v [0].asInt () - v [1].asInt ()))); }
		};
		
		/** A binary function to multiply two OperationElement. */
		class MultiplyFunctionOperationElement final : public BinaryFunctionOperationElement
		{
			public:
			MultiplyFunctionOperationElement (OperationElement* l, OperationElement* r)
				: BinaryFunctionOperationElement (l, r)
							{ }
		
			private:
			virtual std::string functionSymbol () const override
							{ return ("*"); }

			virtual UInt doFunction (const std::vector <UInt>& v) const override
							{ return (UInt::adjust (UInt::fromInt (v [0].asInt () * v [1].asInt ()))); }
		};
		
		/** A binary function to divide two OperationElement. */
		class DivideFunctionOperationElement final : public BinaryFunctionOperationElement
		{
			public:
			DivideFunctionOperationElement (OperationElement* l, OperationElement* r)
				: BinaryFunctionOperationElement (l, r)
							{ }
		
			private:
			virtual std::string functionSymbol () const override
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
			virtual std::string functionSymbol () const override
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
			virtual std::string functionSymbol () const override
							{ return ("<"); }

			virtual UInt doFunction (const std::vector <UInt>& v) const override;
		};    
	}
}

#endif
  
// End of the file
/*@}*/