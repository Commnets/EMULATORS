/** \ingroup language */
/*@{*/

/**	
 *	@file	
 *	File: Instruction.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Forniés (EMULATORS library) \n
 *	Creation Date: 03/04/2021 \n
 *	Description: Defines the set of instructions.
 *	Versions: 1.0 Initial
 */

#ifndef __MCHEMUL_INSTRUCTION__
#define __MCHEMUL_INSTRUCTION__

#include <assert.h>
#include <vector>
#include <map>
#include <ostream>

#include <global.hpp>
#include <core/UBytes.hpp>

namespace MCHEmul
{
	class CPU;
	class Memory;
	class Stack;

	/** Represents a instruction executed by a CPU. */
	class Instruction
	{
		public:
		/** Represents the internal structure of a instruction.
			It is quite useful to be managed later by the parsers! */
		struct Structure
		{
			struct Parameter
			{
				enum class Type { _DATA, _DIR, _JUMP };

				Parameter () 
					: _type (Type::_DATA), _numberBytes (1)
							{ }

				Parameter (Type tp, size_t nB)
					: _type (tp), _numberBytes (nB)
							{ }

				Parameter (const Parameter&) = default;

				Parameter& operator = (const Parameter&) = default;

				Type _type;
				size_t _numberBytes;
			};

			Structure ()
				: _error (false), _templateWithNoParameters (""), _parameters ()
							{ }

			Structure (const std::string& t, const std::vector <Parameter>& prms)
				: _error (false), _templateWithNoParameters (t), _parameters (prms)
							{ }

			Structure (const Structure&) = default;

			Structure& operator = (const Structure&) = default;

			/**
			  * To indicate whether there was or not a mistake after the analysis of the instruction...
			  * In that case the _templateWithNoParameters will hold 
			  * either "-" (instruction error) or "?" (parameterer error). */
			bool _error; 
			std::string _templateWithNoParameters;
			std::vector <Parameter> _parameters;
		};

		Instruction () = delete;

		/** 
		  *	Constructor.
		  * @param c	:	The internal code of the instruction.
		  *	@param mp	:	The number of memory positions (in number of ubytes) occupied by the instruction (and params).
		  * @param cc	:	The number of clock cyles the instruction uses to be executed (usually). \n
		  *					Some ocassions, some instructions in certain circunstances can take longer than expected. \n
		  *					@see for so: _additionalClockCycles (method and variable).
		  *	@param t	:	The template of the instruction. \n
		  *					The way it is defined is key for them to be read by the parsers. \n
		  *					The parameters should be within [] with two additional data: \n
		  *					The type of the parameter, and the number of bytes that parameter occupies. \n
		  *					Regarding the type: # means number, $ means address and & means relative jump.
		  */
		Instruction (unsigned int c, unsigned int mp, unsigned int cc, const std::string& t);

		Instruction (const Instruction&) = default;

		Instruction& operator = (const Instruction&) = default;

		unsigned int code () const
							{ return (_code); }
		unsigned int memoryPositions () const
							{ return (_memoryPositions); }
		unsigned int clockCycles () const
							{ return (_clockCycles); }
		unsigned int additionalClockCycles () const
							{ return (_additionalCycles); }
		const std::string iTemplate () const
							{ return (_iTemplate); }
		const Structure& internalStructure () const
							{ return (_iStructure); }

		/** To know whether the instruction template matches of not with the example received. \n
			Returns true when matches, and false when it doesn't. \n
			When matches the parameter prms received is also filled. \n
			When doesn't the parameter prms will have trash!. */
		bool matchesWith (const std::string& i, std::vector <std::string>& prms);

		const UBytes& parameters () const
							{ return (_lastParameters); }
		std::string lastParametersAsString (size_t p, size_t nP) const; // The UBytes could grouped to get a parameter...
		std::string asString () const;

		/** To execute the instruction. It has to be redefined. 
			It returns true if everything is ok, */
		bool execute (const UBytes& p, CPU* c, Memory* m, Stack* stk);

		friend std::ostream& operator << (std::ostream& o, const Instruction& i);

		protected:
		/** To analyze the structure of the instruction. */
		Structure analyzeInstruction () const;

		/** The implementation of the execution.
			It has to be redefined. */
		virtual bool executeImpl () = 0;

		// Implementation
		const CPU* cpu () const
							{ return (_cpu); }
		CPU* cpu ()
							{ return (_cpu); }
		const Memory* memory () const
							{ return (_memory); }
		Memory* memory () 
							{ return (_memory); }
		const Stack* stack () const
							{ return (_stack); }
		Stack* stack ()
							{ return (_stack); }

		protected:
		// Once they assigned at constrruction level they couldn't be modified...
		const unsigned int _code = 0; 
		const unsigned int _memoryPositions = 0; 
		const unsigned int _clockCycles = 0; 
		std::string _iTemplate; // Can be modified during the construction...

		// The internal structure helps us later to deal better with the instruction...
		Structure _iStructure; 

		unsigned int _additionalCycles; // Sometimes, when executed, 
										// an instruction could take more than expected...

		private:
		MCHEmul::UBytes _lastParameters;
		MCHEmul::CPU* _cpu;
		MCHEmul::Memory* _memory;
		MCHEmul::Stack* _stack;
	};

	using Instructions = std::map <unsigned int, Instruction*>;
}

/** 
  *	To simplify the way a new instruction is defined.
  *	@param _C  : Code.
  *	@param _M  : MemoryPositions occupied.
  *	@param _CC : Clock cycles used. 
  * @param _T  : The templat to print the instruction.
  *	@param _I  : Name of the intruction.
  * @param _J  : Name of the parent class.
  */
#define _INST_FROM(_C, _M, _CC, _T, _I, _J) \
class _I final : public _J \
{ \
	public: \
	_I () : _J (_C, _M, _CC, _T) { } \
	protected: \
	virtual bool executeImpl () override; \
};

/** Idem but inheriting from basic instruction. */
#define _INST(_C, _M, _CC, _T, _I) \
	_INST_FROM(_C, _M, _CC, _T, _I, MCHEmul::Instruction);

#define _INST_IMPL(_I) \
bool _I::executeImpl ()

#endif
  
// End of the file
/*@}*/