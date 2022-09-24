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
		Instruction () = delete;

		Instruction (unsigned int c, unsigned int mp, unsigned int cc, const std::string& t);

		Instruction (const Instruction&) = default;

		Instruction& operator = (const Instruction&) = default;

		constexpr unsigned int code () const
							{ return (_code); }
		constexpr unsigned int memoryPositions () const
							{ return (_memoryPositions); }
		constexpr unsigned int clockCycles () const
							{ return (_clockCycles); }
		unsigned int additionalClockCycles () const
							{ return (_additionalCycles); }
		const std::string iTemplate () const
							{ return (_iTemplate); }

		const UBytes& parameters () const
							{ return (_lastParameters); }
		virtual std::string lastParameterAsString (size_t nP) const // The UBytes could grouped to get a parameter...
							{ return (nP < _lastParameters.size () 
								? _lastParameters [nP].asString (UByte::OutputFormat::_HEXA) : ""); }
		virtual std::string asString () const;

		/** To execute the instruction. It has to be redefined. 
			It returns true if everything is ok, */
		bool execute (const UBytes& p, CPU* c, Memory* m, Stack* stk);

		friend std::ostream& operator << (std::ostream& o, const Instruction& i);

		protected:
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
		const unsigned int _code = 0; // Modified at construction level
		const unsigned int _memoryPositions = 0; // Modified at construction level
		const unsigned int _clockCycles = 0; // Modified at construction level
		const std::string _iTemplate = ""; // Modified at construction level
		unsigned int _additionalCycles; // Sometimes a instruction could take more than expected...

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