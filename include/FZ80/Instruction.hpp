/** \ingroup FZ80 */
/*@{*/

/**	
 *	@file	
 *	File: Instruction.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 29/10/2023 \n
 *	Description:	The type of instruction used by FZ80 family.
 *	Versions: 1.0 Initial
 */

#ifndef __FZX80_INSTRUCTION__
#define __FZX80_INSTRUCTION__

#include <CORE/incs.hpp>
#include <FZ80/CZ80.hpp>

namespace FZ80
{
	/** 
	  *	To simplify the way a new Instruction for this processor is implemented.
	  *	@param _C  : Code.
	  *	@param _M  : MemoryPositions occupied.
	  *	@param _CC : Clock cycles used. 
	  * @param _RCC: Structure of the internal cycles.
	  *				 Maintained empty to declare that they are not defined.
	  * @param _T  : The template to print the instruction.
	  *	@param _I  : Name of the intruction.
	  * @param _J  : Name of the parent class. The last parent class should be InstructionDefined.
	  */
	#define _INSTZ80_FROM(_C, _M, _CC, _RCC, _T, _I, _J) \
	class _I final : public _J \
	{ \
		public: \
		_I () : _J (_C, _M, _CC, _RCC, _T) { } \
		protected: \
		virtual bool executeImpl (bool& _FINISH) override; \
	};

	/** The global definition of a Z80 instruction. */
	class Instruction : public MCHEmul::InstructionDefined
	{
		public:
		Instruction (unsigned int c, unsigned int mp, unsigned int cc, 
				const MCHEmul::InstructionDefined::CycleStructure& cS, 
				const std::string& t)
			: MCHEmul::InstructionDefined (c, mp, cc, cS, t, false)
							{ }

		// To get the reference to registers...
		MCHEmul::Register& registerA ()
							{ return (dynamic_cast <CZ80*> (cpu ()) -> aRegister ()); }
		const MCHEmul::Register& registerA () const
							{ return (dynamic_cast <const CZ80*> (cpu ()) -> aRegister ()); }
		MCHEmul::Register& registerF ()
							{ return (dynamic_cast <CZ80*> (cpu ()) -> fRegister ()); }
		const MCHEmul::Register& registerF () const
							{ return (dynamic_cast <const CZ80*> (cpu ()) -> fRegister ()); }
		MCHEmul::RefRegisters& registerAF ()
							{ return (dynamic_cast <CZ80*> (cpu ()) -> afRegister ()); }
		const MCHEmul::RefRegisters& registerAF () const
							{ return (dynamic_cast <const CZ80*> (cpu ()) -> afRegister ()); }
		MCHEmul::RefRegisters& registerAFP ()
							{ return (dynamic_cast <CZ80*> (cpu ()) -> afpRegister ()); }
		const MCHEmul::RefRegisters& registerAFP () const
							{ return (dynamic_cast <const CZ80*> (cpu ()) -> afpRegister ()); }

		MCHEmul::Register& registerB ()
							{ return (dynamic_cast <CZ80*> (cpu ()) -> bRegister ()); }
		const MCHEmul::Register& registerB () const
							{ return (dynamic_cast <const CZ80*> (cpu ()) -> bRegister ()); }
		MCHEmul::Register& registerC ()
							{ return (dynamic_cast <CZ80*> (cpu ()) -> cRegister ()); }
		const MCHEmul::Register& registerC () const
							{ return (dynamic_cast <const CZ80*> (cpu ()) -> cRegister ()); }
		MCHEmul::RefRegisters& registerBC ()
							{ return (dynamic_cast <CZ80*> (cpu ()) -> bcRegister ()); }
		const MCHEmul::RefRegisters& registerBC () const
							{ return (dynamic_cast <const CZ80*> (cpu ()) -> bcRegister ()); }
		MCHEmul::RefRegisters& registerBCP ()
							{ return (dynamic_cast <CZ80*> (cpu ()) -> bcpRegister ()); }
		const MCHEmul::RefRegisters& registerBCP () const
							{ return (dynamic_cast <const CZ80*> (cpu ()) -> bcpRegister ()); }

