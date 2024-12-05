/** \ingroup CORE */
/*@{*/

/**	
 *	@file	
 *	File: Instruction.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 03/04/2021 \n
 *	Description: Defines the set of instructions.
 *	Versions: 1.0 Initial
 */

#ifndef __MCHEMUL_INSTRUCTION__
#define __MCHEMUL_INSTRUCTION__

#include <CORE/global.hpp>
#include <CORE/UBytes.hpp>
#include <CORE/Address.hpp>
#include <CORE/ProgramCounter.hpp>

namespace MCHEmul
{
	class CPU;
	class Memory;
	class Stack;
	class ProgramCounter;
	class InstructionDefined;

	/** Represents a instruction executed by a CPU. \n
		To define an instruction a "code" is needed. \n
		The code is an unsigned integer number (no more than 4 bytes). \n
		The information within the instruction (following to the code in the memory), 
		can be organized in little or big endian, so this parameter is also needed at construction time. \n
		The instruction is used in many ocasions and it is the central block of execution code 
		in the CPU class (@see MCHEmul::CPU). \n
		There can be two types of instructions: Defined & Undefined (@see below). */
	class Instruction
	{
		public:
		/** When the instruction is executed this data has to be updated. \n
		 	The attribute "parameters" has to contain the data that builts up thefull  definition of the instruction. \n
		 	This attribute must be set up when e4xecuting the transaction, 
			but it can also been set up in other moment, e.g. when decompiling code. */
		struct ExecutionData
		{
			ExecutionData ()
				: _cpu (nullptr), _memory (nullptr), _stack (nullptr),
				  _INOUTAddress (Address ()),
				  _INOUTData (),
				  _programCounter (0),
				  _parameters (),
				  _clockCycles (0), _additionalClockCycles (0),
				  _memoryPositions (0)
							{ }

			/** To get a setr of the parameteres:
				@param p :	The number of ubytes to exclude (since the first one).
				@param nP:	The number of parameters to include (excluding the first p of them).
				@param bE:	Whether the data to return is in big or little endian format. */
			UBytes parameters (size_t p, size_t nP = 1, bool bE = true) const;
			std::string parametersAsString (size_t p, size_t nP = 1, bool bE = true) const; 
			
			CPU* _cpu;
			Memory* _memory;
			Stack* _stack;

			mutable Address _INOUTAddress;
			mutable UBytes _INOUTData;
			mutable ProgramCounter _programCounter;
			mutable UBytes _parameters;
			mutable unsigned int _clockCycles;
			mutable unsigned int _additionalClockCycles;
			mutable unsigned int _memoryPositions;
		};

		Instruction () = delete;

		/** 
		  *	Constructor.
		  * @param c	:	The internal code of the instruction.
		  *	@param bE	:	Indicate whether the info contained is managed big or little endian.
		  */
		Instruction (unsigned int c, bool bE = true);

		Instruction (const Instruction&) = delete;

		Instruction (Instruction&&) = delete;

		Instruction& operator = (const Instruction&) = delete;

		Instruction& operator = (Instruction&&) = delete;

		virtual ~Instruction ()
							{ }

		unsigned int code () const
							{ return (_code); }
		size_t codeLength () const
							{ return (_codeLength); }
		bool bigEndian () const
							{ return (_bigEndian); }

		/** This method is really important,
			as it returns the number of bytes that the instruction will finally ocuppy in memory. \n
			This final number will depend on the type of instruction. \n
			e.g. A "defined instruction" will take the same positions in the memory always,
			whilst a "not defined" one could vary depending on the final form that the instruction takes when executed
			in a specific zone of the memory. */
		// Used more from parser (@see MCHEmul::Parser) and decompiling the memory (MCHEmul::Commads).
		// ...and when a instruction is executed cycle by clycle (@see CPU).
		virtual unsigned int memoryPositions (Memory* m, const Address& a) const = 0;
		/** Similar meaning with the clock cyles. */
		virtual unsigned int clockCycles (Memory* m, const Address& a) const = 0;

