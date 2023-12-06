/** \ingroup CORE */
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

#include <CORE/global.hpp>
#include <CORE/UBytes.hpp>

namespace MCHEmul
{
	class CPU;
	class Memory;
	class Stack;
	class ProgramCounter;

	/** Represents a instruction executed by a CPU. \n
		To process a instruction several CPU cycles are consumed. \n 
		Depending on the type of processor implementing a instruction, the type of cycles can vary. \n
		However, although they list can be extended later, three basic types are provided by default: \n
		Internal (_CYCLEINTERNAL, the default one), \n
		to read data from memory (_CYCLEREAD), 
		and to write data to memory (_CYCLEWRITE). */
	class Instruction
	{
		public:
		/** Represents the internal structure of a instruction.
			It is quite useful to be managed later by the parsers! */
		struct Structure
		{
			struct Parameter
			{
				enum class Type 
				{ 
					_DATA = 0, 
					_DIR, 
					_RELJUMP, 
					_ABSJUMP 
				};

				/** To link a char with the type. 
					It used in many places into the code. */
				static std::map <unsigned char, Type> _TYPES;

				Parameter () 
					: _type (Type::_DATA), _numberBytes (1)
							{ }

				Parameter (Type tp, size_t nB)
					: _type (tp), _numberBytes (nB)
							{ assert (_numberBytes > 0); }

				Type _type;
				size_t _numberBytes;
			};

			Structure ()
				: _error (false), _templateWithNoParameters (""), _waterMark (""), _waterMarkPlus (""), _parameters ()
							{ }

			Structure (const std::string& t, const std::string& wM, const std::vector <Parameter>& prms)
				: _error (false), _templateWithNoParameters (t), _waterMark (wM), _parameters (prms)
							{ }

			/**
			  * To indicate whether there was or not a mistake after the analysis of the instruction...
			  * In that error-case the _templateWithNoParameters will hold 
			  * either "-" (instruction error) or "?" (parameter error). */
			bool _error; 
			std::string _templateWithNoParameters;
			std::string _waterMark;
			std::string _waterMarkPlus;
			std::vector <Parameter> _parameters;
		};

		// The different type of cycles than an instruction could be made of
		// If more types of cycles were defined in classes inheriting from this, 
		// be sure that its id is power of two different than the ones already used.
		// _CYCLEALL will be still valid!
		/** When a cycle is internal, that is the default value. 
			Used in internal calculus, o movement between registers inside the processor. */
		static const unsigned int _CYCLEINTERNAL = 1;
		/** When a cycle read info from the memory. */
		static const unsigned int _CYCLEREAD = 2;
		/** When it used to write info to the memory. */
		static const unsigned int _CYCLEWRITE = 4;
		/** More types might be added... */
		/** All at the same time. */
		static const unsigned int _CYCLEALL = std::numeric_limits <unsigned int>::max ();

		Instruction () = delete;

		/** 
		  *	Constructor.
		  * @param c	:	The internal code of the instruction.
		  *	@param mp	:	The number of memory positions (in number of ubytes) occupied by the instruction (and params).
		  * @param cc	:	The number of clock cyles the instruction uses to be executed (usually). \n
		  *					Some ocassions, some instructions under certain circunstances can take longer than expected. \n
		  *					@see for so: _additionalClockCycles (method and variable).
		  *	@param t	:	The template of the instruction. \n
		  *					The way it is defined is key for it to be read and understood by the parsers. \n
		  *					The parameters should be within [] with two additional data: \n
		  *					The type of the parameter, and the number of bytes that parameter occupies. \n
		  *					Regarding the type: # means number, $ means address and & means relative jump (@see type of parameter).
		  *	@param bE	:	Indicate whether the info contained is managed big or little endian.
		  */
		Instruction (unsigned int c, unsigned int mp, unsigned int cc, 
			const std::string& t, bool bE = true);

		Instruction (const Instruction&) = delete;

		Instruction& operator = (const Instruction&) = delete;

		virtual ~Instruction ()
							{ }