		MCHEmul::Register& registerD ()
							{ return (dynamic_cast <CZ80*> (cpu ()) -> dRegister ()); }
		const MCHEmul::Register& registerD () const
							{ return (dynamic_cast <const CZ80*> (cpu ()) -> dRegister ()); }
		MCHEmul::Register& registerE ()
							{ return (dynamic_cast <CZ80*> (cpu ()) -> eRegister ()); }
		const MCHEmul::Register& registerE () const
							{ return (dynamic_cast <const CZ80*> (cpu ()) -> eRegister ()); }
		MCHEmul::RefRegisters& registerDE ()
							{ return (dynamic_cast <CZ80*> (cpu ()) -> deRegister ()); }
		const MCHEmul::RefRegisters& registerDE () const
							{ return (dynamic_cast <const CZ80*> (cpu ()) -> deRegister ()); }
		MCHEmul::RefRegisters& registerDEP ()
							{ return (dynamic_cast <CZ80*> (cpu ()) -> depRegister ()); }
		const MCHEmul::RefRegisters& registerDEP () const
							{ return (dynamic_cast <const CZ80*> (cpu ()) -> depRegister ()); }

		MCHEmul::Register& registerH ()
							{ return (dynamic_cast <CZ80*> (cpu ()) -> hRegister ()); }
		const MCHEmul::Register& registerH () const
							{ return (dynamic_cast <const CZ80*> (cpu ()) -> hRegister ()); }
		MCHEmul::Register& registerL ()
							{ return (dynamic_cast <CZ80*> (cpu ()) -> lRegister ()); }
		const MCHEmul::Register& registerL () const
							{ return (dynamic_cast <const CZ80*> (cpu ()) -> lRegister ()); }
		MCHEmul::RefRegisters& registerHL ()
							{ return (dynamic_cast <CZ80*> (cpu ()) -> hlRegister ()); }
		const MCHEmul::RefRegisters& registerHL () const
							{ return (dynamic_cast <const CZ80*> (cpu ()) -> hlRegister ()); }
		MCHEmul::RefRegisters& registerHLP ()
							{ return (dynamic_cast <CZ80*> (cpu ()) -> hlpRegister ()); }
		const MCHEmul::RefRegisters& registerHLP () const
							{ return (dynamic_cast <const CZ80*> (cpu ()) -> hlpRegister ()); }

		MCHEmul::Register& registerI ()
							{ return (dynamic_cast <CZ80*> (cpu ()) -> iRegister ()); }
		const MCHEmul::Register& registerI () const
							{ return (dynamic_cast <const CZ80*> (cpu ()) -> iRegister ()); }
		MCHEmul::Register& registerR ()
							{ return (dynamic_cast <CZ80*> (cpu ()) -> rRegister ()); }
		const MCHEmul::Register& registerR () const
							{ return (dynamic_cast <const CZ80*> (cpu ()) -> rRegister ()); }

		MCHEmul::Register& registerIXH ()
							{ return (dynamic_cast <CZ80*> (cpu ()) -> ixhRegister ()); }
		const MCHEmul::Register& registerIXH () const
							{ return (dynamic_cast <const CZ80*> (cpu ()) -> ixhRegister ()); }
		MCHEmul::Register& registerIXL ()
							{ return (dynamic_cast <CZ80*> (cpu ()) -> ixlRegister ()); }
		const MCHEmul::Register& registerIXL () const
							{ return (dynamic_cast <const CZ80*> (cpu ()) -> ixlRegister ()); }
		MCHEmul::RefRegisters& registerIX ()
							{ return (dynamic_cast <CZ80*> (cpu ()) -> ixRegister ()); }
		const MCHEmul::RefRegisters& registerIX () const
							{ return (dynamic_cast <const CZ80*> (cpu ()) -> ixRegister ()); }

		MCHEmul::Register& registerIYH ()
							{ return (dynamic_cast <CZ80*> (cpu ()) -> iyhRegister ()); }
		const MCHEmul::Register& registerIYH () const
							{ return (dynamic_cast <const CZ80*> (cpu ()) -> iyhRegister ()); }
		MCHEmul::Register& registerIYL () 
							{ return (dynamic_cast <CZ80*> (cpu ()) -> iylRegister ()); }
		const MCHEmul::Register& registerIYL () const
							{ return (dynamic_cast <const CZ80*> (cpu ()) -> iylRegister ()); }
		MCHEmul::RefRegisters& registerIY ()
							{ return (dynamic_cast <CZ80*> (cpu ()) -> iyRegister ()); }
		const MCHEmul::RefRegisters& registerIY () const
							{ return (dynamic_cast <const CZ80*> (cpu ()) -> iyRegister ()); }

		// To get the value of the registers...
		const MCHEmul::UByte& valueRegisterA () const
							{ return (registerA ().values ()[0]); }
		const MCHEmul::UByte& valueRegisterF () const
							{ return (registerF ().values ()[0]); }
		MCHEmul::UBytes valueRegisterAF () const
							{ return (MCHEmul::UBytes ({ valueRegisterA (), valueRegisterF () })); }

