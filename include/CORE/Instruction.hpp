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

	/** Represents a instruction executed by a CPU. 
		A instruction is made up of cycles. 
		Depending on the type of processor implementing a instruction, the type of cycles can vary. \n
		However although they can be extended later, three basic types are provided by default: \n
		Internal (the default one), to read data from memory and to write data to memory. */
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

		// The different type of cycles than an instruction could have
		/** When a cycle is internal, that is the default value. 
			Used in internal calculus, o movement between registers inside the processor. */
		static const unsigned int _CYCLEINTERNAL = 0;
		/** When a cycle read info from the memory. */
		static const unsigned int _CYCLEREAD = 1;
		/** When it used to write info to the memory. */
		static const unsigned int _CYCLEWRITE = 2;

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
		
		/** To get the instruction as an string using the parameters of the last execution inside. \n
			If no parameters has been set "blacnk" will be written instead. */
		std::string asString () const;

		/** To execute the instruction. It has to be redefined. \n
			It returns true if everything is ok, */
		bool execute (const UBytes& p, CPU* c, Memory* m, Stack* stk);

		friend std::ostream& operator << (std::ostream& o, const Instruction& i)
							{ return (o << i.asString ()); }

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
		// Once they assigned at construction level they couldn't be modified...
		const unsigned int _code; 
		const size_t _codeLength;
		const unsigned int _memoryPositions; 
		const unsigned int _clockCycles;
		const bool _bigEndian;
		std::string _iTemplate;

		// Implementation
		// The internal structure helps us later to deal better with the instruction...
		Structure _iStructure; 

		unsigned int _additionalCycles; // Sometimes, when executed, 
										// an instruction could take more than expected... (@see additionalClockCylces method)

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
  * @param _RCC: Read clock cycles used within the total.
  * @param _T  : The template to print the instruction.
  *	@param _I  : Name of the intruction.
  * @param _J  : Name of the parent class.
  * @param _K  : Whether the info kept is big or little endian.
  */
#define _INST_FROM(_C, _M, _CC, _RCC, _T, _I, _J) \
class _I final : public _J \
{ \
	public: \
	_I () : _J (_C, _M, _CC, _RCC, _T) { } \
	protected: \
	virtual bool executeImpl () override; \
};

/** Idem but inheriting from basic instruction. */
#define _INST(_C, _M, _CC, _RCC, _T, _I, _K) \
	_INST_FROM(_C, _M, _CC, _RCC, _T, _I, MCHEmul::Instruction);

#define _INST_IMPL(_I) \
bool _I::executeImpl ()

#endif
  
// End of the file
/*@}*/