		// Used in parsers!
		/** 
		  *	To know whether the instruction template matches of not with the template received. \n
		  *	It has to return a reference to the instruction that matched to (when matches), 
		  *	and nullptr when it doesn't. \n
		  *	@param i	The template to verify whether it maches with...
		  *	NOTE: When matches the parameter prms received must also be filled. \n
		  *		  When doesn't the parameter prms will have trash!.
		  */
		virtual InstructionDefined* matchesWith (const std::string& i, Strings& prms) = 0;

		/** Using from the decompilers. \n
			To define the instruction with its parameters considering the info from memory position. \n
			Returns true when everything ok, and false in other case. */
		virtual bool defineInstructionFrom (Memory* m, const Address& addr) = 0;

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

		// Info about the current/last execution...
		const ExecutionData& lastExecutionData () const
							{ return (_lastExecutionData); }
		// Info extracted from the previous structure 
		// To simplify to recover data of it from other classes...
		// Very used in the method that execute the instructions
		const CPU* cpu () const
							{ return (_lastExecutionData._cpu); }
		CPU* cpu ()
							{ return (_lastExecutionData._cpu); }
		const Memory* memory () const
							{ return (_lastExecutionData._memory); }
		Memory* memory ()
							{ return (_lastExecutionData._memory); }
		const Stack* stack () const
							{ return (_lastExecutionData._stack); }
		Stack* stack ()
							{ return (_lastExecutionData._stack); }
		const ProgramCounter& programCounter () const
							{ return (_lastExecutionData._programCounter); }
		const UBytes& parameters () const
							{ return (_lastExecutionData._parameters); }
		/** Very rare. Used from ByCodeLine. */
		void setParameters (const UBytes& prms) const
							{ _lastExecutionData._parameters = prms;}
		const Address& INOUTAddress () const
							{ return (_lastExecutionData._INOUTAddress); }
		const UBytes& INOUTData () const
							{ return (_lastExecutionData._INOUTData); }
		// Very very important method used at CPU level (@see CPU)
		unsigned int clockCyclesExecuted () const
							{ return (_lastExecutionData._clockCycles); }
		unsigned int additionalClockCyclesExecuted () const
							{ return (_lastExecutionData._additionalClockCycles); }
		unsigned int totalClockCyclesExecuted () const
							{ return (clockCyclesExecuted () + additionalClockCyclesExecuted ()); }
		unsigned int memoryPositionsExecuted () const
							{ return (_lastExecutionData._memoryPositions); }
		
		// Used in run time (when debugging)
		/** To get the instruction as an string. \n
			Whatever the imlementation is, the ExecutionData can be used. \n
			Remember that its attribute "parameters" can be filled out from outside. */
		virtual std::string asString () const = 0;

		friend std::ostream& operator << (std::ostream& o, const Instruction& i)
							{ return (o << i.asString ()); }

		protected:
		unsigned int _code;
		size_t _codeLength; // Derivated fromn the previous one...
		bool _bigEndian;	// Should be common to the full set of instructions"!

		protected:
		/** Actualize when running. */
		ExecutionData _lastExecutionData;
	};

	// To simplify the way the instruction set is used...
	using Instructions = std::map <unsigned int, Instruction*>;
	using ListOfInstructions = std::vector <Instruction*>;

	/** The most common type of instruction. \n
		This type of instruction is fully defined at construction time. \n
		That's it: The number of cycles it takes, the bytes in memory it occupies, etc. All are defined at construction time. \n
		This is actually the only type of transaction that can be really "executed"! \n
		The execution of a instruction passes thought several steps called cycles. \n
		The type of each cycle depends on the CPU implementing this instruction (usually common for of instructions). \n
		Three types are defined by defect, but many others can be added later: _INTERAL, _READ, _WRITE. */
	class InstructionDefined : public Instruction
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

				/** To link a char with the type. \n
					It used in many places into the code. \n
					Important in the codification of the instructions: \n
					# = DATA, $ = DIR, & = RELJUMP, % = ABSJUMP. */
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
		/** No type of cycle declared. */
		static const unsigned int _CYCLENOTDEFINED = 0;
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
		/** To simplify the definition of the structure of type of cycles of a instruction. */
		using CycleStructure = std::vector <unsigned int>;