		const MCHEmul::UByte& valueRegisterB () const
							{ return (registerB ().values ()[0]); }
		const MCHEmul::UByte& valueRegisterC () const
							{ return (registerC ().values ()[0]); }
		MCHEmul::UBytes valueRegisterBC () const
						{ return (MCHEmul::UBytes ({ valueRegisterB (), valueRegisterC () })); }

		const MCHEmul::UByte& valueRegisterD () const
							{ return (registerD ().values ()[0]); }
		const MCHEmul::UByte& valueRegisterE () const
							{ return (registerE ().values ()[0]); }
		MCHEmul::UBytes valueRegisterDE () const
							{ return (MCHEmul::UBytes ({ valueRegisterD (), valueRegisterE () })); }

		const MCHEmul::UByte& valueRegisterH () const
							{ return (registerH ().values () [0]); }
		const MCHEmul::UByte& valueRegisterL () const
							{ return (registerL ().values () [0]); }
		MCHEmul::UBytes valueRegisterHL () const
							{ return (MCHEmul::UBytes ({ valueRegisterH (), valueRegisterL () })); }
		inline MCHEmul::UBytes valueRegisterSP () const;

		const MCHEmul::UByte& valueRegisterI () const
							{ return (registerI ().values () [0]); }
		const MCHEmul::UByte& valueRegisterR () const
							{ return (registerR ().values () [0]); }
		MCHEmul::UBytes valueRegisterIR () const
							{ return (MCHEmul::UBytes ({ valueRegisterI (), valueRegisterR () })); }

		const MCHEmul::UByte& valueRegisterIXH () const
							{ return (registerIXH ().values () [0]); }
		const MCHEmul::UByte& valueRegisterIXL () const
							{ return (registerIXL ().values () [0]); }
		MCHEmul::UBytes valueRegisterIX () const
							{ return (MCHEmul::UBytes ({ valueRegisterIXH (), valueRegisterIXL () })); }
		const MCHEmul::UByte& valueRegisterIYH () const
							{ return (registerIYH ().values () [0]); }
		const MCHEmul::UByte& valueRegisterIYL () const
							{ return (registerIYL ().values () [0]); }
		MCHEmul::UBytes valueRegisterIY () const
							{ return (MCHEmul::UBytes ({ valueRegisterIYH (), valueRegisterIYL () })); }

		// To get the address pointed by the register...
		MCHEmul::Address addressBC (char n = 0) const // It can be negative!
							{ return (_lastExecutionData._INOUTAddress = (n >= 0)
								? (MCHEmul::Address (valueRegisterBC (), true) + (size_t) n)
								: (MCHEmul::Address (valueRegisterBC (), true) - (size_t) (-n))); }
		MCHEmul::Address addressDE (char n = 0) const
							{ return (_lastExecutionData._INOUTAddress = (n >= 0)
								? (MCHEmul::Address (valueRegisterDE (), true) + (size_t) n)
								: (MCHEmul::Address (valueRegisterDE (), true) - (size_t) (-n))); } 
		MCHEmul::Address addressHL (char n = 0) const
							{ return (_lastExecutionData._INOUTAddress = (n >= 0)
								? (MCHEmul::Address (valueRegisterHL (), true) + (size_t) n)
								: (MCHEmul::Address (valueRegisterHL (), true) - (size_t) (-n))); }
		MCHEmul::Address addressIX (char n = 0) const
							{ return (_lastExecutionData._INOUTAddress = (n >= 0)
								? (MCHEmul::Address (valueRegisterIX (), true) + (size_t) n)
								: (MCHEmul::Address (valueRegisterIX (), true) - (size_t) (-n))); }
		MCHEmul::Address addressIY (char n = 0) const
							{ return (_lastExecutionData._INOUTAddress = (n >= 0)
								? (MCHEmul::Address (valueRegisterIY (), true) + (size_t) n)
								: (MCHEmul::Address (valueRegisterIY (), true) - (size_t) (-n))); }
		MCHEmul::Address addressSP (char n = 0) const
							{ return (_lastExecutionData._INOUTAddress = (n >= 0)
								? (MCHEmul::Address (valueRegisterSP (), true) + (size_t) n)
								: (MCHEmul::Address (valueRegisterSP (), true) - (size_t) (-n))); }
		MCHEmul::Address addressIR (char n = 0) const
							{ return (_lastExecutionData._INOUTAddress = (n >= 0)
								? (MCHEmul::Address (valueRegisterIR (), true) + (size_t) n)
								: (MCHEmul::Address (valueRegisterIR (), true) + (size_t) (-n))); }

