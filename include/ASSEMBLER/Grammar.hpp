/** \ingroup ASSEMBLER */
/*@{*/

/**	
 *	@file	
 *	File: Grammar.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Forni?s (EMULATORS library) \n
 *	Creation Date: 04/06/2021 \n
 *	Description: The assembler language can always be defined in terms of semantic.
				 Any language can be structured, gramatically speaking, into different elements. \n
				 Those elements relate each other following "gramatical" rules that defines a semantic. \n
				 In the case of the Assembler language, the following gramatical elements could be distinguished: \n
				 LabelElement			:	Represents a point in the memory. \n
				 BytesElement			:	Simple set of bytes filling up memory positions. \n
				 InstructionElement		:	Represents a CPU instruction. It is an special type of BytesElement. \n
				 StartingPointElement	:	Represents the point in the memory where to locate the other elements. \n
				 Some of those elements could hold parameters and that parameters could be based on MACROS.
				 The relation among these all gramatical elements is easy: \n
				 1.- The MACROS can be used as the parameter of any other element.
				 2.- When a MACRO is used it should have been defined before. Otherwise an error can happen.
				 3.- The set of bytes (bytes and instructions) will be one after other (linked) occupying a memory zone. \n
				 4.- The labels point to a byte or an instruction. \n
				 5.- Some instruction parameters can refer to a label, 
					 and that label can be defined before or after the instruction. To allow jump (e.g). 
				 6.- The position where a set of linked gramatical elements starts is defined using an starting point.
				 7.- So groups of code are defined by starting point.
 *	Versions: 1.0 Initial
 */

#ifndef __ASSEMBLER_GRAMMAR__
#define __ASSEMBLER_GRAMMAR__

#include <CORE/incs.hpp>
#include <ASSEMBLER/Error.hpp>

namespace MCHEmul
{
	namespace Assembler
	{
		/** A macro is always made up of a name and a value. \n
			The value can be either directly a value or a set of referencies (with operations) and links with other macros. \n
			When evaluating the macro an internal error could happen. \n
			The macro can be defined making operations (very simpl so farr) among others. */
		class Macro;
		using Macros = std::map <std::string, Macro>;
		class Macro
		{
			public:
			Macro ()
				: _name (""), _equivalent (""),
				  _value ({ }), 
				  _error (ErrorType::_NOERROR) 
								{ }

			Macro (const std::string& n, const std::string& e)
				: _name (n), _equivalent (e),
				  _value ({ }),
				  _error (ErrorType::_NOERROR)
							{ }

			Macro (const Macro&) = default;

			Macro& operator = (const Macro&) = default;

			const std::string& name () const
							{ return (_name); }
			const std::string& equivalent () const
							{ return (_equivalent); }
			ErrorType error () const
							{ return (_error); }

			/** Gets the value of the macro taking into account potential relations with other macros. \n
				The first time it is invoked the very real value is calculated if possible. \n
				The variable _error will point whether there was and error in the calculus. */
			const std::vector <UByte>& value (const Macros& ms) const
							{ return (_value.empty () ? _value = calculateValue (_equivalent, ms) : _value ); }

			/** To make more visual the analysis of the status of macro. */
			bool operator ! () const
							{ value ({ }) /** Calculated already? */; return (_error != ErrorType::_NOERROR); }

			private:
			/** To calculate the value first time. 
				It is able to take into account simple operations in the definition: *,+,- */
			std::vector <UByte> calculateValue (const std::string& e, const Macros& ms) const;

			private:
			/** Name of the macro. */
			const std::string _name;
			/** The initial value of the macro (pure = not calculated). */
			const std::string _equivalent; 

			// Implementation
			mutable std::vector <UByte> _value;
			mutable ErrorType _error;
		};

		/** The grammatical element is the base of any grammar elemnt. */
		class Semantic;
		struct GrammaticalElement
		{
			enum Type { _LABEL = 0, _BYTESINMEMORY, _INSTRUCTION, _STARTINGPOINT  };

			GrammaticalElement ()
				: _type (_BYTESINMEMORY), _id (0), _line (0), 
				  _nextElement (nullptr), _previousElement (nullptr),
				  _error (ErrorType::_NOERROR), _codeBytes ()
							{ }

			GrammaticalElement (const GrammaticalElement&) = default;

			GrammaticalElement& operator = (const GrammaticalElement&) = default;

			virtual ~GrammaticalElement ()
							{ delete (_nextElement); /** Chained. */ }

			/** To get the value of the grammatical element (if makes sense) as a set of executable bytes.
				The value is get taking into account a semantic. \n
				When using this method errors could be generated. */
			std::vector <UByte> codeBytes (const Semantic* s, bool bE = true) const
							{ return (_codeBytes.empty () ? _codeBytes = calculateCodeBytes (s, bE) : _codeBytes); }

			/** To know the address of the grammatical element. */
			virtual Address address (const MCHEmul::Assembler::Semantic* s) const;

			ErrorType error () const
							{ return (_error); }

			/** To visually simplify the way the error system is managed. */
			bool operator ! () const
							{ return (_error != ErrorType::_NOERROR); }

			Type _type;
			unsigned int _id; // Sequential...
			unsigned int _line; // The line where the definition appears...
			/** The next gramatical element linked. */
			GrammaticalElement* _nextElement; // Could be nullptr (in the last gramatical element of a semantic block)
			/** The previous gramatical element linked. */
			GrammaticalElement* _previousElement; // It could also be nullptr (in the first gramatical element of a semantic block)
			/** To define whether it has or not some mistake inside, it could be modified even consulting data. */
			mutable ErrorType _error;
			