		/** 
		  *	Constructor.
		  * @param c	:	The internal code of the instruction.
		  *	@param mp	:	The number of memory positions (in number of ubytes) occupied by the instruction (and params).
		  * @param cc	:	The number of clock cyles the instruction uses to be executed (usually). \n
		  *					Some ocassions, some instructions under certain circunstances can take longer than expected. \n
		  *					@see for so: _additionalClockCycles (method and variable).
		  * @param cS	:	The structure of cycles. Is empty, then all are declared as nor defined...
		  *	@param t	:	The template of the instruction. \n
		  *					The way it is defined is key for it to be read and understood by the parsers. \n
		  *					The parameters should be within [] with two additional data: \n
		  *					The type of the parameter, and the number of bytes that parameter occupies. \n
		  *					Regarding the type: # means number, $ means address and & means relative jump (@see type of parameter).
		  *	@param bE	:	Indicate whether the info contained is managed big or little endian.
		  */
		InstructionDefined (unsigned int c, unsigned int mp, unsigned int cc, const CycleStructure& cS,
			const std::string& t, bool bE = true);

		// The internal structure...
		const std::string iTemplate () const
							{ return (_iTemplate); }
		const Structure& internalStructure () const
							{ return (_iStructure); }

		unsigned int memoryPositions () const
							{ return (_memoryPositions); }
		virtual unsigned int memoryPositions (Memory*, const Address&) const override
							{ return (_memoryPositions); } // it is the same always...
		unsigned int clockCycles () const
							{ return (_clockCycles); }
		virtual unsigned int clockCycles (Memory* m, const Address& a) const override
							{ return (_clockCycles); } // The ones defined at construction time...

		/** To get the type of the cycle. */
		const CycleStructure& cycleStructure () const
							{ return (_cycleStructure); }
		unsigned int typeOfCycle (size_t c) const // Bear in mind that no boundary check is done!
							{ return (_cycleStructure [c]); }

		virtual InstructionDefined* matchesWith (const std::string& i, Strings& prms) override;

		virtual bool defineInstructionFrom (Memory* m, const Address& addr) override;

		/** This method is key when, e.g, compiling assembler code. \n
			When a instruction is finally detected (@see matchesWith) the way the different
			bytes of the instruction are lined up might depend on the CPU and the instruction itself.
			Usually (the default behaviour) the main code (o codes) is (are) first and then the different parameters,
			that the instruction uses, but in some CPU this can change. \n
			e.g. in a Z80 the instruction SET 7,xx is made of 4 bits: \n
			1st, 2nd and 4th are the ones describing the instruction and the 3rd is the parameter. \n
			Don't forget to overload this method if the standard way doesn't work. \n
			If something is wrong with the execution e mut be filled up. */
		virtual std::vector <UByte> shapeCodeWithData (const std::vector <std::vector <UByte>>& b, bool& e) const;
		
		virtual std::string asString () const override;

		virtual bool execute (CPU* c, Memory* m, Stack* stk, ProgramCounter* pc) override;

		/** Some times, when a instruction is executed the final cycles taken. */
		unsigned int additionalClockCycles () const
							{ return (_additionalCycles); }
		/** Very rare. \n
			In some chips the addicional cycles come from external circunstances. */
		void addAdditionalClockCycles (unsigned int nc)
							{ _additionalCycles += nc; }

		protected:
		/** To analyze the structure of the instruction. \n
		 	Colled from the constructor. */
		Structure analyzeInstruction () const;

		/** Used in "asString" method (the default implementation). \n
			That method fills up the template in order. 
			It is: when the template defines a parameter takes the next byte/bytes in the list
			of _lastParameters to complete. \n
			But in some CPUS (Z80 e.g) this is not the case and when a bye is requested another
			byte is returned instead. By default this method doesn't do anything. */
		virtual size_t byteParameter (size_t nP) const
							{ return (nP); }

		/** 
		  * The implementation of the execution.
		  *	It has to be redefined when the instruction was implemented.
		  *	@return		true if the execution was ok, and false in other situation. 
		  *	The parameter f must be set to false whether the execution didin't finish
		  *	and the next CPU cycle has to considere it back. 
		  */
		virtual bool executeImpl (bool &f) = 0;