		Instruction (Instruction&&) = delete;

		Instruction& operator = (Instruction&&) = delete;

		unsigned int code () const
							{ return (_code); }
		size_t codeLength () const
							{ return (_codeLength); }
		unsigned int memoryPositions () const
							{ return (_memoryPositions); }
		bool bigEndian () const
							{ return (_bigEndian); }

		// Related with the cycles of the operation...
		/** The total, without any additional one after its execuction. */
		unsigned int clockCycles () const
							{ return (_clockCycles); }
		unsigned int additionalClockCycles () const
							{ return (_additionalCycles); }
		const std::string iTemplate () const
							{ return (_iTemplate); }
		const Structure& internalStructure () const
							{ return (_iStructure); }

		/** To get the type of the cycle. \n
			This operation can be overloaded later depending on the set of instructions. 
			From 0 to _clockCycles - 1. */
		virtual unsigned int typeOfCycle (unsigned int c) const
							{ return (_CYCLEINTERNAL); }

		/** To know whether the instruction template matches of not with the example received. \n
			Returns true when matches, and false when it doesn't. \n
			When matches the parameter prms received is also filled. \n
			When doesn't the parameter prms will have trash!. */
		bool matchesWith (const std::string& i, Strings& prms);

		// Managing the parameters...
		const UBytes& parameters () const
							{ return (_lastParameters); }
		void setParameters (const UBytes& prms) // rarely used...
							{ _lastParameters = prms; }
		const UBytes parameters (size_t p, size_t nP = 1, bool bE = true) const;
		std::string parametersAsString (size_t p, size_t nP = 1, bool bE = true) const; // The UBytes could grouped to get a parameter...

		/** This method is key when, i.e. compiling assembler code. \n
			Given a set of data (or several sets) in the for of bytes, 
			the method returns the bytes to complete the full instruction. \n
			The data is already provided in the right order that the instruction defines (i.e. so the big endian effect, if any, is included). \n
			Usually the code is written at the beginning of the data. This is the default behaviour. \n
			If might not be possible to build the instruction, the e variable would get set. 
			The method can be overloaded. */
		virtual std::vector <UByte> shapeCodeWithData (const std::vector <std::vector <UByte>>& b, bool& e) const;
		
		/** To get the instruction as an string using the parameters of the last execution inside. \n
			If no parameters has been set "blankk" will be written instead. */
		virtual std::string asString () const;

		/**	
		  *	To execute the instruction. \n
		  * It can be redefined, because the way the instruction is executed will depend on the type of it.
		  * @param	c	:	A reference to the CPU asking for the execution.
		  * @param	m	:	A reference to the memory used in the execution.
		  * @param	stk :	Same for the stack.
		  * @param	pc	:	A reference to the program counter, that will be affected depending on the instruction.
		  *	It returns true if everything is ok.
		  */
		virtual bool execute (CPU* c, Memory* m, Stack* stk, ProgramCounter* pc) = 0;

		friend std::ostream& operator << (std::ostream& o, const Instruction& i)
							{ return (o << i.asString ()); }

		protected:
		/** To analyze the structure of the instruction. */
		Structure analyzeInstruction () const;

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
		// Once they assigned at construction level they couldn't be modified...
		// IMPORTANT NOTE:
		// Most of these attributes are usually modified! (there is no method for so)
		// So they could be defined as const.
		// However the InstructionUndefined does when executing, take this into account!
		unsigned int _code;
		size_t _codeLength;
		unsigned int _memoryPositions; 
		unsigned int _clockCycles;
		bool _bigEndian;
		std::string _iTemplate;

		// Implementation
		// The internal structure helps us later to deal better with the instruction...
		Structure _iStructure; 

		unsigned int _additionalCycles; // Sometimes, when executed, 
										// an instruction could take more than expected... (@see additionalClockCylces method)

		protected:
		MCHEmul::UBytes _lastParameters;
		MCHEmul::CPU* _cpu;
		MCHEmul::Memory* _memory;
		MCHEmul::Stack* _stack;
	};