		// To get the value pointed by the registers...
		const MCHEmul::UByte& valueAddressBC (char n = 0) const
							{ return ((_lastExecutionData._INOUTData = 
								MCHEmul::UBytes ({ memory () -> value (addressBC (n)) }))[0]); }
		const MCHEmul::UByte& valueAddressDE (char n = 0) const
							{ return ((_lastExecutionData._INOUTData = 
								MCHEmul::UBytes ({ memory () -> value (addressDE (n)) }))[0]); }
		const MCHEmul::UByte& valueAddressHL (char n = 0) const
							{ return ((_lastExecutionData._INOUTData = 
								MCHEmul::UBytes ({ memory () -> value (addressHL (n)) }))[0]); }
		const MCHEmul::UByte& valueAddressIX (char n = 0) const
							{ return ((_lastExecutionData._INOUTData = 
								MCHEmul::UBytes ({ memory () -> value (addressIX (n)) }))[0]); }
		const MCHEmul::UByte& valueAddressIY (char n = 0) const
							{ return ((_lastExecutionData._INOUTData = 
								MCHEmul::UBytes ({ memory () -> value (addressIY (n)) }))[0]); }
		const MCHEmul::UByte& valueAddressSP (char n = 0) const
							{ return ((_lastExecutionData._INOUTData = 
								MCHEmul::UBytes ({ memory () -> value (addressSP (n)) }))[0]); }

		/** In Z80, every opcode fecth (M1 cycle usually) increments the R register (7 bits). \n
			It is done apparently to refresh RAM. */
		virtual bool execute (MCHEmul::CPU* c, MCHEmul::Memory* m, 
			MCHEmul::Stack* stk, MCHEmul::ProgramCounter* pc) final override;
	};

	// ---
	inline MCHEmul::UBytes Instruction::valueRegisterSP () const
	{ 
		// After this instruction they will be in big endian...
		MCHEmul::UBytes result = memory () -> stack () -> currentAddress ().bytes ();
		result.setMinLength (2);
		return (result);
	}

	/** In Z80 there are instructions that are undefined reading the first byte.
		It is needed to read more bytes to know what the final instruction is. */
	class InstructionUndefined : public MCHEmul::InstructionUndefined
	{
		public:
		/** c = the code that all instructions shared. */
		InstructionUndefined (unsigned int c, const MCHEmul::Instructions& inst);

		protected:
		// Implementation
		/** To speed up the access to the list of instructions later. */
		MCHEmul::ListOfInstructions _rawInstructions;
	};

	/** Instruction code in the second byte in memory next to program counter location. 
		This is only useful for execution purposes. */
	class Byte2InstructionCode final : public InstructionUndefined
	{
		public:
		Byte2InstructionCode (unsigned int c, const MCHEmul::Instructions& inst)
			: InstructionUndefined (c, inst)
							{ }

		/** This type of instructions increments the Register R in one cycle more. */
		virtual bool execute (MCHEmul::CPU* c, MCHEmul::Memory* m, 
			MCHEmul::Stack* stk, MCHEmul::ProgramCounter* pc) final override;
	
		virtual std::string asString () const override
							{ return (_rawInstructions [size_t (_lastExecutionData._parameters [1].value ())] -> asString ()); }

		private:
		virtual const MCHEmul::Instruction* selectInstruction (MCHEmul::Memory* m, 
			const MCHEmul::Address& addr) const override
							{ // The code is given by the second byte next to program counter in the memory...
							  return (_rawInstructions [size_t (m -> values (addr, 2)[1].value ())]); }
	};

	/** Instruction code in the fourth byte in memory next to program counter location. 
		This is only useful for execution purposes. */
	class Byte4InstructionCode final : public InstructionUndefined
	{
		public:
		Byte4InstructionCode (unsigned int c, const MCHEmul::Instructions& inst)
			: InstructionUndefined (c, inst)
							{ }
		
		// The byte 4 instructions that inherits from 2 byte instructions
		// doesn't increment more the R register!

		virtual std::string asString () const override
							{ return (_rawInstructions [size_t (_lastExecutionData._parameters [3].value ())] -> asString ()); }

		private:
		virtual MCHEmul::Instruction* selectInstruction (MCHEmul::Memory* m, 
			const MCHEmul::Address& addr) const override
							{ // The code is given by the second byte next to program counter in the memory...
							  return (_rawInstructions [size_t (m -> values (addr, 4)[3].value ())]); }
	};
}

#endif
  
// End of the file
/*@}*/