		protected:
		unsigned int _memoryPositions; 
		unsigned int _clockCycles;
		CycleStructure _cycleStructure;
		std::string _iTemplate;

		// Implementation
		// The internal structure helps us later to deal better with the instruction...
		Structure _iStructure;
		mutable unsigned int _additionalCycles;
	};

	/** This type of instruction is defined as it is processed. \n
		The instruction needs a list of different possibilities to choose. \n 
		The one to execute finally can depend on, e.g, other bytes read from the memory (@see method selectInstruction). */
	class InstructionUndefined : public Instruction
	{
		public:
		/** 
		  * Constructor. \n
		  * @param	c	:	A generic code for the Undefined Instruction (to group all instructions iside)
		  * @param	inst:	The list of instruction where to select the right one to finally execute. 
		  */
		InstructionUndefined (unsigned int c, const Instructions& inst);

		const Instructions& instructions () const
							{ return (_instructions); }
		Instructions& instructions ()
							{ return (_instructions); }

		/** Only valid when something has been executed... */
		virtual unsigned int memoryPositions (Memory* m, const Address& a) const override
							{ return (selectInstruction (m, a) -> memoryPositions (m, a)); }
		virtual unsigned int clockCycles (Memory* m, const Address& a) const override
							{ return (selectInstruction (m, a) -> clockCycles (m, a)); }

		virtual InstructionDefined* matchesWith (const std::string& i, Strings& prms) override;

		virtual bool defineInstructionFrom (Memory* m, const Address& addr) override;

		/** The final instruction executed should be always a defined one,
			and is is dragged up to the first level. */
		virtual bool execute (CPU* c, Memory* m, Stack* stk, ProgramCounter* pc) override;

		// Info just useful when "executing" the instruction...
		const Instruction* lastInstruction () const
							{ return (_lastInstruction); }
		Instruction* lasInstruction ()
							{ return (_lastInstruction); }

		// By default the asString method is based in the last instruction executed, 
		// ...but it should be based more in the parameters, as they can be set using external calls.
		// e.g. @see ByCodeLine class uses setParameters to set the parameters of the instruction before invoking it!
		virtual std::string asString () const override
							{ return ((_lastInstruction == nullptr) ? "" : _lastInstruction -> asString ()); }

		protected:
		/** As the instruction is undefined, 
			it is needed to select first the right instruction to execute. \n 
			This method has to be redefined to follow the right processes of the CPU emulated. */
		virtual const Instruction* selectInstruction (Memory* m, const Address& a) const = 0;

		protected:
		Instructions _instructions;

		// Implementation
		/** The last instruction executed inside finally. */
		Instruction* _lastInstruction;
	};
}

/** 
  *	To simplify the way a new InstructionDefined is implemented.
  *	@param _C  : Code.
  *	@param _M  : MemoryPositions occupied.
  *	@param _CC : Clock cycles used. 
  * @param _RCC: Structure of the internal cycles.
  *				 Maintained empty to declare that they are not defined.
  * @param _T  : The template to print the instruction.
  *	@param _I  : Name of the intruction.
  * @param _J  : Name of the parent class. The last parent class should be InstructionDefined.
  * @param _K  : Whether the info kept is big or little endian.
  * Always this method is used take into account that the variable _FINISH 
  * must be set to true of false to indicate whether the instruction has or not finished.
  */
#define _INST_FROM(_C, _M, _CC, _RCC, _T, _I, _J, K) \
class _I final : public _J \
{ \
	public: \
	_I () : _J (_C, _M, _CC, _RCC, _T, K) { } \
	protected: \
	virtual bool executeImpl (bool& _FINISH) override; \
};

/** Idem but inheriting from basic instruction. */
#define _INST(_C, _M, _CC, _RCC, _T, _I, _K) \
	_INST_FROM(_C, _M, _CC, _RCC, _T, _I, MCHEmul::InstructionDefined, K);

#define _INST_IMPL(_I) \
bool _I::executeImpl (bool& _FINISH)

#endif
  
// End of the file
/*@}*/