	// To simplify the way the instruction set is used...
	using Instructions = std::map <unsigned int, Instruction*>;
	using ListOfInstructions = std::vector <Instruction*>;

	/** The most common type of instruction. \n
		This type of instruction is fully defined at costruction time. \n
		That's it, the number of cycles it takes, the bytes in memory it occupies, etc. is defined at construction time. */
	class InstructionDefined : public Instruction
	{
		public:
		InstructionDefined (unsigned int c, unsigned int mp, unsigned int cc, 
			const std::string& t, bool bE = true)
			: Instruction (c, mp, cc, t, bE)
							{ }

		/** Modifies the pc with the length of the instruction, before executed it. \n
			Take this into account when defining jump instructions. 
			This methid can not be longer extended. \n
			The one to define the datail of the instruction is executeImpl. */
		virtual bool execute (CPU* c, Memory* m, Stack* stk, ProgramCounter* pc) final override;

		protected:
		/** The implementation of the execution.
			It has to be redefined when the instruction was implemented.
			@return		true if the execution was ok, and false in other situation. 
			The parameter f must be set to false whether the execution didin't finish
			and the next CPU cycle has to considere it back. */
		virtual bool executeImpl (bool &f) = 0;
	};

	/** This type of instruction is defined as it is processed. \n
		The instruction needs a list of different possibilities to choose. \n 
		The one to execute finally can depend on, e.g, other bytes read from the memory. */
	class InstructionUndefined : public Instruction
	{
		public:
		/** 
		  * @param	c	:	A generic code for the Undefined Instruction.
		  * @param	inst:	The list of instruction where to select the right one to finally execute. 
		  */
		InstructionUndefined (unsigned int c, const Instructions& inst);

		const Instructions& instructions () const
							{ return (_instructions); }
		Instructions& instructions ()
							{ return (_instructions); }

		const InstructionDefined* lastInstruction () const
							{ return (_lastInstruction); }
		InstructionDefined* lasInstruction ()
							{ return (_lastInstruction); }

		/** The final instruction executed should be always a defined one,
			and is is dragged up to the first level. */
		virtual bool execute (CPU* c, Memory* m, Stack* stk, ProgramCounter* pc) override;

		protected:
		/** As the instruction is undefined, 
			it is needed to select first the right instruction to execute. \n 
			This method has to be redefined to follow the right processes of the CPU emulated. */
		virtual Instruction* selectInstruction (CPU* c, Memory* m, Stack* stk, ProgramCounter* pc) = 0;

		protected:
		Instructions _instructions;

		// Implementation
		/** The last defined instruction executed inside finally. */
		InstructionDefined* _lastInstruction;
	};
}

/** 
  *	To simplify the way a new InstructionDefined is implemented.
  *	@param _C  : Code.
  *	@param _M  : MemoryPositions occupied.
  *	@param _CC : Clock cycles used. 
  * @param _RCC: Read clock cycles used within the total.
  * @param _T  : The template to print the instruction.
  *	@param _I  : Name of the intruction.
  * @param _J  : Name of the parent class. The last parent class should be InstructionDefined.
  * @param _K  : Whether the info kept is big or little endian.
  * Always this method is used take into account that the variable _FINISH 
  * must be set to true of false to indicate whether the instruction has or not finished.
  */
#define _INST_FROM(_C, _M, _CC, _RCC, _T, _I, _J) \
class _I final : public _J \
{ \
	public: \
	_I () : _J (_C, _M, _CC, _RCC, _T) { } \
	protected: \
	virtual bool executeImpl (bool& _FINISH) override; \
};

/** Idem but inheriting from basic instruction. */
#define _INST(_C, _M, _CC, _RCC, _T, _I, _K) \
	_INST_FROM(_C, _M, _CC, _RCC, _T, _I, MCHEmul::InstructionDefined);

#define _INST_IMPL(_I) \
bool _I::executeImpl (bool& _FINISH)

#endif
  
// End of the file
/*@}*/