			protected:
			// Implementation
			mutable std::vector <UByte> _codeBytes;

			protected:
			/** To calculat the codeBytes first time. */
			virtual std::vector <UByte> calculateCodeBytes (const Semantic* s, bool bE = true) const
							{ return (std::vector <UByte> ()); }

			// Implementation
			std::vector <UByte> bytesFromExpression (const std::string& e, const Macros& ms, bool& er) const;
		};

		/** @see explanation at the beggining of the file. */
		struct LabelElement final : public GrammaticalElement
		{
			LabelElement ()
				: GrammaticalElement (), 
				  _name ()
							{ _type = Type::_LABEL; }

			LabelElement (const LabelElement&) = default;

			LabelElement& operator = (const LabelElement&) = default;

			std::string _name;
		};

		/** @see explanation at the beggining of the file. */
		struct BytesInMemoryElement : public GrammaticalElement
		{
			BytesInMemoryElement ()
				: GrammaticalElement (), _elements ()
							{ _type = Type::_BYTESINMEMORY; }

			BytesInMemoryElement (const BytesInMemoryElement&) = default;

			BytesInMemoryElement& operator = (const BytesInMemoryElement&) = default;

			std::vector <std::string> _elements;

			private:
			virtual std::vector <UByte> calculateCodeBytes (const Semantic* s, bool bE = true) const override;
		};

		/** @see explanation at the beggining of the file. 
			An instruction is just also a set of bytes in memory. */
		struct InstructionElement final : public GrammaticalElement
		{
			InstructionElement ()
				: GrammaticalElement (), 
				  _possibleInstructions (),
				  _parameters (),
				  _selectedInstruction (nullptr)
							{ _type = Type::_INSTRUCTION; }

			InstructionElement (const InstructionElement&) = default;

			InstructionElement& operator = (const InstructionElement&) = default;

			/** To know whether any of the parameters could be or not a label. */
			bool hasAnyLabelAsParameter (const Semantic* s) const;
			/** To know the list of the the parameters that could be a label. */
			std::vector <size_t> labelParameters (const Semantic* s) const;

			std::vector <Instruction*> _possibleInstructions;
			std::vector <std::string> _parameters;
			mutable Instruction* _selectedInstruction;

			private:
			/** The bytes of the first possible instruction taking into account the size of the parameters,
				once they have been calculated (they could includee also macros). */
			virtual std::vector <UByte> calculateCodeBytes (const Semantic* s, bool bE = true) const override;

			// Implementation
			std::vector <UByte> calculateCodeBytesForInstruction 
				(const Instruction* inst, const Semantic* s, bool bE = true) const;
		};

		/** @see explanation at the beggining of the file. */
		struct StartingPointElement final : public GrammaticalElement
		{
			StartingPointElement ()
				: GrammaticalElement (), 
				  _value ()
							{ _type = Type::_STARTINGPOINT; }

			StartingPointElement (const StartingPointElement&) = default;

			StartingPointElement& operator = (const StartingPointElement&) = default;

			virtual Address address (const Semantic* s) const override
							{ return (Address (codeBytes (s /**, big Endian implicit. */))); }

			std::string _value;

			private:
			virtual std::vector <UByte> calculateCodeBytes (const Semantic* s, bool bE = true) const override;
		};

		/** Notice that it is a set of pointers. */
		using StartingPointElements = std::vector <StartingPointElement*>;

		/** A semantic will be a set of macros used to simplfy how the code is read, 
			and a set of starting points. \n
			This object is is created little by little adding other elments, 
			so it can not b neither copied or equaled in the standard way. 
			*/
		class CommandParser;
		class Semantic
		{
			public:
			friend CommandParser;

			Semantic ()
				: _macros (), _startingPoints (), 
				  _error (ErrorType::_NOERROR), _lastGrammaticalElementAdded (nullptr)
							{ }

			Semantic (const Semantic&) = delete;

			~Semantic ()
							{ for (auto i : _startingPoints) delete (i); }

			Semantic& operator = (const Semantic&) = delete;

			// Managing the semantic...
			const Macros& macros () const
							{ return (_macros); }
			void addMacro (const Macro& m);
			const StartingPointElements& startingPoints () const
							{ return (_startingPoints); }
			StartingPointElement* addNewStartingPoint ();
			void addGrammaticalElement (GrammaticalElement* g);
			const GrammaticalElement* lastGrammaticalElementAdded () const
							{ return (_lastGrammaticalElementAdded); }
			GrammaticalElement* lastGrammaticalElementAdded ()
							{ return (_lastGrammaticalElementAdded); }

			ErrorType error () const
							{ return (_error); }

			/** To simplify the analysis of the macro. */
			bool operator ! () const
							{ return (_error != ErrorType::_NOERROR); }

			/** The semantic can add elements from other (macros are also added). */
			void addFrom (const Semantic* s);

			/** To verify whether exists a label. */
			bool existsLabel (const std::string& l) const;
			/** To determine the distance of an Address with that label. 
				If the label has't been defined in the semantic, 0 is returnd but it wouldn't mean anything. */
			Address addressForLabel (const std::string& l) const;
			/** To get the list of all labels. */
			std::vector <const LabelElement*> labels () const;
			/** to get the list of labels and addrresses associated. */
			std::map <std::string, Address> labelAddresses () const;

			private:
			Macros _macros;
			StartingPointElements _startingPoints;

			// Implementation
			mutable ErrorType _error;
			mutable GrammaticalElement* _lastGrammaticalElementAdded;
		};
	}
}

#endif
  
// End of the file
/*@}*/